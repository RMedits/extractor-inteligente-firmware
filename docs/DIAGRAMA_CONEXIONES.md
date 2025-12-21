# 🔌 Diagrama de Conexiones - v7.1C FINAL
PROYECTO: Extractor Inteligente (Delta 12V 2.70A)

---

## ⚠️ ADVERTENCIA DE SEGURIDAD
Este montaje utiliza un ventilador de alta potencia. Es **OBLIGATORIO** el uso del **Diodo 1N5408** y las resistencias en el MOSFET para evitar daños irreversibles en el ESP32. Todos los GND deben estar unidos.

---

## 1. Conexiones de Lógica y Control (ESP32)

| Pin ESP32 | Componente | Función | Notas |
| :--- | :--- | :--- | :--- |
| **GPIO 21** | Bus I2C | SDA | OLED + AHT20/BMP280 |
| **GPIO 22** | Bus I2C | SCL | OLED + AHT20/BMP280 |
| **GPIO 32** | Encoder | TRA (CLK) | Giro del encoder |
| **GPIO 33** | Encoder | TRB (DT) | Giro del encoder |
| **GPIO 27** | Encoder PUSH | OK | Pulsar la rueda |
| **GPIO 25** | Botón CONFIRM | BACK | Botón físico lateral |
| **GPIO 26** | Botón BAK | PAUSE | Botón físico lateral (2s) |
| **GPIO 34** | Sensor MQ135 | AOUT | Analógico (Calidad Aire) |
| **GPIO 23** | Relé KY-019 | Signal (S) | Corte general seguridad |
| **GPIO 19** | MOSFET Gate | PWM | Control velocidad (Seguro) |
| **GPIO 4**  | LED Rojo | Ánodo (+) | Error / Standby |
| **GPIO 15** | LED Verde | Ánodo (+) | Funcionamiento OK |

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

## 3. Circuito de Potencia (12V)

### MOSFET FQP30N06L (Control PWM)
- **GATE (Pin 1):** Conectado a **GPIO 19** a través de resistencia de 220Ω.
- **DRAIN (Pin 2):** Conectado al **NEGATIVO (-)** del Ventilador.
- **SOURCE (Pin 3):** Conectado a **GND**.

### Relé KY-019 (Seguridad/Corte General)
- **S (Señal):** GPIO 23
- **+ (VCC):** 5V
- **- (GND):** GND

### Protección (Diodo 1N5408)
- **Cátodo (Franja):** Al cable POSITIVO del ventilador.
- **Ánodo:** Al cable NEGATIVO del ventilador (Drain del MOSFET).

### LEDs de Estado Externos (Opcionales)
- **LED Rojo (Standby/Error):** Ánodo a **GPIO 4** (vía 220Ω), Cátodo a GND.
- **LED Verde (Funcionamiento):** Ánodo a **GPIO 15** (vía 220Ω), Cátodo a GND.

---

## 4. Componentes de Protección y Estabilidad
- **Resistencia 10kΩ:** Entre GATE del MOSFET y GND (Pulldown obligatorio).
- **Resistencia 220Ω:** Para el GATE del MOSFET y para los LEDs.

---

## ⚠️ Advertencia de Alimentación
Asegúrate de unir todos los **GND** (Tierra) de las diferentes fuentes (12V del ventilador y 5V/USB del ESP32).
