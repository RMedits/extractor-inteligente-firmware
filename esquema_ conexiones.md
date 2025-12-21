# 🔌 Esquema de Conexiones - v6.4C FINAL
PROYECTO: Extractor Inteligente (Delta 12V 2.70A - Control PWM Protegido)

---

## ⚠️ ADVERTENCIA DE SEGURIDAD
Este montaje utiliza un ventilador de alta potencia y protege al ESP32 mediante una etapa de buffer.
**Importante:** Todos los GND (Tierra) del ESP32 y de la fuente de 12V deben estar unidos.

---

## 1. Conexiones de Lógica y Control (ESP32)

| Pin ESP32 | Componente | Función | Notas |
| :--- | :--- | :--- | :--- |
| **GPIO 21** | Bus I2C | SDA | OLED SH1106 + AHT20 + BMP280 |
| **GPIO 22** | Bus I2C | SCL | OLED SH1106 + AHT20 + BMP280 |
| **GPIO 32** | Encoder | TRA (CLK) | Giro del encoder |
| **GPIO 33** | Encoder | TRB (DT) | Giro del encoder |
| **GPIO 27** | Encoder PUSH | OK | Pulsar la rueda |
| **GPIO 25** | Botón CONFIRM | BACK | Botón físico lateral |
| **GPIO 26** | Botón BAK | PAUSE | Botón físico lateral (2s) |
| **GPIO 34** | Sensor MQ135 | AOUT | Analógico (Calidad Aire) |
| **GPIO 23** | Relé KY-019 | Signal (S) | Corte de Energía (ON/OFF) |
| **GPIO 14** | MOSFET Gate | PWM Control | Buffer Inversor para PWM |
| **GPIO 4**  | LED Rojo | Ánodo (+) | Error / Standby |
| **GPIO 15** | LED Verde | Ánodo (+) | Funcionamiento OK |

---

## 2. Circuito de Potencia y Protección

Se utiliza una topología Híbrida: Relé para corte de energía y MOSFET (Buffer) para protección de la señal PWM.

### A. Etapa de Relé (Corte de Seguridad)
Controla la alimentación principal del ventilador.
- **Relé VCC/GND:** A 5V y GND.
- **Relé Signal:** A GPIO 23.
- **Relé COM:** A Fuente +12V.
- **Relé NO (Abierto):** A Cable POSITIVO (Rojo) del Ventilador.

### B. Etapa de MOSFET (Buffer de Señal PWM)
Protege el GPIO del ESP32 de los 5V/12V del ventilador. Actúa como interruptor a tierra ("Open Drain").
- **Componente:** MOSFET FQP30N06L (o 2N7000).
- **Gate (G):** A GPIO 14 (vía resistencia 1kΩ).
- **Source (S):** A GND.
- **Drain (D):** A Cable PWM (Azul/Verde) del Ventilador.
  * *Nota:* No se requiere resistencia pull-up externa (el ventilador la tiene interna).

### C. Ventilador (Conector 4 Pines)
- **Cable GND (-):** A GND Común.
- **Cable 12V (+):** A Salida del Relé.
- **Cable PWM:** Al Drain del MOSFET.
- **Cable Tach:** No conectado.

---

## 3. Sensores y Periféricos
- **OLED 1.3" (SH1106):** VCC a 3.3V, I2C a 21/22.
- **AHT20 + BMP280:** I2C a 21/22.
- **Encoder EC11:** A 32, 33, 27.
- **MQ135:** VCC a 5V, AOUT a 34.
