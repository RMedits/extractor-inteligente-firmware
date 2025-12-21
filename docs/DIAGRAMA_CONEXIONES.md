# 🔌 Diagrama de Conexiones - v6.0C FINAL
Este documento detalla el cableado exacto para el proyecto del Extractor Inteligente.

---

## 1. Módulo OLED 1.3" + Encoder Estardyn (3 Botones)
Este módulo centraliza la pantalla y todos los controles de usuario.

| Pin Módulo | Función | Pin ESP32 | Notas |
| :--- | :--- | :--- | :--- |
| **VCC** | Alimentación | 3.3V | Puede alimentarse a 5V pero se recomienda 3.3V |
| **GND** | Tierra | GND | Común con el sistema |
| **OLED_SCL** | Reloj I2C | GPIO 22 | Compartido con BME280 |
| **OLED_SDA** | Datos I2C | GPIO 21 | Compartido con BME280 |
| **ENCODER_TRA** | Phase A (CLK) | GPIO 32 | Lectura de giro |
| **ENCODER_TRB** | Phase B (DT) | GPIO 33 | Lectura de giro |
| **ENCODER_PUSH**| Botón OK | GPIO 27 | **Pulsar rueda del encoder** |
| **CONFIRM** | Botón BACK | GPIO 25 | Botón físico lateral |
| **BAK** | Botón PAUSA | GPIO 26 | Botón físico lateral (Mantener 2s) |

---

## 2. Sensores (AHT20+BMP280 y MQ135)

### Módulo Dual AHT20 + BMP280 (Humedad/Temp/Presión)
| Pin Módulo | Función | Pin ESP32 | Notas |
| :--- | :--- | :--- | :--- |
| **VDD / VCC**| Alimentación | 3.3V | |
| **GND** | Tierra | GND | |
| **SCL** | Reloj I2C | GPIO 22 | Compartido con OLED |
| **SDA** | Datos I2C | GPIO 21 | Compartido con OLED |

### Módulo MQ135 (Calidad de Aire)
| Pin Módulo | Función | Pin ESP32 | Notas |
| :--- | :--- | :--- | :--- |
| **VCC** | Alimentación | 5V (Vin) | Necesita 5V para el calefactor |
| **GND** | Tierra | GND | |
| **AO / AD** | Salida Analóg.| GPIO 34 | Valor 0-4095 |
| **DO** | Salida Digital| NC | No utilizado |

---

## 3. Control de Potencia (Ventilador Delta 12V)

### Relé KY-019 (Seguridad/Corte General)
- **S (Señal):** GPIO 23
- **+ (VCC):** 5V
- **- (GND):** GND

### MOSFET FQP30N06L (Control PWM)
- **GATE (Pin 1):** Conectado a **GPIO 14** a través de resistencia de 220Ω.
- **DRAIN (Pin 2):** Conectado al **NEGATIVO (-)** del Ventilador.
- **SOURCE (Pin 3):** Conectado a **GND**.

### LEDs de Estado Externos (Opcionales)
- **LED Rojo (Standby/Error):** Ánodo a **GPIO 4** (vía 220Ω), Cátodo a GND.
- **LED Verde (Funcionamiento):** Ánodo a **GPIO 15** (vía 220Ω), Cátodo a GND.

---

## 4. Componentes de Protección y Estabilidad
- **Diodo 1N5408:** En paralelo con el ventilador (Cátodo a 12V+, Ánodo a Ventilador-).
- **Resistencia 10kΩ:** Entre GATE del MOSFET y GND (Pulldown obligatorio).
- **Resistencia 220Ω:** Para el GATE del MOSFET y para el LED de estado.

---

## ⚠️ Advertencia de Alimentación
Asegúrate de unir todos los **GND** (Tierra) de las diferentes fuentes (12V del ventilador y 5V/USB del ESP32) para que el control PWM funcione correctamente.
