🔌 Esquemas y Conexiones

Este documento detalla las conexiones de hardware necesarias para el proyecto.

¡ADVERTENCIA! Trabajarás con dos voltajes: 3.3V/5V para la lógica y 12V para el ventilador. Asegúrate de que las alimentaciones estén separadas. Todos los GND (ESP32, 12V, Sensores) deben estar conectados entre sí (GND Común).

1. Asignación de Pines (ESP32 DevKit HW-394)

| GPIO / Pin Lógico | Pin físico (mirando la placa de frente, USB arriba) | Serigrafía impresa | Componente / Nodo | Conexión | Propósito |
|-------------------|------------------------------------------------------|--------------------|-------------------|----------|-----------|
| 3V3               | 1                                                    | 3V3                | Bus de 3.3V       | VCC      | Alimentación de lógica a 3.3V |
| GND               | 2 / 17                                               | GND                | Tierra común      | GND      | Referencia común para todo |
| VIN               | 16                                                   | VIN                | Bus de 5V         | VCC      | Alimentación desde USB/5V externo (Relé, MQ135) |
| GPIO21            | 11                                                   | D21                | Bus I2C           | SDA      | Datos I2C (OLED + BME280) |
| GPIO22            | 14                                                   | D22                | Bus I2C           | SCL      | Reloj I2C (OLED + BME280) |
| GPIO34            | 27                                                   | D34                | Sensor MQ135      | AOUT     | Lectura analógica de calidad de aire |
| GPIO32            | 25                                                   | D32                | Encoder           | CLK      | Canal A (giro) |
| GPIO33            | 24                                                   | D33                | Encoder           | DT       | Canal B (giro) |
| GPIO27            | 21                                                   | D27                | Botón OK / Enter  | encoder_push | Confirmar selección (usar INPUT_PULLUP) |
| GPIO25            | 23                                                   | D25                | Botón Back / Cancel | confirm | Cancelar / volver atrás (usar INPUT_PULLUP) |
| GPIO26            | 22                                                   | D26                | Botón Pausa       | bak      | Pausar / reanudar ciclo manual (usar INPUT_PULLUP) |
| GPIO23            | 15                                                   | D23                | Módulo Relé       | IN       | Control de encendido general (HIGH activa) |
| GPIO14            | 20                                                   | D14                | MOSFET            | Gate     | PWM para velocidad del ventilador |

> 💡 **Orientación rápida**: si sostienes la placa con el conector USB-C hacia arriba, los pines numerados del 1 al 15 quedan a la izquierda (de arriba abajo) y los pines 16 al 30 a la derecha. Consulta el diagrama de pines actualizado (`docs/DIAGRAMA_PINOUT_ESP32.md`) para ver la vista completa a escala.

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