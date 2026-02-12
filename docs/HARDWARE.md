# 📍 Configuración de Pines - ESP32 30 Pines (VERSIÓN FINAL)

Este proyecto usa ESP32 de **30 pines SIN SHIELD**.

---

## ⬅️ Lateral Izquierdo (USB ARRIBA)

| Posición | Pin | Función en el Proyecto |
| :--- | :--- | :--- |
| 1 | **3V3** | **VCC (OLED y Sensores I2C)** |
| 2 | **EN** | Reset |
| 3 | **SVP / D36** | - |
| 4 | **SVN / D39** | - |
| 5 | **D34** | **Sensor Aire MQ135** |
| 6 | **D35** | - |
| 7 | **D32** | **Encoder CLK** |
| 8 | **D33** | **Encoder DT** |
| 9 | **D25** | **Botón BACK** |
| 10 | **D26** | **Botón PAUSA** |
| 11 | **D27** | **Botón OK (Encoder Push)** |
| 12 | **D14** | **⚠️ NO USAR (Pin Inestable)** |
| 13 | **D12** | - |
| 14 | **GND** | **Tierra Común** |
| 15 | **5V** | **VIN (Alimentación ESP32)** |

---

## ➡️ Lateral Derecho (USB ARRIBA)

| Posición | Pin | Función en el Proyecto |
| :--- | :--- | :--- |
| 1 | **GND** | **Tierra Común** |
| 2 | **D23** | - |
| 3 | **D22** | **I2C SCL (OLED + AHT20 + BMP280)** |
| 4 | **TX** | - |
| 5 | **RX** | - |
| 6 | **D21** | **I2C SDA (OLED + AHT20 + BMP280)** |
| 7 | **GND** | **Tierra Común** |
| 8 | **D19** | **Ventilador PWM** |
| 9 | **D18** | **LED Rojo** |
| 10 | **D5** | **LED Amarillo** |
| 11 | **D17** | **LED Verde** |
| 12 | **D16** | **Ventilador TACH (Feedback)** |
| 13 | **D4** | **Radar LD2410C (OUT)** |
| 14 | **D0** | **Radar LD2410C (TX/RX o Presence)** |
| 15 | **D2** | - |

---

### 💡 NOTA IMPORTANTE SOBRE FILAS G-V-S
Para cada pin de la lista anterior, tienes una fila de 3 pines macho:
1.  **S (Signal):** El cable de datos va AQUÍ.
2.  **V (Voltage):** 5V (si el jumper está en 5V). Úsalo para MQ135 y otros periféricos 5V.
3.  **G (Ground):** Tierra. Úsalo para todos los componentes.

---

## 🔗 Pines Compartidos I2C

El bus I2C permite conectar múltiples dispositivos en solo 2 cables. En este proyecto, **3 dispositivos comparten los mismos pines**:

| Pin | Función | Dispositivos Conectados |
|-----|---------|-------------------------|
| **D21** | SDA (Datos I2C) | • OLED SSD1306 (0x3C)<br>• AHT20 (0x38)<br>• BMP280 (0x77) |
| **D22** | SCL (Reloj I2C) | • OLED SSD1306 (0x3C)<br>• AHT20 (0x38)<br>• BMP280 (0x77) |
| **3V3** | Alimentación | • OLED + AHT20 + BMP280 |
| **GND** | Tierra Común | • OLED + AHT20 + BMP280 |

**Nota:** Cada dispositivo I2C tiene una dirección única (mostrada entre paréntesis) que permite al ESP32 comunicarse con ellos individualmente sin conflictos.

---

## 📟 Conexiones Detalladas por Dispositivo

### Pantalla OLED SSD1306 (Módulo Estardyn)
| Pin del OLED | → | Pin del Shield ESP32 |
|--------------|---|----------------------|
| VCC / VDD    | → | **3V3** (Extremo Izquierdo) |
| GND          | → | **G** (Fila Negra) |
| SCL          | → | **D22** (Fila S) |
| SDA          | → | **D21** (Fila S) |

### Sensor AHT20 (Módulo Dual)
| Pin del AHT20 | → | Pin del Shield ESP32 |
|---------------|---|----------------------|
| VDD / +       | → | **3V3** (Extremo Izquierdo) |
| GND / -       | → | **G** (Fila Negra) |
| SCL           | → | **D22** (Fila S) |
| SDA           | → | **D21** (Fila S) |

### Sensor BMP280 (Módulo Dual)
| Pin del BMP280 | → | Pin del Shield ESP32 |
|----------------|---|----------------------|
| VCC / VDD      | → | **3V3** (Extremo Izquierdo) |
| GND            | → | **G** (Fila Negra) |
| SCL            | → | **D22** (Fila S) |
| SDA            | → | **D21** (Fila S) |

### Sensor MQ135 (Calidad de Aire)
| Pin del MQ135 | → | Pin del Shield ESP32 |
|---------------|---|----------------------|
| VCC           | → | **Fila V** (Roja - 5V con Jumper) |
| GND           | → | **G** (Fila Negra) |
| AO (Analógico)| → | **D34** (Fila S) |
| DO (Digital)  | → | No conectado |

**⚠️ Resumen:** Todos los dispositivos I2C (OLED, AHT20, BMP280) **comparten físicamente** los mismos 4 cables: VCC→3V3, GND→G, SDA→D21, SCL→D22.
