# 📍 Configuración de Pines - ESP32 30 Pines (v7.2C)

⚠️ **ESTE DOCUMENTO ESTÁ OBSOLETO**  
👉 **Ver [PINES_DEFINITIVOS.md](../PINES_DEFINITIVOS.md) para la configuración actual**

Este proyecto usa ESP32 de **30 pines SIN SHIELD**.

---

## ⬅️ Lateral Izquierdo (USB ARRIBA)

| Etiqueta Shield | Función en el Proyecto |
| :--- | :--- |
| **3V3** | **VCC (OLED y Sensor Dual)** |
| **EN** | Reset |
| **SVP** | - |
| **SVN** | - |
| **P34** | **Sensor Aire MQ135** |
| **P35** | - |
| **P32** | **Encoder TRA** |
| **P33** | **Encoder TRB** |
| **P25** | **Botón BACK (Confirm)** |
| **P26** | **Botón PAUSA (Bak)** |
| **P27** | **Botón OK (Encoder Push)** |
| **P14** | NO USAR (Inestable) |
| **P12** | - |
| **GND** | GND |
| **P13** | - |
| **SD2** | - |
| **SD3** | - |
| **CMD** | - |
| **5V** | **Vin (Alimentación ESP32)** |

---

## ➡️ Lateral Derecho (USB ARRIBA)

| Etiqueta Shield | Función en el Proyecto |
| :--- | :--- |
| **GND** | GND Común |
| **P23** | (No usado - anteriormente destinado al relé) |
| **P22** | **I2C SCL (OLED + AHT20 + BMP280)** |
| **TX** | - |
| **RX** | - |
| **P21** | **I2C SDA (OLED + AHT20 + BMP280)** |
| **GND** | GND |
| **P19** | **Ventilador PWM** |
| **P18** | - |
| **P5** | - |
| **P17** | - |
| **P16** | - |
| **P4** | **LED Rojo** |
| **P0** | - |
| **P2** | - |
| **P15** | **LED Verde** |
| **SD1** | - |
| **SD0** | - |
| **CLK** | - |

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
| **P21** | SDA (Datos I2C) | • OLED SSD1306 (0x3C)<br>• AHT20 (0x38)<br>• BMP280 (0x77) |
| **P22** | SCL (Reloj I2C) | • OLED SSD1306 (0x3C)<br>• AHT20 (0x38)<br>• BMP280 (0x77) |
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
| SCL          | → | **P22** (Fila S) |
| SDA          | → | **P21** (Fila S) |

### Sensor AHT20 (Módulo Dual)
| Pin del AHT20 | → | Pin del Shield ESP32 |
|---------------|---|----------------------|
| VDD / +       | → | **3V3** (Extremo Izquierdo) |
| GND / -       | → | **G** (Fila Negra) |
| SCL           | → | **P22** (Fila S) |
| SDA           | → | **P21** (Fila S) |

### Sensor BMP280 (Módulo Dual)
| Pin del BMP280 | → | Pin del Shield ESP32 |
|----------------|---|----------------------|
| VCC / VDD      | → | **3V3** (Extremo Izquierdo) |
| GND            | → | **G** (Fila Negra) |
| SCL            | → | **P22** (Fila S) |
| SDA            | → | **P21** (Fila S) |

### Sensor MQ135 (Calidad de Aire)
| Pin del MQ135 | → | Pin del Shield ESP32 |
|---------------|---|----------------------|
| VCC           | → | **Fila V** (Roja - 5V con Jumper) |
| GND           | → | **G** (Fila Negra) |
| AO (Analógico)| → | **P34** (Fila S) |
| DO (Digital)  | → | No conectado |

**⚠️ Resumen:** Todos los dispositivos I2C (OLED, AHT20, BMP280) **comparten físicamente** los mismos 4 cables: VCC→3V3, GND→G, SDA→P21, SCL→P22.
