# ESP32 DevKit HW-394 (USB-C, 30 pines)

Este documento resume las características de la placa utilizada en el proyecto: una variante del ESP32 DevKit comercializada como **HW-394** o **ESP32 Development Board Type-C 5V**.

## 📦 Resumen rápido

- **MCU**: ESP32-WROOM-32 (dual core, WiFi 2.4 GHz + Bluetooth/BLE).
- **Conectividad USB**: conector USB-C con chip USB-serial integrado (según la remesa puede ser CP2102, CH340C o CH9102).
- **Pines expuestos**: 30 pines totales (15 por lado) con serigrafía `Dxx` que corresponde al número de GPIO real.
- **Entradas de alimentación**:
  - **USB-C** (5V) para programación y alimentación directa.
  - **VIN (5V)** para alimentar la placa desde una fuente externa regulada.
- **Regulador on-board**: AMS1117 (3.3 V / 800 mA máx). Usa esta salida para sensores lógicos.
- **Conversión de niveles**: todos los GPIO trabajan a 3.3 V. No aplicar 5 V directos a las entradas/salidas.

## 🧭 Orientación y numeración de pines

- Con el **USB-C apuntando hacia arriba**, los pines **1 a 15** están en la columna izquierda y los **16 a 30** en la derecha.
- La serigrafía `Dxx` coincide con el número de GPIO salvo en los pines especiales (3V3, GND, VIN, EN, VP, VN, RX0, TX0).
- Consulta `docs/DIAGRAMA_PINOUT_ESP32.md` para una vista gráfica a escala y la tabla con los pines utilizados por el firmware.

## ⚡ Alimentación recomendada en el proyecto

| Tensión | Pin de la placa | Uso en este proyecto | Notas |
|--------:|-----------------|----------------------|-------|
| 3.3 V   | 3V3 (pin 1)     | OLED, BME280, encoder, lógica MQ135 | Máx. 500 mA aprox. compartidos. Evita alimentar cargas grandes aquí. |
| 5 V     | VIN (pin 16)    | Relé, calefactor MQ135 | La entrada VIN pasa por el regulador AMS1117. Asegúrate de no superar 12 V. |
| GND     | GND (pines 2 y 17) | Tierra común | Conecta todos los GND (12 V, relé, MOSFET, sensores, ESP32). |

> ⚠️ **Importante**: El relé y el ventilador comparten GND con el ESP32, pero el ventilador se alimenta desde la fuente de 12 V a través del relé + MOSFET. No conectes la fuente de 12 V directamente a VIN.

## 🔌 Pines críticos del proyecto

| GPIO | Pin físico / serigrafía | Función en firmware | Comentario |
|------|-------------------------|---------------------|------------|
| 21   | 11 · D21                | I2C SDA (`oled_sda`) | Comparte bus con OLED + BME280. |
| 22   | 14 · D22                | I2C SCL (`oled_scl`) | Mantén cables cortos (<30 cm). |
| 27   | 21 · D27                | Botón OK (`encoder_push`) | INPUT_PULLUP activado. |
| 25   | 23 · D25                | Botón Back (`confirm`) | INPUT_PULLUP activado. |
| 26   | 22 · D26                | Botón Pausa (`bak`) | INPUT_PULLUP activado. |
| 32   | 25 · D32                | Encoder A (`encoder_tra`) | Señal digital, usa GND común. |
| 33   | 24 · D33                | Encoder B (`encoder_trb`) | Señal digital, usa GND común. |
| 34   | 27 · D34                | MQ135 Analógico (`AOUT`) | Entrada solo ADC, no soporta pull-up interno. |
| 23   | 15 · D23                | Control del relé (`IN`) | HIGH activa el relé. |
| 14   | 20 · D14                | PWM ventilador (`Gate`) | Conectado al gate del MOSFET de potencia. |

## 🛠️ Consejos de uso

1. **Programación**: mantener presionado `BOOT` (GPIO0) solo si el cargador serie lo requiere; en la mayoría de HW-394 no es necesario.
2. **Flotantes al arranque**: evita usar GPIO0, GPIO2 y GPIO15 para señales externas que puedan quedar en estado indeterminado, ya que afectan el modo de arranque.
3. **Protección del MOSFET**: añade un diodo flyback en paralelo al ventilador y una resistencia de 10 kΩ entre Gate y Source.
4. **Cableado ordenado**: ubica el relé y el MOSFET lejos del ESP32 para minimizar ruido, pero comparte GND.

Con esta información podrás replicar el cableado de la placa HW-394 sin depender de referencias genéricas de otros modelos.
