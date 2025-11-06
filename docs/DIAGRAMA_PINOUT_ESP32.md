# Diagrama de Pines - ESP32 DevKit (WROOM-32D) - Corregido

Este diagrama muestra la disposición de los 38 pines de la placa de desarrollo ESP32 DevKit C, imitando su apariencia física para una fácil identificación. **Esta versión corrige el orden de los pines para que coincida con la placa real.**

**Los pines utilizados en el proyecto "Extractor Inteligente v6.1" están resaltados en color.**

```mermaid
graph TD
    subgraph "ESP32 DevKit C (Vista Superior)"
        direction LR

        subgraph "Pines Izquierda (1-19)"
            direction TB
            L1["1: EN"] --- L2["2: GPIO36"] --- L3["3: GPIO39"] --- L4["4: GPIO34"] --- L5["5: GPIO35"] --- L6["6: GPIO32"] --- L7["7: GPIO33"] --- L8["8: GPIO25"] --- L9["9: GPIO26"] --- L10["10: GPIO27"] --- L11["11: GPIO14"] --- L12["12: GPIO12"] --- L13["13: GND"] --- L14["14: GPIO13"] --- L15["15: GPIO9"] --- L16["16: GPIO10"] --- L17["17: GPIO11"] --- L18["18: 3.3V"] --- L19["19: GND"]
        end

        subgraph " "
           direction TB
           ESP["<br/><br/><br/><br/><br/><br/><b>ESP32</b><br/>WROOM-32D<br/><br/><br/><br/><br/><br/><br/><br/>"]
        end

        subgraph "Pines Derecha (20-38)"
            direction TB
            R20["20: 5V (VIN)"] --- R21["21: GND"] --- R22["22: GPIO23"] --- R23["23: GPIO22 (SCL)"] --- R24["24: GPIO1 (TX0)"] --- R25["25: GPIO3 (RX0)"] --- R26["26: GPIO21 (SDA)"] --- R27["27: GND"] --- R28["28: GPIO19"] --- R29["29: GPIO18"] --- R30["30: GPIO5"] --- R31["31: GPIO17"] --- R32["32: GPIO16"] --- R33["33: GPIO4"] --- R34["34: GPIO0"] --- R35["35: GPIO2"] --- R36["36: GPIO15"] --- R37["37: GPIO8"] --- R38["38: GPIO7"]
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
    
    R20 --- VCC5V["(VIN) Alimentación 5V -->"]
    R22 --- Relay["Control Relé -->"]
    R23 --- SCL["(SCL) I2C Bus -->"]
    R26 --- SDA["(SDA) I2C Bus -->"]
    
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
    
    style R20 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black
    style R22 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R23 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style R26 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    
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
