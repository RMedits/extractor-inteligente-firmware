#include "Config.h"
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <ESP32Encoder.h>
#include <OneButton.h>
#include <Wire.h>
#include <time.h>

// --- WIFI & WEB ---
#include "Secrets.h"
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFi.h>

// =====================================================
// CONSTANTES DE CONTROL INTELIGENTE
// =====================================================
#define MAX_FAN_SPEED 90   // NUNCA superar 90%
#define NIGHT_MAX_SPEED 40 // Modo noche silencioso
#define IDLE_SPEED 18      // Ventilación ASHRAE continua
#define SHOWER_SPEED 85    // Boost ducha
#define ODOR_SPEED 60      // Purga olores

#define NIGHT_START_HOUR 23
#define NIGHT_END_HOUR 7

#define DEW_POINT_THRESHOLD 20.0 // °C - Activar ducha
#define DEW_POINT_HYSTERESIS 1.5 // °C - Histéresis salida
#define RH_RATE_THRESHOLD 5.0    // %/min - Detección rápida ducha
#define ODOR_SENSITIVITY 0.80    // 80% del baseline
#define ODOR_RECOVERY 0.95       // 95% para considerar limpio
#define ODOR_PURGE_TIME 300000   // 5 min purga adicional

#define RH_BUFFER_SIZE 30     // 1 minuto de muestras (2s cada una)
#define SENSOR_SAMPLE_MS 2000 // Muestreo cada 2 segundos

// =====================================================
// OBJETOS GLOBALES
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display =
    Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
ESP32Encoder encoder;
OneButton btnOk(PIN_ENC_SW, true);
OneButton btnBack(PIN_BTN_BACK, true);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// --- MQTT ---
WiFiClient mqttWifiClient;
PubSubClient mqtt(mqttWifiClient);
unsigned long lastMqttPublish = 0;
unsigned long lastMqttReconnect = 0;
bool mqttDiscoverySent = false;

// MQTT Topics
const char *MQTT_TOPIC_SENSORS = "extractor/sensors";
const char *MQTT_TOPIC_STATE = "extractor/state";
const char *MQTT_TOPIC_AVAIL = "extractor/availability";
const char *MQTT_TOPIC_CMD_MODE = "extractor/cmd/mode";
const char *MQTT_TOPIC_CMD_FAN = "extractor/cmd/fan_speed";
const char *MQTT_TOPIC_CMD_NIGHT_START = "extractor/cmd/night_start";
const char *MQTT_TOPIC_CMD_NIGHT_END = "extractor/cmd/night_end";

// =====================================================
// MÁQUINA DE ESTADOS
// =====================================================
enum SystemState {
  EST_IDLE,            // Automático (REPOSO)
  EST_SHOWER,          // Detección ducha
  EST_ODOR,            // Detección olor
  EST_NIGHT,           // Modo noche automático
  EST_MANUAL_CONFIG,   // Asistente configuración (4 pasos)
  EST_MANUAL_LIMITED,  // Modo manual temporizado
  EST_MANUAL_INFINITE, // Modo manual sin fin
  EST_DIAGNOSTIC,      // Debug
  EST_ERROR            // Error
};
SystemState currentState = EST_IDLE; // Siempre arranca en AUTOMÁTICO
SystemState previousState = EST_IDLE;

// =====================================================
// ESTRUCTURAS DE DATOS
// =====================================================
struct Sensors {
  float temp = 0;          // AHT20
  float hum = 0;           // AHT20
  float pressure = 0;      // BMP280
  float temp_bmp = 0;      // BMP280 Temp (Backup)
  int aqi_raw = 0;         // MQ-135 sin procesar
  float aqi_corrected = 0; // MQ-135 compensado
  float dewPoint = 0;      // Punto de Rocío calculado
  int fan_rpm = 0;
} sensors;

struct Config {
  int fanSpeed = IDLE_SPEED;
  int timerMinutes = 0;
  unsigned long timerStart = 0;
  bool screenOn = true;
  unsigned long lastInteraction = 0;
} sysConfig;

// Configuración del asistente manual
struct ManualConfig {
  int configStep = 0;          // 0=Tiempo, 1=Velocidad
  int timeSelectionIndex = 1;  // Índice de opción de tiempo (0..6)
  int timeMinutes = 30;        // Valor real en minutos
  int speedPercent = 60;       // Velocidad seleccionada
  unsigned long startTime = 0; // Para cuenta regresiva
} manualCfg;

// Opciones de tiempo predefinidas (en minutos)
const int manualTimeOptions[] = {15, 30, 45, 60};
const int manualTimeOptionsCount = 4;

// =====================================================
// VARIABLES GLOBALES
// =====================================================
volatile int tachPulses = 0;
unsigned long lastRpmCalc = 0;
unsigned long lastWsUpdate = 0;
unsigned long lastSensorSample = 0;
unsigned long lastDisplayUpdate = 0;
bool wifiConnected = false;
String wifiStatusMsg = "Init...";

// Rate-of-Rise buffer
float rhBuffer[RH_BUFFER_SIZE];
int rhBufferIndex = 0;
bool rhBufferFull = false;

// Línea base adaptativa
float aqiBaseline = 2500;
float dewPointBaseline = 15.0;
unsigned long odorTimerStart = 0;

// NTP Time
int currentHour = 12;
bool ntpSynced = false;

// Stall protection
unsigned long stallDetectStart = 0;
int stallRetryCount = 0;

// =====================================================
// ISR
// =====================================================
void IRAM_ATTR tachISR() { tachPulses++; }

// =====================================================
// FUNCIONES DE CÁLCULO
// =====================================================

// Punto de Rocío (Fórmula de Magnus)
float calculateDewPoint(float T, float RH) {
  if (RH <= 0 || RH > 100)
    return T; // Fallback
  const float A = 17.62;
  const float B = 243.12;
  float alpha = log(RH / 100.0) + ((A * T) / (B + T));
  return (B * alpha) / (A - alpha);
}

// Factor de corrección MQ-135 por temperatura y humedad
float getCorrectionFactor(float t, float h) {
  // Referencia: 20°C, 33% RH (datasheet)
  return 1.0 + 0.005 * (t - 20.0) - 0.002 * (h - 33.0);
}

// AQI corregido
float getCorrectedAQI(int rawValue, float temp, float hum) {
  float cf = getCorrectionFactor(temp, hum);
  if (cf <= 0)
    cf = 1.0; // Evitar división por cero
  return rawValue / cf;
}

// Rate of Rise (cambio de humedad en 1 minuto)
float getRateOfRise() {
  if (!rhBufferFull)
    return 0; // No hay suficientes datos
  int oldIndex = (rhBufferIndex + 1) % RH_BUFFER_SIZE;
  return sensors.hum - rhBuffer[oldIndex];
}

// Verificar si es hora nocturna
bool isNightTime() {
  return (currentHour >= NIGHT_START_HOUR || currentHour < NIGHT_END_HOUR);
}

// =====================================================
// CONTROL DEL VENTILADOR
// =====================================================
void setFanSpeedSafe(int percent) {
  // NUNCA superar MAX_FAN_SPEED (90%)
  if (percent < 0)
    percent = 0;
  if (percent > MAX_FAN_SPEED)
    percent = MAX_FAN_SPEED;

  // En modo noche, limitar aún más (excepto emergencia)
  if (currentState == EST_NIGHT && percent > NIGHT_MAX_SPEED) {
    percent = NIGHT_MAX_SPEED;
  }

  sysConfig.fanSpeed = percent;

  // CONTROL DEL RELÉ (Active LOW: LOW = ON, HIGH = OFF)
  digitalWrite(PIN_RELAY, (percent > 0) ? LOW : HIGH);

  // CONTROL PWM (Lógica Invertida por Open Drain en cable Azul)
  // 0% Speed -> PWM 255 (Logic Low -> Fan Min/Stop)
  // 100% Speed -> PWM 0 (Logic High -> Fan Max)
  int duty = map(percent, 0, 100, 255, 0);
  ledcWrite(FAN_PWM_CHANNEL, duty);
}

int getTargetSpeed() {
  switch (currentState) {
  case EST_IDLE:
    return IDLE_SPEED;
  case EST_SHOWER:
    return SHOWER_SPEED;
  case EST_ODOR:
    return ODOR_SPEED;
  case EST_NIGHT:
    return min(IDLE_SPEED, NIGHT_MAX_SPEED);
  case EST_MANUAL_CONFIG:
    return sysConfig.fanSpeed; // Mantener velocidad actual durante config
  case EST_MANUAL_LIMITED:
  case EST_MANUAL_INFINITE:
    return manualCfg.speedPercent;
  case EST_DIAGNOSTIC:
    return sysConfig.fanSpeed;
  case EST_ERROR:
    return 0;
  default:
    return IDLE_SPEED;
  }
}

// =====================================================
// ACTUALIZACIÓN DE SENSORES
// =====================================================
void updateSensors() {
  sensors_event_t humidity, temp;
  if (aht.getEvent(&humidity, &temp)) {
    sensors.temp = temp.temperature;
    sensors.hum = humidity.relative_humidity;
  }
  sensors.pressure = bmp.readPressure() / 100.0F;
  sensors.temp_bmp = bmp.readTemperature();
  sensors.aqi_raw = analogRead(PIN_MQ135);

  // Cálculos derivados
  sensors.dewPoint = calculateDewPoint(sensors.temp, sensors.hum);
  sensors.aqi_corrected =
      getCorrectedAQI(sensors.aqi_raw, sensors.temp, sensors.hum);

  // Actualizar buffer Rate-of-Rise
  rhBuffer[rhBufferIndex] = sensors.hum;
  rhBufferIndex = (rhBufferIndex + 1) % RH_BUFFER_SIZE;
  if (rhBufferIndex == 0)
    rhBufferFull = true;

  // Actualizar línea base (solo si aire está limpio)
  if (sensors.aqi_corrected > aqiBaseline * 0.95) {
    aqiBaseline = aqiBaseline * 0.995 +
                  sensors.aqi_corrected * 0.005; // Media móvil muy lenta
  }

  // RPM
  if (millis() - lastRpmCalc > 1000) {
    sensors.fan_rpm = (tachPulses / 2) * 60;
    tachPulses = 0;
    lastRpmCalc = millis();
  }
}

// =====================================================
// MÁQUINA DE ESTADOS - LÓGICA DE TRANSICIÓN
// =====================================================
void updateStateMachine() {
  float rateOfRise = getRateOfRise();
  bool night = isNightTime();

  // Guardar estado anterior para debug
  previousState = currentState;

  switch (currentState) {
  case EST_IDLE:
    // Detección de DUCHA (Rate-of-Rise O Punto de Rocío alto)
    if (rateOfRise > RH_RATE_THRESHOLD ||
        sensors.dewPoint > DEW_POINT_THRESHOLD) {
      currentState = EST_SHOWER;
      dewPointBaseline = sensors.dewPoint - 3.0; // Guardar referencia
    }
    // Detección de OLOR
    else if (sensors.aqi_corrected < aqiBaseline * ODOR_SENSITIVITY) {
      currentState = EST_ODOR;
      odorTimerStart = millis();
    }
    // Cambio a NOCHE
    else if (night) {
      currentState = EST_NIGHT;
    }
    break;

  case EST_SHOWER:
    // Salir cuando Td baje con histéresis
    if (sensors.dewPoint < dewPointBaseline + DEW_POINT_HYSTERESIS) {
      currentState = night ? EST_NIGHT : EST_IDLE;
    }
    break;

  case EST_ODOR:
    // Salir cuando aire se limpie + purga adicional
    if (sensors.aqi_corrected > aqiBaseline * ODOR_RECOVERY &&
        millis() - odorTimerStart > ODOR_PURGE_TIME) {
      currentState = night ? EST_NIGHT : EST_IDLE;
    }
    break;

  case EST_NIGHT:
    // Emergencia: detectar ducha incluso de noche
    if (rateOfRise > RH_RATE_THRESHOLD * 1.5 ||
        sensors.dewPoint > DEW_POINT_THRESHOLD + 2) {
      currentState = EST_SHOWER;
      dewPointBaseline = sensors.dewPoint - 3.0;
    }
    // Salir de modo noche
    else if (!night) {
      currentState = EST_IDLE;
    }
    break;

  case EST_MANUAL_CONFIG:
    // No hay timeout en configuración
    break;

  case EST_MANUAL_LIMITED:
    // Verificar si el tiempo se agotó
    if (millis() - manualCfg.startTime >
        (unsigned long)manualCfg.timeMinutes * 60000) {
      currentState = night ? EST_NIGHT : EST_IDLE;
      setFanSpeedSafe(IDLE_SPEED);
    }
    break;

  case EST_MANUAL_INFINITE:
    // Sin timeout, solo se sale con Back
    break;

  case EST_ERROR:
    // Recuperación automática tras 60 segundos
    if (millis() - stallDetectStart > 60000) {
      stallRetryCount++;
      if (stallRetryCount < 3) {
        currentState = EST_IDLE;
      }
    }
    break;

  case EST_DIAGNOSTIC:
    // Se sale manualmente
    break;
  }
}

// =====================================================
// PROTECCIÓN STALL (Bloqueo de Rotor)
// =====================================================
void checkStallProtection() {
  // TODO: ACTIVAR cuando se monte el ventilador
  // Actualmente desactivado porque no hay ventilador conectado
  return;

  /*
  if (currentState == EST_ERROR)
    return;

  if (sysConfig.fanSpeed > 20 && sensors.fan_rpm == 0) {
    if (stallDetectStart == 0)
      stallDetectStart = millis();
    if (millis() - stallDetectStart > 5000) {
      currentState = EST_ERROR;
      setFanSpeedSafe(0);
    }
  } else {
    stallDetectStart = 0;
  }
  */
}

// =====================================================
// ACTUALIZAR HORA VIA NTP
// =====================================================
void updateNTPTime() {
  if (!wifiConnected)
    return;

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    currentHour = timeinfo.tm_hour;
    ntpSynced = true;
  }
}

// =====================================================
// LEDs
// =====================================================
void updateLeds() {
  bool green = false, yellow = false, red = false;

  switch (currentState) {
  case EST_IDLE:
    green = true;
    break;
  case EST_SHOWER:
    // Amarillo parpadeante sería ideal, pero simplificamos
    yellow = true;
    red = true; // Rojo + Amarillo = Alerta vapor
    break;
  case EST_ODOR:
    yellow = true;
    break;
  case EST_NIGHT:
    // Solo verde tenue (encendido)
    green = true;
    break;
  case EST_MANUAL_CONFIG:
  case EST_MANUAL_LIMITED:
  case EST_MANUAL_INFINITE:
    yellow = true;
    green = true; // Verde + Amarillo = Manual activo
    break;
  case EST_ERROR:
    red = true;
    break;
  case EST_DIAGNOSTIC:
    // Todos encendidos para test
    green = true;
    yellow = true;
    red = true;
    break;
  default:
    green = true;
  }

  // PIN_LED_GREEN Active HIGH
  digitalWrite(PIN_LED_RED, red ? HIGH : LOW);
  digitalWrite(PIN_LED_YELLOW, yellow ? HIGH : LOW);
  digitalWrite(PIN_LED_GREEN, green ? HIGH : LOW);

  // LEDs de Puerta (indicadores de ocupación)
  // Rojo = Ocupado (baño en uso), Verde = Libre
  bool occupied = (currentState != EST_IDLE);
  digitalWrite(PIN_LED_DOOR_RED, occupied ? HIGH : LOW);
  digitalWrite(PIN_LED_DOOR_GREEN, occupied ? LOW : HIGH);
}

// =====================================================
// HELPER: Estado a String
// =====================================================
String getStateString() {
  switch (currentState) {
  case EST_IDLE:
    return "IDLE";
  case EST_SHOWER:
    return "SHOWER";
  case EST_ODOR:
    return "ODOR";
  case EST_NIGHT:
    return "NIGHT";
  case EST_MANUAL_CONFIG:
    return "CONFIG";
  case EST_MANUAL_LIMITED:
    return "MANUAL_LIM";
  case EST_MANUAL_INFINITE:
    return "MANUAL_INF";
  case EST_DIAGNOSTIC:
    return "DIAG";
  case EST_ERROR:
    return "ERROR";
  default:
    return "IDLE";
  }
}

// =====================================================
// NOTIFICAR CLIENTES WEBSOCKET
// =====================================================
void notifyClients() {
  JsonDocument doc;
  doc["temp"] = sensors.temp;
  doc["hum"] = sensors.hum;
  doc["pressure"] = sensors.pressure;
  doc["aqi"] = sensors.aqi_raw;
  doc["aqi_c"] = sensors.aqi_corrected;
  doc["dewPoint"] = sensors.dewPoint;
  doc["fan"] = sysConfig.fanSpeed;
  doc["rpm"] = sensors.fan_rpm;
  doc["mode"] = getStateString();
  doc["hour"] = currentHour;

  String output;
  serializeJson(doc, output);
  ws.textAll(output);
}

// =====================================================
// MQTT: Auto-Discovery para Home Assistant
// =====================================================
void mqttSendDiscovery() {
  String macId = WiFi.macAddress();
  macId.replace(":", "");
  macId.toLowerCase();
  String deviceId = "extractor_" + macId;

  // Bloque device compartido (JSON)
  String deviceBlock = "\"dev\":{\"ids\":[\"" + deviceId +
                       "\"],"
                       "\"name\":\"Extractor Inteligente\","
                       "\"mf\":\"DIY\","
                       "\"mdl\":\"ESP32-Extractor\","
                       "\"sw\":\"1.0-mqtt\"}";

  // Helper lambda para publicar sensor discovery
  auto publishSensor = [&](const char *name, const char *uid,
                           const char *valueTpl, const char *unit,
                           const char *devClass, const char *stTopic) {
    String topic =
        "homeassistant/sensor/" + deviceId + "/" + String(uid) + "/config";
    String payload = "{\"name\":\"" + String(name) +
                     "\","
                     "\"uniq_id\":\"" +
                     deviceId + "_" + String(uid) +
                     "\","
                     "\"stat_t\":\"" +
                     String(stTopic) +
                     "\","
                     "\"val_tpl\":\"" +
                     String(valueTpl) +
                     "\","
                     "\"unit_of_meas\":\"" +
                     String(unit) + "\"";
    if (strlen(devClass) > 0) {
      payload += ",\"dev_cla\":\"" + String(devClass) + "\"";
    }
    payload +=
        ",\"avty_t\":\"" + String(MQTT_TOPIC_AVAIL) + "\"," + deviceBlock + "}";
    mqtt.publish(topic.c_str(), payload.c_str(), true);
    delay(50); // Dar tiempo al broker
  };

  // --- SENSORES ---
  publishSensor("Temperatura", "temp", "{{value_json.temp}}", "°C",
                "temperature", MQTT_TOPIC_SENSORS);
  publishSensor("Humedad", "hum", "{{value_json.hum}}", "%", "humidity",
                MQTT_TOPIC_SENSORS);
  publishSensor("Presion", "pressure", "{{value_json.pressure}}", "hPa",
                "pressure", MQTT_TOPIC_SENSORS);
  publishSensor("Calidad Aire", "aqi", "{{value_json.aqi}}", "", "aqi",
                MQTT_TOPIC_SENSORS);
  publishSensor("Punto de Rocio", "dew", "{{value_json.dew}}", "°C",
                "temperature", MQTT_TOPIC_SENSORS);
  publishSensor("RPM Ventilador", "rpm", "{{value_json.rpm}}", "rpm", "",
                MQTT_TOPIC_STATE);
  publishSensor("Velocidad Fan", "fan_pct", "{{value_json.fan_speed}}", "%", "",
                MQTT_TOPIC_STATE);
  publishSensor("Modo", "mode", "{{value_json.mode}}", "", "",
                MQTT_TOPIC_STATE);

  // --- BINARY SENSOR: Ocupado ---
  {
    String topic =
        "homeassistant/binary_sensor/" + deviceId + "/occupied/config";
    String payload = "{\"name\":\"Bano Ocupado\","
                     "\"uniq_id\":\"" +
                     deviceId +
                     "_occupied\","
                     "\"stat_t\":\"" +
                     String(MQTT_TOPIC_STATE) +
                     "\","
                     "\"val_tpl\":\"{{value_json.occupied}}\","
                     "\"pl_on\":\"true\",\"pl_off\":\"false\","
                     "\"dev_cla\":\"occupancy\","
                     "\"avty_t\":\"" +
                     String(MQTT_TOPIC_AVAIL) + "\"," + deviceBlock + "}";
    mqtt.publish(topic.c_str(), payload.c_str(), true);
    delay(50);
  }

  // --- SELECT: Modo ---
  {
    String topic = "homeassistant/select/" + deviceId + "/mode/config";
    String payload = "{\"name\":\"Modo Extractor\","
                     "\"uniq_id\":\"" +
                     deviceId +
                     "_mode_select\","
                     "\"stat_t\":\"" +
                     String(MQTT_TOPIC_STATE) +
                     "\","
                     "\"val_tpl\":\"{{value_json.mode}}\","
                     "\"cmd_t\":\"" +
                     String(MQTT_TOPIC_CMD_MODE) +
                     "\","
                     "\"options\":[\"IDLE\",\"MANUAL_INF\",\"NIGHT\",\"DIAG\"],"
                     "\"avty_t\":\"" +
                     String(MQTT_TOPIC_AVAIL) + "\"," + deviceBlock + "}";
    mqtt.publish(topic.c_str(), payload.c_str(), true);
    delay(50);
  }

  // --- NUMBER: Velocidad manual ---
  {
    String topic = "homeassistant/number/" + deviceId + "/fan_speed/config";
    String payload = "{\"name\":\"Velocidad Ventilador\","
                     "\"uniq_id\":\"" +
                     deviceId +
                     "_fan_speed_num\","
                     "\"stat_t\":\"" +
                     String(MQTT_TOPIC_STATE) +
                     "\","
                     "\"val_tpl\":\"{{value_json.fan_speed}}\","
                     "\"cmd_t\":\"" +
                     String(MQTT_TOPIC_CMD_FAN) +
                     "\","
                     "\"min\":0,\"max\":90,\"step\":5,"
                     "\"unit_of_meas\":\"%\","
                     "\"avty_t\":\"" +
                     String(MQTT_TOPIC_AVAIL) + "\"," + deviceBlock + "}";
    mqtt.publish(topic.c_str(), payload.c_str(), true);
    delay(50);
  }

  Serial.println("MQTT Discovery publicado");
  mqttDiscoverySent = true;
}

// =====================================================
// MQTT: Publicar datos
// =====================================================
void mqttPublishData() {
  if (!mqtt.connected())
    return;

  // Sensores
  JsonDocument sDoc;
  sDoc["temp"] = round(sensors.temp * 10) / 10.0;
  sDoc["hum"] = round(sensors.hum * 10) / 10.0;
  sDoc["pressure"] = round(sensors.pressure * 10) / 10.0;
  sDoc["aqi"] = sensors.aqi_raw;
  sDoc["dew"] = round(sensors.dewPoint * 10) / 10.0;
  String sOut;
  serializeJson(sDoc, sOut);
  mqtt.publish(MQTT_TOPIC_SENSORS, sOut.c_str());

  // Estado
  JsonDocument stDoc;
  stDoc["mode"] = getStateString();
  stDoc["fan_speed"] = sysConfig.fanSpeed;
  stDoc["rpm"] = sensors.fan_rpm;
  stDoc["occupied"] = (currentState != EST_IDLE && currentState != EST_NIGHT);
  stDoc["hour"] = currentHour;
  String stOut;
  serializeJson(stDoc, stOut);
  mqtt.publish(MQTT_TOPIC_STATE, stOut.c_str());
}

// =====================================================
// MQTT: Callback de comandos
// =====================================================
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();
  Serial.print("MQTT cmd: ");
  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(msg);

  String t = String(topic);

  if (t == MQTT_TOPIC_CMD_MODE) {
    if (msg == "IDLE")
      currentState = EST_IDLE;
    else if (msg == "MANUAL_INF")
      currentState = EST_MANUAL_INFINITE;
    else if (msg == "NIGHT")
      currentState = EST_NIGHT;
    else if (msg == "DIAG")
      currentState = EST_DIAGNOSTIC;
    sysConfig.lastInteraction = millis();
    sysConfig.screenOn = true;
  } else if (t == MQTT_TOPIC_CMD_FAN) {
    int val = msg.toInt();
    sysConfig.fanSpeed = constrain(val, 0, MAX_FAN_SPEED);
    setFanSpeedSafe(sysConfig.fanSpeed);
    if (currentState != EST_MANUAL_LIMITED &&
        currentState != EST_MANUAL_INFINITE) {
      currentState = EST_MANUAL_INFINITE;
    }
    sysConfig.lastInteraction = millis();
    sysConfig.screenOn = true;
  }

  // Publicar estado actualizado inmediatamente
  mqttPublishData();
  notifyClients();
}

// =====================================================
// MQTT: Conectar y reconectar
// =====================================================
void mqttReconnect() {
  if (mqtt.connected())
    return;
  if (millis() - lastMqttReconnect < 5000)
    return; // Intentar cada 5s
  lastMqttReconnect = millis();

  String clientId = "extractor-" + WiFi.macAddress();
  Serial.print("MQTT conectando... ");

  bool connected;
  if (strlen(MQTT_USER) > 0) {
    connected = mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASS,
                             MQTT_TOPIC_AVAIL, 1, true, "offline");
  } else {
    connected =
        mqtt.connect(clientId.c_str(), MQTT_TOPIC_AVAIL, 1, true, "offline");
  }

  if (connected) {
    Serial.println("OK!");
    mqtt.publish(MQTT_TOPIC_AVAIL, "online", true);
    mqtt.subscribe(MQTT_TOPIC_CMD_MODE);
    mqtt.subscribe(MQTT_TOPIC_CMD_FAN);
    mqtt.subscribe(MQTT_TOPIC_CMD_NIGHT_START);
    mqtt.subscribe(MQTT_TOPIC_CMD_NIGHT_END);

    if (!mqttDiscoverySent) {
      mqttSendDiscovery();
    }
  } else {
    Serial.print("FAIL rc=");
    Serial.println(mqtt.state());
  }
}

// =====================================================
// WEBSOCKET EVENT HANDLER
// =====================================================
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("WS Client Connected");
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len &&
        info->opcode == WS_TEXT) {
      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, data);
      if (err)
        return;

      if (doc["cmd"].is<String>()) {
        String cmd = doc["cmd"];
        if (cmd == "set_mode") {
          String val = doc["val"];
          if (val == "IDLE")
            currentState = EST_IDLE;
          if (val == "MANUAL")
            currentState = EST_MANUAL_INFINITE;
          if (val == "NIGHT")
            currentState = EST_NIGHT;
          sysConfig.lastInteraction = millis();
          sysConfig.screenOn = true;
        }
        if (cmd == "set_fan") {
          int val = doc["val"];
          sysConfig.fanSpeed = constrain(val, 0, MAX_FAN_SPEED);
          setFanSpeedSafe(sysConfig.fanSpeed);
          if (currentState != EST_MANUAL_LIMITED &&
              currentState != EST_MANUAL_INFINITE)
            currentState = EST_MANUAL_INFINITE;
          sysConfig.lastInteraction = millis();
          sysConfig.screenOn = true;
        }
        notifyClients();
      }
    }
  }
}

// =====================================================
// PANTALLA OLED
// =====================================================
void drawScreen() {
  if (!sysConfig.screenOn) {
    display.clearDisplay();
    display.display();
    return;
  }

  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  // === HEADER ===
  display.setTextSize(1);
  display.setCursor(0, 0);

  // WiFi Symbol (más visible)
  if (wifiConnected) {
    display.print("*"); // Asterisco como WiFi
  } else {
    display.print(" "); // Espacio si no hay WiFi
  }
  display.print(" ");

  // Estado actual EN ESPAÑOL
  switch (currentState) {
  case EST_IDLE:
    display.print("AUTO");
    break;
  case EST_SHOWER:
    display.print("DUCHA");
    break;
  case EST_ODOR:
    display.print("PURGA");
    break;
  case EST_NIGHT:
    display.print("NOCHE");
    break;
  case EST_MANUAL_CONFIG:
    display.print("CONFIG");
    break;
  case EST_MANUAL_LIMITED:
    display.print("MANUAL");
    break;
  case EST_MANUAL_INFINITE:
    display.print("MANUAL");
    display.print((char)236); // Simbolo infinito aproximado
    break;
  case EST_DIAGNOSTIC:
    display.print("DIAG");
    break;
  case EST_ERROR:
    display.print("ERROR");
    break;
  }

  // Fan% derecha
  display.setCursor(85, 0);
  display.print("F:");
  display.print(sysConfig.fanSpeed);
  display.print("%");
  display.drawLine(0, 10, 128, 10, SH110X_WHITE);

  // === BODY ===
  if (currentState == EST_DIAGNOSTIC) {
    display.setCursor(0, 14);
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.print("RSSI: ");
    display.print(WiFi.RSSI());
    display.println(" dBm");
    display.print("AQI: ");
    display.print(sensors.aqi_raw);
    display.print(" Base:");
    display.println((int)aqiBaseline);
    display.print("Hora: ");
    display.print(currentHour);
    display.print(":00");

  } else if (currentState == EST_ERROR) {
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.print("ERROR");
    display.setTextSize(1);
    display.setCursor(0, 45);
    display.print("Revisar ventilador");

  } else if (currentState == EST_SHOWER) {
    display.setTextSize(1);
    display.setCursor(0, 14);
    display.print("EXTRAYENDO VAPOR");
    display.setTextSize(3);
    display.setCursor(30, 28);
    display.print(sysConfig.fanSpeed);
    display.setTextSize(1);
    display.print("%");
    display.setCursor(0, 55);
    display.print("Temp:");
    display.print(sensors.temp, 1);
    display.print(" Hum:");
    display.print(sensors.hum, 0);
    display.print("%");

  } else if (currentState == EST_MANUAL_CONFIG) {
    // === PANTALLA ASISTENTE DE CONFIGURACIÓN ===
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("CONFIG MANUAL   ");
    display.print(manualCfg.configStep + 1);
    display.print("/2");
    display.drawLine(0, 10, 128, 10, SH110X_WHITE);

    if (manualCfg.configStep == 0) {
      // --- PASO 1: TIEMPO ---
      display.setCursor(0, 20);
      display.print("DURACION:");

      display.setTextSize(2);
      display.setCursor(10, 35);

      int t = manualCfg.timeMinutes;
      if (t == -1) {
        display.print("INFINITO"); // O usar simbolo si se prefiere
      } else {
        display.print(t);
        display.print(" min");
      }

    } else {
      // --- PASO 2: VELOCIDAD ---
      display.setCursor(0, 20);
      display.print("VELOCIDAD:");

      display.setTextSize(2);
      display.setCursor(30, 35);
      display.print(manualCfg.speedPercent);
      display.print(" %");

      // Barra indicadora
      display.drawRect(10, 55, 108, 6, SH110X_WHITE);
      display.fillRect(12, 57, manualCfg.speedPercent, 2, SH110X_WHITE);
    }

    // Footer
    // display.setTextSize(1);
    // display.setCursor(0, 55);
    // display.print("[OK]=Siguiente");

  } else if (currentState == EST_MANUAL_LIMITED) {
    // === PANTALLA MANUAL LIMITADO ===
    unsigned long elapsed = millis() - manualCfg.startTime;
    unsigned long totalMs = (unsigned long)manualCfg.timeMinutes * 60000;
    unsigned long remaining = (elapsed < totalMs) ? (totalMs - elapsed) : 0;
    int mins = remaining / 60000;
    int secs = (remaining % 60000) / 1000;

    display.setTextSize(1);
    display.setCursor(0, 14);
    display.print("Tiempo restante:");

    display.setTextSize(3);
    display.setCursor(20, 26);
    if (mins < 10)
      display.print("0");
    display.print(mins);
    display.print(":");
    if (secs < 10)
      display.print("0");
    display.print(secs);

    display.setTextSize(1);
    display.setCursor(0, 55);
    display.print("V:");
    display.print(manualCfg.speedPercent);
    display.print("%  [Back=salir]");

  } else if (currentState == EST_MANUAL_INFINITE) {
    // === PANTALLA MANUAL INFINITO ===
    display.setTextSize(1);
    display.setCursor(0, 14);
    display.print("FUNCIONANDO...");

    display.setTextSize(2);
    display.setCursor(0, 26);
    display.print(sensors.temp, 1);
    display.print("C ");
    display.print(sensors.hum, 0);
    display.print("%");

    display.setTextSize(1);
    display.setCursor(0, 45);
    display.print("AQI:");
    display.print(sensors.aqi_raw);
    display.print(sensors.aqi_raw < 1500
                      ? " OK"
                      : (sensors.aqi_raw < 2500 ? " MED" : " MAL"));

    display.setCursor(0, 55);
    display.print("V:");
    display.print(manualCfg.speedPercent);
    display.print("%  [Back=salir]");

  } else {
    // === PANTALLA PRINCIPAL ===
    // Fila 1: TEMP grande + HUM grande
    display.setTextSize(2);
    display.setCursor(0, 12);
    display.print(sensors.temp, 1);
    display.setTextSize(1);
    display.print("C");

    display.setTextSize(2);
    display.setCursor(75, 12);
    display.print(sensors.hum, 0);
    display.setTextSize(1);
    display.print("%");

    // Fila 2: AQI grande centrado
    display.setTextSize(2);
    display.setCursor(0, 35);
    display.print("AQI:");
    int aqiVal = sensors.aqi_raw;
    display.print(aqiVal);

    // Indicador calidad
    display.setTextSize(1);
    display.setCursor(95, 40);
    if (aqiVal < 1500) {
      display.print("OK");
    } else if (aqiVal < 2500) {
      display.print("MED");
    } else {
      display.print("MAL");
    }

    // Fila 3: Info extra pequeña
    display.setCursor(0, 55);
    display.print("P:");
    display.print(sensors.pressure, 0);
    display.print("hPa");
  }

  display.display();
}

// =====================================================
// CALLBACKS DE BOTONES
// =====================================================
void onOkClick() {
  sysConfig.lastInteraction = millis();
  sysConfig.screenOn = true;

  // Simple Software Debounce adicional
  static unsigned long lastOkTime = 0;
  if (millis() - lastOkTime < 500)
    return; // Ignorar clicks muy seguidos
  lastOkTime = millis();

  if (currentState == EST_MANUAL_CONFIG) {
    // --- ESTAMOS CONFIGURANDO ---
    if (manualCfg.configStep == 0) {
      // Confirmar Tiempo -> Pasar a Velocidad
      manualCfg.configStep = 1;

      // Reset encoder para velocidad
      // 60% / 5 = 12 steps
      manualCfg.speedPercent = 60;
      encoder.setCount(12);

    } else if (manualCfg.configStep == 1) {
      // Confirmar Velocidad -> ARRANCAR
      manualCfg.startTime = millis();

      // Aplicar velocidad inicial
      setFanSpeedSafe(manualCfg.speedPercent);

      // Sync encoder con velocidad actual
      encoder.setCount(manualCfg.speedPercent / 5);

      currentState = EST_MANUAL_LIMITED;
    }
  } else if (currentState == EST_MANUAL_LIMITED) {
    // YA EN MANUAL: Click no hace nada salvo encender pantalla
  } else {
    // EN AUTO: Click NO entra a config (lo hace el giro), solo enciende
    // pantalla
  }
}

void onBackClick() {
  sysConfig.lastInteraction = millis();
  sysConfig.screenOn = true;

  if (currentState == EST_MANUAL_CONFIG) { // Retroceder paso
    if (manualCfg.configStep > 0) {
      manualCfg.configStep--;
      // Restaurar encoder al valor correcto del paso anterior
      if (manualCfg.configStep == 0) {
        // Volver a Tiempo
        encoder.setCount(manualCfg.timeSelectionIndex);
      }
    } else {
      // Salir de config
      currentState = previousState; // O EST_IDLE
      if (currentState == EST_MANUAL_CONFIG)
        currentState = EST_IDLE;
      setFanSpeedSafe(IDLE_SPEED); // O restaurar anterior
    }
  } else if (currentState == EST_MANUAL_LIMITED) {
    // Si estamos en manual, BACK sale a AUTO
    currentState = EST_IDLE;
    setFanSpeedSafe(IDLE_SPEED);
  }
}

void onOkMultiClick() {
  sysConfig.lastInteraction = millis();
  sysConfig.screenOn = true;
  currentState = EST_DIAGNOSTIC;
}

// =====================================================
// WIFI EVENT HANDLER
// =====================================================
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
  case SYSTEM_EVENT_STA_GOT_IP:
    wifiConnected = true;
    wifiStatusMsg = WiFi.localIP().toString();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    wifiConnected = false;
    break;
  default:
    break;
  }
}

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(100000);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  // LEDs de Puerta (indicadores externos)
  pinMode(PIN_LED_DOOR_RED, OUTPUT);
  pinMode(PIN_LED_DOOR_GREEN, OUTPUT);
  digitalWrite(PIN_LED_DOOR_RED, LOW); // Apagado por defecto
  digitalWrite(PIN_LED_DOOR_GREEN,
               LOW); // Apagado por defecto
                     // CRÍTICO: Inicializar relé PRIMERO para fail-safe
  // Ventilador debe estar APAGADO al arrancar
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH); // Relé APAGADO (Active LOW: HIGH=OFF, LOW=ON)

  ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQ, FAN_PWM_RES);
  ledcAttachPin(PIN_FAN_PWM, FAN_PWM_CHANNEL);
  ledcWrite(FAN_PWM_CHANNEL,
            255); // PWM 255 -> 0% Duty (Inverted) -> Fan STOP/Min
  pinMode(PIN_FAN_TACH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_FAN_TACH), tachISR, FALLING);

  // Intentar iniciar pantalla (si falla, no bloquear)
  // SH1106 Init
  if (!display.begin(OLED_ADDR, true)) {
    Serial.println("OLED Failed");
  } else {
    display.display();
    delay(200);
    display.clearDisplay();
  }

  if (!aht.begin())
    Serial.println("AHT20 fail");
  if (!bmp.begin())
    Serial.println("BMP280 fail");

  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(PIN_ENC_DT, PIN_ENC_CLK);
  encoder.setCount(IDLE_SPEED);

  btnOk.attachClick(onOkClick);
  btnOk.attachMultiClick(onOkMultiClick);
  btnOk.setClickMs(250);
  btnBack.attachClick(onBackClick);

  // WiFi
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(200);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    wifiStatusMsg = WiFi.localIP().toString();
    Serial.println("\nWiFi OK! IP: " + wifiStatusMsg);

    // Configurar NTP
    configTime(3600, 3600, "pool.ntp.org"); // GMT+1 con DST
  }

  // MQTT Setup
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(1024); // Para los payloads de discovery

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["t"] = sensors.temp;
    doc["h"] = sensors.hum;
    doc["p"] = sensors.pressure;
    doc["q"] = sensors.aqi_raw;
    doc["td"] = sensors.dewPoint;
    String r;
    serializeJson(doc, r);
    request->send(200, "application/json", r);
  });
  server.begin();

  // Velocidad inicial
  setFanSpeedSafe(0); // Asegurar todo apagado

  /*
  // === SECUENCIA DE TEST DE ARRANQUE (DESACTIVADA) ===
  // Evitar ruido al arrancar. El usuario ya sabe que funciona.
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("INICIANDO...");
  display.display();
  delay(1000);
  */

  // 3. INICIO NORMAL
  setFanSpeedSafe(IDLE_SPEED);
}

// =====================================================
// LOOP
// =====================================================
void loop() {
  btnOk.tick();
  btnBack.tick();

  sysConfig.screenOn = true; // Pantalla siempre ON

  // Encoder handling
  if (currentState == EST_MANUAL_CONFIG) {
    long encVal = encoder.getCount();

    if (manualCfg.configStep == 0) {
      // PASO 0: TIEMPO
      encVal = constrain(encVal, 0, manualTimeOptionsCount - 1);
      encoder.setCount(encVal);
      manualCfg.timeSelectionIndex = (int)encVal;
      manualCfg.timeMinutes = manualTimeOptions[manualCfg.timeSelectionIndex];

    } else if (manualCfg.configStep == 1) {
      // PASO 1: VELOCIDAD
      encVal = constrain(encVal, 4, 18); // 4*5=20% a 18*5=90%
      encoder.setCount(encVal);
      manualCfg.speedPercent = encVal * 5;
    }
    sysConfig.lastInteraction = millis();

  } else if (currentState == EST_MANUAL_LIMITED) {
    // EN EJECUCIÓN MANUAL: OVERRIDE DE VELOCIDAD
    long encVal = encoder.getCount();
    encVal = constrain(encVal, 4, 18);
    encoder.setCount(encVal);

    int newSpeed = encVal * 5;
    if (newSpeed != manualCfg.speedPercent) {
      manualCfg.speedPercent = newSpeed;
      setFanSpeedSafe(manualCfg.speedPercent);
      sysConfig.lastInteraction = millis();
    }

  } else {
    // ESTADO AUTO/IDLE: Detectar giro del encoder para entrar a CONFIG
    static bool encInitialized = false;
    static long lastEncVal = 0;
    long encVal = encoder.getCount();

    // Primera iteración: capturar baseline sin disparar
    if (!encInitialized) {
      lastEncVal = encVal;
      encInitialized = true;
    }
    // Detectar giro real (cambio >= 2 pasos)
    else if (abs(encVal - lastEncVal) >= 2) {
      currentState = EST_MANUAL_CONFIG;
      manualCfg.configStep = 0;
      manualCfg.timeSelectionIndex = 1;
      manualCfg.timeMinutes = 30;
      encoder.setCount(1);
      sysConfig.lastInteraction = millis();
      encInitialized = false; // Reset para próxima vez
    }
  }

  // Muestreo de sensores cada 2 segundos
  if (millis() - lastSensorSample > SENSOR_SAMPLE_MS) {
    updateSensors();
    lastSensorSample = millis();
  }

  // WiFi Reconnection
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 5000) {
    lastWiFiCheck = millis();
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      if (!ntpSynced)
        updateNTPTime();
    } else {
      wifiConnected = false;
      WiFi.reconnect();
    }
  }

  // Actualizar FSM (solo si no esta en modo manual/diagnostico)
  if (currentState != EST_MANUAL_CONFIG && currentState != EST_MANUAL_LIMITED &&
      currentState != EST_DIAGNOSTIC) {
    updateStateMachine();
  }

  // Aplicar velocidad objetivo
  if (currentState != EST_MANUAL_LIMITED) {
    setFanSpeedSafe(getTargetSpeed());
  }

  // Protección stall
  checkStallProtection();

  // MQTT
  if (wifiConnected) {
    if (!mqtt.connected()) {
      mqttReconnect();
    }
    mqtt.loop();

    // Publicar datos MQTT cada 5 segundos
    if (mqtt.connected() && millis() - lastMqttPublish > 5000) {
      mqttPublishData();
      lastMqttPublish = millis();
    }
  }

  // WebSocket updates
  if (wifiConnected && millis() - lastWsUpdate > 500) {
    notifyClients();
    lastWsUpdate = millis();
  }

  updateLeds();

  // Throttle display updates to 5Hz to reduce I2C bus usage
  if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL_MS) {
    drawScreen();
    lastDisplayUpdate = millis();
  }

  ws.cleanupClients();
  delay(20); // Pequeño retardo para ceder tiempo al WiFi
}
