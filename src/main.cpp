/*
  Firmware Extractor Inteligente v4.0
  ===================================
  Controla un extractor de aire basado en sensores y permite operación manual
  con selección de tiempo y velocidad.

  - Microcontrolador: ESP32 DevKit
  - Sensores: BME280 (I2C), MQ135 (Analógico)
  - Interfaz: OLED 128x64 (I2C), Encoder Rotativo, 2 Botones
  - Actuadores: Relé + MOSFET (PWM)

  Autor: Gemini CLI
  Fecha: 2025-11-04
*/

// ============================================================================
//  sección de librerías
// ============================================================================
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <ESP32Encoder.h>

// ============================================================================
// sección de configuración de hardware (pines)
// ============================================================================
// Pantalla OLED I2C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define I2C_ADDRESS   0x3C

// Sensor BME280 (se probarán las direcciones 0x76 y 0x77)

// Sensor de Calidad de Aire MQ135
#define MQ135_PIN 34

// Encoder Rotativo
#define ENCODER_CLK_PIN 32
#define ENCODER_DT_PIN  33

// Botones Físicos (con pull-up interno)
#define BTN_ENTER_PIN   25
#define BTN_BACK_PIN    26

// Actuadores del Ventilador
#define RELAY_PIN    27
#define FAN_PWM_PIN  14
#define PWM_CHANNEL  0
#define PWM_FREQUENCY 25000
#define PWM_RESOLUTION 8

// ============================================================================
// sección de configuración de software (parámetros)
// ============================================================================
// Umbrales para el Modo Automático
#define HUMIDITY_THRESHOLD_HIGH 70.0f
#define HUMIDITY_THRESHOLD_LOW  65.0f
#define TEMP_THRESHOLD          30.0f
#define AIR_QUALITY_THRESHOLD   600

// Parámetros de anti-rebote para botones
#define DEBOUNCE_DELAY 250 // ms

// Rango del PWM para el motor (evita que se detenga a baja potencia)
#define PWM_MIN_DUTY 80
#define PWM_MAX_DUTY 255

// ============================================================================
// sección de objetos y variables globales
// ============================================================================
// --- Objetos de Librerías ---
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme;
ESP32Encoder encoder;

// --- Máquina de Estados ---
enum Mode { AUTOMATICO, MANUAL_SELECCION_TIEMPO, MANUAL_SELECCION_VELOCIDAD, MANUAL_ACTIVO };
Mode currentMode = AUTOMATICO;

// --- Variables de Sensores ---
float temperature = 0.0;
float humidity = 0.0;
int airQuality = 0;

// --- Variables de Modo Manual ---
const long manualDurations[] = { 30 * 60000, 60 * 60000, 90 * 60000 }; // en ms
const char* manualDurationLabels[] = {"30 MIN", "60 MIN", "90 MIN"};
int selectedDurationIndex = 0;

const int manualSpeeds[] = {25, 50, 75, 100};
const char* manualSpeedLabels[] = {"25% (Baja)", "50% (Media)", "75% (Alta)", "100% (Max)"};
int selectedSpeedIndex = 0;

unsigned long manualTimerStartTime = 0;
long manualTimeLeft = 0;

// --- Variables de Controles ---
long oldEncoderPosition = 0;
bool btnEnterPressed = false;
bool btnBackPressed = false;
unsigned long lastEnterPressTime = 0;
unsigned long lastBackPressTime = 0;

// --- Variables del Ventilador ---
int currentFanSpeed = 0; // En porcentaje (0-100)

// --- Prototipos de Funciones ---
void handleControls();
void readSensors();
void runLogic();
void controlFan(int percentage);
void updateDisplay();

// ============================================================================
// sección de setup (inicialización)
// ============================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n╔════════════════════════════════╗");
  Serial.println("║  EXTRACTOR INTELIGENTE v4.0   ║");
  Serial.println("╚════════════════════════════════╝\n");

  // --- Inicialización de Pines ---
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(BTN_ENTER_PIN, INPUT_PULLUP);
  pinMode(BTN_BACK_PIN, INPUT_PULLUP);

  // --- Configuración del PWM ---
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0);

  // --- Configuración del Encoder ---
  encoder.attachHalfQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.setCount(0);
  
  // --- Inicialización de la Pantalla OLED ---
  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDRESS)) {
    Serial.println("❌ ERROR: Fallo al iniciar SSD1306. Verifica la dirección I2C.");
    while(true);
  }
  
  // --- Pantalla de Bienvenida ---
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 5);
  display.println("Extractor");
  display.setCursor(5, 30);
  display.println("Inteligente");
  display.setTextSize(1);
  display.setCursor(35, 52);
  display.println("v4.0");
  display.display();
  delay(2000);

  // --- Inicialización del Sensor BME280 ---
  Serial.print("Iniciando BME280... ");
  if (!bme.begin(0x76) && !bme.begin(0x77)) {
    Serial.println("❌ ERROR: Sensor BME280 no encontrado.");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.println("ERROR: BME280");
    display.println("no detectado.");
    display.println("Verifica I2C.");
    display.display();
    while(true);
  }
  Serial.println("✓ OK");
  
  // --- Calibración del Sensor MQ135 ---
  Serial.println("Calibrando sensor MQ135 (30s)...");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println("Calibrando Aire...");
  display.display();
  for(int i = 30; i > 0; i--) {
    display.fillRect(10, 40, 108, 10, SSD1306_BLACK);
    display.setCursor(10, 40);
    display.printf("Espere... %02d s", i);
    display.display();
    delay(1000);
  }
  
  Serial.println("✓ Sistema listo.\n");
}

// ============================================================================
// sección de loop principal
// ============================================================================
void loop() {
  handleControls();
  readSensors();
  runLogic();
  updateDisplay();
  delay(50); // Pequeña pausa para estabilidad
}

// ============================================================================
// sección de manejo de controles (encoder y botones)
// ============================================================================
void handleControls() {
  // --- Lectura del Botón ENTER con Anti-rebote ---
  if (digitalRead(BTN_ENTER_PIN) == LOW && (millis() - lastEnterPressTime > DEBOUNCE_DELAY)) {
    lastEnterPressTime = millis();
    btnEnterPressed = true;
    Serial.println("🔘 ENTER presionado");
  }

  // --- Lectura del Botón BACK con Anti-rebote ---
  if (digitalRead(BTN_BACK_PIN) == LOW && (millis() - lastBackPressTime > DEBOUNCE_DELAY)) {
    lastBackPressTime = millis();
    btnBackPressed = true;
    Serial.println("🔙 BACK presionado");
  }

  // --- Lectura del Encoder Rotativo ---
  if (currentMode == MANUAL_SELECCION_TIEMPO || currentMode == MANUAL_SELECCION_VELOCIDAD) {
    long newEncoderPosition = encoder.getCount() / 2;
    if (newEncoderPosition != oldEncoderPosition) {
      int direction = (newEncoderPosition > oldEncoderPosition) ? 1 : -1;

      if (currentMode == MANUAL_SELECCION_TIEMPO) {
        selectedDurationIndex = (selectedDurationIndex + direction + 3) % 3;
        Serial.printf("🔄 Selección Tiempo: %s\n", manualDurationLabels[selectedDurationIndex]);
      } else {
        selectedSpeedIndex = (selectedSpeedIndex + direction + 4) % 4;
        Serial.printf("🔄 Selección Velocidad: %s\n", manualSpeedLabels[selectedSpeedIndex]);
      }
      oldEncoderPosition = newEncoderPosition;
    }
  }
}

// ============================================================================
// sección de lectura de sensores
// ============================================================================
void readSensors() {
  // --- Lectura BME280 ---
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("⚠️ Lectura de BME280 inválida.");
    temperature = 0.0; humidity = 0.0; // Evitar valores corruptos
  }

  // --- Lectura MQ135 ---
  airQuality = analogRead(MQ135_PIN);
  if (airQuality < 0 || airQuality > 4095) {
      Serial.println("⚠️ Lectura de MQ135 fuera de rango.");
      airQuality = 0; // Evitar valores corruptos
  }

  // --- Log de Sensores (cada 10 segundos) ---
  static unsigned long lastDebugTime = 0;
  if (millis() - lastDebugTime > 10000) {
    Serial.printf("📊 Sensores - T: %.1f°C | H: %.1f%% | Aire: %d\n", temperature, humidity, airQuality);
    lastDebugTime = millis();
  }
}

// ============================================================================
// sección de lógica de control (máquina de estados)
// ============================================================================
void runLogic() {
  // --- Lógica de Transición por Botón ENTER ---
  if (btnEnterPressed) {
    btnEnterPressed = false;
    switch (currentMode) {
      case AUTOMATICO:
        currentMode = MANUAL_SELECCION_TIEMPO;
        selectedDurationIndex = 0;
        encoder.setCount(0); oldEncoderPosition = 0;
        controlFan(0);
        break;
      case MANUAL_SELECCION_TIEMPO:
        currentMode = MANUAL_SELECCION_VELOCIDAD;
        selectedSpeedIndex = 0;
        encoder.setCount(0); oldEncoderPosition = 0;
        break;
      case MANUAL_SELECCION_VELOCIDAD:
        currentMode = MANUAL_ACTIVO;
        manualTimerStartTime = millis();
        manualTimeLeft = manualDurations[selectedDurationIndex];
        controlFan(manualSpeeds[selectedSpeedIndex]);
        Serial.printf("✅ Iniciando modo MANUAL: %s a %s\n", manualDurationLabels[selectedDurationIndex], manualSpeedLabels[selectedSpeedIndex]);
        break;
      case MANUAL_ACTIVO:
        currentMode = AUTOMATICO;
        controlFan(0);
        break;
    }
    return;
  }

  // --- Lógica de Transición por Botón BACK ---
  if (btnBackPressed) {
    btnBackPressed = false;
    switch (currentMode) {
      case MANUAL_SELECCION_TIEMPO:
      case MANUAL_ACTIVO:
        currentMode = AUTOMATICO;
        controlFan(0);
        break;
      case MANUAL_SELECCION_VELOCIDAD:
        currentMode = MANUAL_SELECCION_TIEMPO;
        encoder.setCount(0); oldEncoderPosition = 0;
        break;
      case AUTOMATICO:
        // Sin función
        break;
    }
    return;
  }

  // --- Lógica de Operación Continua por Modo ---
  if (currentMode == AUTOMATICO) {
    int fanSpeed = 0;
    if (humidity >= HUMIDITY_THRESHOLD_HIGH) fanSpeed = 100;
    else if (humidity >= HUMIDITY_THRESHOLD_LOW) fanSpeed = 70;
    else if (temperature >= TEMP_THRESHOLD) fanSpeed = 60;
    else if (airQuality >= AIR_QUALITY_THRESHOLD) fanSpeed = 40;
    controlFan(fanSpeed);
  } 
  else if (currentMode == MANUAL_ACTIVO) {
    manualTimeLeft = manualDurations[selectedDurationIndex] - (millis() - manualTimerStartTime);
    if (manualTimeLeft <= 0) {
      Serial.println("⏰ Tiempo agotado - Volviendo a AUTOMÁTICO");
      currentMode = AUTOMATICO;
      controlFan(0);
    }
  }
}

// ============================================================================
// sección de control del ventilador
// ============================================================================
void controlFan(int percentage) {
  if (percentage == currentFanSpeed) return; // Evitar cambios innecesarios
  currentFanSpeed = constrain(percentage, 0, 100);
  
  if (currentFanSpeed == 0) {
    digitalWrite(RELAY_PIN, LOW);
    ledcWrite(PWM_CHANNEL, 0);
    Serial.println("💨 Ventilador: APAGADO");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    int pwmValue = map(currentFanSpeed, 1, 100, PWM_MIN_DUTY, PWM_MAX_DUTY);
    ledcWrite(PWM_CHANNEL, pwmValue);
    Serial.printf("💨 Ventilador: %d%% (PWM: %d)\n", currentFanSpeed, pwmValue);
  }
}

// ============================================================================
// sección de actualización de la pantalla oled
// ============================================================================
void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // --- Barra Superior de Sensores (siempre visible) ---
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("T:%.0fC H:%.0f%% A:%d", temperature, humidity, airQuality);
  display.drawLine(0, 10, SCREEN_WIDTH - 1, 10, SSD1306_WHITE);
  
  // --- Contenido Principal según el Modo ---
  switch (currentMode) {
    case AUTOMATICO:
      display.setTextSize(2);
      display.setCursor(15, 22);
      display.println("MODO");
      display.setCursor(5, 42);
      display.println("AUTOMATICO");
      if (currentFanSpeed > 0) {
        display.setTextSize(1);
        display.setCursor(25, 56);
        display.printf("VENTILADOR: %d%%", currentFanSpeed);
      }
      break;
    
    case MANUAL_SELECCION_TIEMPO:
      display.setTextSize(1);
      display.setCursor(15, 14);
      display.println("SELECCIONA TIEMPO");
      display.drawLine(0, 24, SCREEN_WIDTH - 1, 24, SSD1306_WHITE);
      for(int i = 0; i < 3; i++) {
        display.setCursor(25, 30 + (i * 11));
        if (i == selectedDurationIndex) display.printf("> %s <", manualDurationLabels[i]);
        else display.printf("  %s  ", manualDurationLabels[i]);
      }
      display.setCursor(10, 56);
      display.print("ENTER=OK  BACK=X");
      break;

    case MANUAL_SELECCION_VELOCIDAD:
      display.setTextSize(1);
      display.setCursor(10, 12);
      display.println("SELECCIONA VELOCIDAD");
      display.setCursor(10, 22);
      display.printf("(Tiempo: %s)", manualDurationLabels[selectedDurationIndex]);
      display.drawLine(0, 32, SCREEN_WIDTH - 1, 32, SSD1306_WHITE);
      for(int i = 0; i < 4; i++) {
        display.setCursor(10, 35 + (i * 7));
        if (i == selectedSpeedIndex) display.printf("> %s <", manualSpeedLabels[i]);
        else display.printf("  %s  ", manualSpeedLabels[i]);
      }
      display.setCursor(5, 56);
      display.print("ENTER=OK  BACK=<");
      break;
      
    case MANUAL_ACTIVO:
      display.setTextSize(2);
      display.setCursor(10, 18);
      display.println("MANUAL");
      int secondsLeft = (manualTimeLeft + 999) / 1000;
      int minutes = secondsLeft / 60;
      int seconds = secondsLeft % 60;
      display.setTextSize(1);
      display.setCursor(5, 42);
      display.printf("T: %02d:%02d | %d%%", minutes, seconds, currentFanSpeed);
      break;
  }

  display.display();
}
