# 📝 Guía de Montaje Físico y Consideraciones v7.2C

La correcta disposición física de los componentes es fundamental para el buen funcionamiento del sistema, la precisión de los sensores y la seguridad.

## 1. Diseño General y Separación de Circuitos

La regla más importante es **separar la electrónica de potencia (12V) de la electrónica de control y sensores (3.3V/5V)**.

- **Carcasa/Caja**: Se recomienda montar todo dentro de una caja de plástico.
- **Zonas**: Crea una "Zona de Potencia" (MOSFET, Ventilador, 12V) y una "Zona de Control" (ESP32, Sensores).
- **GND Común**: Es obligatorio que el negativo (-) de la fuente de 12V y el GND del ESP32 estén unidos en un solo punto.

---

## 2. Control PWM del Ventilador (4 Hilos)

El ventilador se controla por la línea PWM dedicada.

- **Señal PWM**: Conecta **GPIO 19** directamente al cable PWM del ventilador (4 hilos).
- **Alimentación/Corte**: Para ON/OFF de potencia se emplea un **MOSFET N-channel** en configuración low-side en lugar de un relé mecánico.

---

## 3. Protección con Diodo 1N5408

El ventilador Delta es un motor potente que genera picos de voltaje al apagarse (fuerza contraelectromotriz).

- **Instalación**: El diodo debe ir en paralelo con el ventilador.
- **Polaridad**:
    - El lado con la **franja blanca (Cátodo)** va al cable **POSITIVO (+12V)** del ventilador.
    - El otro lado (Ánodo) va al cable **NEGATIVO** del ventilador.
- **Importancia**: Sin este diodo, pueden aparecer picos que dañen el sistema.

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
