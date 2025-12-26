#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // USAMOS ESTA LIBRERIA PORQUE ES LA QUE FUNCIONA
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
#define BAK_BUTTON_PIN 26
#define MQ135_ANALOG_PIN 34
#define RELAY_PIN 23
#define PWM_FAN_PIN 19

// --- LEDS EN PINES SEGUROS (LOS QUE PROBAMOS AL FINAL) ---
#define LED_RED_PIN     18  // Rojo
#define LED_YELLOW_PIN  5   // Amarillo
#define LED_GREEN_PIN   17  // Verde

// --- CONFIGURACION OLED SSD1306 (LA QUE FUNCIONA) ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- OBJETOS SENSORES ---
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
ESP32Encoder encoder;

// Variables de estado
bool ventiladorActivo = false;
bool ledAmarilloState = false;

// Variables de tiempo para multithreading cooperativo
unsigned long lastDisplayUpdate = 0;
unsigned long lastSensorUpdate = 0;
unsigned long lastDebounceTime = 0;

// Variables globales de sensores (cacheadas)
float tempC = 0.0;
float humi = 0.0;
int mq135Val = 0;
long encVal = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("\n--- FUSION: PANTALLA SSD1306 + LEDS SEGUROS ---");

  // 1. Configurar Salidas (LEDs y Relé)
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Apagar todo al inicio
  digitalWrite(LED_GREEN_PIN, LOW);
  digitalWrite(LED_YELLOW_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  // 2. Inicializar I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // 3. Inicializar Pantalla (CODIGO DEL USUARIO QUE FUNCIONA)
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Fallo SSD1306");
    // Parpadeo de pánico en LED Rojo si falla pantalla
    for(;;) { digitalWrite(LED_RED_PIN, !digitalRead(LED_RED_PIN)); delay(100); }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE); // Usar constante de SSD1306
  display.setCursor(0,0);
  display.println("SISTEMA OK");
  display.println("SSD1306 + LEDS");
  display.display();
  delay(1000);

  // 4. Inicializar Sensores
  if (!aht.begin()) Serial.println("Fallo AHT20");
  if (!bmp.begin(0x77)) Serial.println("Fallo BMP280");

  // 5. Configurar Encoder y Botones
  encoder.attachHalfQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.setCount(0);
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  pinMode(CONFIRM_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BAK_BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Listo para testear.");
}

void loop() {
  unsigned long now = millis();

  // -- 1. LECTURA DE BOTONES (Rápido - Sin delays) --
  bool btnEncoder = !digitalRead(ENCODER_SW_PIN);
  bool btnConfirm = !digitalRead(CONFIRM_BUTTON_PIN);
  bool btnPausa   = !digitalRead(BAK_BUTTON_PIN);

  // Debounce no bloqueante (300ms)
  if (now - lastDebounceTime > 300) {
      // 1. Pulsar Encoder -> Toggle Verde y Relé
      if (btnEncoder) {
        ventiladorActivo = !ventiladorActivo;
        digitalWrite(LED_GREEN_PIN, ventiladorActivo);
        digitalWrite(RELAY_PIN, ventiladorActivo);
        lastDebounceTime = now;
      }

      // 2. Pulsar Confirm -> Toggle Amarillo
      if (btnConfirm) {
        ledAmarilloState = !ledAmarilloState;
        digitalWrite(LED_YELLOW_PIN, ledAmarilloState);
        lastDebounceTime = now;
      }
  }

  // 3. Pulsar Pausa (Momentáneo) -> Rojo mientras se pulsa
  digitalWrite(LED_RED_PIN, btnPausa);


  // -- 2. LECTURA SENSORES (Lento - 0.5Hz / 2000ms) --
  if (now - lastSensorUpdate > 2000) {
    lastSensorUpdate = now;

    sensors_event_t h, t;
    aht.getEvent(&h, &t);
    tempC = t.temperature;
    humi = h.relative_humidity;
    mq135Val = analogRead(MQ135_ANALOG_PIN);
  }


  // -- 3. ACTUALIZAR PANTALLA (Medio - 4Hz / 250ms) --
  if (now - lastDisplayUpdate > 250) {
    lastDisplayUpdate = now;

    // Encoder lectura
    encVal = encoder.getCount() / 2;

    display.clearDisplay();
    display.setCursor(0,0);
    
    // Fila 1: Temp y Humedad (Valores cacheados)
    display.print("T:"); display.print(tempC, 1);
    display.print(" H:"); display.print(humi, 0); display.println("%");
    
    // Fila 2: MQ135 y Encoder
    display.print("Air:"); display.print(mq135Val);
    display.print(" Enc:"); display.println(encVal);

    // Fila 3: Estado Botones/LEDs
    display.println("----------------");
    display.print("Fan(G):"); display.println(ventiladorActivo ? "ON" : "OFF");
    display.print("Yel(C):"); display.print(ledAmarilloState ? "ON" : "OFF");
    display.print(" Red(B):"); display.println(btnPausa ? "ON" : ".");
    
    display.display();
  }
}
