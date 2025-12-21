# 🔌 Esquema de Conexiones - v6.3C FINAL
PROYECTO: Extractor Inteligente (Delta 12V 2.70A - Control 4 Pines)

---

## ⚠️ ADVERTENCIA DE SEGURIDAD
Este montaje utiliza un ventilador de alta potencia. Asegúrese de que la fuente de alimentación 12V sea capaz de suministrar al menos 3A-4A.
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
| **GPIO 14** | Ventilador PWM | PWM | Control de Velocidad (Cable Azul/Verde) |
| **GPIO 4**  | LED Rojo | Ánodo (+) | Error / Standby |
| **GPIO 15** | LED Verde | Ánodo (+) | Funcionamiento OK |

---

## 2. Circuito de Potencia (Ventilador 4 Pines)

El ventilador se controla mediante señal PWM directa y un Relé para corte total de energía. **No use MOSFET en la línea de tierra.**

### Relé KY-019 (Corte de Seguridad)
- **VCC/GND:** A 5V y GND del ESP32/Fuente.
- **Signal (S):** A GPIO 23.
- **COM (Salida Relé):** A Fuente +12V.
- **NO (Normalmente Abierto):** A Cable POSITIVO (+12V) del Ventilador.

### Ventilador (Conector 4 Pines)
- **Cable GND (-):** A GND Común (Fuente y ESP32).
- **Cable 12V (+):** A Salida NO del Relé.
- **Cable PWM (Control):** A GPIO 14 del ESP32.
  - *Nota:* Si el ventilador requiere PWM de 5V y no funciona con los 3.3V del ESP32, utilice un Level Shifter o un transistor pequeño.
- **Cable Tach (RPM):** No conectado (Opcional).

---

## 3. Sensores y Periféricos (I2C / 3.3V)
Todos los dispositivos I2C comparten los pines 21 (SDA) y 22 (SCL).

- **OLED 1.3" (SH1106):** VCC a 3.3V, GND a GND.
- **AHT20 (Humedad/Temp):** VCC a 3.3V, GND a GND.
- **BMP280 (Presión/Temp):** VCC a 3.3V, GND a GND. (Dirección I2C 0x76 o 0x77).
- **Encoder EC11:** Pines A y B a GPIO 32/33. Pin C a GND. Pulsador a GPIO 27.
- **MQ135:** VCC a 5V (Requiere 5V para el calentador), GND a GND, AOUT a GPIO 34.
