# 📝 Guía de Montaje Físico y Consideraciones

La correcta disposición física de los componentes es fundamental para el buen funcionamiento del sistema, la precisión de los sensores y la seguridad.

## 1. Diseño General y Separación de Circuitos

La regla más importante es **separar la electrónica de potencia (12V) de la electrónica de control y sensores (3.3V/5V)**.

- **Carcasa/Caja**: Se recomienda encarecidamente montar todo dentro de una caja de plástico o un material no conductor. Esto protege los circuitos y evita cortocircuitos accidentales.
- **Separación Física**: Dentro de la caja, intenta crear dos "zonas":
  - **Zona de Potencia**: Donde se ubican el relé, el MOSFET, los terminales para la fuente de 12V y el ventilador.
  - **Zona de Control**: Donde se ubican el ESP32, los sensores y los controles.
- **Distancia Mínima**: Mantén una distancia de al menos **2-3 centímetros** entre los componentes de potencia y los de control para minimizar la interferencia electromagnética.

---

## 2. Ubicación de los Sensores (¡Crítico!)

La ubicación de los sensores determina la eficacia del modo automático.

- **BME280 (Temperatura y Humedad)**:
  - **Flujo de Aire**: Debe estar en un lugar donde pueda medir el aire real del ambiente, no el aire estancado dentro de la caja.
  - **Lejos del Calor**: **No lo coloques cerca del ESP32, del MOSFET o del sensor MQ135**. Estos componentes generan calor y alterarán drásticamente las lecturas de temperatura, provocando que el ventilador se active incorrectamente.
  - **Sugerencia**: Móntalo en una pequeña rejilla de ventilación de la propia caja, con el sensor mirando hacia afuera.

- **MQ135 (Calidad de Aire)**:
  - **Flujo de Aire**: Al igual que el BME280, necesita estar expuesto al aire del ambiente.
  - **Separación del BME280**: El MQ135 se calienta por diseño para funcionar. Colócalo a **al menos 1-2 cm de distancia** del BME280 para no falsear la lectura de temperatura.
  - **Sugerencia**: Móntalo en la misma rejilla que el BME280, pero con una pequeña separación.

---

## 3. Longitud y Gestión de Cables

La longitud de los cables puede afectar la integridad de la señal, especialmente para los sensores.

- **Cables de Potencia (12V para el ventilador)**:
  - **Calibre**: Usa un cable de calibre adecuado para la corriente que consume tu ventilador (AWG 22 o 20 suele ser suficiente para la mayoría de los ventiladores de PC).
  - **Longitud**: Mantenlos lo más cortos posible.

- **Cables de Sensores (I2C y Analógico)**:
  - **Límite de I2C (SDA/SCL)**: El bus I2C es sensible a la longitud. Para una comunicación fiable sin componentes adicionales, los cables para el BME280 y la pantalla OLED **no deberían superar los 20-30 cm**. Si necesitas más distancia, tendrías que usar un módulo expansor de bus I2C.
  - **Límite Analógico (MQ135)**: Las señales analógicas son susceptibles al ruido. Mantén el cable del pin `AOUT` del MQ135 al ESP32 **lo más corto posible (idealmente menos de 20 cm)**.

- **Cables de Controles (Encoder y Botones)**:
  - Son señales digitales y menos sensibles. Pueden ser más largos, pero por buena práctica, mantenlos ordenados y sin una longitud excesiva.

---

## 4. Reducción de Ruido e Interferencias

El motor del ventilador y el relé pueden introducir ruido eléctrico en el sistema.

- **Cruzar Cables**: Evita pasar los cables de los sensores en paralelo y pegados a los cables de 12V del ventilador. Si tienen que cruzarse, hazlo en un ángulo de 90 grados para minimizar la interferencia.
- **Cables Trenzados (Opcional)**: Si experimentas lecturas de sensores inestables y los cables son algo largos, puedes trenzar el cable de señal (SDA, SCL, AOUT) con un cable conectado a GND. Esto crea un blindaje simple que puede reducir el ruido.

---

## 5. Ejemplo de Disposición Física (Layout)

Imagina una caja de montaje estándar:

1.  **Panel Frontal**:
    - Monta la pantalla OLED en una ventana recortada.
    - Monta el encoder rotativo y los dos botones (ENTER/BACK) para que sean accesibles desde fuera.

2.  **Interior - Parte Trasera/Inferior**:
    - Fija la placa del ESP32.
    - Cerca del ESP32, pero en la "zona de control", monta una pequeña protoboard o placa de circuito para organizar las conexiones de los sensores y controles.

3.  **Interior - Parte Separada (Zona de Potencia)**:
    - Monta el relé y el MOSFET.
    - Coloca los terminales de tornillo para la entrada de 12V y la salida al ventilador.

4.  **Pared Lateral o Superior (con rejilla)**:
    - Haz pequeños agujeros o una rejilla.
    - Monta los sensores BME280 y MQ135 de manera que queden expuestos al aire exterior, pero protegidos de salpicaduras directas.

Este diseño asegura que los controles sean accesibles, los sensores midan correctamente y los circuitos de potencia y control estén debidamente separados.
