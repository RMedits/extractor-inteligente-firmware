# 🔒 PINES DEFINITIVOS - ESP32 30 PINES (SIN SHIELD)

**⚠️ DOCUMENTO MAESTRO - ÚNICA FUENTE DE VERDAD**
**📅 Actualizado: 15 Enero 2026**
**✅ Hardware: ESP32-WROOM-32 (30 pines) - DIRECTO SIN SHIELD**
**✅ Estado: PROBADO Y FUNCIONAL - NO MODIFICAR**

---

## 📍 PINOUT COMPLETO

### 🔌 I2C (Bus Compartido)
| GPIO | Función | Dispositivos |
|------|---------|--------------|
| **21** | SDA | OLED (0x3C) + AHT20 (0x38) + BMP280 (0x77) |
| **22** | SCL | OLED (0x3C) + AHT20 (0x38) + BMP280 (0x77) |

### 🎛️ Controles
| GPIO | Función | Componente |
|------|---------|------------|
| **32** | ENCODER_CLK | Encoder Fase A |
| **33** | ENCODER_DT | Encoder Fase B |
| **27** | ENCODER_SW | Botón Encoder (OK) |
| **25** | CONFIRM_BUTTON | Botón BACK/Cancelar |
| **13** | BAK_BUTTON | Botón PAUSA |

### 📊 Sensores
| GPIO | Función | Sensor |
|------|---------|--------|
| **34** | MQ135_ANALOG | Calidad Aire (ADC) |
| **21** | SDA I2C | AHT20 + BMP280 (compartido) |
| **22** | SCL I2C | AHT20 + BMP280 (compartido) |

### ⚙️ Actuadores y Realimentación
| GPIO | Función | Uso |
|------|---------|-----|
| **19** | PWM_FAN | Control Velocidad (MOSFET Gate) |
| **16** | FAN_TACH | Lectura RPM Ventilador (cable amarillo) |

### 💡 LEDs
| GPIO | Color | Estado |
|------|-------|--------|
| **18** | ROJO | Error/Standby |
| **5** | AMARILLO | Manual/Advertencia |
| **17** | VERDE | Normal/Funcionamiento |

### 📡 Sensor Radar LD2410 (24GHz) - RESERVADO
| GPIO | Función | Conexión |
|------|---------|----------|
| **14** | LD2410_RX | TX del sensor → RX ESP32 |
| **15** | LD2410_TX | RX del sensor → TX ESP32 |
| **23** | LD2410_OUT | Salida digital detección |
| **26** | LD2410_EN | Enable/Control (opcional) |

> **Nota:** LD2410 usa comunicación UART (Serial). Pines reservados para implementación futura.

---

## 🔗 Alimentación

| Terminal | Voltaje | Dispositivos |
|----------|---------|--------------|
| **3.3V** | 3.3V | OLED + AHT20 + BMP280 |
| **5V** | 5V | MQ135 (calentador) |
| **GND** | Común | Todos los componentes |
| **12V** | 12V | Ventilador (externo) |

---

## ⚠️ IMPORTANTE - SIN SHIELD

Este proyecto usa **ESP32 de 30 pines DIRECTO** (sin shield de expansión).
Las conexiones son **directamente a los GPIO** del ESP32 mediante cables dupont o PCB.

**NO hay shield morado, NO hay sistema G-V-S, NO hay 38 pines.**

---

## 🚫 Pines Prohibidos (No Usar)

| GPIO | Razón | Estado Actual |
|------|-------|---------------|
| **0** | Boot Mode (strapping pin) | Evitar |
| **2** | LED interno ESP32 | Evitar |
| **12** | Conflicto con FLASH (strapping pin) | Evitar |
| **35** | Solo entrada (sin pull-up interno) | Libre |
| **36** | Solo entrada (sin pull-up interno) | Libre |
| **39** | Solo entrada (sin pull-up interno) | Libre |

---

## 📋 Código de Referencia (main.cpp)

```cpp
// I2C
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// Encoder
#define ENCODER_CLK_PIN 32
#define ENCODER_DT_PIN 33
#define ENCODER_SW_PIN 27

// Botones
#define CONFIRM_BUTTON_PIN 25
#define BAK_BUTTON_PIN 13

// Sensores
#define MQ135_ANALOG_PIN 34

// Actuadores
#define PWM_FAN_PIN 19
#define FAN_TACH_PIN 16

// LEDs
#define LED_RED_PIN 18
#define LED_YELLOW_PIN 5
#define LED_GREEN_PIN 17

// Radar LD2410 (24GHz) - RESERVADO
#define LD2410_RX_PIN 14  // RX del ESP32 (conectar a TX del sensor)
#define LD2410_TX_PIN 15  // TX del ESP32 (conectar a RX del sensor)
#define LD2410_OUT_PIN 23 // Detección digital
#define LD2410_EN_PIN 26  // Enable/Control (opcional)
```

---

## 📊 Diagrama de Conexión I2C

```
ESP32 (30 pines)
     |
     ├─ GPIO21 (SDA) ──┬─ OLED SSD1306 (SDA)
     |                 ├─ AHT20 (SDA)
     |                 └─ BMP280 (SDA)
     |
     ├─ GPIO22 (SCL) ──┬─ OLED SSD1306 (SCL)
     |                 ├─ AHT20 (SCL)
     |                 └─ BMP280 (SCL)
     |
     ├─ 3.3V ──────────┬─ OLED (VCC)
     |                 ├─ AHT20 (VDD)
     |                 └─ BMP280 (VCC)
     |
     └─ GND ───────────┴─ Común (todos)
```

---

## 🚨 REGLA DE ORO

**SI FUNCIONA → NO SE TOCA**

Esta configuración ha sido probada exhaustivamente. Cualquier cambio debe:
1. Documentarse AQUÍ primero
2. Verificarse en main.cpp
3. Probarse completamente
4. Validarse antes de commit

---

## 📝 Historial de Cambios

| Fecha | Versión | Cambio |
|-------|---------|--------|
| 16-01-2026 | v1.0 | Reservados pines 14,15,23,26 para LD2410 (radar 24GHz) |
| 15-01-2026 | v7.2C | Documento creado - Configuración definitiva sin shield |

---

**Versión del Firmware:** v1.0-hardware-validado + pantallas  
**Hardware:** ESP32-WROOM-32 (30 pines)  
**Estado:** ✅ PRODUCCIÓN  
**Autor:** RAUL
