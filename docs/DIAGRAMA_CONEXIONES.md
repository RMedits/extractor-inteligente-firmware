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
| **I2C Data**| SDA | **D21 (Fila S)** | GPIO 21 |
| **I2C Clock**| SCL | **D22 (Fila S)** | GPIO 22 |
| **Encoder A**| TRA | **D32 (Fila S)** | GPIO 32 |
| **Encoder B**| TRB | **D33 (Fila S)** | GPIO 33 |
| **OK (Push)**| ENCODER_PUSH| **D27 (Fila S)** | GPIO 27 |
| **BACK (Lateral)**| CONFIRM | **D25 (Fila S)** | GPIO 25 |
| **PAUSA (Lateral)**| BAK | **D26 (Fila S)** | GPIO 26 |

---

## 2. Sensores (Módulo Dual y Calidad de Aire)

### Módulo Dual AHT20 + BMP280
| Función | Pin Módulo | Etiqueta en Shield |
| :--- | :--- | :--- |
| **Corriente**| VDD | **3V3 (Extremo Izq)** |
| **Tierra** | GND | **G (Fila Negra)** |
| **I2C SCL** | SCL | **D22 (Fila S, GPIO22)** |
| **I2C SDA** | SDA | **D21 (Fila S, GPIO21)** |

### Módulo MQ135
| Función | Pin Módulo | Etiqueta en Shield |
| :--- | :--- | :--- |
| **Corriente**| VCC | **V (Fila Roja - Jumper 5V)**|
| **Tierra** | GND | **G (Fila Negra)** |
| **Analógico**| AO / AD | **D34 (Fila S, GPIO34)** |

---

## 3. Circuito de Potencia y LEDs

### Control del Ventilador (PWM 4 hilos)
- **Cable Azul (PWM):** Pin **D19 (GPIO19, Fila S)**.
- **Relé KY-019 (Signal, si se usa corte):** Pin **D23 (GPIO23, Fila S)**.

### LEDs de Estado Externos
- **LED Rojo (Standby/Error):** Pin **D4 (GPIO4, Fila S)** vía resistencia 220Ω.
- **LED Verde (Funcionamiento):** Pin **D15 (GPIO15, Fila S)** vía resistencia 220Ω.

---

## 4. Resumen de Colores de Headers en Shield
- **G (Negro/Azul):** GND Común.
- **V (Rojo):** VCC (Debe estar el Jumper en 5V).
- **S (Amarillo/Blanco):** Señal (Donde van conectados todos los cables de datos anteriores).
