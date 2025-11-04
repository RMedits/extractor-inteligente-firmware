🔌 Esquemas y Conexiones

Este documento detalla las conexiones de hardware necesarias para el proyecto.

¡ADVERTENCIA! Trabajarás con dos voltajes: 3.3V/5V para la lógica y 12V para el ventilador. Asegúrate de que las alimentaciones estén separadas. Todos los GND (ESP32, 12V, Sensores) deben estar conectados entre sí (GND Común).

1. Asignación de Pines (ESP32 DevKit)

Pin (ESP32)

Componente

Conexión

Propósito

GPIO21

Bus I2C

SDA

OLED y BME280

GPIO22

Bus I2C

SCL

OLED y BME280

GPIO34

Sensor MQ135

AOUT

Lectura Analógica Calidad Aire

GPIO32

Encoder

CLK

Navegación Menú

GPIO33

Encoder

DT

Navegación Menú

GPIO25

Botón ENTER

-

Confirmar (usar INPUT_PULLUP)

GPIO26

Botón BACK

-

Cancelar (usar INPUT_PULLUP)

GPIO27

Módulo Relé

IN

Control Encendido General (Seguridad)

GPIO14

MOSFET

Gate (G)

Control PWM Velocidad Ventilador

5V

Varios

VCC

Alimentación Relé, MQ135

3.3V

Varios

VCC

Alimentación BME280, OLED, Encoder

GND

Común

GND

GND Común para todo

2. Circuito de Potencia (12V Ventilador)

Este es el circuito para controlar la velocidad del ventilador de 12V usando PWM desde el ESP32 (que funciona a 3.3V).

Fuente 12V (+): Conectar al pin COM del Relé.

Relé (NO - Normally Open): Conectar al cable positivo (+) del Ventilador 12V.

Ventilador 12V (-): Conectar al pin Drain (D) del MOSFET (ej. IRLZ44N).

MOSFET Source (S): Conectar al GND común (que también va al GND de la fuente de 12V y al GND del ESP32).

MOSFET Gate (G): Conectar directamente al GPIO14 del ESP32.

(Opcional pero recomendado): Añadir una resistencia de 10kΩ entre el Gate (G) y el Source (S) (pull-down) para asegurar que el MOSFET esté apagado durante el arranque del ESP32.

DIODO FLYBACK (¡OBLIGATORIO!):

Conectar un diodo (ej. 1N4007) en paralelo con el ventilador.

El cátodo (la raya plateada del diodo) va al lado positivo del ventilador (al cable que viene del relé).

El ánodo (el lado negro) va al lado negativo del ventilador (al Drain del MOSFET).

Esto protege tu MOSFET de picos de tensión inversa cuando el ventilador se apaga.

3. Conexión de Controles

Botones (ENTER/BACK): Conectar un pin del botón a GND y el otro pin al GPIO correspondiente (ej. GPIO25). Habilitar la resistencia INPUT_PULLUP interna en el código.

Encoder: Conectar CLK, DT, y SW (si se usa) a los GPIOs. Conectar VCC a 3.3V y GND a GND.