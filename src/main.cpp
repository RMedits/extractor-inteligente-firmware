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

// ============ SISTEMA DE ESTADOS ============
enum Estado {
  AUTO,
  MANUAL_CONFIG,
  MANUAL_RUN_LIMITED,
  MANUAL_RUN_INFINITE,
  DIAGNOSTIC
};

Estado estadoActual = AUTO;

// Variables configuración manual
int manualTimeSel = 30;      // 15-120 min
int manualSpeedSel = 50;     // 25-100%
bool modoInfinito = false;   // false=limitado, true=infinito
bool nightModeEnabled = false; // false=apaga pantalla, true=siempre ON
int configStep = 0;          // 0=tiempo, 1=velocidad, 2=modo, 3=noche

// Control de pantalla en modo noche
bool pantallaDurmiendo = false; // true si pantalla está apagada en modo noche
unsigned long lastActivityTime = 0; // última actividad de usuario
const unsigned long SCREEN_TIMEOUT = 300000; // 5 minutos en milisegundos
bool ledsErrorActivos = true; // true=LEDs de error activos, false=desactivados en modo noche

// Variables de estado
bool ventiladorActivo = false;
bool ledAmarilloState = false;
bool ledRojoState = false;
unsigned long lastUpdate = 0;
unsigned long manualStartTime = 0;

// Sistema de pantallas diagnóstico
int pantallaActual = 0;
const int NUM_PANTALLAS = 4;
long lastEncoderValue = 0;

// Combo especial para diagnóstico (4 pulsaciones rápidas)
int comboCount = 0;
unsigned long lastComboTime = 0;
const int COMBO_TIMEOUT = 2000; // 2 segundos para completar combo

// Variables animaciones pantalla infinito
static unsigned long animationTime = 0;
static int scrollPos = 0;
static unsigned long lineBlinkTime = 0;
static bool lineVisible = true;
static unsigned long raulBlinkTime = 0;
static int raulBlinkPhase = 0;

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

// -------- MODO AUTO --------
void drawAutoScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10, 0);
  display.println("AUTO MODE");
  
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("T:");
  display.print(temperatura, 1);
  display.print("C H:");
  display.print((int)humedad);
  display.println("%");
  
  display.print("Aire: ");
  display.print(calidadAire);
  
  // Emoji calidad aire
  display.print(" ");
  if (calidadAire < 400) {
    display.println("[*_*]"); // Bueno
  } else if (calidadAire < 800) {
    display.println("[-_-]"); // Regular
  } else if (calidadAire < 1500) {
    display.println("[o_o]"); // Malo
  } else {
    display.println("[X_X]"); // Crítico
  }
  
  display.println("");
  display.print("Ventilador: ");
  display.println(ventiladorActivo ? "ON" : "OFF");
  
  display.println("");
  display.println("Gira para MANUAL");
  
  display.display();
}

// -------- CONFIGURACIÓN MANUAL --------
void drawManualConfigScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.println("CONFIG MANUAL");
  
  // Paso 0: Tiempo
  display.setCursor(0, 10);
  if (configStep == 0) display.print(">");
  else display.print(" ");
  display.print(" Tiempo: ");
  display.print(manualTimeSel);
  display.println(" min");
  
  // Paso 1: Velocidad
  display.setCursor(0, 20);
  if (configStep == 1) display.print(">");
  else display.print(" ");
  display.print(" Veloc:  ");
  display.print(manualSpeedSel);
  display.println("%");
  
  // Paso 2: Modo
  display.setCursor(0, 30);
  if (configStep == 2) display.print(">");
  else display.print(" ");
  display.print(" Modo:   ");
  display.println(modoInfinito ? "Infinito" : "Limitado");
  
  // Paso 3: Noche
  display.setCursor(0, 40);
  if (configStep == 3) display.print(">");
  else display.print(" ");
  display.print(" Noche:  ");
  display.println(nightModeEnabled ? "SI" : "NO");
  
  display.setCursor(0, 55);
  display.println("Click=OK Back=Auto");
  
  display.display();
}

// -------- MODO INFINITO CON ANIMACIONES --------
void drawManualInfiniteScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  // Animación: Scroll del título "EXTRACTOR TUNEADO" (500ms)
  if (millis() - animationTime > 500) {
    scrollPos++;
    if (scrollPos > 4) scrollPos = 0;
    animationTime = millis();
  }
  
  // Título animado con scroll
  display.setCursor(scrollPos, 0);
  display.println("EXTRACTOR TUNEADO");
  
  // Animación: Línea divisoria parpadeante (600ms)
  if (millis() - lineBlinkTime > 600) {
    lineVisible = !lineVisible;
    lineBlinkTime = millis();
  }
  
  if (lineVisible) {
    display.setCursor(0, 9);
    display.println("-------------------");
  }
  
  // Modo manual infinito
  display.setCursor(0, 18);
  display.print("[");
  display.write(236); // Símbolo infinito
  display.println("] MANUAL INFINITO");
  
  // Barra de velocidad
  display.setCursor(0, 27);
  display.print("[");
  int barFill = map(manualSpeedSel, 0, 100, 0, 9);
  for (int i = 0; i < barFill; i++) display.write(219); // █
  for (int i = barFill; i < 9; i++) display.write(176); // ░
  display.print("]");
  display.print(manualSpeedSel);
  display.println("%");
  
  // Datos sensores
  display.setCursor(0, 36);
  display.write(15); // ★
  display.print(" T:");
  display.print(temperatura, 0);
  display.write(167); // °
  display.print(" H:");
  display.print((int)humedad);
  display.println("%");
  
  display.setCursor(0, 45);
  display.print("Aire: ");
  display.print(calidadAire);
  display.print(" - ");
  if (calidadAire < 400) {
    display.println("OK");
  } else if (calidadAire < 1000) {
    display.println("MEDIO");
  } else {
    display.println("MALO");
  }
  
  // Animación: "BY RAUL" pulsante (300ms on-on-off-off)
  if (millis() - raulBlinkTime > 300) {
    raulBlinkPhase++;
    if (raulBlinkPhase > 3) raulBlinkPhase = 0;
    raulBlinkTime = millis();
  }
  
  if (raulBlinkPhase < 2) {
    display.setCursor(82, 56);
    display.print("BY RAUL");
  }
  
  display.display();
}

// -------- MODO LIMITADO --------
void drawManualLimitedScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.println("MANUAL RUN");
  
  // Tiempo restante
  unsigned long elapsed = (millis() - manualStartTime) / 1000;
  unsigned long remaining = (manualTimeSel * 60) - elapsed;
  int minRemain = remaining / 60;
  int secRemain = remaining % 60;
  
  display.setTextSize(2);
  display.setCursor(20, 20);
  if (minRemain < 10) display.print("0");
  display.print(minRemain);
  display.print(":");
  if (secRemain < 10) display.print("0");
  display.print(secRemain);
  
  // Velocidad
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.print("Velocidad: ");
  display.print(manualSpeedSel);
  display.println("%");
  
  display.setCursor(0, 55);
  display.println("BACK para cancelar");
  
  display.display();
}

// -------- PANTALLAS DIAGNÓSTICO --------

// -------- PANTALLAS DIAGNÓSTICO --------

void drawDiagPantalla1() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(10, 0);
  display.println("DIAG 1/4");
  
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
  
  display.display();
}

void drawPantallaTemperatura() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  // Título
  display.setCursor(0, 0);
  display.println("DIAG TEMP 2/4");
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
  
  display.display();
}

void drawPantallaCalidadAire() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.println("DIAG AIRE 3/4");
  display.println("----------");
  
  // Valor MQ135
  display.setTextSize(3);
  display.setCursor(20, 20);
  display.println(calidadAire);
  
  // Evaluación con emoji
  display.setTextSize(1);
  display.print("Estado: ");
  if (calidadAire < 400) {
    display.println("OK [*_*]");
  } else if (calidadAire < 1000) {
    display.println("MEDIO [-_-]");
  } else {
    display.println("MALO [o_o]");
  }
  
  // Referencia (solo una línea)
  display.println("");
  display.println("<400:OK  >1K:Malo");
  
  display.display();
}

void drawPantallaSistema() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  display.setCursor(0, 0);
  display.println("DIAG SYS 4/4");
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

    // -- LECTURA DE BOTONES --
    bool btnEncoder = !digitalRead(ENCODER_SW_PIN);
    bool btnConfirm = !digitalRead(CONFIRM_BUTTON_PIN);
    bool btnBak   = !digitalRead(BAK_BUTTON_PIN);

    // Antirebote para botones
    static bool lastBtnEncoder = false;
    static bool lastBtnConfirm = false;
    static bool lastBtnBak = false;

    // ===== MANEJO DE PANTALLA EN MODO NOCHE =====
    // Si la pantalla está durmiendo y se pulsa algo -> despertar
    if (pantallaDurmiendo && (btnEncoder || btnConfirm || btnBak)) {
      pantallaDurmiendo = false;
      display.oled_command(0xAF); // Encender display
      Serial.println("*** PANTALLA DESPERTADA ***");
    }
    
    // Si estamos en modo infinito sin nightMode, apagar pantalla tras 5 min sin actividad
    if (estadoActual == MANUAL_RUN_INFINITE && !nightModeEnabled) {
      if (!pantallaDurmiendo && (millis() - lastActivityTime > SCREEN_TIMEOUT)) {
        pantallaDurmiendo = true;
        display.oled_command(0xAE); // Apagar display
        Serial.println("*** PANTALLA APAGADA POR TIMEOUT ***");
      }
    }
    
    // Actualizar último evento de actividad si hay pulsación
    if (btnEncoder || btnConfirm || btnBak) {
      lastActivityTime = millis();
    }
    
    // ===== DESACTIVAR LEDs DE ERROR EN MODO NOCHE =====
    // En modo noche, desactivar parpadeos de error
    if (estadoActual == MANUAL_RUN_INFINITE && nightModeEnabled) {
      ledsErrorActivos = false;
    } else {
      ledsErrorActivos = true;
    }

    // COMBO ESPECIAL: 4 pulsaciones rápidas de CONFIRM para DIAGNOSTIC
    if (btnConfirm && !lastBtnConfirm) {
      if (millis() - lastComboTime < COMBO_TIMEOUT) {
        comboCount++;
        if (comboCount >= 4 && estadoActual != DIAGNOSTIC) {
          estadoActual = DIAGNOSTIC;
          pantallaActual = 0;
          Serial.println("*** MODO DIAGNOSTICO ACTIVADO ***");
          comboCount = 0;
        }
      } else {
        comboCount = 1;
      }
      lastComboTime = millis();
    }

    // Encoder value para navegación
    long currentEncoderValue = encoder.getCount() / 2;
    int encoderDiff = currentEncoderValue - lastEncoderValue;
    
    // ============ MÁQUINA DE ESTADOS ============
    
    switch (estadoActual) {
      
      // -------- MODO AUTO --------
      case AUTO:
        if (encoderDiff != 0) {
          estadoActual = MANUAL_CONFIG;
          configStep = 0;
          Serial.println("Entrando a MANUAL_CONFIG");
        }
        drawAutoScreen();
        break;
      
      // -------- CONFIGURACIÓN MANUAL --------
      case MANUAL_CONFIG:
        // BAK -> volver a AUTO
        if (btnBak && !lastBtnBak) {
          estadoActual = AUTO;
          Serial.println("Cancelado, volviendo a AUTO");
          break;
        }
        
        // Encoder -> cambiar valores según paso actual
        if (encoderDiff != 0) {
          switch (configStep) {
            case 0: // Tiempo
              manualTimeSel += encoderDiff * 5;
              if (manualTimeSel < 15) manualTimeSel = 15;
              if (manualTimeSel > 120) manualTimeSel = 120;
              break;
            case 1: // Velocidad
              manualSpeedSel += encoderDiff * 5;
              if (manualSpeedSel < 25) manualSpeedSel = 25;
              if (manualSpeedSel > 100) manualSpeedSel = 100;
              break;
            case 2: // Modo
              modoInfinito = !modoInfinito;
              break;
            case 3: // Noche
              nightModeEnabled = !nightModeEnabled;
              break;
          }
        }
        
        // ENCODER_SW -> siguiente paso / confirmar
        if (btnEncoder && !lastBtnEncoder) {
          configStep++;
          if (configStep > 3) {
            // Confirmar configuración
            if (modoInfinito) {
              estadoActual = MANUAL_RUN_INFINITE;
              pantallaDurmiendo = false;
              lastActivityTime = millis(); // Iniciar timer de actividad
              ledsErrorActivos = !nightModeEnabled; // Desactivar LEDs si modo noche
              Serial.println("Iniciando MANUAL INFINITO");
            } else {
              estadoActual = MANUAL_RUN_LIMITED;
              manualStartTime = millis();
              pantallaDurmiendo = false;
              Serial.println("Iniciando MANUAL LIMITADO");
            }
            ventiladorActivo = true;
            digitalWrite(LED_GREEN_PIN, LOW); // Encender LED verde
            configStep = 0;
          }
        }
        
        drawManualConfigScreen();
        break;
      
      // -------- MANUAL INFINITO --------
      case MANUAL_RUN_INFINITE:
        // BAK -> volver a AUTO
        if (btnBak && !lastBtnBak) {
          estadoActual = AUTO;
          ventiladorActivo = false;
          digitalWrite(LED_GREEN_PIN, HIGH);
          pantallaDurmiendo = false;
          display.oled_command(0xAF); // Asegurar pantalla encendida
          Serial.println("Saliendo de INFINITO");
        }
        
        // Dibujar solo si pantalla no está durmiendo
        if (!pantallaDurmiendo) {
          drawManualInfiniteScreen();
        }
        break;
      
      // -------- MANUAL LIMITADO --------
      case MANUAL_RUN_LIMITED:
        {
          // Verificar si terminó el tiempo
          unsigned long elapsed = (millis() - manualStartTime) / 1000;
          if (elapsed >= (manualTimeSel * 60)) {
            estadoActual = AUTO;
            ventiladorActivo = false;
            digitalWrite(LED_GREEN_PIN, HIGH);
            Serial.println("Tiempo finalizado, volviendo a AUTO");
          }
          
          // BAK -> volver a AUTO
          if (btnBak && !lastBtnBak) {
            estadoActual = AUTO;
            ventiladorActivo = false;
            digitalWrite(LED_GREEN_PIN, HIGH);
            Serial.println("Cancelado LIMITADO");
          }
          drawManualLimitedScreen();
        }
        break;
      
      // -------- DIAGNÓSTICO --------
      case DIAGNOSTIC:
        // BAK -> volver a AUTO
        if (btnBak && !lastBtnBak) {
          estadoActual = AUTO;
          Serial.println("Saliendo de DIAGNOSTIC");
          break;
        }
        
        // Navegación con encoder entre pantallas diagnóstico
        if (encoderDiff != 0) {
          pantallaActual += encoderDiff;
          if (pantallaActual < 0) pantallaActual = NUM_PANTALLAS - 1;
          if (pantallaActual >= NUM_PANTALLAS) pantallaActual = 0;
          Serial.print("Pantalla DIAG: ");
          Serial.println(pantallaActual);
        }
        
        // LEDs de test
        if (btnEncoder && !lastBtnEncoder) {
          ventiladorActivo = !ventiladorActivo;
          digitalWrite(LED_GREEN_PIN, ventiladorActivo ? LOW : HIGH);
        }
        if (btnConfirm && !lastBtnConfirm) {
          ledAmarilloState = !ledAmarilloState;
          digitalWrite(LED_YELLOW_PIN, ledAmarilloState ? LOW : HIGH);
        }
        
        // Dibujar pantalla diagnóstico actual
        switch (pantallaActual) {
          case 0:
            drawDiagPantalla1();
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
        break;
    }
    
    lastBtnEncoder = btnEncoder;
    lastBtnConfirm = btnConfirm;
    lastBtnBak = btnBak;
    lastEncoderValue = currentEncoderValue;

    // -- CALCULAR RPM DEL VENTILADOR --
    if (millis() - lastTachCheck >= 1000) {
      noInterrupts();
      unsigned long pulses = tachPulseCount;
      tachPulseCount = 0;
      interrupts();
      
      fanRPM = (pulses * 60) / 2;
      lastTachCheck = millis();
    }

    // -- LECTURA DE SENSORES --
    sensors_event_t h, t;
    if (aht.getEvent(&h, &t)) {
      if (!isnan(t.temperature) && !isnan(h.relative_humidity)) {
        temperatura = t.temperature;
        humedad = h.relative_humidity;
      }
    }
    
    presion = bmp.readPressure() / 100.0F;
    calidadAire = analogRead(MQ135_ANALOG_PIN);
  }
}
