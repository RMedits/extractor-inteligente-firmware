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
| **P23** | **Relé Signal** |
| **P22** | **I2C SCL (Reloj)** |
| **TX** | - |
| **RX** | - |
| **P21** | **I2C SDA (Datos)** |
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
2.  **V (Voltage):** 5V (si el jumper está en 5V). Úsalo para el Relé y MQ135.
3.  **G (Ground):** Tierra. Úsalo para todos los componentes.
