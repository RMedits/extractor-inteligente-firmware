/*
  Extractor Inteligente para Baño/Galería
  ESP32 + AHT20 + BME280 + MQ135 + Módulo OLED Integrado
  
  CONTROLES OPTIMIZADOS:
  - Encoder (girar): Navegar opciones
  - ENCODER_PUSH (pulsar rueda): OK/Confirmar/Avanzar
  - CONFIRM (botón lateral): BACK/Cancelar/Volver
  - BACK (botón lateral): PAUSA de emergencia (ON/OFF)
  
  VERSIÓN 6.1 - Integración de sensor AHT20
  https://github.com/RMedits/extractor-inteligente-firmware
*/

// --- LIBRERÍAS ---
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_AHTX0.h>
#include <ESP32Encoder.h>

// --- CONFIGURACIÓN DE PANTALLA OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- CONFIGURACIÓN DE SENSORES ---
Adafruit_BME280 bme;
Adafruit_AHTX0 aht;
#define MQ135_PIN 34

// --- CONFIGURACIÓN DEL MÓDULO INTEGRADO ---
#define ENCODER_TRA_PIN  32  // ENCODER_TRA → CLK
#define ENCODER_TRB_PIN  33  // ENCODER_TRB → DT
#define BTN_ENCODER_PUSH 27  // Botón integrado encoder (pulsar rueda) → OK
#define BTN_CONFIRM_PIN  25  // Botón lateral CONFIRM → BACK/Cancelar
#define BTN_BACK_PIN     26  // Botón lateral BACK → PAUSA de emergencia

ESP32Encoder encoder;

// Nota I2C: OLED_SDA → GPIO21, OLED_SCL → GPIO22

// --- CONFIGURACIÓN DEL VENTILADOR ---
#define RELAY_PIN    23
#define FAN_PWM_PIN  14
#define PWM_CHANNEL  0
#define PWM_FREQUENCY 25000
#define PWM_RESOLUTION 8

// --- UMBRALES PARA MODO AUTOMÁTICO ---
#define HUMIDITY_THRESHOLD_HIGH 70.0f
#define HUMIDITY_THRESHOLD_LOW  65.0f
#define TEMP_THRESHOLD          30.0f
#define AIR_QUALITY_THRESHOLD   600

// --- ESTADOS DEL SISTEMA ---
enum Mode { 
  AUTOMATICO, 
  SELECCION_TIEMPO, 
  SELECCION_VELOCIDAD, 
  MANUAL_ACTIVO,
  PAUSA  // Nuevo estado para pausa de emergencia
};
Mode currentMode = AUTOMATICO;
Mode previousMode = AUTOMATICO; // Para recordar el estado antes de pausa

// Variables de sensores (valores promediados)
float temperature = 0.0;
float humidity = 0.0;
int airQuality = 0;

// Variables del modo manual - TIEMPO
const long manualDurations[] = { 30 * 60000, 60 * 60000, 90 * 60000 };
const char* manualDurationLabels[] = {"30 MIN", "60 MIN", "90 MIN"};
int selectedDuration = 0;

// Variables del modo manual - VELOCIDAD
const int speedOptions[] = { 25, 50, 75, 100 };
const char* speedLabels[] = {"25% (Baja)", "50% (Media)", "75% (Alta)", "100% (Max)"};
int selectedSpeed = 1;

// Variables del temporizador
unsigned long manualTimerStartTime = 0;
long manualTimeLeft = 0;
long pausedTimeLeft = 0;  // Tiempo guardado al pausar
int manualFanSpeed = 0;
int pausedFanSpeed = 0;   // Velocidad guardada al pausar

// Variables del encoder
long oldEncoderPosition = 0;

// Variables de botones con anti-rebote
bool btnOkPressed = false;        // ENCODER_PUSH
bool btnBackPressed = false;      // CONFIRM (actúa como BACK)
bool btnPausePressed = false;     // BACK (pausa emergencia)
unsigned long lastOkPressTime = 0;
unsigned long lastBackPressTime = 0;
unsigned long lastPausePressTime = 0;
const long debounceDelay = 250;

// Control del ventilador
int currentFanSpeed = 0;
bool isPaused = false;

// --- PROTOTIPOS DE FUNCIONES ---
void handleControls();
void readSensors();
void runLogic();
void controlFan(int percentage);
void updateDisplay();

// --- FUNCIÓN SETUP ---
void setup() {
  Serial.begin(115200);
  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  EXTRACTOR INTELIGENTE v6.1       ║");
  Serial.println("║  Integracion de sensor AHT20      ║");
  Serial.println("╚════════════════════════════════════╝\n");

  // Inicializar pines
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  pinMode(BTN_ENCODER_PUSH, INPUT_PULLUP);
  pinMode(BTN_CONFIRM_PIN, INPUT_PULLUP);
  pinMode(BTN_BACK_PIN, INPUT_PULLUP);

  // Configurar PWM
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0);

  // Configurar encoder
  encoder.attachHalfQuad(ENCODER_TRA_PIN, ENCODER_TRB_PIN);
  encoder.setCount(0);
  
  // Inicializar OLED
  Serial.print("Iniciando OLED... ");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ ERROR");
    while(true);
  }
  Serial.println("✓ OK");
  
  // Pantalla de bienvenida
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 5);
  display.println("Extractor");
  display.setCursor(5, 30);
  display.println("v6.1");
  display.setTextSize(1);
  display.setCursor(10, 52);
  display.println("Sensor AHT20");
  display.display();
  delay(2000);

  // Inicializar BME280
  Serial.print("Iniciando BME280... ");
  if (!bme.begin(0x76) && !bme.begin(0x77)) {
    Serial.println("❌ ERROR");
    // No detenemos el programa, podemos continuar con el AHT20
  } else {
    Serial.println("✓ OK");
  }

  // Inicializar AHT20
  Serial.print("Iniciando AHT20... ");
  if (!aht.begin()) {
    Serial.println("❌ ERROR");
    // No detenemos el programa, podemos continuar con el BME280
  } else {
    Serial.println("✓ OK");
  }
  
  // Calibración MQ135
  Serial.println("Calibrando MQ135...");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 15);
  display.println("Calibrando");
  display.setCursor(10, 30);
  display.println("sensor de aire");
  display.display();
  
  for(int i = 30; i > 0; i--) {
    display.fillRect(0, 50, 128, 14, SSD1306_BLACK);
    display.setCursor(40, 52);
    display.print(i);
    display.print(" seg");
    display.display();
    delay(1000);
  }
  
  Serial.println("\n✓ Sistema listo");
  Serial.println("\nCONTROLES:");
  Serial.println("  Encoder (girar): Navegar");
  Serial.println("  Encoder (pulsar): OK/Confirmar");
  Serial.println("  CONFIRM: Cancelar/Volver");
  Serial.println("  BACK: Pausa emergencia ON/OFF\n");
}

// --- FUNCIÓN LOOP ---
void loop() {
  handleControls();
  readSensors();
  runLogic();
  updateDisplay();
  delay(50);
}

// --- MANEJO DE CONTROLES ---
void handleControls() {
  // Botón ENCODER_PUSH (pulsar rueda) → OK
  if (digitalRead(BTN_ENCODER_PUSH) == LOW && 
      (millis() - lastOkPressTime > debounceDelay)) {
    lastOkPressTime = millis();
    btnOkPressed = true;
    Serial.println("✓ OK presionado");
  }

  // Botón CONFIRM (lateral) → BACK/Cancelar
  if (digitalRead(BTN_CONFIRM_PIN) == LOW && 
      (millis() - lastBackPressTime > debounceDelay)) {
    lastBackPressTime = millis();
    btnBackPressed = true;
    Serial.println("⬅️ BACK presionado");
  }

  // Botón BACK (lateral) → PAUSA emergencia
  if (digitalRead(BTN_BACK_PIN) == LOW && 
      (millis() - lastPausePressTime > debounceDelay)) {
    lastPausePressTime = millis();
    btnPausePressed = true;
    Serial.println("⏸️ PAUSA presionado");
  }

  // Encoder rotativo (solo en modos de selección)
  if (currentMode == SELECCION_TIEMPO || currentMode == SELECCION_VELOCIDAD) {
    long newEncoderPosition = encoder.getCount() / 2;
    
    if (newEncoderPosition != oldEncoderPosition) {
      if (currentMode == SELECCION_TIEMPO) {
        if (newEncoderPosition > oldEncoderPosition) {
          selectedDuration++;
          if (selectedDuration > 2) selectedDuration = 0;
        } else {
          selectedDuration--;
          if (selectedDuration < 0) selectedDuration = 2;
        }
        Serial.print("🔄 Tiempo: ");
        Serial.println(manualDurationLabels[selectedDuration]);
      }
      else if (currentMode == SELECCION_VELOCIDAD) {
        if (newEncoderPosition > oldEncoderPosition) {
          selectedSpeed++;
          if (selectedSpeed > 3) selectedSpeed = 0;
        } else {
          selectedSpeed--;
          if (selectedSpeed < 0) selectedSpeed = 3;
        }
        Serial.print("🔄 Velocidad: ");
        Serial.println(speedLabels[selectedSpeed]);
      }
      oldEncoderPosition = newEncoderPosition;
    }
  }
}

// --- LECTURA DE SENSORES ---
void readSensors() {
  float tempBME = NAN, humBME = NAN;
  float tempAHT = NAN, humAHT = NAN;
  int sensorCount = 0;

  // Leer BME280
  tempBME = bme.readTemperature();
  humBME = bme.readHumidity();

  // Leer AHT20
  sensors_event_t humidity_event, temp_event;
  aht.getEvent(&humidity_event, &temp_event);
  tempAHT = temp_event.temperature;
  humAHT = humidity_event.relative_humidity;

  // Calcular promedio de temperatura
  float totalTemp = 0;
  int tempSensorCount = 0;
  if (!isnan(tempBME)) { totalTemp += tempBME; tempSensorCount++; }
  if (!isnan(tempAHT)) { totalTemp += tempAHT; tempSensorCount++; }
  if (tempSensorCount > 0) temperature = totalTemp / tempSensorCount;

  // Calcular promedio de humedad
  float totalHum = 0;
  int humSensorCount = 0;
  if (!isnan(humBME)) { totalHum += humBME; humSensorCount++; }
  if (!isnan(humAHT)) { totalHum += humAHT; humSensorCount++; }
  if (humSensorCount > 0) humidity = totalHum / humSensorCount;

  // Leer MQ135
  airQuality = analogRead(MQ135_PIN);

  static unsigned long lastDebugTime = 0;
  if (millis() - lastDebugTime > 10000) {
    Serial.printf("📊 BME: T:%.1f H:%.1f | AHT: T:%.1f H:%.1f | PROMEDIO: T:%.1f H:%.1f | Aire: %d\n",
                  tempBME, humBME, tempAHT, humAHT, temperature, humidity, airQuality);
    lastDebugTime = millis();
  }
}

// --- LÓGICA PRINCIPAL ---
void runLogic() {
  // === BOTÓN PAUSA (PRIORIDAD MÁXIMA) ===
  if (btnPausePressed) {
    btnPausePressed = false;
    
    if (!isPaused) {
      // ACTIVAR PAUSA
      Serial.println("⏸️ PAUSA ACTIVADA");
      isPaused = true;
      previousMode = currentMode;
      
      // Guardar estado del temporizador si está en manual
      if (currentMode == MANUAL_ACTIVO) {
        pausedTimeLeft = manualTimeLeft;
        pausedFanSpeed = manualFanSpeed;
      }
      
      // Apagar ventilador inmediatamente
      controlFan(0);
      currentMode = PAUSA;
    } 
    else {
      // DESACTIVAR PAUSA
      Serial.println("▶️ PAUSA DESACTIVADA - Reanudando");
      isPaused = false;
      currentMode = previousMode;
      
      // Restaurar temporizador si estaba en manual
      if (currentMode == MANUAL_ACTIVO) {
        manualTimeLeft = pausedTimeLeft;
        manualFanSpeed = pausedFanSpeed;
        manualTimerStartTime = millis() - (manualDurations[selectedDuration] - pausedTimeLeft);
        controlFan(manualFanSpeed);
      }
    }
    return;
  }

  // No procesar otros botones si está en pausa
  if (isPaused) {
    return;
  }

  // === BOTÓN OK (ENCODER_PUSH) ===
  if (btnOkPressed) {
    btnOkPressed = false;
    
    switch (currentMode) {
      case AUTOMATICO:
        Serial.println("➡️ Modo: SELECCIÓN TIEMPO");
        currentMode = SELECCION_TIEMPO;
        selectedDuration = 0;
        encoder.setCount(0);
        oldEncoderPosition = 0;
        controlFan(0);
        break;
        
      case SELECCION_TIEMPO:
        Serial.print("➡️ Tiempo: ");
        Serial.print(manualDurationLabels[selectedDuration]);
        Serial.println(" → SELECCIÓN VELOCIDAD");
        currentMode = SELECCION_VELOCIDAD;
        selectedSpeed = 1;
        encoder.setCount(selectedSpeed * 2);
        oldEncoderPosition = selectedSpeed;
        break;
        
      case SELECCION_VELOCIDAD:
        Serial.print("✅ MANUAL ACTIVO: ");
        Serial.print(manualDurationLabels[selectedDuration]);
        Serial.print(" a ");
        Serial.print(speedOptions[selectedSpeed]);
        Serial.println("%");
        currentMode = MANUAL_ACTIVO;
        manualTimerStartTime = millis();
        manualTimeLeft = manualDurations[selectedDuration];
        manualFanSpeed = speedOptions[selectedSpeed];
        controlFan(manualFanSpeed);
        break;
        
      case MANUAL_ACTIVO:
        Serial.println("⏹️ Manual cancelado → AUTO");
        currentMode = AUTOMATICO;
        controlFan(0);
        break;
        
      default:
        break;
    }
    return;
  }

  // === BOTÓN BACK (CONFIRM actúa como BACK) ===
  if (btnBackPressed) {
    btnBackPressed = false;
    
    switch (currentMode) {
      case SELECCION_TIEMPO:
        Serial.println("⬅️ Cancelado → AUTO");
        currentMode = AUTOMATICO;
        controlFan(0);
        break;
        
      case SELECCION_VELOCIDAD:
        Serial.println("⬅️ Volver a SELECCIÓN TIEMPO");
        currentMode = SELECCION_TIEMPO;
        encoder.setCount(selectedDuration * 2);
        oldEncoderPosition = selectedDuration;
        break;
        
      case MANUAL_ACTIVO:
        Serial.println("⬅️ Manual cancelado → AUTO");
        currentMode = AUTOMATICO;
        controlFan(0);
        break;
        
      default:
        break;
    }
    return;
  }

  // === LÓGICA SEGÚN MODO ===
  if (currentMode == AUTOMATICO) {
    int fanSpeed = 0;
    
    if (humidity >= HUMIDITY_THRESHOLD_HIGH) {
      fanSpeed = 100;
    } 
    else if (humidity >= HUMIDITY_THRESHOLD_LOW) {
      fanSpeed = 70;
    }
    else if (temperature >= TEMP_THRESHOLD) {
      fanSpeed = 60;
    }
    else if (airQuality >= AIR_QUALITY_THRESHOLD) {
      fanSpeed = 40;
    }
    
    controlFan(fanSpeed);
  } 
  else if (currentMode == MANUAL_ACTIVO) {
    manualTimeLeft = manualDurations[selectedDuration] - (millis() - manualTimerStartTime);
    
    if (manualTimeLeft <= 0) {
      Serial.println("⏰ Tiempo agotado → AUTO");
      currentMode = AUTOMATICO;
      controlFan(0);
    }
  }
}

// --- CONTROL DEL VENTILADOR ---
void controlFan(int percentage) {
  if (percentage == currentFanSpeed) return;
  
  currentFanSpeed = percentage;
  
  if (percentage <= 0) {
    digitalWrite(RELAY_PIN, LOW);
    ledcWrite(PWM_CHANNEL, 0);
    Serial.println("💨 Ventilador: APAGADO");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    int pwmValue = map(percentage, 1, 100, 80, 255);
    ledcWrite(PWM_CHANNEL, pwmValue);
    Serial.printf("💨 Ventilador: %d%% (PWM: %d)\n", percentage, pwmValue);
  }
}

// --- ACTUALIZAR PANTALLA ---
void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Barra superior: sensores
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("T:%.0fC H:%.0f%% A:%d", temperature, humidity, airQuality);
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  
  switch (currentMode) {
    case AUTOMATICO:
      display.setTextSize(2);
      display.setCursor(15, 18);
      display.println("MODO");
      display.setCursor(5, 38);
      display.println("AUTOMATICO");
      
      if (currentFanSpeed > 0) {
        display.setTextSize(1);
        display.setCursor(25, 56);
        display.printf("VENT: %d%%", currentFanSpeed);
      } else {
        display.setTextSize(1);
        display.setCursor(30, 56);
        display.print("[ STANDBY ]");
      }
      break;
    
    case SELECCION_TIEMPO:
      display.setTextSize(1);
      display.setCursor(10, 14);
      display.println("SELECCIONA TIEMPO");
      display.drawLine(0, 24, 127, 24, SSD1306_WHITE);
      
      for(int i = 0; i < 3; i++) {
        display.setCursor(25, 30 + (i * 11));
        if (i == selectedDuration) {
          display.print(">");
          display.print(" ");
          display.print(manualDurationLabels[i]);
          display.print(" <");
        } else {
          display.print("  ");
          display.print(manualDurationLabels[i]);
        }
      }
      
      display.setCursor(15, 56);
      display.print("OK=Pulsar BACK=X");
      break;
      
    case SELECCION_VELOCIDAD:
      display.setTextSize(1);
      display.setCursor(5, 13);
      display.print("SELEC. VELOCIDAD");
      display.setCursor(15, 22);
      display.printf("(T:%s)", manualDurationLabels[selectedDuration]);
      display.drawLine(0, 30, 127, 30, SSD1306_WHITE);
      
      int startIdx = max(0, selectedSpeed - 1);
      int endIdx = min(3, startIdx + 2);
      
      for(int i = startIdx; i <= endIdx && i < 4; i++) {
        int yPos = 33 + ((i - startIdx) * 9);
        display.setCursor(5, yPos);
        
        if (i == selectedSpeed) {
          display.print(">");
          display.print(speedLabels[i]);
        } else {
          display.print(" ");
          display.print(speedLabels[i]);
        }
      }
      
      display.setCursor(8, 56);
      display.print("OK=Pulsar BACK=<");
      break;
      
    case MANUAL_ACTIVO:
      display.setTextSize(2);
      display.setCursor(10, 16);
      display.println("MANUAL");
      display.setCursor(15, 36);
      display.println("ACTIVO");
      
      display.setTextSize(1);
      display.setCursor(5, 52);
      int secondsLeft = manualTimeLeft / 1000;
      int minutes = secondsLeft / 60;
      int seconds = secondsLeft % 60;
      display.printf("V:%d%% T:%02d:%02d", manualFanSpeed, minutes, seconds);
      break;
      
    case PAUSA:
      display.setTextSize(2);
      display.setCursor(20, 20);
      display.println("PAUSA");
      
      display.setTextSize(1);
      display.setCursor(10, 45);
      display.print("Ventilador apagado");
      
      // Mostrar tiempo congelado si estaba en manual
      if (previousMode == MANUAL_ACTIVO) {
        display.setCursor(20, 56);
        int secLeft = pausedTimeLeft / 1000;
        int min = secLeft / 60;
        int sec = secLeft % 60;
        display.printf("Tiempo:%02d:%02d", min, sec);
      }
      break;
  }

  display.display();
}
