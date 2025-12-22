#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <ESP32Encoder.h>
#include <Arduino.h>

// --- PINOUT DEFINITIVO (ESP32) ---
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
#define LED_RED_PIN 4
#define LED_GREEN_PIN 15

// --- CONFIGURACION OLED ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- OBJETOS SENSORES ---
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
ESP32Encoder encoder;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nIniciando sistema Extractor Inteligente...");

  // Inicializar bus I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Serial.println("I2C iniciado.");

  // Inicializar pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Fallo al iniciar SSD1306. Verifique conexiones y direccion.");
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN, HIGH);
    for (;;) delay(10);
  }
  Serial.println("Pantalla OLED iniciada correctamente.");

  // Pantalla de inicio
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("EXTRACTOR");
  display.println("INTELIGENTE");
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.println("INICIANDO");
  display.display();

  // Inicializar sensor AHT20
  if (!aht.begin()) {
    Serial.println("Fallo al encontrar AHT20.");
  } else {
    Serial.println("Sensor AHT20 encontrado.");
  }

  // Inicializar sensor BMP280 (direccion 0x77 confirmada)
  if (!bmp.begin(0x77)) { 
    Serial.println("Fallo al encontrar BMP280 en 0x77.");
  } else {
    Serial.println("Sensor BMP280 encontrado.");
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
  }

  // Configurar encoder
  encoder.attachHalfQuad(ENCODER_CLK_PIN, ENCODER_DT_PIN);
  encoder.setCount(0);
  Serial.println("Encoder configurado.");

  // Configurar botones
  pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
  pinMode(CONFIRM_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BAK_BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Botones configurados.");

  // Configurar rele
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  // Configurar PWM del ventilador (25kHz, 8 bits)
  // Nueva API ESP32 Arduino Core 3.x
  ledcAttach(PWM_FAN_PIN, 25000, 8);
  ledcWrite(PWM_FAN_PIN, 0);
  Serial.println("PWM configurado.");

  // Configurar LEDs indicadores
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);
  Serial.println("Actuadores y LEDs configurados.");

  delay(2000);

  // Pantalla de confirmacion
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 10);
  display.println("Sistema OK!");
  display.setTextSize(2);
  display.setCursor(25, 35);
  display.println("LISTO!");
  display.display();

  digitalWrite(LED_GREEN_PIN, HIGH);
  Serial.println("\n=== Sistema inicializado correctamente ===\n");
}

void loop() {
  // Aqui ira la logica principal del sistema
  
  // Por ahora, solo leer sensores cada segundo como test
  static unsigned long lastRead = 0;
  if (millis() - lastRead > 1000) {
    lastRead = millis();
    
    // Leer AHT20
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp);
    Serial.print("AHT20 - Temp: ");
    Serial.print(temp.temperature);
    Serial.print("C, Hum: ");
    Serial.print(humidity.relative_humidity);
    Serial.println("%");
    
    // Leer BMP280
    Serial.print("BMP280 - Temp: ");
    Serial.print(bmp.readTemperature());
    Serial.print("C, Presion: ");
    Serial.print(bmp.readPressure() / 100.0);
    Serial.println(" hPa");
    
    // Leer MQ135
    int airQuality = analogRead(MQ135_ANALOG_PIN);
    Serial.print("MQ135 - Calidad aire: ");
    Serial.println(airQuality);
    
    Serial.println("---");
  }
  
  delay(10);
}
