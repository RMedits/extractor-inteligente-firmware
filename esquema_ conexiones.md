# 🔌 Esquema de Conexiones - v7.1C FINAL (Hardware Real)
PROYECTO: Extractor Inteligente (ESP32 38P + Shield Morada)

---

## 1. Conexiones de Lógica y Control (Shield)

| Etiqueta en Shield | Componente | Función |
| :--- | :--- | :--- |
| **P21** | Bus I2C | SDA (Datos) |
| **P22** | Bus I2C | SCL (Reloj) |
| **P32** | Encoder | TRA (Giro) |
| **P33** | Encoder | TRB (Giro) |
| **P27** | Encoder PUSH | Botón OK |
| **P25** | Botón CONFIRM | Botón BACK |
| **P26** | Botón BAK | Botón PAUSE (2s) |
| **P34** | Sensor MQ135 | Salida Analógica |
| **P23** | Relé KY-019 | Señal Control |
| **P19** | MOSFET Gate | PWM Ventilador |
| **P4**  | LED Rojo | Estado Error/Standby |
| **P15** | LED Verde | Estado Funcionando |

---

## 2. Alimentación y Tierra
- **GND:** Usa cualquier pin de la **Fila G (Negra/Azul)**.
- **5V:** Usa cualquier pin de la **Fila V (Roja)** (Asegura Jumper en 5V). Para Relé y MQ135.
- **3.3V:** Usa el pin marcado como **3V3** en la esquina superior izquierda. Para OLED y AHT20/BMP280.

---

## 3. Circuito de Potencia
- **MOSFET Gate:** Conectar a **P19** vía resistencia 220Ω. Poner resistencia 10kΩ a GND.
- **MOSFET Drain:** Al negativo del ventilador.
- **MOSFET Source:** A GND.
- **Diodo 1N5408:** En paralelo al ventilador (Cátodo al positivo).
