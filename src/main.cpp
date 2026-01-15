#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <ESP32Encoder.h>
#include <Arduino.h>

// --- PINOUT DEFINITIVO (PANTALLA OK + LEDS SEGUROS) ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define ENCODER_CLK_PIN 32
#define ENCODER_DT_PIN 33
#define ENCODER_SW_PIN 27
#define CONFIRM_BUTTON_PIN 25
#define BAK_BUTTON_PIN 13
#define MQ135_ANALOG_PIN 34
#define PWM_FAN_PIN 19
#define FAN_TACH_PIN 16

// --- LEDS EN PINES SEGUROS (LOS QUE PROBAMOS AL FINAL) ---
#define LED_RED_PIN     18  // Rojo
#define LED_YELLOW_PIN  5   // Amarillo
#define LED_GREEN_PIN   17  // Verde (lógica invertida)

// --- CONFIGURACION OLED SH1106 (1.3 pulgadas) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- OBJETOS SENSORES ---
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
ESP32Encoder encoder;

// Variables de estado
bool ventiladorActivo = false;
bool ledAmarilloState = false;
bool ledRojoState = false;
unsigned long lastUpdate = 0;

// Sistema de pantallas
int pantallaActual = 0;
const int NUM_PANTALLAS = 4;
long lastEncoderValue = 0;

// Variables tacógrafo
volatile unsigned long tachPulseCount = 0;
unsigned long lastTachCheck = 0;
int fanRPM = 0;

// ISR para tacógrafo
void IRAM_ATTR tachISR() {
  tachPulseCount++;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n--- FUSION: PANTALLA SSD1306 + LEDS SEGUROS ---");

  // 1. Configurar Salidas (LEDs)
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  
  // Apagar LEDs al inicio (lógica invertida para todos)
  digitalWrite(LED_GREEN_PIN, HIGH);  // HIGH = apagado
  digitalWrite(LED_YELLOW_PIN, HIGH); // HIGH = apagado
  digitalWrite(LED_RED_PIN, HIGH);    // HIGH = apagado
  
  // 1b. Configurar entrada tacógrafo ventilador
  pinMode(FAN_TACH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FAN_TACH_PIN), tachISR, FALLING);

  // 2. Inicializar I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(100000); // 100kHz para estabilidad

  // 3. Inicializar Pantalla SH1106
  if (!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println("Fallo SH1106");
    // Parpadeo de pánico en LED Rojo si falla pantalla (lógica invertida)
    for(;;) { digitalWrite(LED_RED_PIN, digitalRead(LED_RED_PIN) == HIGH ? LOW : HIGH); delay(100); }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println("SISTEMA OK");
  display.println("Iniciando...");
  display.display();
  delay(500);

  // 4. Inicializar Sensores con delay
  Serial.println("Iniciando sensores I2C...");
  delay(100);
  
  bool aht_ok = aht.begin();
  Serial.print("AHT20: ");
  Serial.println(aht_ok ? "OK" : "FALLO");
  
  bool bmp_ok = bmp.begin(0x77);
  Serial.print("BMP280: ");
  Serial.println(bmp_ok ? "OK" : "FALLO");

  // 5. Configurar Encoder y Botones
  encoder.attachHalfQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.setCount(0);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  pinMode(CONFIRM_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BAK_BUTTON_PIN, INPUT_PULLUP);
  
  // Test LEDs al inicio (lógica invertida: LOW=ON, HIGH=OFF)
  Serial.println("Test LEDs...");
  digitalWrite(LED_RED_PIN, LOW);    // encender
  delay(300);
  digitalWrite(LED_RED_PIN, HIGH);   // apagar
  digitalWrite(LED_YELLOW_PIN, LOW); // encender
  delay(300);
  digitalWrite(LED_YELLOW_PIN, HIGH);// apagar
  digitalWrite(LED_GREEN_PIN, LOW);  // encender
  delay(300);
  digitalWrite(LED_GREEN_PIN, HIGH); // apagar
  
  Serial.println("Listo para testear.");
}

void loop() {
  if (millis() - lastUpdate > 200) {
    lastUpdate = millis();

    // -- LECTURA DE BOTONES --
    bool btnEncoder = !digitalRead(ENCODER_SW_PIN);
    bool btnConfirm = !digitalRead(CONFIRM_BUTTON_PIN);
    bool btnPausa   = !digitalRead(BAK_BUTTON_PIN);

    // Antirebote para botones
    static bool lastBtnEncoder = false;
    static bool lastBtnConfirm = false;
    static bool lastBtnPausa = false;

    // -- LÓGICA DE BOTONES Y LEDs --
    
    // 1. Pulsar Encoder -> Toggle Verde (Simula Ventilador)
    if (btnEncoder && !lastBtnEncoder) {
      ventiladorActivo = !ventiladorActivo;
      digitalWrite(LED_GREEN_PIN, ventiladorActivo ? LOW : HIGH);
      Serial.print("LED Verde: ");
      Serial.println(ventiladorActivo ? "ON" : "OFF");
      delay(200);
    }

    // 2. Pulsar Confirm -> Toggle Amarillo
    if (btnConfirm && !lastBtnConfirm) {
      ledAmarilloState = !ledAmarilloState;
      digitalWrite(LED_YELLOW_PIN, ledAmarilloState ? LOW : HIGH);
      Serial.print("LED Amarillo: ");
      Serial.println(ledAmarilloState ? "ON" : "OFF");
      delay(200);
    }

    // 3. Pulsar BAK -> Toggle Rojo
    if (btnPausa && !lastBtnPausa) {
      ledRojoState = !ledRojoState;
      digitalWrite(LED_RED_PIN, ledRojoState ? LOW : HIGH);
      Serial.print("LED Rojo: ");
      Serial.println(ledRojoState ? "ON" : "OFF");
      delay(200);
    }

    lastBtnEncoder = btnEncoder;
    lastBtnConfirm = btnConfirm;
    lastBtnPausa = btnPausa;

    // 4. Calcular RPM del ventilador (cada 1 segundo)
    if (millis() - lastTachCheck >= 1000) {
      noInterrupts();
      unsigned long pulses = tachPulseCount;
      tachPulseCount = 0;
      interrupts();
      
      // Ventiladores 4-hilos: 2 pulsos por revolución
      fanRPM = (pulses * 60) / 2;
      lastTachCheck = millis();
    }

    // -- LECTURA DE SENSORES --
    float temp = 0, hum = 0;
    int mq135 = 0;
    long encVal = encoder.getCount() / 2;

    // Intentar leer AHT20
    sensors_event_t h, t;
    if (aht.getEvent(&h, &t)) {
      if (!isnan(t.temperature) && !isnan(h.relative_humidity)) {
        temp = t.temperature;
        hum = h.relative_humidity;
      }
    }
    
    // Leer MQ135
    mq135 = analogRead(MQ135_ANALOG_PIN);

    // -- ACTUALIZAR PANTALLA --
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    
    // Fila 1: Temp y Humedad
    display.print("T:");
    if (temp > 0) {
      display.print(temp, 1);
    } else {
      display.print("--");
    }
    display.print(" H:");
    if (hum > 0) {
      display.print((int)hum);
    } else {
      display.print("--");
    }
    display.println("%");
    
    // Fila 2: MQ135 y RPM
    display.print("Air:"); display.print(mq135);
    display.print(" RPM:"); display.println(fanRPM);

    // Fila 3: Encoder
    display.print("Enc:"); display.println(encVal);

    // Fila 4: Divisor
    display.println("----");
    
    // Fila 5: Estado Botones/LEDs
    display.print("G:"); display.print(ventiladorActivo ? "ON" : ".");
    display.print(" Y:"); display.print(ledAmarilloState ? "ON" : ".");
    display.print(" R:"); display.println(btnPausa ? "ON" : ".");
    
    display.display();
  }
}
