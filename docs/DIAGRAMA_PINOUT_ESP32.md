# Diagrama de Pines - ESP32 DevKit (WROOM-32D)

Este diagrama muestra la disposición de los 38 pines de la placa de desarrollo ESP32 DevKit C, imitando su apariencia física para una fácil identificación.

**Los pines utilizados en el proyecto "Extractor Inteligente v6.1" están resaltados en color.**

```mermaid
graph TD
    subgraph "ESP32 DevKit C (Vista Superior)"
        direction LR

        subgraph "Pines Izquierda"
            direction TB
            L1["1: EN"] --- L2["2: GPIO36"] --- L3["3: GPIO39"] --- L4["4: GPIO34"] --- L5["5: GPIO35"] --- L6["6: GPIO32"] --- L7["7: GPIO33"] --- L8["8: GPIO25"] --- L9["9: GPIO26"] --- L10["10: GPIO27"] --- L11["11: GPIO14"] --- L12["12: GPIO12"] --- L13["13: GND"] --- L14["14: GPIO13"] --- L15["15: GPIO9"] --- L16["16: GPIO10"] --- L17["17: GPIO11"] --- L18["18: 3.3V"] --- L19["19: GND"]
        end

        subgraph " "
           direction TB
           ESP["<br/><br/><br/><br/><br/><br/><b>ESP32</b><br/>WROOM-32D<br/><br/><br/><br/><br/><br/><br/><br/>"]
        end

        subgraph "Pines Derecha"
            direction TB
            R19["20: 5V (VIN)"] --- R18["21: GPIO7"] --- R17["22: GPIO8"] --- R16["23: GPIO15"] --- R15["24: GPIO2"] --- R14["25: GPIO0"] --- R13["26: GPIO4"] --- R12["27: GPIO16"] --- R11["28: GPIO17"] --- R10["29: GPIO5"] --- R9["30: GPIO18"] --- R8["31: GPIO19"] --- R7["32: GND"] --- R6["33: GPIO21 (SDA)"] --- R5["34: GPIO3 (RX0)"] --- R4["35: GPIO1 (TX0)"] --- R3["36: GPIO22 (SCL)"] --- R2["37: GPIO23"] --- R1["38: GND"]
        end
    end

    %% --- Conexiones del Proyecto ---
    L4 --- MQ135["<-- (AOUT) Sensor MQ135"]
    L6 --- EncA["<-- (TRA) Encoder A"]
    L7 --- EncB["<-- (TRB) Encoder B"]
    L8 --- BtnBack["<-- (confirm) Botón Back/Cancel"]
    L9 --- BtnPause["<-- (bak) Botón Pausa"]
    L10 --- BtnOK["<-- (push) Botón OK/Enter"]
    L11 --- PWM["<-- (Gate) MOSFET PWM"]
    L18 --- VCC3V["<-- Alimentación 3.3V"]
    L19 --- GND1["<-- Tierra Común"]
    
    R19 --- VCC5V["(VIN) Alimentación 5V -->"]
    R6 --- SDA["(SDA) I2C Bus -->"]
    R3 --- SCL["(SCL) I2C Bus -->"]
    R2 --- Relay["Control Relé -->"]
    
    %% --- Estilos de los Pines Usados ---
    style L4 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L6 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L7 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L8 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L9 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L10 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L11 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style L18 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black
    style L19 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black
    
    style R19 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black
    style R6 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style R3 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style R2 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    
    %% --- Estilos de las Etiquetas ---
    style MQ135 fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style EncA fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style EncB fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style BtnBack fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style BtnPause fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style BtnOK fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style PWM fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style VCC3V fill:#80CBC4,stroke:#004D40,stroke-width:1px,color:black
    style GND1 fill:#BDBDBD,stroke:#212121,stroke-width:1px,color:black
    style VCC5V fill:#80CBC4,stroke:#004D40,stroke-width:1px,color:black
    style SDA fill:#90CAF9,stroke:#0D47A1,stroke-width:1px,color:black
    style SCL fill:#90CAF9,stroke:#0D47A1,stroke-width:1px,color:black
    style Relay fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black

```

### Leyenda de Colores
- **Naranja**: Pines de control y sensores (GPIO).
- **Azul**: Pines del bus de comunicación I2C.
- **Verde**: Pines de alimentación (3.3V / 5V).
- **Gris**: Pines de tierra (GND).
