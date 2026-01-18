# 🔌 Guía de Alimentación y Requisitos de Corriente v7.2C

Esta guía detalla los requisitos de alimentación para el proyecto v7.2C. El sistema utiliza una configuración de **doble voltaje**: 5V para la lógica y 12V para el ventilador.

## 1. Circuito de Lógica (5V)
Este circuito alimenta el "cerebro" y los sensores.

-- **Componentes**: ESP32, OLED, AHT20/BMP280, MQ135. Para el corte/seguridad de potencia se utiliza un MOSFET N-channel (gate control), no relé.
- **Fuente Recomendada**: Cargador de móvil USB de buena calidad (5V / 1A mínimo).
- **Conexión**: Puerto USB del ESP32 o pin Vin (5V) de la placa de expansión.

## 2. Circuito de Potencia (12V)
Dedicado exclusivamente al ventilador de alta potencia.

- **Componente**: Ventilador Delta `QFR1212GHE`.
- **Consumo**: **2.70 Amperios**.
- **Fuente Recomendada**: Fuente de alimentación 12V estable de al menos **4 Amperios** (48W).
- **Nota**: Tu fuente de 5A también es perfecta (proporciona más margen).

## 3. El GND Común (¡CONFIGURACIÓN CRÍTICA!)

### ¿Qué es?
Es la unión física de los polos negativos (-) de ambas fuentes (5V y 12V).

### ¿Por qué es obligatorio?
El ESP32 controla el ventilador mediante una señal PWM de 3.3V. Si el circuito de 12V no comparte el mismo "suelo" (referencia de 0V) que el ESP32, la señal PWM no funcionará y el ventilador no girará o lo hará de forma errática.

### ¿Cómo conectarlo?
**Conecta un cable desde el polo negativo (-) de la fuente de 12V a cualquier pin GND de la placa de expansión del ESP32.**

---

## Resumen Rápido

| Circuito | Voltaje | Amperaje Mín. | Estado |
| :--- | :---: | :---: | :---: |
| Lógica | 5V | 1 A | ✅ Requerido |
| Potencia | 12V | 4 A | ✅ Requerido |
| **Referencia** | - | **GND Común** | ⚠️ **OBLIGATORIO** |
