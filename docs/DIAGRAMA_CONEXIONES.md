# 🔌 Diagrama de Conexiones - v7.1C FINAL (Hardware Real)
PROYECTO: Extractor Inteligente (Delta 12V 2.70A)

---

## ⚠️ NOTA DE HARDWARE
Este diagrama utiliza las etiquetas serigrafiadas en la **Placa de Expansión Morada**. Conecta los cables Dupont a la fila de pines macho marcada con la letra **S (Señal)**.

---

## 1. Módulo OLED 1.3" + Encoder Estardyn (3 Botones)
| Función | Pin en Módulo | Etiqueta en Shield | Pin Real ESP32 |
| :--- | :--- | :--- | :--- |
| **GND** | GND | **G (Cualquier negro)**| GND |
| **VCC** | VCC | **3V3 (Extremo Izq)** | 3.3V |
| **I2C Data**| SDA | **P21** | GPIO 21 |
| **I2C Clock**| SCL | **P22** | GPIO 22 |
| **Encoder A**| TRA | **P32** | GPIO 32 |
| **Encoder B**| TRB | **P33** | GPIO 33 |
| **OK (Push)**| ENCODER_PUSH| **P27** | GPIO 27 |
| **BACK (Lateral)**| CONFIRM | **P25** | GPIO 25 |
| **PAUSA (Lateral)**| BAK | **P26** | GPIO 26 |

---

## 2. Sensores (Módulo Dual y Calidad de Aire)

### Módulo Dual AHT20 + BMP280
| Función | Pin Módulo | Etiqueta en Shield |
| :--- | :--- | :--- |
| **Corriente**| VDD | **3V3 (Extremo Izq)** |
| **Tierra** | GND | **G (Fila Negra)** |
| **I2C SCL** | SCL | **P22 (Fila S)** |
| **I2C SDA** | SDA | **P21 (Fila S)** |

### Módulo MQ135
| Función | Pin Módulo | Etiqueta en Shield |
| :--- | :--- | :--- |
| **Corriente**| VCC | **V (Fila Roja - Jumper 5V)**|
| **Tierra** | GND | **G (Fila Negra)** |
| **Analógico**| AO / AD | **P34 (Fila S)** |

---

## 3. Circuito de Potencia y LEDs

### Control del Ventilador (PWM 4 Hilos)
- **PWM Ventilador:** Pin **P19** (Fila S). *GPIO 14 descartado por seguridad*.
 - **MOSFET (Gate / Control ON/OFF):** Controlado desde **P19 (GPIO 19)** o el pin PWM asignado; no se utiliza relé.

### LEDs de Estado Externos
- **LED Rojo (Standby/Error):** Pin **P4** (Fila S) vía resistencia 220Ω.
- **LED Verde (Funcionamiento):** Pin **P15** (Fila S) vía resistencia 220Ω.

---

## 4. Resumen de Colores de Headers en Shield
- **G (Negro/Azul):** GND Común.
- **V (Rojo):** VCC (Debe estar el Jumper en 5V).
- **S (Amarillo/Blanco):** Señal (Donde van conectados todos los cables de datos anteriores).
