# Diagrama de Pines - ESP32 DevKit (WROOM-32D)

Este diagrama muestra la disposición de los 38 pines de la placa de desarrollo ESP32 DevKit C, comúnmente utilizada con el módulo ESP32-WROOM-32D.

**Los pines utilizados en el proyecto "Extractor Inteligente v6.1" están resaltados en color.**

```mermaid
graph LR
    subgraph "ESP32 DevKit C (38 Pines)"
        direction TB

        subgraph "Fila Izquierda"
            direction TB
            L1["1: EN (Reset)"]
            L2["2: VP (GPIO36)"]
            L3["3: VN (GPIO39)"]
            L4["4: GPIO34"]
            L5["5: GPIO35"]
            L6["6: GPIO32"]
            L7["7: GPIO33"]
            L8["8: GPIO25"]
            L9["9: GPIO26"]
            L10["10: GPIO27"]
            L11["11: GPIO14"]
            L12["12: GPIO12"]
            L13["13: GND"]
            L14["14: GPIO13"]
            L15["15: D2 (GPIO9)"]
            L16["16: D3 (GPIO10)"]
            L17["17: CMD (GPIO11)"]
            L18["18: 3.3V"]
            L19["19: GND"]
        end

        subgraph "Fila Derecha"
            direction TB
            R1["38: GND"]
            R2["37: GPIO23"]
            R3["36: GPIO22 (SCL)"]
            R4["35: GPIO1 (TX0)"]
            R5["34: GPIO3 (RX0)"]
            R6["33: GPIO21 (SDA)"]
            R7["32: GND"]
            R8["31: GPIO19"]
            R9["30: GPIO18"]
            R10["29: GPIO5"]
            R11["28: GPIO17"]
            R12["27: GPIO16"]
            R13["26: GPIO4"]
            R14["25: GPIO0"]
            R15["24: GPIO2"]
            R16["23: GPIO15"]
            R17["22: D1 (GPIO8)"]
            R18["21: CLK (GPIO7)"]
            R19["20: VIN (5V)"]
        end
    end

    %% --- Resaltar Pines Usados en el Proyecto v6.1 ---

    %% Fila Izquierda
    style L4 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L6 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L7 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L8 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L9 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L10 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L11 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L18 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black
    style L19 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black

    %% Fila Derecha
    style R2 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R3 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style R6 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style R19 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black

```

### Leyenda de Colores
- **Naranja**: Pines de control y sensores (GPIO).
- **Azul**: Pines del bus de comunicación I2C.
- **Verde**: Pines de alimentación (3.3V / 5V).
- **Gris**: Pines de tierra (GND).
