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

// Variables sensores (globales para acceso desde pantallas)
float temperatura = 0;
float humedad = 0;
float presion = 0;
int calidadAire = 0;

// ISR para tacógrafo
void IRAM_ATTR tachISR() {
  tachPulseCount++;
}

// ============ FUNCIONES DE PANTALLAS ============

void drawPantallaPrincipal() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  // Título
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("EXTRACTOR");
  
  display.setTextSize(1);
  display.println("");
  
  // Datos principales
  display.print("Temp: ");
  display.print(temperatura, 1);
  display.println(" C");
  
  display.print("Hum:  ");
  display.print((int)humedad);
  display.println(" %");
  
  display.print("Aire: ");
  display.println(calidadAire);
  
  // Indicador de pantalla
  display.setCursor(110, 56);
  display.print("1/4");
  
  display.display();
}

void drawPantallaTemperatura() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  // Título
  display.setCursor(0, 0);
  display.println("TEMPERATURA");
  display.println("----------");
  
  // Temperatura grande
  display.setTextSize(3);
  display.setCursor(10, 18);
  display.print(temperatura, 1);
  display.setTextSize(2);
  display.print("C");
  
  // Humedad y Presión en posiciones fijas
  display.setTextSize(1);
  display.setCursor(0, 42);
  display.print("Humedad: ");
  display.print((int)humedad);
  display.println("%");
  
  display.setCursor(0, 51);
  display.print("Presion: ");
  display.print(presion, 0);
  display.print("hPa");
  
  // Indicador
  display.setCursor(110, 56);
  display.print("2/4");
  
  display.display();
}

void drawPantallaCalidadAire() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.println("CALIDAD AIRE");
  display.println("----------");
  
  // Valor MQ135
  display.setTextSize(3);
  display.setCursor(20, 20);
  display.println(calidadAire);
  
  // Evaluación
  display.setTextSize(1);
  display.print("Estado: ");
  if (calidadAire < 400) {
    display.println("BUENO");
  } else if (calidadAire < 1000) {
    display.println("MEDIO");
  } else {
    display.println("MALO");
  }
  
  // Referencia (solo una línea)
  display.println("");
  display.println("<400:OK  >1K:Malo");
  
  display.setCursor(110, 56);
  display.print("3/4");
  
  display.display();
}

void drawPantallaSistema() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.println("SISTEMA");
  display.println("----------");
  
  // Estados LEDs
  display.print("Verde:  ");
  display.println(ventiladorActivo ? "ON " : "OFF");
  
  display.print("Amarillo: ");
  display.println(ledAmarilloState ? "ON " : "OFF");
  
  display.print("Rojo:   ");
  display.println(ledRojoState ? "ON " : "OFF");
  
  display.println("");
  
  // Encoder
  display.print("Encoder: ");
  display.println(encoder.getCount() / 2);
  
  // RPM ventilador
  display.print("RPM: ");
  display.println(fanRPM);
  
  display.setCursor(110, 56);
  display.print("4/4");
  
  display.display();
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

    // -- NAVEGACIÓN CON ENCODER --
    long currentEncoderValue = encoder.getCount() / 2;
    if (currentEncoderValue != lastEncoderValue) {
      int diff = currentEncoderValue - lastEncoderValue;
      pantallaActual += diff;
      
      // Circular: volver al inicio o al final
      if (pantallaActual < 0) pantallaActual = NUM_PANTALLAS - 1;
      if (pantallaActual >= NUM_PANTALLAS) pantallaActual = 0;
      
      Serial.print("Pantalla: ");
      Serial.println(pantallaActual);
      
      lastEncoderValue = currentEncoderValue;
    }

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
    // Intentar leer AHT20
    sensors_event_t h, t;
    if (aht.getEvent(&h, &t)) {
      if (!isnan(t.temperature) && !isnan(h.relative_humidity)) {
        temperatura = t.temperature;
        humedad = h.relative_humidity;
      }
    }
    
    // Leer BMP280 (presión)
    presion = bmp.readPressure() / 100.0F; // convertir a hPa
    
    // Leer MQ135
    calidadAire = analogRead(MQ135_ANALOG_PIN);

    // -- DIBUJAR PANTALLA ACTUAL --
    switch (pantallaActual) {
      case 0:
        drawPantallaPrincipal();
        break;
      case 1:
        drawPantallaTemperatura();
        break;
      case 2:
        drawPantallaCalidadAire();
        break;
      case 3:
        drawPantallaSistema();
        break;
    }
  }
}
