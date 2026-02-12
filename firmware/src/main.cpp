#include "Config.h"
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <OneButton.h>
#include <Wire.h>
#include <time.h>

// --- WIFI & WEB ---
#include "Secrets.h"
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
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
Adafruit_SH1106G display =
    Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
ESP32Encoder encoder;
OneButton btnOk(PIN_ENC_SW, true);
OneButton btnBack(PIN_BTN_BACK, true);

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

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

  // CONTROL DEL RELÉ (Lógica Invertida: LOW = ON, HIGH = OFF)
  // Si velocidad > 0 -> ACTIVAR RELÉ (LOW)
  // Si velocidad == 0 -> DESACTIVAR RELÉ (HIGH)
  digitalWrite(PIN_RELAY, (percent > 0) ? LOW : HIGH);

  int duty = map(percent, 0, 100, 0, 255);
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

  // PIN_LED_GREEN tiene lógica invertida (LOW = ON)
  digitalWrite(PIN_LED_RED, red ? HIGH : LOW);
  digitalWrite(PIN_LED_YELLOW, yellow ? HIGH : LOW);
  digitalWrite(PIN_LED_GREEN, green ? LOW : HIGH);

  // LEDs de Puerta (indicadores de ocupación)
  // Rojo = Ocupado (baño en uso), Verde = Libre
  bool occupied = (currentState != EST_IDLE);
  digitalWrite(PIN_LED_DOOR_RED, occupied ? HIGH : LOW);
  digitalWrite(PIN_LED_DOOR_GREEN, occupied ? LOW : HIGH);
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

  String stateStr = "IDLE";
  switch (currentState) {
  case EST_IDLE:
    stateStr = "IDLE";
    break;
  case EST_SHOWER:
    stateStr = "SHOWER";
    break;
  case EST_ODOR:
    stateStr = "ODOR";
    break;
  case EST_NIGHT:
    stateStr = "NIGHT";
    break;
  case EST_MANUAL_CONFIG:
    stateStr = "CONFIG";
    break;
  case EST_MANUAL_LIMITED:
    stateStr = "MANUAL_LIM";
    break;
  case EST_MANUAL_INFINITE:
    stateStr = "MANUAL_INF";
    break;
  case EST_DIAGNOSTIC:
    stateStr = "DIAG";
    break;
  case EST_ERROR:
    stateStr = "ERROR";
    break;
  }
  doc["mode"] = stateStr;
  doc["hour"] = currentHour;

  String output;
  serializeJson(doc, output);
  ws.textAll(output);
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
  digitalWrite(PIN_RELAY,
               HIGH); // Relé desactivado (Logic Inversa: HIGH=OFF)

  ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQ, FAN_PWM_RES);
  ledcAttachPin(PIN_FAN_PWM, FAN_PWM_CHANNEL);
  ledcWrite(FAN_PWM_CHANNEL, 0); // PWM a 0 → MOSFET cerrado
  pinMode(PIN_FAN_TACH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_FAN_TACH), tachISR, FALLING);

  display.begin(OLED_ADDR, true);
  display.display();
  delay(200);
  display.clearDisplay();

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
  setFanSpeedSafe(0); // Primero todo apagado

  // === SECUENCIA DE TEST DE ARRANQUE ===
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("TEST HARDWARE...");
  display.display();
  delay(1000);

  // 1. TEST RELÉ (Active LOW: LOW=ON)
  display.setCursor(0, 15);
  display.print("1. RELE ON (CLICK?)");
  display.display();
  digitalWrite(PIN_RELAY, LOW); // ON (Active Low)
  delay(2000);
  digitalWrite(PIN_RELAY, HIGH); // OFF (Active Low)
  display.print(" OK");
  display.display();
  delay(1000);

  // 2. TEST VENTILADOR
  display.setCursor(0, 30);
  display.print("2. FAN MAX (3s)");
  display.display();
  digitalWrite(PIN_RELAY, LOW);    // Energía ON (Active Low)
  ledcWrite(FAN_PWM_CHANNEL, 255); // PWM MAX 100%
  delay(3000);
  ledcWrite(FAN_PWM_CHANNEL, 0); // PWM OFF
  digitalWrite(PIN_RELAY, HIGH); // Energía OFF

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

  // WebSocket updates
  if (wifiConnected && millis() - lastWsUpdate > 500) {
    notifyClients();
    lastWsUpdate = millis();
  }

  updateLeds();
  // Limit display updates to 5Hz to prevent blocking
  if (millis() - lastDisplayUpdate > 200) {
    lastDisplayUpdate = millis();
    drawScreen();
  }

  ws.cleanupClients();
  delay(20); // Pequeño retardo para ceder tiempo al WiFi
}
