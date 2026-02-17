#include <Arduino.h>

// ============================================
// TEST FINAL: BAJA FRECUENCIA (SILENCIO?)
// ============================================
// Intentamos bajar las RPM minimas usando PWM de muy baja frecuencia (20Hz).
// Algunos ventiladores Delta interpretan esto como "Low Speed Mode".

#define PIN_RELAY 23
#define PIN_FAN_PWM 19
#define PIN_LED_RED 18
#define PIN_LED_YELLOW 5

#define PIN_FAN_TACH 35

#define FAN_PWM_CHANNEL 0
// CAMBIO RADICAL: 20 Hz (Muy baja)
#define FAN_PWM_FREQ 20
#define FAN_PWM_RES 8

volatile int tachPulses = 0;
unsigned long lastRPMCalc = 0;
int rpm = 0;

void IRAM_ATTR tachISR() { tachPulses++; }

int testState = 0;
unsigned long lastStateChange = 0;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);

  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH); // SIEMPRE ON (Para probar PWM solo)

  ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQ, FAN_PWM_RES);
  ledcAttachPin(PIN_FAN_PWM, FAN_PWM_CHANNEL);

  // MINIMA VELOCIDAD (Logic Low 100% time -> GND -> PWM 0%)
  // Si Freq es 20Hz, son pulsos de 50ms.
  ledcWrite(FAN_PWM_CHANNEL, 255);

  pinMode(PIN_FAN_TACH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_FAN_TACH), tachISR, FALLING);

  Serial.println("=== TEST 20Hz LOW FREQUENCY PWM ===");
}

void loop() {
  unsigned long now = millis();

  if (now - lastRPMCalc >= 1000) {
    detachInterrupt(digitalPinToInterrupt(PIN_FAN_TACH));
    int pulses = tachPulses;
    tachPulses = 0;
    attachInterrupt(digitalPinToInterrupt(PIN_FAN_TACH), tachISR, FALLING);
    rpm = (pulses / 2) * 60;

    Serial.print("Estado: ");
    Serial.print(testState);
    Serial.print(" | PWM: ");
    if (testState == 0)
      Serial.print("255 (MIN)");
    if (testState == 1)
      Serial.print("128 (MID)");
    if (testState == 2)
      Serial.print("0 (MAX)");
    Serial.print(" | RPM: ");
    Serial.println(rpm);
    lastRPMCalc = now;
  }

  if (now - lastStateChange > 5000) {
    lastStateChange = now;
    testState++;
    if (testState > 2)
      testState = 0;

    switch (testState) {
    case 0: // MINIMA (255)
      ledcWrite(FAN_PWM_CHANNEL, 255);
      digitalWrite(PIN_LED_YELLOW, LOW);
      digitalWrite(PIN_LED_RED, HIGH);
      break;
    case 1: // MEDIA (128)
      ledcWrite(FAN_PWM_CHANNEL, 128);
      digitalWrite(PIN_LED_YELLOW, HIGH);
      digitalWrite(PIN_LED_RED, LOW);
      break;
    case 2: // MAXIMA (0)
      ledcWrite(FAN_PWM_CHANNEL, 0);
      digitalWrite(PIN_LED_YELLOW, HIGH);
      digitalWrite(PIN_LED_RED, HIGH);
      break;
    }
  }
}
