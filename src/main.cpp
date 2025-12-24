/*
  Extractor Inteligente para Bano/Galeria v7.1C FINAL
  Hardware: ESP32 38-pin + Shield + AHT20/BMP280 + MQ135 + OLED 3-Botones + LEDs Estado
  Mejoras v7.1C: Ajustes cosmeticos finales y rangos ampliados.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <ESP32Encoder.h>
#include <esp_task_wdt.h>
#include <Preferences.h>

// --- CONFIGURACION HARDWARE ---
#define WDT_TIMEOUT 8 

// OLED 1.3" SH1106
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

// Sensores
Adafruit_BMP280 bmp;
Adafruit_AHTX0 aht;
#define MQ135_PIN 34
#define MQ135_BASELINE 350 

// Encoder
#define ENCODER_TRA_PIN  32  
#define ENCODER_TRB_PIN  33  
#define ENCODER_PUSH_PIN 27  
ESP32Encoder encoder;

// Botones
#define BTN_CONFIRM_PIN  25  
#define BTN_BAK_PIN      26  

// Actuadores
#define RELAY_PIN    23  
#define FAN_PWM_PIN  19  
#define PWM_CHANNEL  0
#define PWM_FREQUENCY 25000
#define PWM_RESOLUTION 8
#define PWM_MIN_VALUE 51   
#define PWM_MAX_VALUE 255  

// LEDS Estado
#define LED_RED_PIN   4     
#define LED_GREEN_PIN 15    

// --- UMBRALES ---
#define HUMIDITY_THRESHOLD_HIGH 70.0f
#define HUMIDITY_THRESHOLD_LOW  65.0f
#define TEMP_THRESHOLD          30.0f
#define AIR_QUALITY_THRESHOLD   600 

// --- ESTADOS Y VARIABLES ---
enum Mode { AUTOMATICO, SELECCION_TIEMPO, SELECCION_VELOCIDAD, MANUAL_ACTIVO, PAUSA, DEBUG_INFO, SENSORS_FAIL };
Mode currentMode = AUTOMATICO;
Mode previousMode = AUTOMATICO;

float temperature = 0.0;
float humidity = 0.0;
int airQualityRaw = 0;
int airQualityScore = 0; 
bool bmpReady = false;
bool ahtReady = false;
bool oledWorking = true;

// Throttling timers
unsigned long lastSensorReadTime = 0;
unsigned long lastDisplayUpdateTime = 0;
const unsigned long SENSOR_READ_INTERVAL = 2000;
const unsigned long DISPLAY_UPDATE_INTERVAL = 250;

const long manualDurations[] = { 30 * 60000, 60 * 60000, 90 * 60000 };
const char* manualDurationLabels[] = {"30 MIN", "60 MIN", "90 MIN"};
int selectedDuration = 0;

const int speedOptions[] = { 25, 50, 75, 100 };
const char* speedLabels[] = {"25% (Baja)", "50% (Media)", "75% (Alta)", "100% (Max)"};
int selectedSpeed = 1; 

// Timer Variables (Overflow Safe)
unsigned long manualTimerStartTime = 0;
unsigned long manualDurationMillis = 0;
unsigned long pausedTimeLeft = 0; 
unsigned long manualTimeLeft = 0; 

int manualFanSpeed = 0;
int pausedFanSpeed = 0;
long oldEncoderPosition = 0;

// Debounce Variables 
volatile bool btnOkPressed = false;          
volatile bool btnBackPressed = false;        
volatile bool btnPausePressed = false;       
unsigned long lastOkPressTime = 0;
unsigned long lastBackPressTime = 0;
unsigned long lastPausePressTime = 0;
unsigned long pausePressStartTime = 0;
bool pauseLongPressDetected = false;
const long debounceDelay = 250;
const long longPressDelay = 2000;   

int currentFanSpeed = 0;
bool isPaused = false;

// --- PROTOTIPOS ---
void handleControls();
void readSensors();
void runLogic();
void controlFan(int percentage);
void updateDisplay();
void updateLeds();
void formatTime(unsigned long ms, int &m, int &s);

void setup() {
  Serial.begin(115200);
  
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, HIGH); delay(300); digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, HIGH); delay(300); digitalWrite(LED_GREEN_PIN, LOW);
  
  pinMode(ENCODER_PUSH_PIN, INPUT_PULLUP);
  pinMode(BTN_CONFIRM_PIN, INPUT_PULLUP);
  pinMode(BTN_BAK_PIN, INPUT_PULLUP);

  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0);

  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(ENCODER_TRA_PIN, ENCODER_TRB_PIN);
  encoder.setCount(0);
  
  if(!display.begin(0x3C, true)) {
    Serial.println("OLED Fail - Blind Mode");
    oledWorking = false;
    for(int i=0; i<3; i++) { digitalWrite(LED_RED_PIN, HIGH); delay(100); digitalWrite(LED_RED_PIN, LOW); delay(100); }
  }
  
  if (oledWorking) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(10, 10);
      display.println("INICIANDO");
      display.setCursor(10, 35);
      display.println("V7.1C...");
      display.display();
  }

  bmpReady = bmp.begin(0x76);
  if (!bmpReady) bmpReady = bmp.begin(0x77);
  ahtReady = aht.begin();
  
  if (!bmpReady && !ahtReady) {
     Serial.println("FALLO CRITICO SENSORES");
     currentMode = SENSORS_FAIL;
  }
  
  delay(1500);
  
  if (currentMode != SENSORS_FAIL) {
      for(int i = 5; i > 0; i--) {
        if (oledWorking) {
            display.clearDisplay();
            display.setTextSize(1);
            display.setCursor(10, 20);
            display.println("Calibrando Aire...");
            display.setCursor(50, 40);
            display.printf("%ds", i);
            display.display();
        }
        esp_task_wdt_reset();
        delay(1000);
      }
  }
}

void loop() {
  esp_task_wdt_reset();

  // Critical fast path (Input & Logic)
  handleControls();
  runLogic();
  updateLeds();
  
  // Throttled path (Sensors)
  if (millis() - lastSensorReadTime > SENSOR_READ_INTERVAL) {
    readSensors();
    lastSensorReadTime = millis();
  }

  // Throttled path (Display)
  if (oledWorking) {
      if (millis() - lastDisplayUpdateTime > DISPLAY_UPDATE_INTERVAL) {
        updateDisplay();
        lastDisplayUpdateTime = millis();
      }
  } else {
      if (millis() % 2000 < 100) digitalWrite(LED_RED_PIN, HIGH);
  }

  delay(10); // Reduce blocking delay for better responsiveness
}

void updateLeds() {
    bool fanIsOn = (currentFanSpeed > 0);
    if (currentMode == SENSORS_FAIL) {
        digitalWrite(LED_GREEN_PIN, LOW);
        digitalWrite(LED_RED_PIN, HIGH); 
    } else {
        if (fanIsOn) {
            digitalWrite(LED_GREEN_PIN, HIGH);
            digitalWrite(LED_RED_PIN, LOW);
        } else {
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_RED_PIN, HIGH); 
        }
    }
}

void handleControls() {
  if (digitalRead(ENCODER_PUSH_PIN) == LOW && (millis() - lastOkPressTime > debounceDelay)) {
    lastOkPressTime = millis();
    btnOkPressed = true;
  }
  if (digitalRead(BTN_CONFIRM_PIN) == LOW && (millis() - lastBackPressTime > debounceDelay)) {
    lastBackPressTime = millis();
    btnBackPressed = true;
  }
  
  bool bakPressed = (digitalRead(BTN_BAK_PIN) == LOW);
  if (bakPressed) {
    if (pausePressStartTime == 0) {
      pausePressStartTime = millis();
      pauseLongPressDetected = false;
    } else if (!pauseLongPressDetected && (millis() - pausePressStartTime > longPressDelay)) {
      pauseLongPressDetected = true;
      btnPausePressed = true;
    }
  } else {
    pausePressStartTime = 0;
    pauseLongPressDetected = false;
  }

  if (currentMode == SELECCION_TIEMPO || currentMode == SELECCION_VELOCIDAD) {
    long newPos = encoder.getCount() / 2;
    if (newPos != oldEncoderPosition) {
      if (currentMode == SELECCION_TIEMPO) {
        selectedDuration += (newPos > oldEncoderPosition) ? 1 : -1;
        if (selectedDuration > 2) selectedDuration = 0;
        if (selectedDuration < 0) selectedDuration = 2;
      } else {
        selectedSpeed += (newPos > oldEncoderPosition) ? 1 : -1;
        if (selectedSpeed > 3) selectedSpeed = 0;
        if (selectedSpeed < 0) selectedSpeed = 3;
      }
      oldEncoderPosition = newPos;
    }
  } else if (currentMode == AUTOMATICO) {
      long newPos = encoder.getCount() / 2;
      if (abs(newPos - oldEncoderPosition) > 0) {
          currentMode = DEBUG_INFO;
          oldEncoderPosition = newPos;
      }
  }
}

void readSensors() {
  if (currentMode == SENSORS_FAIL) return;

  float t_sum = 0;
  int t_count = 0;
  float h_sum = 0;
  int h_count = 0;

  // Sanity Check Limits
  const float MIN_TEMP = -40.0; // Rango real
  const float MAX_TEMP = 85.0;
  const float MIN_HUM = 0.0;
  const float MAX_HUM = 100.0;

  if (bmpReady) {
      float t = bmp.readTemperature();
      if (!isnan(t) && t > MIN_TEMP && t < MAX_TEMP) { t_sum += t; t_count++; }
  }

  if (ahtReady) {
      sensors_event_t hum, temp;
      aht.getEvent(&hum, &temp);
      if (!isnan(temp.temperature) && temp.temperature > MIN_TEMP && temp.temperature < MAX_TEMP) { 
          t_sum += temp.temperature; t_count++; 
      }
      if (!isnan(hum.relative_humidity) && hum.relative_humidity >= MIN_HUM && hum.relative_humidity <= MAX_HUM) { 
          h_sum += hum.relative_humidity; h_count++; 
      }
  }

  if (t_count > 0) temperature = t_sum / t_count;
  else temperature = 25.0; // Fail-Safe

  if (h_count > 0) humidity = h_sum / h_count;
  else humidity = 50.0; // Fail-Safe

  int raw = analogRead(MQ135_PIN);
  if (raw >= 0 && raw <= 4095) airQualityRaw = raw;
  else airQualityRaw = 0; 

  // Mapea RAW (350-4095) a Score (0-1000). 250+ activa ventilador (~600 RAW aprox)
  if (airQualityRaw <= MQ135_BASELINE) {
      airQualityScore = 0;
  } else {
      airQualityScore = map(airQualityRaw, MQ135_BASELINE, 4095, 0, 1000);
  }
}

void runLogic() {
  if (currentMode == SENSORS_FAIL) {
      controlFan(0);
      return; 
  }

  if (btnPausePressed) {
    btnPausePressed = false;
    isPaused = !isPaused;
    if (isPaused) {
      previousMode = currentMode;
      if (currentMode == MANUAL_ACTIVO) { 
          unsigned long elapsed = millis() - manualTimerStartTime;
          if (elapsed < manualDurationMillis) {
              pausedTimeLeft = manualDurationMillis - elapsed;
          } else {
              pausedTimeLeft = 0;
          }
          pausedFanSpeed = manualFanSpeed; 
      }
      controlFan(0);
      currentMode = PAUSA;
    } else {
      currentMode = previousMode;
      if (currentMode == MANUAL_ACTIVO) {
        manualDurationMillis = pausedTimeLeft; 
        manualTimerStartTime = millis(); 
        manualFanSpeed = pausedFanSpeed;
        controlFan(manualFanSpeed);
      }
    }
    return;
  }

  if (isPaused) return;

  if (btnOkPressed) {
    btnOkPressed = false;
    if (currentMode == AUTOMATICO) { currentMode = SELECCION_TIEMPO; selectedDuration = 0; encoder.setCount(0); oldEncoderPosition = 0; controlFan(0); }
    else if (currentMode == DEBUG_INFO) { currentMode = AUTOMATICO; }
    else if (currentMode == SELECCION_TIEMPO) { currentMode = SELECCION_VELOCIDAD; selectedSpeed = 1; encoder.setCount(2); oldEncoderPosition = 1; }
    else if (currentMode == SELECCION_VELOCIDAD) { 
        currentMode = MANUAL_ACTIVO; 
        manualTimerStartTime = millis(); 
        manualDurationMillis = manualDurations[selectedDuration]; 
        manualFanSpeed = speedOptions[selectedSpeed]; 
        controlFan(manualFanSpeed); 
    }
    else if (currentMode == MANUAL_ACTIVO) { currentMode = AUTOMATICO; controlFan(0); }
  }

  if (btnBackPressed) {
    btnBackPressed = false;
    if (currentMode == SELECCION_TIEMPO) currentMode = AUTOMATICO;
    else if (currentMode == DEBUG_INFO) currentMode = AUTOMATICO;
    else if (currentMode == SELECCION_VELOCIDAD) { currentMode = SELECCION_TIEMPO; encoder.setCount(selectedDuration * 2); oldEncoderPosition = selectedDuration; }
    else if (currentMode == MANUAL_ACTIVO) { currentMode = AUTOMATICO; controlFan(0); }
  }

  if (currentMode == AUTOMATICO || currentMode == DEBUG_INFO) {
    int speed = 0;
    if (humidity >= HUMIDITY_THRESHOLD_HIGH) speed = 100;
    else if (humidity >= HUMIDITY_THRESHOLD_LOW) speed = 70;
    else if (temperature >= TEMP_THRESHOLD) speed = 60;
    else if (airQualityScore >= 250) speed = 40; 
    controlFan(speed);
  } else if (currentMode == MANUAL_ACTIVO) {
    unsigned long elapsed = millis() - manualTimerStartTime;
    if (elapsed >= manualDurationMillis) {
        currentMode = AUTOMATICO;
        controlFan(0);
    } else {
        manualTimeLeft = manualDurationMillis - elapsed;
    }
  }
}

void controlFan(int percentage) {
  if (percentage == currentFanSpeed) return;
  currentFanSpeed = percentage;
  if (percentage <= 0) { digitalWrite(RELAY_PIN, LOW); ledcWrite(PWM_CHANNEL, 0); }
  else { digitalWrite(RELAY_PIN, HIGH); ledcWrite(PWM_CHANNEL, map(percentage, 1, 100, PWM_MIN_VALUE, PWM_MAX_VALUE)); }
}

void formatTime(unsigned long ms, int &m, int &s) {
    unsigned long totSec = ms / 1000;
    m = (int)(totSec / 60);
    s = (int)(totSec % 60);
    if(m > 99) m = 99;
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  
  if (currentMode == SENSORS_FAIL) {
      display.setTextSize(2); 
      display.setCursor(10, 5); display.println("! ERROR !");
      display.setTextSize(1); 
      display.setCursor(5, 30); display.println("Fallo de Sensores:");
      display.setCursor(5, 45);
      if (!bmpReady && !ahtReady) display.println("TOTAL FALLO");
      else if (!bmpReady) display.println("Solo AHT20 OK"); // BMP fallo
      else display.println("Solo BMP OK"); // AHT fallo
      display.display();
      return;
  }

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("T:%.1fC H:%.0f%% A:%d", temperature, humidity, airQualityScore);
  
  display.setCursor(95, 0);
  if (airQualityScore < 200) display.print("OK");
  else display.print("!!");
  
  display.drawLine(0, 10, 127, 10, SH110X_WHITE);
  
  int m, s;

  if (currentMode == AUTOMATICO) {
    display.setTextSize(2); display.setCursor(15, 20); display.println("MODO AUTO");
    display.setTextSize(1); display.setCursor(30, 50);
    if (currentFanSpeed > 0) display.printf("VENT: %d%%", currentFanSpeed); else display.print("[STANDBY]");
  } else if (currentMode == SELECCION_TIEMPO) {
    display.setCursor(10, 15); display.println("ELIGA TIEMPO:");
    for(int i=0; i<3; i++) { display.setCursor(25, 30+i*10); if(i==selectedDuration) display.printf("> %s <", manualDurationLabels[i]); else display.printf("  %s", manualDurationLabels[i]); }
  } else if (currentMode == SELECCION_VELOCIDAD) {
    display.setCursor(10, 15); display.println("VELOCIDAD:");
    int sIdx = max(0, selectedSpeed - 1);
    for(int i=sIdx; i<min(sIdx+3, 4); i++) { display.setCursor(10, 30+(i-sIdx)*10); if(i==selectedSpeed) display.printf("> %s", speedLabels[i]); else display.printf("  %s", speedLabels[i]); }
  } else if (currentMode == MANUAL_ACTIVO) {
    display.setTextSize(2); display.setCursor(10, 20); display.println("MANUAL");
    display.setTextSize(1); display.setCursor(10, 45);
    formatTime(manualTimeLeft, m, s);
    display.printf("V:%d%% T:%02d:%02d", manualFanSpeed, m, s);
  } else if (currentMode == PAUSA) {
    display.setTextSize(2); display.setCursor(30, 25); display.println("PAUSA");
  } else if (currentMode == DEBUG_INFO) {
    display.setTextSize(1);
    display.setCursor(0, 20); display.printf("MQ RAW: %d SC: %d", airQualityRaw, airQualityScore);
    display.setCursor(0, 30); display.printf("PWM: %d (Pin 19)", map(currentFanSpeed, 1, 100, PWM_MIN_VALUE, PWM_MAX_VALUE));
    display.setCursor(0, 40); display.printf("BMP: %s | AHT: %s", bmpReady?"OK":"NO", ahtReady?"OK":"NO");
    display.setCursor(0, 50); display.print("[OK] to Exit");
  }
  display.display();
}
