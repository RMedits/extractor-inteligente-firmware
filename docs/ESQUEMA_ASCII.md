# Esquema de Conexiones (Arte ASCII)

Este es un esquema simplificado de las conexiones, diseñado para ser legible en cualquier editor de texto. Es una referencia rápida y universal.

```text
                               +---------------------------------+
                               |         ESP32 DevKit            |
                               |                                 |
      (OLED, BME280) --[SDA]---| GPIO 21                         |
      (OLED, BME280) --[SCL]---| GPIO 22                         |
                               |                                 |
(Botón ENTER) ---[PULLUP]------| GPIO 25                         |
  (a GND)                      |                                 |
(Botón BACK) ----[PULLUP]------| GPIO 26                         |
  (a GND)                      |                                 |
                               |                                 |
(Relé IN) --------[CONTROL]---| GPIO 27                         |
                               |                                 |
(Encoder CLK) ----------------| GPIO 32                         |
(Encoder DT) -----------------| GPIO 33                         |
                               |                                 |
(MQ135 AOUT) ----[ANALOG]-----| GPIO 34                         |
                               |                                 |
(MOSFET Gate) ------[PWM]------| GPIO 14                         |
                               |                                 |
                               | 3.3V ----> (Alimentación BME280, OLED, Encoder)
                               | 5V ------> (Alimentación Relé, MQ135)
                               | GND ------> (GND Común para TODO)
                               +---------------------------------+


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
