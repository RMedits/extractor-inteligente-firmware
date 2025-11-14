# Esquema de Conexiones (Arte ASCII) - v6.0

Este esquema refleja el cableado para el firmware v6.0 con la lógica de botones optimizada.
**¡Atención al cambio del relé al GPIO 23!**

```text
                               +--------------------------------+
                               |      ESP32 DevKit HW-394       |
                               |                                |
      (BME280 SDA)----[SDA]---| GPIO 21 ---[oled_sda]------------\
      (BME280 SCL)----[SCL]---| GPIO 22 ---[oled_scl]-------------\
                               |                                 |
                               | GPIO 27 ---[encoder_push -> OK]--|
                               | GPIO 25 ---[confirm -> Back]-----| Módulo
                               | GPIO 26 ---[bak -> Pausa]--------| Integrado
                               | GPIO 32 ---[encoder_tra]---------|
                               | GPIO 33 ---[encoder_trb]---------|
                               |                                 |
      (MQ135 AOUT)---[ANALOG]-| GPIO 34                          |
                               |                                |
 (Relé IN)--------[CONTROL]--| GPIO 23  <-- ¡NUEVO PIN!          |
 (MOSFET Gate)-------[PWM]----| GPIO 14                          |
                               |                                |
                               | 3.3V ----- [3v3-5v]--------------/
                               | GND ------ [gnd]----------------/
                               |                                |
                               | 5V ------> (Alimentación Relé, MQ135)
                               | GND ------> (GND Común para TODO)
                               +--------------------------------+


    Circuito de Potencia 12V:
    -------------------------

    (+) ---->[Fuente 12V]----> (COM) [RELÉ] (NO) ----> (+) [VENTILADOR] (-) ----> (Drain) [MOSFET] (Source) ----> (-) [GND Común]
                                                                                             |
                                                                                           (Gate)
                                                                                             ^
                                                                                             |
                                                                                         (GPIO 14)

    IMPORTANTE: No olvides el diodo Flyback en paralelo con el ventilador.
```
