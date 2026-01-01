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
#define RELAY_PIN         23  // Corte de energía (+12V)
#define PWM_FAN_PIN       19  // Control de velocidad (PWM Directo)
#define FAN_TACH_PIN      16  // Lectura de RPM (Cable Amarillo) - PREPARADO PARA FUTURO

// Sensores Analógicos
#define MQ135_ANALOG_PIN  34  // Entrada analógica (Input Only)

// LEDs de Estado
#define LED_RED_PIN       18  // Error / Standby
#define LED_YELLOW_PIN    4   // Advertencia / Modo Manual - MOVIDO AL 4 POR SEGURIDAD
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
#define DEBOUNCE_DELAY    250   // ms
#define PAUSE_HOLD_TIME   2000  // ms para activar pausa

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
enum SystemMode { MODE_AUTO, MODE_MANUAL_SETUP, MODE_MANUAL_RUN, MODE_PAUSE, MODE_ERROR };
SystemMode currentMode = MODE_AUTO;

// Variables de Control
int targetSpeed = 0;       // 0-255
int currentSpeed = 0;      // Para rampas suaves (opcional)
unsigned long fanTimerStart = 0; // RENOMBRADO PARA EVITAR CONFLICTO
unsigned long timerDuration = 0; // En milisegundos
bool fanRunning = false;

// Variables Manual Setup
int menuStep = 0; // 0: Tiempo, 1: Velocidad, 2: Confirmar
int manualTimeSel = 30; // Minutos
int manualSpeedSel = 50; // %

// Variables Botones/Encoder
long oldEncPos = 0;
unsigned long lastButtonPress = 0;
unsigned long bakButtonPressStart = 0;
bool bakButtonHeld = false;

// Variables Sensores
float hum = 0, temp = 0;
int airQuality = 0;
unsigned long lastSensorRead = 0;
int sensorFailCount = 0;
const int MAX_SENSOR_FAILS = 3;

// Variables Display
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_INTERVAL = 100; // 10 FPS (100ms) - Optimización Bolt

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
void drawAutoScreen();
void drawManualSetupScreen();
void drawManualRunScreen();
void drawPauseScreen();
void updateDisplayRouter();

// -------------------------------------------------------------------------
// --- SETUP ---
// -------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- INICIANDO EXTRACTOR INTELIGENTE v7.1 ---");

  // 1. Configurar Pines
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Asegurar RELÉ APAGADO al inicio

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
  Wire.setClock(400000); // Aumentar velocidad I2C
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Error OLED");
    fatalError("OLED FAIL");
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.println("INICIANDO...");
  display.display();

  // 4. Inicializar Sensores
  bool sensorsOK = true;
  if (!aht.begin()) { Serial.println("Error AHT20"); sensorsOK = false; }
  if (!bmp.begin(0x77)) { Serial.println("Error BMP280"); sensorsOK = false; }
  
  if (!sensorsOK) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("ERROR SENSORES!");
    display.println("Revise cableado");
    display.display();
    digitalWrite(LED_RED_PIN, HIGH);
    delay(3000); 
    // Continuamos aunque fallen, pero forzamos modo ERROR (Fail-Safe)
    currentMode = MODE_ERROR;
  }

  // 5. Configurar Encoder
  encoder.attachHalfQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.setCount(0);

  // 6. Watchdog Timer (8 segundos)
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
  unsigned long currentMillis = millis();

  readSensors();
  checkButtons();
  
  // Ejecutar lógica de control (SIN DIBUJAR PANTALLA)
  switch (currentMode) {
    case MODE_AUTO:
      runAutoLogic();
      break;

    case MODE_MANUAL_SETUP:
      runManualSetup();
      break;

    case MODE_MANUAL_RUN:
      runManualLogic();
      break;

    case MODE_PAUSE:
      // Ventilador apagado, esperar reanudar
      setFanSpeed(0);
      break;
      
    case MODE_ERROR:
      setFanSpeed(128); // Fail-Safe: 50% Velocidad
      digitalWrite(LED_RED_PIN, HIGH);
      break;
  }
  
  updateLEDs();

  // Optimización Bolt:
  // Controlamos la actualización de la pantalla con millis() en lugar de
  // actualizar en cada ciclo del loop (50Hz), lo cual saturaba el I2C.
  if (currentMillis - lastDisplayUpdate >= DISPLAY_INTERVAL) {
    lastDisplayUpdate = currentMillis;
    updateDisplayRouter();
  }

  // Pequeña pausa para que el RTOS respire, pero mínima para no bloquear inputs
  delay(1);
}

void updateDisplayRouter() {
  switch (currentMode) {
    case MODE_AUTO:
      drawAutoScreen();
      break;
    case MODE_MANUAL_SETUP:
      drawManualSetupScreen();
      break;
    case MODE_MANUAL_RUN:
      drawManualRunScreen();
      break;
    case MODE_PAUSE:
      drawPauseScreen();
      break;
    case MODE_ERROR:
       // Error mode might need faster updates if blinking?
       // For now 10fps is fine.
       // Reuse auto screen layout or create specific one if needed
       // Logic in loop() sets LED.
       // We can just draw basic info or error message
       display.clearDisplay();
       display.setCursor(0,0);
       display.println("CRITICAL ERROR");
       display.display();
      break;
  }
}

// -------------------------------------------------------------------------
// --- FUNCIONES DE LÓGICA ---
// -------------------------------------------------------------------------

void setFanSpeed(int speedPWM) {
  // speedPWM: 0-255
  if (speedPWM > 0) {
    digitalWrite(RELAY_PIN, HIGH); // Activar Relé (Energía)
    // Pequeño delay para asegurar que el relé cierre antes de meter PWM (opcional pero bueno)
    if (!fanRunning) { delay(50); fanRunning = true; }
    ledcWrite(PWM_CHANNEL, speedPWM);
    currentSpeed = speedPWM;
  } else {
    ledcWrite(PWM_CHANNEL, 0);
    delay(100); // Esperar a que baje RPM
    digitalWrite(RELAY_PIN, LOW); // Cortar Relé
    fanRunning = false;
    currentSpeed = 0;
  }
}

void readSensors() {
  if (millis() - lastSensorRead > 2000) { // Leer cada 2 segundos
    lastSensorRead = millis();
    
    sensors_event_t h, t;
    aht.getEvent(&h, &t);

    // Validación de lecturas (Fail-Safe)
    if (isnan(h.relative_humidity) || isnan(t.temperature)) {
      sensorFailCount++;
      if (sensorFailCount >= MAX_SENSOR_FAILS) {
        currentMode = MODE_ERROR;
      }
    } else {
      hum = h.relative_humidity;
      temp = t.temperature;
      sensorFailCount = 0; // Reset si leemos bien

      // Si estábamos en error por sensores y se recuperan, ¿volvemos a AUTO?
      if (currentMode == MODE_ERROR) {
        currentMode = MODE_AUTO;
      }
    }
    
    // MQ135 - Lectura simple y suavizado
    int raw = analogRead(MQ135_ANALOG_PIN);
    airQuality = (airQuality * 0.8) + (raw * 0.2); // Media móvil simple
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
      }
    } else if (currentMode == MODE_AUTO) {
       // Si giramos en AUTO, entramos a MANUAL SETUP
       currentMode = MODE_MANUAL_SETUP;
       menuStep = 0;
       manualTimeSel = 30;
    }
    oldEncPos = encVal;
  }

  // 2. Encoder Switch (OK / Next)
  if (digitalRead(ENCODER_SW_PIN) == LOW) {
    if (millis() - lastButtonPress > DEBOUNCE_DELAY) {
      lastButtonPress = millis();
      if (currentMode == MODE_MANUAL_SETUP) {
        menuStep++;
        if (menuStep > 1) { // Iniciar Manual
          currentMode = MODE_MANUAL_RUN;
          timerDuration = manualTimeSel * 60000UL;
          fanTimerStart = millis(); // USAR fanTimerStart
        }
      }
    }
  }

  // 3. Confirm Button (BACK / Cancelar)
  if (digitalRead(CONFIRM_BUTTON_PIN) == LOW) {
    if (millis() - lastButtonPress > DEBOUNCE_DELAY) {
      lastButtonPress = millis();
      // Volver siempre a AUTO
      currentMode = MODE_AUTO;
      menuStep = 0;
    }
  }

  // 4. BAK Button (PAUSA - Mantener)
  if (digitalRead(BAK_BUTTON_PIN) == LOW) {
    if (!bakButtonHeld) {
      bakButtonPressStart = millis();
      bakButtonHeld = true;
    } else {
      if (millis() - bakButtonPressStart > PAUSE_HOLD_TIME) {
        // Toggle Pausa
        if (currentMode == MODE_PAUSE) {
          currentMode = MODE_AUTO; // O volver al anterior (más complejo)
        } else {
          currentMode = MODE_PAUSE;
        }
        bakButtonHeld = false; // Reset para obligar a soltar y volver a pulsar
        delay(1000); // Evitar rebotes
      }
    }
  } else {
    bakButtonHeld = false;
  }
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
  while(1) {
    digitalWrite(LED_RED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_RED_PIN, LOW);
    delay(200);
  }
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
  
  display.setCursor(10, 20);
  display.print(menuStep == 0 ? "> Tiempo: " : "  Tiempo: ");
  display.print(manualTimeSel); display.println(" min");
  
  display.setCursor(10, 35);
  display.print(menuStep == 1 ? "> Veloc:  " : "  Veloc:  ");
  display.print(manualSpeedSel); display.println(" %");

  display.setCursor(0, 55);
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
