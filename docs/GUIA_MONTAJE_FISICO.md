# 📝 Guía de Montaje Físico y Consideraciones v7.1C FINAL

La correcta disposición física de los componentes es fundamental para el buen funcionamiento del sistema, la precisión de los sensores y la seguridad.

## 1. Diseño General y Separación de Circuitos

La regla más importante es **separar la electrónica de potencia (12V) de la electrónica de control y sensores (3.3V/5V)**.

- **Carcasa/Caja**: Se recomienda montar todo dentro de una caja de plástico.
- **Zonas**: Crea una "Zona de Potencia" (Relé, 12V, ventilador) y una "Zona de Control" (ESP32, Sensores).
- **GND Común**: Es obligatorio que el negativo (-) de la fuente de 12V y el GND del ESP32 estén unidos en un solo punto.

---

## 2. Conexión del Ventilador PWM (4 hilos)

El ventilador Delta usa cable **azul** para la señal PWM y requiere GND común.

- **Cable Azul (PWM):** Conéctalo al pin **D19 (GPIO19)** del ESP32 (fila S). Mantén el cable corto para minimizar ruido.
- **Alimentación del Ventilador:** Alimenta el cable rojo con +12V y el negro a GND. Si usas el relé KY-019 como corte principal, coloca la línea de +12V a través de COM/NO y controla el relé con **D23**.
- **GND Común:** Une el GND del ventilador (negro) con el GND de la placa de expansión del ESP32.

---

## 3. Protección con Diodo 1N5408 (si usas relé)

El relé KY-019 tiene bobina y necesita un diodo de rueda libre para proteger el ESP32.

- **Instalación**: Coloca el diodo en paralelo con la bobina del relé.
- **Polaridad**:
    - Franja blanca (Cátodo) al pin **VCC (+5V)** de la bobina.
    - Ánodo al pin **GND** de la bobina.
- **Importancia**: Evita picos inversos cuando el relé abre/cierra.

---

## 4. Ubicación de los Sensores

- **AHT20/BMP280**: Lejos de fuentes de calor (ESP32, MQ135). Debe estar cerca de una rejilla para medir el aire real.
- **MQ135**: Necesita precalentarse. Colócalo de forma que el calor que genera no afecte al AHT20/BMP280.

---

## 5. Módulo de Pantalla y Controles

El módulo Estardyn facilita el montaje:
1. Realiza un corte rectangular en el frontal de la caja.
2. Fija el módulo con tornillos o adhesivo.
3. Asegúrate de que el **Encoder PUSH** (pulsar rueda) tiene espacio para hacer "clic" correctamente.

---

## 6. Longitud de Cables
- **I2C (OLED/AHT20/BMP280)**: Máximo 25cm. Si el cable es muy largo, la pantalla puede mostrar basura o el sensor fallar.
- **Potencia**: Usa cable de calibre 18 AWG o similar para los 12V del ventilador.
