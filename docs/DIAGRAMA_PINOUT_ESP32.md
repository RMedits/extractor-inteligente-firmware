# 📍 Traductor de Pines: Shield Morado -> v7.1C FINAL
Usa este documento para saber qué agujero del Shield corresponde a cada función.

---

## ⬅️ Lateral Izquierdo (USB ARRIBA)

| Etiqueta Shield | Función en el Proyecto |
| :--- | :--- |
| **3V3** | **VCC (OLED y Sensor Dual)** |
| **EN** | Reset |
| **SVP** | - |
| **SVN** | - |
| **D34 (GPIO34)** | **Sensor Aire MQ135** |
| **D35 (GPIO35)** | - |
| **D32 (GPIO32)** | **Encoder TRA** |
| **D33 (GPIO33)** | **Encoder TRB** |
| **D25 (GPIO25)** | **Botón BACK (Confirm)** |
| **D26 (GPIO26)** | **Botón PAUSA (Bak)** |
| **D27 (GPIO27)** | **Botón OK (Encoder Push)** |
| **D14 (GPIO14)** | NO USAR (Inestable) |
| **D12 (GPIO12)** | - |
| **GND** | GND |
| **D13 (GPIO13)** | - |
| **SD2** | - |
| **SD3** | - |
| **CMD** | - |
| **5V** | **Vin (Alimentación ESP32)** |

---

## ➡️ Lateral Derecho (USB ARRIBA)

| Etiqueta Shield | Función en el Proyecto |
| :--- | :--- |
| **GND** | GND Común |
| **D23 (GPIO23)** | **Relé Signal** |
| **D22 (GPIO22)** | **I2C SCL (Reloj)** |
| **TX** | - |
| **RX** | - |
| **D21 (GPIO21)** | **I2C SDA (Datos)** |
| **GND** | GND |
| **D19 (GPIO19)** | **Ventilador PWM (Cable Azul)** |
| **D18 (GPIO18)** | - |
| **D5  (GPIO5)** | - |
| **D17 (GPIO17)** | - |
| **D16 (GPIO16)** | - |
| **D4  (GPIO4)** | **LED Rojo** |
| **D0  (GPIO0)** | - |
| **D2  (GPIO2)** | - |
| **D15 (GPIO15)** | **LED Verde** |
| **SD1** | - |
| **SD0** | - |
| **CLK** | - |

---

### 💡 NOTA IMPORTANTE SOBRE FILAS G-V-S
Para cada pin de la lista anterior, tienes una fila de 3 pines macho:
1.  **S (Signal):** El cable de datos va AQUÍ.
2.  **V (Voltage):** 5V (si el jumper está en 5V). Úsalo para el Relé y MQ135.
3.  **G (Ground):** Tierra. Úsalo para todos los componentes.
