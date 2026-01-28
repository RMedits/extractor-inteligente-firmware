# ✅ Esquema visual de montaje del MOSFET (low‑side)

Este esquema muestra el montaje recomendado para el MOSFET como interruptor de bajo lado, con PWM desde el ESP32.

```mermaid
graph TD
    subgraph "Potencia (12V/24V)"
        VPLUS["+12V / +24V"] --> FAN["Ventilador (Delta)"]
        FAN --> DRAIN["Drain (D) MOSFET N‑channel"]
        DRAIN --> SOURCE["Source (S) MOSFET"]
        SOURCE --> GND_POT["GND Potencia"]
    end

    subgraph "Control (ESP32)"
        GPIO19["GPIO 19 (PWM)"] --> R100["R 100Ω (serie)"]
        R100 --> GATE["Gate (G) MOSFET"]
        GATE --> PD["Pull‑down 100k"] --> GND_LOG["GND ESP32"]
    end

    %% GND común obligatorio
    GND_LOG -- "GND común obligatorio" --> GND_POT
```

## Conexiones clave (resumen)
- **+V (12V/24V)** directo al ventilador.
- **Retorno del ventilador** al **Drain** del MOSFET.
- **Source** del MOSFET a **GND**.
- **Gate** desde **GPIO 19 (PWM)** con **R 100Ω** en serie.
- **Pull‑down 100k** a GND para apagar al inicio.
- **GND común** entre fuente de potencia y ESP32.
