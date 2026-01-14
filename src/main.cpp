#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <ESP32Encoder.h>
#include <Arduino.h>
#include <esp_task_wdt.h>

// -------------------------------------------------------------------------
// --- CONFIGURACIÓN DE PINES (FINAL v7.1C - ESP32 30 PINES) ---
// -------------------------------------------------------------------------

// I2C (OLED + Sensores)
#define I2C_SDA_PIN       21
#define I2C_SCL_PIN       22

// Encoder Rotativo
#define ENCODER_CLK_PIN   32
#define ENCODER_DT_PIN    33
#define ENCODER_SW_PIN    27  // Botón del eje (OK/Seleccionar)

// Botones Físicos Extra
#define CONFIRM_BUTTON_PIN 25 // Botón Lateral 1 (BACK/Cancelar)
#define BAK_BUTTON_PIN    13  // Botón Lateral 2 (PAUSA/Emergencia) - MOVIDO AL 13 (Pin 26 dañado)

// Actuadores
// El control de potencia se realiza con un MOSFET, pilotado por la salida PWM
// `PWM_FAN_PIN` (no se usa relé físico).
#define PWM_FAN_PIN       19  // Control de velocidad (PWM al ventilador / gate MOSFET)
#define FAN_TACH_PIN      16  // Lectura de RPM (Cable Amarillo) - PREPARADO PARA FUTURO

// Sensores Analógicos
#define MQ135_ANALOG_PIN  34  // Entrada analógica (Input Only)

// LEDs de Estado
#define LED_RED_PIN       18  // Error / Standby
#define LED_YELLOW_PIN    12  // Advertencia / Modo Manual - GPIO 12 (seguro, no interfiere FLASH)
#define LED_GREEN_PIN     17  // Funcionamiento Normal

// -------------------------------------------------------------------------
// --- CONFIGURACIÓN DEL SISTEMA ---
// -------------------------------------------------------------------------
#define PWM_FREQ          25000 // 25kHz (Estándar ventiladores PC/Industriales)
#define PWM_RES           8     // 8 bits (0-255)
#define PWM_CHANNEL       0

#define SCREEN_WIDTH      128
#define SCREEN_HEIGHT     64
#define OLED_RESET        -1
#define SCREEN_ADDRESS    0x3C

// Umbrales Automáticos (Histéresis simple)
#define HUMIDITY_HIGH     70.0  // 100% velocidad
#define HUMIDITY_MED      60.0  // 70% velocidad
#define TEMP_HIGH         30.0  // 60% velocidad
#define AIR_BAD           600   // 40% velocidad (Valor RAW analógico)

// Tiempos
#define DEBOUNCE_DELAY    50    // ms (más corto, debounce por muestreo)
#define PAUSE_HOLD_TIME   2000  // ms para activar pausa
#define PWM_RAMP_STEP     5     // ms entre pasos de rampa PWM (suave)
#define PWM_RAMP_DELTA    10    // cambio máximo de PWM por paso
#define OLED_TIMEOUT      300000 // ms (5 min) antes de apagar pantalla
#define MQ135_WARMUP_TIME 30000 // ms (30 seg) precalentamiento mínimo
#define I2C_RETRY_TIMES   3     // reintentos en lectura I2C
#define SENSOR_READ_INTERVAL 2000 // ms entre lecturas de sensores

// -------------------------------------------------------------------------
// --- OBJETOS GLOBALES ---
// -------------------------------------------------------------------------
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
ESP32Encoder encoder;

// -------------------------------------------------------------------------
// --- VARIABLES DE ESTADO ---
// -------------------------------------------------------------------------
enum SystemMode { MODE_AUTO, MODE_MANUAL_SETUP, MODE_MANUAL_RUN, MODE_MANUAL_INFINITE, MODE_PAUSE, MODE_ERROR };
SystemMode currentMode = MODE_AUTO;
enum SensorState { SENSOR_OK, SENSOR_DEGRADED, SENSOR_FAILED };
SensorState sensorState = SENSOR_OK;

// Variables de Control
int targetSpeed = 0;           // 0-255 (destino)
int currentSpeed = 0;          // 0-255 (actual - con rampa)
unsigned long fanTimerStart = 0;
unsigned long timerDuration = 0; // En milisegundos
bool fanRunning = false;
unsigned long lastPwmRampTime = 0; // Control de rampa PWM
bool infiniteManualMode = false; // Modo manual sin límite de tiempo

// Variables Manual Setup
int menuStep = 0; // 0: Tiempo, 1: Velocidad, 2: Modo (Limitado/Infinito), 3: Modo Noche, 4: Confirmar
int manualTimeSel = 30; // Minutos
int manualSpeedSel = 50; // %
bool manualInfiniteSelected = false; // Selección de modo infinito
bool manualNightModeSelected = false; // Selección de modo noche

// Variables Botones/Encoder (con debounce por muestreo)
long oldEncPos = 0;
unsigned long lastButtonPress = 0;
unsigned long bakButtonPressStart = 0;
bool bakButtonHeld = false;
int encoderSwitchSamples = 0;      // Muestreo: contador de muestras LOW
int confirmButtonSamples = 0;
int bakButtonSamples = 0;
const int DEBOUNCE_SAMPLES = 3;    // Necesarias 3 muestras para considerar pulsado

// Variables Sensores
float hum = 0, temp = 0;           // AHT20
float temp_bmp = 0;                // BMP280 (redundancia)
int airQuality = 0;                // MQ135
unsigned long lastSensorRead = 0;
int sensorFailCount = 0;
const int MAX_SENSOR_FAILS = 3;
unsigned long setupTime = 0;        // Para MQ135 warmup
bool mq135_warmed = false;
int mq135_baseline = 400;           // Baseline inicial (aire relativo limpio)
unsigned long oledLastActivity = 0; // Timeout OLED
bool oledOn = true;                 // Estado OLED

// Home Assistant Integration (futuro)
bool nightModeEnabled = false;      // Modo noche: activable desde HA

// -------------------------------------------------------------------------
// --- PROTOTIPOS DE FUNCIONES (CORRECCIÓN IMPORTANTE) ---
// -------------------------------------------------------------------------
void updateLEDs();
void fatalError(String msg);
void setFanSpeed(int speedPWM);
void readSensors();
void checkButtons();
void runAutoLogic();
void runManualLogic();
void runManualSetup();
void updateFanSpeedRamp();
void drawAutoScreen();
void drawManualSetupScreen();
void drawManualRunScreen();
void drawManualInfiniteScreen();
void drawPauseScreen();

// -------------------------------------------------------------------------
// --- SETUP ---
// -------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  unsigned long setupStartTime = millis();
  while (!Serial && (millis() - setupStartTime) < 500); // Espera rápida (sin delay bloqueante)
  Serial.println("\n--- INICIANDO EXTRACTOR INTELIGENTE v7.1C REFACTORIZADO ---");

  // 1. Configurar Pines
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  pinMode(CONFIRM_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BAK_BUTTON_PIN, INPUT_PULLUP);

  // 2. Configurar PWM
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWM_FAN_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0);

  // 3. Inicializar I2C y Pantalla
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("ERROR: OLED no detectada");
    sensorState = SENSOR_FAILED;
    currentMode = MODE_ERROR;
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(10, 20);
    display.println("INICIANDO...");
    display.display();
    oledOn = true;
    oledLastActivity = millis();
  }

  // 4. Inicializar Sensores (con reintentos)
  bool aht_ok = false, bmp_ok = false;
  for (int i = 0; i < I2C_RETRY_TIMES; i++) {
    if (!aht_ok && aht.begin()) {
      aht_ok = true;
      Serial.println("AHT20 OK");
    }
    if (!bmp_ok && bmp.begin(0x77)) {
      bmp_ok = true;
      Serial.println("BMP280 OK");
    }
    if (aht_ok && bmp_ok) break;
  }
  
  if (!aht_ok) {
    Serial.println("ERROR: AHT20 no disponible");
    sensorState = SENSOR_DEGRADED;
  }
  if (!bmp_ok) {
    Serial.println("WARNING: BMP280 no disponible (usar AHT20)");
  }
  if (!aht_ok && !bmp_ok) {
    sensorState = SENSOR_FAILED;
    currentMode = MODE_ERROR;
  }

  // 5. Configurar Encoder
  encoder.attachHalfQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.setCount(0);

  // 6. Inicializar MQ135 (comenzar precalentamiento)
  setupTime = millis();
  mq135_warmed = false;
  Serial.println("MQ135 en precalentamiento (30 seg)...");

  // 7. Watchdog Timer (8 segundos)
  esp_task_wdt_init(8, true);
  esp_task_wdt_add(NULL);

  Serial.println("Sistema Listo.");
  updateLEDs();
}

// -------------------------------------------------------------------------
// --- LOOP PRINCIPAL ---
// -------------------------------------------------------------------------
void loop() {
  esp_task_wdt_reset(); // Alimentar al perro guardián
  readSensors();
  checkButtons();
  updateFanSpeedRamp(); // Ejecutar rampa PWM no-bloqueante
  
  // Timeout OLED (apagar después de 5 minutos inactividad)
  // EXCEPCIÓN: En modo INFINITO, mantener OLED encendida siempre
  // EXCEPCIÓN: En modo NOCHE, mantener OLED encendida también
  bool keepOledOn = (currentMode == MODE_MANUAL_INFINITE) || nightModeEnabled;
  
  if (!keepOledOn && oledOn && (millis() - oledLastActivity > OLED_TIMEOUT * 1000UL)) {
    display.ssd1306_command(0xAE); // Apagar OLED
    oledOn = false;
    Serial.println("OLED off (timeout)");
  }
  
  // Reactivar OLED si está apagado (checkButtons ya lo marca)
  if (!oledOn && (millis() - oledLastActivity < OLED_TIMEOUT * 1000UL)) {
    display.ssd1306_command(0xAF); // Encender OLED
    oledOn = true;
    Serial.println("OLED on");
  }
  
  // Parpadeo LED rojo en modo ERROR (no bloqueante)
  if (currentMode == MODE_ERROR) {
    static unsigned long lastErrorBlink = 0;
    if (millis() - lastErrorBlink > 500) {
      digitalWrite(LED_RED_PIN, !digitalRead(LED_RED_PIN));
      lastErrorBlink = millis();
    }
  }
  
  switch (currentMode) {
    case MODE_AUTO:
      runAutoLogic();
      drawAutoScreen();
      break;

    case MODE_MANUAL_SETUP:
      runManualSetup();
      drawManualSetupScreen();
      break;

    case MODE_MANUAL_RUN:
      runManualLogic();
      drawManualRunScreen();
      break;

    case MODE_MANUAL_INFINITE: {
      // Modo manual sin límite de tiempo
      if (millis() - fanTimerStart < 300000UL) { // Reset timer cada 5 min (evitar overflow)
        fanTimerStart = millis();
      }
      int pwmVal = map(manualSpeedSel, 0, 100, 0, 255);
      setFanSpeed(pwmVal);
      drawManualInfiniteScreen();
      break;
    }

    case MODE_PAUSE:
      // Ventilador apagado, esperar reanudar
      setFanSpeed(0);
      drawPauseScreen();
      break;
      
    case MODE_ERROR:
      setFanSpeed(0); // Apagar completamente en error
      // fatalError() ya muestra LED rojo
      if (oledOn) {
        display.display(); // Mantener error visible
      }
      break;
  }
  
  updateLEDs();
  // SIN delay() bloqueante: usar timers no-bloqueantes en funciones específicas
}

// -------------------------------------------------------------------------
// --- FUNCIONES DE LÓGICA ---
// -------------------------------------------------------------------------

void setFanSpeed(int speedPWM) {
  // speedPWM: 0-255 (se aplica rampa suave)
  targetSpeed = speedPWM;
  if (speedPWM > 0) {
    if (!fanRunning) fanRunning = true;
  } else {
    fanRunning = false;
  }
  // La rampa se ejecuta en updateFanSpeedRamp() desde loop()
}

void updateFanSpeedRamp() {
  // No-bloqueante: ejecutar desde loop() para rampa PWM suave
  if (millis() - lastPwmRampTime < PWM_RAMP_STEP) {
    return; // Esperar siguiente paso de rampa
  }
  lastPwmRampTime = millis();

  if (currentSpeed < targetSpeed) {
    int delta = targetSpeed - currentSpeed;
    if (delta > PWM_RAMP_DELTA) {
      currentSpeed += PWM_RAMP_DELTA;
    } else {
      currentSpeed = targetSpeed;
    }
  } else if (currentSpeed > targetSpeed) {
    int delta = currentSpeed - targetSpeed;
    if (delta > PWM_RAMP_DELTA) {
      currentSpeed -= PWM_RAMP_DELTA;
    } else {
      currentSpeed = targetSpeed;
    }
  }
  
  ledcWrite(PWM_CHANNEL, currentSpeed);
}

void readSensors() {
  if (millis() - lastSensorRead > 2000) { // Leer cada 2 segundos
    lastSensorRead = millis();
    oledLastActivity = millis(); // Resetear timeout OLED
    
    // 1. Leer AHT20 con reintentos I2C
    sensors_event_t h, t;
    bool aht_ok = false;
    for (int i = 0; i < I2C_RETRY_TIMES; i++) {
      if (aht.getEvent(&h, &t)) {
        aht_ok = true;
        break;
      }
    }

    if (aht_ok && !isnan(h.relative_humidity) && !isnan(t.temperature)) {
      hum = h.relative_humidity;
      temp = t.temperature;
      sensorFailCount = 0;
      sensorState = SENSOR_OK;
    } else {
      // AHT20 falló, intentar BMP280 como redundancia
      bool bmp_ok = false;
      for (int i = 0; i < I2C_RETRY_TIMES; i++) {
        if (bmp.readTemperature() != 0) {
          bmp_ok = true;
          break;
        }
      }
      
      if (bmp_ok) {
        temp_bmp = bmp.readTemperature();
        temp = temp_bmp; // Usar BMP280 como fallback
        sensorState = SENSOR_DEGRADED;
        Serial.println("BMP280 redundancia activa (AHT20 falló)");
      } else {
        sensorFailCount++;
        if (sensorFailCount >= MAX_SENSOR_FAILS) {
          sensorState = SENSOR_FAILED;
          currentMode = MODE_ERROR;
        }
      }
    }

    // Si fue error y se recupera, volver a AUTO
    if (currentMode == MODE_ERROR && sensorState != SENSOR_FAILED) {
      currentMode = MODE_AUTO;
    }
    
    // 2. MQ135 - con precalentamiento
    unsigned long warmupElapsed = millis() - setupTime;
    if (warmupElapsed >= MQ135_WARMUP_TIME * 1000) {
      mq135_warmed = true;
      if (mq135_baseline == 400) { // Primera lectura después de warmup
        int raw = analogRead(MQ135_ANALOG_PIN);
        mq135_baseline = raw;
        Serial.print("MQ135 baseline: ");
        Serial.println(mq135_baseline);
      }
    }
    
    if (mq135_warmed) {
      int raw = analogRead(MQ135_ANALOG_PIN);
      airQuality = (airQuality * 0.8) + (raw * 0.2); // Media móvil
    } else {
      airQuality = mq135_baseline; // Mientras precalienta, usar baseline
    }
  }
}

void runAutoLogic() {
  int newTarget = 0;

  // Lógica de Prioridades
  if (hum >= HUMIDITY_HIGH) {
    newTarget = 255; // 100%
  } else if (hum >= HUMIDITY_MED) {
    newTarget = 180; // ~70%
  } else if (temp >= TEMP_HIGH) {
    newTarget = 150; // ~60%
  } else if (airQuality >= AIR_BAD) {
    newTarget = 100; // ~40%
  } else {
    newTarget = 0; // Apagado
  }

  // Histéresis básica para evitar encendidos/apagados rápidos
  if (abs(newTarget - currentSpeed) > 10) { // Solo cambiar si la diferencia es significativa
     setFanSpeed(newTarget);
  } else if (newTarget == 0 && currentSpeed != 0) {
     setFanSpeed(0); // Forzar apagado si el target es 0
  }
}

void runManualSetup() {
  // Esta función estaba vacía/no definida, solo es lógica de UI
  // Realmente la lógica está en checkButtons(), así que aquí no hace falta nada
  // Pero la definimos vacía para que compile.
}

void runManualLogic() {
  // Verificar temporizador
  if (millis() - fanTimerStart >= timerDuration) { // USAR fanTimerStart
    // Tiempo agotado
    currentMode = MODE_AUTO;
    setFanSpeed(0);
  } else {
    // Mantener velocidad seleccionada
    int pwmVal = map(manualSpeedSel, 0, 100, 0, 255);
    setFanSpeed(pwmVal);
  }
}

// -------------------------------------------------------------------------
// --- INTERFAZ Y ENTRADAS ---
// -------------------------------------------------------------------------

void checkButtons() {
  // 1. Encoder Rotativo (Navegación)
  long encVal = encoder.getCount() / 2;
  if (encVal != oldEncPos) {
    if (currentMode == MODE_MANUAL_SETUP) {
      if (menuStep == 0) { // Tiempo
        if (encVal > oldEncPos) manualTimeSel += 15; else manualTimeSel -= 15;
        if (manualTimeSel < 15) manualTimeSel = 15;
        if (manualTimeSel > 120) manualTimeSel = 120;
      } else if (menuStep == 1) { // Velocidad
        if (encVal > oldEncPos) manualSpeedSel += 25; else manualSpeedSel -= 25;
        if (manualSpeedSel < 25) manualSpeedSel = 25;
        if (manualSpeedSel > 100) manualSpeedSel = 100;
      } else if (menuStep == 2) { // Seleccionar modo (Limitado vs Infinito)
        if (encVal > oldEncPos) manualInfiniteSelected = true;
        else manualInfiniteSelected = false;
      } else if (menuStep == 3) { // Seleccionar modo noche
        if (encVal > oldEncPos) manualNightModeSelected = true;
        else manualNightModeSelected = false;
      }
    } else if (currentMode == MODE_AUTO) {
       // Si giramos en AUTO, entramos a MANUAL SETUP
       currentMode = MODE_MANUAL_SETUP;
       menuStep = 0;
       manualTimeSel = 30;
    }
    oldEncPos = encVal;
    oledLastActivity = millis(); // Resetear timeout
  }

  // 2. Encoder Switch (OK / Next) - Debounce con máquina de estados
  if (digitalRead(ENCODER_SW_PIN) == LOW) {
    encoderSwitchSamples++;
    if (encoderSwitchSamples >= DEBOUNCE_SAMPLES) {
      // Botón confirmado presionado
      if (currentMode == MODE_MANUAL_SETUP) {
        menuStep++;
        if (menuStep > 3) { // Después del paso 3 (modo noche), confirmar
          infiniteManualMode = manualInfiniteSelected;
          nightModeEnabled = manualNightModeSelected;
          if (infiniteManualMode) {
            currentMode = MODE_MANUAL_INFINITE;
          } else {
            currentMode = MODE_MANUAL_RUN;
            timerDuration = manualTimeSel * 60000UL;
            fanTimerStart = millis();
          }
          menuStep = 0;
        }
      }
      encoderSwitchSamples = 0;
    }
  } else {
    encoderSwitchSamples = 0;
  }

  // 3. Confirm Button (BACK / Cancelar) - Debounce estado
  if (digitalRead(CONFIRM_BUTTON_PIN) == LOW) {
    confirmButtonSamples++;
    if (confirmButtonSamples >= DEBOUNCE_SAMPLES) {
      // Botón confirmado presionado
      currentMode = MODE_AUTO;
      menuStep = 0;
      manualNightModeSelected = false; // Resetear selección de modo noche
      confirmButtonSamples = 0;
    }
  } else {
    confirmButtonSamples = 0;
  }

  // 4. BAK Button (PAUSA - Mantener) - Debounce estado
  if (digitalRead(BAK_BUTTON_PIN) == LOW) {
    bakButtonSamples++;
    if (bakButtonSamples >= DEBOUNCE_SAMPLES) {
      // Botón confirmado presionado
      if (!bakButtonHeld) {
        bakButtonPressStart = millis();
        bakButtonHeld = true;
      } else {
        if (millis() - bakButtonPressStart > PAUSE_HOLD_TIME) {
          // Toggle Pausa
          if (currentMode == MODE_PAUSE) {
            currentMode = MODE_AUTO;
          } else {
            currentMode = MODE_PAUSE;
          }
          bakButtonHeld = false;
        }
      }
    }
  } else {
    bakButtonSamples = 0;
    bakButtonHeld = false;
  }
  oledLastActivity = millis(); // Resetear timeout OLED en cualquier botón
}

void updateLEDs() {
  if (currentMode == MODE_PAUSE || currentMode == MODE_ERROR) {
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
  } else if (currentMode == MODE_MANUAL_RUN || currentMode == MODE_MANUAL_SETUP) {
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, HIGH); // Amarillo indica Manual
  } else { // AUTO
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, fanRunning ? HIGH : LOW); // Verde si ventilador ON
  }
}

void fatalError(String msg) {
  // Estado seguro: NO bucle infinito (evita watchdog reboot)
  digitalWrite(LED_RED_PIN, HIGH);
  digitalWrite(LED_YELLOW_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);
  setFanSpeed(0); // Apagar ventilador
  currentMode = MODE_ERROR;
  
  // Mostrar error en OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 10);
  display.println("ERROR!");
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.println(msg.substring(0, 16)); // Primeros 16 caracteres
  display.display();
  oledOn = true;
  
  // LED rojo parpadea cada 500ms (señal de error)
  // pero la función retorna para que loop() siga ejecutando
  Serial.println("FATAL ERROR: " + msg);
}

// -------------------------------------------------------------------------
// --- PANTALLAS OLED ---
// -------------------------------------------------------------------------

void drawAutoScreen() {
  display.clearDisplay();
  
  // Header
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("AUTO MODE");
  
  // Icono o estado fan
  display.setCursor(80,0);
  display.print(fanRunning ? "FAN:ON" : "STBY");

  // Datos Grandes
  display.setTextSize(2);
  display.setCursor(0, 15);
  display.print(hum, 0); display.print("% ");
  display.setCursor(64, 15);
  display.print(temp, 0); display.print("C");

  // Datos Secundarios
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("Air Q: "); display.print(airQuality);
  
  // Footer
  display.setCursor(0, 55);
  display.print("Gire -> Manual");
  
  display.display();
}

void drawManualSetupScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("CONFIG MANUAL");
  
  display.setCursor(10, 15);
  display.print(menuStep == 0 ? "> Tiempo: " : "  Tiempo: ");
  display.print(manualTimeSel); display.println(" min");
  
  display.setCursor(10, 25);
  display.print(menuStep == 1 ? "> Veloc:  " : "  Veloc:  ");
  display.print(manualSpeedSel); display.println(" %");
  
  display.setCursor(10, 35);
  display.print(menuStep == 2 ? "> Modo:   " : "  Modo:   ");
  display.println(manualInfiniteSelected ? "Infinito" : "Limitado");

  display.setCursor(10, 45);
  display.print(menuStep == 3 ? "> Noche:  " : "  Noche:  ");
  display.println(manualNightModeSelected ? "SI" : "NO");

  display.setCursor(0, 60);
  display.print("Click=OK Back=Auto");
  display.display();
}

void drawManualRunScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("MANUAL RUNNING");

  long remaining = (timerDuration - (millis() - fanTimerStart)) / 60000; // USAR fanTimerStart
  
  display.setTextSize(2);
  display.setCursor(15, 20);
  display.print(remaining); display.print(" min");
  
  display.setTextSize(1);
  display.setCursor(30, 45);
  display.print("Vel: "); display.print(manualSpeedSel); display.print("% ");

  display.display();
}

void drawManualInfiniteScreen() {
  // Pantalla Dashboard para modo manual infinito
  display.clearDisplay();
  
  // Animación de título: "EXTRACTOR TUNEADO" + "BY RAUL" con efecto de deslizamiento suave
  static unsigned long animationTime = 0;
  static int scrollPos = 0;
  
  // Cambiar posición cada 500ms (más lento = más elegante)
  if (millis() - animationTime > 500) {
    animationTime = millis();
    scrollPos = (scrollPos + 1) % 5; // 5 posiciones de scroll
  }
  
  // Título principal con efecto de desplazamiento - "EXTRACTOR TUNEADO BY RAUL"
  display.setTextSize(1);
  display.setCursor(scrollPos, 0);
  display.print("EXTRACTOR TUNEADO BY RAUL");
  
  // Línea divisoria animada (parpadea suavemente)
  display.setCursor(0, 10);
  static unsigned long lineBlinkTime = 0;
  static bool lineVisible = true;
  
  if (millis() - lineBlinkTime > 600) {
    lineBlinkTime = millis();
    lineVisible = !lineVisible;
  }
  
  if (lineVisible) {
    for (int i = 0; i < 21; i++) display.print("-");
  } else {
    for (int i = 0; i < 21; i++) display.print(" ");
  }
  
  // Modo infinito debajo del título
  display.setCursor(0, 20);
  display.print("[");
  display.print((char)236);  // ∞ Infinito
  display.print("] MANUAL INFINITO");
  
  // Barra de velocidad con % a la derecha
  display.setTextSize(1);
  int barFill = map(manualSpeedSel, 0, 100, 0, 18);
  display.setCursor(0, 32);
  display.print("[");
  for (int i = 0; i < barFill; i++) display.print((char)254);      // █ Lleno
  for (int i = barFill; i < 18; i++) display.print((char)176);     // ░ Vacío
  display.print("]");
  display.print(manualSpeedSel);
  display.print("%");
  
  // Info sensores: Temperatura, Humedad
  display.setCursor(0, 42);
  display.print((char)42); display.print(" ");  // ★
  display.print("T:");
  display.print((int)temp);
  display.print((char)167);  // °
  display.print(" H:");
  display.print((int)hum);
  display.print("%");
  
  // Indicadores ASCII de Calidad del Aire en la línea donde estaba "BY RAUL"
  // Representan: BUENA / REGULAR / MALA / CRÍTICA
  display.setCursor(0, 52);
  display.print("Aire: ");
  
  // Emoji ASCII compuesto según calidad
  if (mq135_warmed) {
    if (airQuality < 300) {
      display.print("BUENA   ");
      display.print("[*_*]"); // Sonrisa feliz
    } else if (airQuality < 600) {
      display.print("REGULAR ");
      display.print("[-_-]"); // Cara neutral
    } else if (airQuality < 900) {
      display.print("MALA    ");
      display.print("[o_o]"); // Sorpresa/Preocupación
    } else {
      display.print("CRITICA ");
      display.print("[X_X]"); // Alarma crítica
    }
  } else {
    display.print("CALENT. [...]");
  }
  
  display.display();
}

void drawPauseScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.print("PAUSADO");
  display.setTextSize(1);
  display.setCursor(10, 45);
  display.print("Mantener btn PAUSA");
  display.display();
}
