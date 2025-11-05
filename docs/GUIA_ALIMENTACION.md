# 🔌 Guía de Alimentación y Requisitos de Corriente

Esta guía detalla los requisitos de alimentación para el proyecto, asegurando un funcionamiento estable y seguro. El sistema utiliza una configuración de **doble voltaje**: 5V para la lógica y 12V para el ventilador.

## 1. Circuito de Lógica (5V)

Este circuito alimenta el cerebro del sistema.

- **Componentes Alimentados**: ESP32 DevKit, Pantalla OLED, Sensor BME280, Sensor MQ135, Bobina del Relé.
- **Consumo Estimado**:
  - **Reposo**: ~230 mA (0.23 A)
  - **Máximo**: ~510 mA (0.51 A)
- **Fuente de Alimentación Recomendada**:
  - **Voltaje**: 5V
  - **Amperaje Mínimo**: **1 Amperio (1000 mA)**

> ✅ **Uso de un Cargador de Móvil**: Utilizar un cargador de móvil USB de buena calidad (que proporcione 5V y al menos 1A) es una **opción excelente y recomendada** para alimentar este circuito. Puedes conectarlo directamente al puerto micro-USB del ESP32.

---

## 2. Circuito de Potencia (12V)

Este circuito se dedica exclusivamente a alimentar el ventilador.

- **Componente Alimentado**: Ventilador DC Brushless `QFR1212GHE`.
- **Consumo Requerido por el Ventilador**: **2.70 Amperios**.
- **Fuente de Alimentación Utilizada**: `Electronic LED Converter ED-S60W12V`.
- **Capacidad de la Fuente**: **5 Amperios**.

> ✅ **Compatibilidad**: Tu fuente de alimentación de 12V y 5A es **perfectamente adecuada** para el ventilador de 2.70A. Proporciona un excelente margen de seguridad, lo que garantiza que la fuente no se sobrecargará y el ventilador recibirá toda la potencia que necesita.

---

## 3. El GND Común (¡Configuración Crítica!)

### ¿Qué es?
Es la conexión física entre el terminal negativo (-) de tu fuente de 5V y el terminal negativo (-) de tu fuente de 12V.

### ¿Por qué es tan importante?
Los circuitos eléctricos necesitan un punto de referencia común para "entender" los niveles de voltaje. El ESP32 envía una señal de 3.3V al MOSFET para controlar el ventilador. Si el circuito del MOSFET (12V) no comparte la misma referencia de "cero voltios" (GND) que el ESP32 (5V), esa señal de 3.3V no tiene sentido y el control no funcionará.

**Sin un GND común, el ventilador no se encenderá o se comportará de forma errática.**

### ¿Cómo se conecta?
Es muy simple:
**Tira un cable que conecte el terminal negativo de tu fuente de 12V a cualquiera de los pines GND de tu placa ESP32.**

(El pin GND del ESP32 ya está conectado al negativo de la fuente de 5V a través del cable USB).

---

## Resumen Rápido

| Circuito | Voltaje | Amperaje Mínimo Recomendado | Tu Configuración | Estado |
| :--- | :---: | :---: | :--- | :---: |
| Lógica | 5V | **1 A** | Cargador de móvil (≥ 1A) | ✅ **OK** |
| Potencia | 12V | **2.7 A** | Fuente 12V / 5A | ✅ **OK** |
| **Conexión** | \- | **GND Común** | Conectar negativos de ambas fuentes | ⚠️ **¡OBLIGATORIO!** |
