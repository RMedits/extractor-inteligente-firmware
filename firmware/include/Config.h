#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- PINOUT DEFINITIONS ---

// I2C (OLED, AHT20, BMP280)
#define PIN_I2C_SDA 21
#define PIN_I2C_SCL 22

// ENCODER & INPUTS
#define PIN_ENC_CLK 32
#define PIN_ENC_DT 33
#define PIN_ENC_SW 27   // Boton OK = PULSAR el Encoder
#define PIN_BTN_BACK 25 // Boton inferior = Cancelar/Back

// SENSORS
#define PIN_MQ135 34 // Analog Input

// RADAR LD2410C (UART2)
#define PIN_RADAR_TX 16 // TX del radar -> RX del ESP32
#define PIN_RADAR_RX 17 // RX del radar -> TX del ESP32

// ACTUATORS
#define PIN_FAN_PWM 19
#define PIN_FAN_TACH 35 // Cable AZUL del ventilador
#define PIN_RELAY 23    // Control del relé (corte 12V)
#define PIN_LED_RED 18
#define PIN_LED_YELLOW 5
#define PIN_LED_GREEN 15 // Lógica Invertida (LOW = ON)

// LEDs de Puerta (Indicadores externos)
#define PIN_LED_DOOR_RED 26   // Ocupado
#define PIN_LED_DOOR_GREEN 14 // Libre

// --- CONFIGURATION ---

// FAN
#define FAN_PWM_FREQ 25000
#define FAN_PWM_CHANNEL 0
#define FAN_PWM_RES 8

// DISPLAY
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

// SENSORS
#define AHT20_ADDR 0x38
#define BMP280_ADDR 0x77

// LOGIC
#define AUTO_OFF_SCREEN_MS 300000 // 5 minutos

#endif
