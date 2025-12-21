# 🔌 Esquema de Conexiones - v6.5C (Variantes 12V/24V)
PROYECTO: Extractor Inteligente Dual (Soporte Universal 12V/24V)

Este documento detalla las conexiones para dos variantes de hardware:
*   **Variante A (12V):** Galería (Fan Delta 12V - 2.70A)
*   **Variante B (24V):** Baño (Fan Delta 24V - 2.40A)

---

## ⚠️ ADVERTENCIA CRÍTICA: CÓDIGO DE COLORES DELTA
Los ventiladores industriales Delta NO siguen el estándar de PC. Verifique SIEMPRE antes de conectar:

| Función | Estándar PC (Noctua, etc) | **DELTA INDUSTRIAL (Su Ventilador)** |
| :--- | :--- | :--- |
| **GND (-)** | Negro | **NEGRO** |
| **VCC (+)** | Amarillo | **ROJO** |
| **TACH (RPM)** | Verde | **AZUL** (¡PELIGRO! No conectar a PWM) |
| **PWM (Control)** | Azul | **AMARILLO** (Conectar al Buffer) |

**IMPORTANTE:** Si conecta el cable AZUL al pin PWM, podría dañar el ventilador o el ESP32. **Use el cable AMARILLO para la señal PWM.**

---

## 1. Conexiones Lógicas Comunes (ESP32)

| Pin ESP32 | Componente | Función |
| :--- | :--- | :--- |
| **GPIO 21/22** | I2C | OLED, AHT20, BMP280 |
| **GPIO 32/33/27** | Encoder | A, B, Push |
| **GPIO 25/26** | Botones | Back, Pause |
| **GPIO 34** | MQ135 | Analog Out |
| **GPIO 23** | Relé | Señal Activación (3.3V) |
| **GPIO 14** | PWM Out | Señal hacia Buffer (MOSFET u Opto) |

---

## 2. Variante A: Montaje 12V (Galería)
*Uso típico:* Ventilador 12V hasta 3A. Tierra común compartida.

### Componentes de Potencia
*   **Fuente:** 12V DC (Mínimo 5A).
*   **Convertidor:** Buck Converter (12V -> 5V) para alimentar ESP32.
*   **Protección PWM:** MOSFET FQP30N06L o 2N7000.

### Diagrama de Conexión
1.  **Alimentación:**
    *   Fuente 12V (+) -> COM Relé.
    *   Fuente 12V (-) -> GND Común (Unir con GND ESP32).
    *   NO Relé -> **Cable ROJO** Ventilador.
    *   GND Común -> **Cable NEGRO** Ventilador.

2.  **Control PWM (Buffer MOSFET):**
    *   ESP32 GPIO 14 -> Resistencia 1kΩ -> Gate (G) MOSFET.
    *   Gate (G) -> Resistencia 10kΩ -> GND (Pull-down).
    *   Source (S) MOSFET -> GND.
    *   Drain (D) MOSFET -> **Cable AMARILLO** Ventilador.

---

## 3. Variante B: Montaje 24V (Baño)
*Uso típico:* Ventilador 24V Alta Potencia. **REQUIERE AISLAMIENTO.**

### Componentes de Potencia
*   **Fuente:** 24V DC (Mínimo 5A).
*   **Convertidor:** Buck Converter **High Voltage** (24V -> 5V) (ej. LM2596HV).
*   **Protección PWM:** Optoacoplador PC817 (Aislamiento Total).

### Diagrama de Conexión
1.  **Alimentación:**
    *   Fuente 24V (+) -> COM Relé (Bobina 5V).
    *   Fuente 24V (-) -> **Cable NEGRO** Ventilador.
    *   NO Relé -> **Cable ROJO** Ventilador.

2.  **Control PWM (Aislamiento Optoacoplador):**
    *   *Lado ESP32:*
        *   ESP32 GPIO 14 -> Resistencia 330Ω -> Ánodo (Pin 1) PC817.
        *   Cátodo (Pin 2) PC817 -> GND ESP32.
    *   *Lado Ventilador (24V):*
        *   Emisor (Pin 3) PC817 -> GND Fuente 24V.
        *   Colector (Pin 4) PC817 -> **Cable AMARILLO** Ventilador.

---

## 4. Configuración de Software
El firmware v6.5C detecta y usa lógica invertida automáticamente para ambas variantes:
*   **0% Velocidad:** PWM 255 (ESP32 High -> Buffer Conduce -> Fan Low).
*   **100% Velocidad:** PWM 0 (ESP32 Low -> Buffer Corta -> Fan High/Pullup).
