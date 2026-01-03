# 🔌 Esquema de Conexiones - v7.1C FINAL (Hardware Real)
PROYECTO: Extractor Inteligente (ESP32 38P + Shield Morada)

---

## 1. Conexiones de Lógica y Control (Shield)

| Etiqueta en Shield | Componente | Función |
| :--- | :--- | :--- |
| **D21 (GPIO21)** | Bus I2C | SDA (Datos) |
| **D22 (GPIO22)** | Bus I2C | SCL (Reloj) |
| **D32 (GPIO32)** | Encoder | TRA (Giro) |
| **D33 (GPIO33)** | Encoder | TRB (Giro) |
| **D27 (GPIO27)** | Encoder PUSH | Botón OK |
| **D25 (GPIO25)** | Botón CONFIRM | Botón BACK |
| **D26 (GPIO26)** | Botón BAK | Botón PAUSE (2s) |
| **D34 (GPIO34)** | Sensor MQ135 | Salida Analógica |
| **D23 (GPIO23)** | Relé KY-019 | Señal Control |
| **D19 (GPIO19)** | Cable azul Ventilador | PWM Ventilador |
| **D4  (GPIO4)**  | LED Rojo | Estado Error/Standby |
| **D15 (GPIO15)** | LED Verde | Estado Funcionando |

---

## 2. Alimentación y Tierra
- **GND:** Usa cualquier pin de la **Fila G (Negra/Azul)**.
- **5V:** Usa cualquier pin de la **Fila V (Roja)** (Asegura Jumper en 5V). Para Relé y MQ135.
- **3.3V:** Usa el pin marcado como **3V3** en la esquina superior izquierda. Para OLED y AHT20/BMP280.

---

## 3. Circuito de Potencia
- **Ventilador 12V 4 Hilos:** Alimenta con +12V y GND directos; conecta el cable azul (PWM) al pin **D19 (GPIO19)**.
- **Relé KY-019 (opcional de corte):** Alimenta con 5V y GND; controla con **D23 (GPIO23)** si quieres desconectar la línea de 12V.
- **GND común obligatorio** entre la fuente de 12V y el ESP32 para que la señal PWM funcione.
