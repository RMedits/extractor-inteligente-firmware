# **Especificación de Ingeniería: Sistema de Extracción Inteligente Dual (Variantes 12V y 24V)**

## **1\. Alcance del Proyecto**

Este documento detalla la implementación técnica para dos sistemas de extracción de aire independientes. Aunque ambos comparten la lógica de control basada en ESP32, las diferencias en los actuadores (ventiladores Delta) obligan a segregar el diseño de hardware en dos variantes distintas para garantizar la seguridad eléctrica y la estabilidad.

* **Variante A (Galería):** Sistema de 12V basado en Delta QFR1212GHE.  
* **Variante B (Baño):** Sistema de 24V basado en Delta PFB1224UHE.

## ---

**2\. Variante A: Montaje 12V (Galería)**

**Actuador:** Delta QFR1212GHE (12V DC, 2.70A, \~32.4W).

### **2.1 Lista de Materiales (BOM) \- Variante A**

1. **Fuente de Alimentación Principal:** 12V DC, **mínimo 5A** (60W). *Nota: Se requiere margen de seguridad sobre los 2.7A nominales para el pico de arranque.*  
2. **Conversor de Voltaje (Para el ESP32):** Módulo Buck Converter (ej. LM2596 o MP1584) de **12V a 5V**.  
   * *Propósito:* Alimentar el ESP32 desde la misma fuente de 12V del ventilador.  
3. **Relé de Corte:** Módulo Relé de 5V o 12V (capacidad de contacto min. 10A).  
4. **Protección PWM:** 1x MOSFET Canal N (2N7000 o BS170) \+ Resistencias (1kΩ, 10kΩ).

### **2.2 Esquema de Conexión (Variante 12V)**

En este escenario, compartimos la referencia de voltaje (GND) fácilmente.

1. **Potencia:** Fuente 12V (+) → COM del Relé → NO del Relé → Cable Rojo Ventilador. Fuente 12V (-) → Cable Negro Ventilador.  
2. **Lógica:** Fuente 12V (+) → Entrada Buck Converter → Salida 5V → Pin 5V/VIN del ESP32.  
3. **Control PWM (Buffer MOSFET):**  
   * **ESP32 GPIO** → Resistencia 1kΩ → Gate (G) del 2N7000.  
   * **Gate (G)** → Resistencia 10kΩ → GND (Pull-down para evitar arranque accidental).  
   * **Source (S)** → GND Común.  
   * **Drain (D)** → Cable **AMARILLO** (PWM) del Ventilador.  
   * *Nota:* Este circuito invierte la lógica (HIGH en ESP \= Ventilador OFF/Mínimo).

## ---

**3\. Variante B: Montaje 24V (Baño)**

Actuador: Delta PFB1224UHE (24V DC, 2.40A, 48.0W).  
Alerta Crítica: Este entorno es eléctricamente más hostil. El ventilador maneja casi 50W y cualquier "fuga" de 24V hacia el ESP32 (que trabaja a 3.3V) destruirá el microcontrolador instantáneamente.

### **3.1 Lista de Materiales (BOM) \- Variante B**

1. **Fuente de Alimentación Principal:** 24V DC, **mínimo 4A-5A** (100W+ recomendado).  
2. **Conversor de Voltaje (Para el ESP32):** Módulo Buck Converter "High Voltage" (ej. **LM2596HV** o XL4015) de **24V a 5V**.  
   * *Precaución:* Asegúrese de que el módulo soporte entrada de hasta 30V-40V. Muchos convertidores baratos de 12V se queman si se conectan a 24V.  
3. **Relé de Corte:** Módulo Relé con bobina de 5V (controlado por ESP) capaz de conmutar 24V DC.  
4. **Protección PWM (Aislamiento Total):** **Optoacoplador PC817** o 4N35.  
   * *Motivo:* En la variante de 24V, no queremos unión eléctrica directa entre el pin PWM del ventilador y el ESP32. El optoacoplador usa luz para transmitir la señal, protegiendo al 100% el ESP32 de los 24V.

### **3.2 Esquema de Conexión (Variante 24V)**

1. **Potencia:** Fuente 24V (+) → Relé → Cable Rojo Ventilador. Fuente 24V (-) → Cable Negro Ventilador.  
2. **Lógica:** Fuente 24V (+) → Entrada Buck Converter (HV) → Salida 5V → Pin 5V/VIN del ESP32.  
3. **Control PWM (Vía Optoacoplador):**  
   * **Lado ESP32 (Entrada):** Pin GPIO → Resistencia 330Ω → Ánodo (Pin 1\) del PC817. Cátodo (Pin 2\) del PC817 → GND ESP32.  
   * **Lado Ventilador (Salida):** Colector (Pin 4\) del PC817 → Cable **AMARILLO** (PWM) del Ventilador. Emisor (Pin 3\) del PC817 → GND de la Fuente de 24V.  
   * *Funcionamiento:* Cuando el ESP32 enciende el LED interno del opto, el transistor de salida conduce y conecta el PWM del ventilador a tierra (Señal LOW).

## ---

**4\. Identificación de Cables (Crucial para ambos)**

Las hojas de datos de Delta Industrial indican un código de colores diferente al estándar de PC. **Verifique esto con multímetro antes de conectar:**

| Función | Estándar PC (Noctua, etc.) | Estándar Delta Industrial (Probable) |
| :---- | :---- | :---- |
| **GND (-)** | Negro | **Negro** |
| **VCC (+)** | Amarillo | **Rojo** |
| **Tacómetro** | Verde | **Azul** (Frecuencia/FG) |
| **PWM** | Azul | **Amarillo** (Control) |

**Error Común:** Conectar el cable Azul del Delta al PWM pensando que es estándar de PC. En Delta, el Azul suele ser el sensor de RPM. Si envía 24V por ahí, dañará el ESP32. **Use el cable Amarillo para PWM en los Delta.**

## **5\. Estrategia de Software (Común para A y B)**

El código es idéntico para ambos sitios, solo cambia la calibración de la temperatura de disparo si es necesario. Dado que usamos MOSFET (Variante A) y Optoacoplador (Variante B), la señal se invierte en ambos casos.

**Configuración YAML para ESPHome:**

YAML

\# Definición de la salida PWM  
output:  
  \- platform: ledc  
    pin: GPIO18  
    frequency: 25000 Hz  \# Frecuencia obligatoria para Delta (evita zumbidos)  
    id: pwm\_fan\_output  
    \# Inverted: True es necesario para ambos circuitos (Mosfet y Opto)  
    \# Logic High en ESP32 \-\> Conecta PWM a GND \-\> Ventilador para/baja  
    inverted: true 

\# Componente Ventilador  
fan:  
  \- platform: speed  
    output: pwm\_fan\_output  
    name: "Extractor Inteligente"  
    id: main\_fan  
    \# Velocidad mínima para evitar stall (ajustar probando)  
    speed\_count: 100 

## **6\. Resumen de Diferencias Críticas**

| Característica | Variante A (Galería) | Variante B (Baño) |
| :---- | :---- | :---- |
| **Voltaje Principal** | 12V DC | 24V DC |
| **Fuente Recomendada** | 12V / 5A | 24V / 5A |
| **Conversor DC-DC** | Estándar (12V a 5V) | **Alto Voltaje (Input hasta 40V)** |
| **Interfaz PWM** | MOSFET (2N7000) | **Optoacoplador (PC817)** |
| **Riesgo Eléctrico** | Moderado | **Alto (Peligro de daño a 3.3V)** |

**Recomendación final:** Para el montaje del baño (24V), no intente "ahorrar" usando el circuito de MOSFET simple. El optoacoplador cuesta céntimos y garantiza que si el ventilador falla o hay un pico de 24V, su ESP32 sobrevivirá.