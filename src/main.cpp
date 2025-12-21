/*
  Extractor Inteligente para Bano/Galeria v6.1C
  Hardware: ESP32 38-pin + Shield + AHT20/BMP280 + MQ135 + OLED 3-Botones
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <ESP32Encoder.h>

// --- CONFIGURACION HARDWARE ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BMP280 bmp;
Adafruit_AHTX0 aht;
#define MQ135_PIN 34

#define ENCODER_TRA_PIN  32  
#define ENCODER_TRB_PIN  33  
#define ENCODER_PUSH_PIN 27  
ESP32Encoder encoder;

#define BTN_CONFIRM_PIN  25  
#define BTN_BAK_PIN      26  

#define RELAY_PIN    23  
#define FAN_PWM_PIN  14  
#define PWM_CHANNEL  0
#define PWM_FREQUENCY 25000
#define PWM_RESOLUTION 8
#define PWM_MIN_VALUE 51   
#define PWM_MAX_VALUE 255  
#define LED_ERROR_PIN 2

// --- UMBRALES ---
#define HUMIDITY_THRESHOLD_HIGH 70.0f
#define HUMIDITY_THRESHOLD_LOW  65.0f
#define TEMP_THRESHOLD          30.0f
#define AIR_QUALITY_THRESHOLD   600

// --- ESTADOS Y VARIABLES ---
enum Mode { AUTOMATICO, SELECCION_TIEMPO, SELECCION_VELOCIDAD, MANUAL_ACTIVO, PAUSA, DEBUG_INFO };
Mode currentMode = AUTOMATICO;
Mode previousMode = AUTOMATICO;

float temperature = 0.0;
float humidity = 0.0;
int airQuality = 0;
bool oledWorking = true;

const long manualDurations[] = { 30 * 60000, 60 * 60000, 90 * 60000 };
const char* manualDurationLabels[] = {"30 MIN", "60 MIN", "90 MIN"};
int selectedDuration = 0;

const int speedOptions[] = { 25, 50, 75, 100 };
const char* speedLabels[] = {"25% (Baja)", "50% (Media)", "75% (Alta)", "100% (Max)"};
int selectedSpeed = 1; 

unsigned long manualTimerStartTime = 0;
long manualTimeLeft = 0;
long pausedTimeLeft = 0;
int manualFanSpeed = 0;
int pausedFanSpeed = 0;
long oldEncoderPosition = 0;

bool btnOkPressed = false;          
bool btnBackPressed = false;        
bool btnPausePressed = false;       
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

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(LED_ERROR_PIN, OUTPUT);
  digitalWrite(LED_ERROR_PIN, LOW);
  
  pinMode(ENCODER_PUSH_PIN, INPUT_PULLUP);
  pinMode(BTN_CONFIRM_PIN, INPUT_PULLUP);
  pinMode(BTN_BAK_PIN, INPUT_PULLUP);

  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);
  ledcWrite(PWM_CHANNEL, 0);

  ESP32Encoder::useInternalWeakPullResistors = UP;
  encoder.attachHalfQuad(ENCODER_TRA_PIN, ENCODER_TRB_PIN);
  encoder.setCount(0);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED Fail - Continuing Blindly");
    oledWorking = false;
    // Parpadear LED de error 3 veces
    for(int i=0; i<3; i++) {
        digitalWrite(LED_ERROR_PIN, HIGH); delay(200);
        digitalWrite(LED_ERROR_PIN, LOW); delay(200);
    }
  }
  
  if (oledWorking) {
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 10);
      display.println("EXTRACTOR");
      display.setCursor(10, 35);
      display.println("V6.1C OK");
      display.display();
  }

  bool bmpStatus = bmp.begin(0x76);
  if (!bmpStatus) bmpStatus = bmp.begin(0x77);
  
  bool ahtStatus = aht.begin();
  
  if (!bmpStatus || !ahtStatus) {
     Serial.println("Sensor Error");
  }
  
  delay(2000);
  // Warm-up MQ135
  for(int i = 10; i > 0; i--) {
    if (oledWorking) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(20, 20);
        display.printf("Calibrando: %ds", i);
        display.display();
    }
    delay(1000);
  }
}

void loop() {
  handleControls();
  readSensors();
  runLogic();
  if (oledWorking) updateDisplay();
  else {
      // Si OLED falla, parpadear LED heartbeat muy lento
      if (millis() % 2000 < 100) digitalWrite(LED_ERROR_PIN, HIGH);
      else digitalWrite(LED_ERROR_PIN, LOW);
  }
  delay(50);
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
      // En modo automático, girar el encoder cambia a modo DEBUG
      long newPos = encoder.getCount() / 2;
      if (abs(newPos - oldEncoderPosition) > 0) {
          currentMode = DEBUG_INFO;
          oldEncoderPosition = newPos;
      }
  }
}

void readSensors() {
  float t_bmp = bmp.readTemperature();
  sensors_event_t hum, temp;
  aht.getEvent(&hum, &temp);
  float t_aht = temp.temperature;
  
  if (!isnan(t_bmp) && !isnan(t_aht)) temperature = (t_bmp + t_aht) / 2.0;
  else if (!isnan(t_bmp)) temperature = t_bmp;
  else temperature = t_aht;

  humidity = hum.relative_humidity;
  airQuality = analogRead(MQ135_PIN);
}

void runLogic() {
  if (btnPausePressed) {
    btnPausePressed = false;
    isPaused = !isPaused;
    if (isPaused) {
      previousMode = currentMode;
      if (currentMode == MANUAL_ACTIVO) { pausedTimeLeft = manualTimeLeft; pausedFanSpeed = manualFanSpeed; }
      controlFan(0);
      currentMode = PAUSA;
    } else {
      currentMode = previousMode;
      if (currentMode == MANUAL_ACTIVO) {
        manualTimeLeft = pausedTimeLeft;
        manualFanSpeed = pausedFanSpeed;
        manualTimerStartTime = millis() - (manualDurations[selectedDuration] - pausedTimeLeft);
        controlFan(manualFanSpeed);
      }
    }
    return;
  }

  if (isPaused) return;

  if (btnOkPressed) {
    btnOkPressed = false;
    if (currentMode == AUTOMATICO) { currentMode = SELECCION_TIEMPO; selectedDuration = 0; encoder.setCount(0); oldEncoderPosition = 0; controlFan(0); }
    else if (currentMode == DEBUG_INFO) { currentMode = AUTOMATICO; } // Salir de debug con OK
    else if (currentMode == SELECCION_TIEMPO) { currentMode = SELECCION_VELOCIDAD; selectedSpeed = 1; encoder.setCount(2); oldEncoderPosition = 1; }
    else if (currentMode == SELECCION_VELOCIDAD) { currentMode = MANUAL_ACTIVO; manualTimerStartTime = millis(); manualTimeLeft = manualDurations[selectedDuration]; manualFanSpeed = speedOptions[selectedSpeed]; controlFan(manualFanSpeed); }
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
    else if (airQuality >= AIR_QUALITY_THRESHOLD) speed = 40;
    controlFan(speed);
  } else if (currentMode == MANUAL_ACTIVO) {
    manualTimeLeft = manualDurations[selectedDuration] - (millis() - manualTimerStartTime);
    if (manualTimeLeft <= 0) { currentMode = AUTOMATICO; controlFan(0); }
  }
}

void controlFan(int percentage) {
  if (percentage == currentFanSpeed) return;
  currentFanSpeed = percentage;
  if (percentage <= 0) { digitalWrite(RELAY_PIN, LOW); ledcWrite(PWM_CHANNEL, 0); }
  else { digitalWrite(RELAY_PIN, HIGH); ledcWrite(PWM_CHANNEL, map(percentage, 1, 100, PWM_MIN_VALUE, PWM_MAX_VALUE)); }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("T:%dC H:%d%% A:%d", (int)temperature, (int)humidity, airQuality);
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
  
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
    int s = manualTimeLeft/1000; display.printf("V:%d%% T:%02d:%02d", manualFanSpeed, s/60, s%60);
  } else if (currentMode == PAUSA) {
    display.setTextSize(2); display.setCursor(30, 25); display.println("PAUSA");
  } else if (currentMode == DEBUG_INFO) {
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.printf("MQ135 RAW: %d", airQuality);
    display.setCursor(0, 30);
    // Calc aprox voltage (assuming 3.3V ref and attenuation default)
    float volt = (airQuality / 4095.0) * 3.3; 
    display.printf("Volt Aprox: %.2fV", volt);
    display.setCursor(0, 40);
    display.printf("Fan PWM: %d", map(currentFanSpeed, 1, 100, PWM_MIN_VALUE, PWM_MAX_VALUE));
    display.setCursor(0, 50);
    display.print("[OK] to Exit");
  }
  display.display();
}
