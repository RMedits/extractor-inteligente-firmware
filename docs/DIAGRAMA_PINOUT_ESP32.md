# Diagrama de Pines - ESP32 DevKit (30 Pines) - Layout Fiel a tu Placa

Este diagrama muestra la disposición de los 30 pines de tu placa, imitando su apariencia física con dos columnas enfrentadas y usando la numeración que proporcionaste.

**Los pines utilizados en el proyecto "Extractor Inteligente v6.1" están resaltados en color.**

```mermaid
graph LR
    subgraph "ESP32 DevKit (30 Pines)"
        direction TB

        subgraph "Pines Izquierda"
            direction TB
            I1["1: 3V3"]
            I2["2: GND"]
            I3["3: D15 (GPIO15)"]
            I4["4: D2 (GPIO2)"]
            I5["5: D4 (GPIO4)"]
            I6["6: D16 (GPIO16)"]
            I7["7: D17 (GPIO17)"]
            I8["8: D5 (GPIO5)"]
            I9["9: D18 (GPIO18)"]
            I10["10: D19 (GPIO19)"]
            I11["11: D21 (GPIO21)"]
            I12["12: RX0 (GPIO3)"]
            I13["13: TX0 (GPIO1)"]
            I14["14: D22 (GPIO22)"]
            I15["15: D23 (GPIO23)"]
        end

        subgraph " "
           direction TB
           M["<br/><br/><br/><br/><b>ESP32</b><br/>WROOM-32D<br/>(30 Pines)<br/><br/><br/><br/><br/>"]
        end

        subgraph "Pines Derecha"
            direction TB
            R1["16: VIN"]
            R2["17: GND"]
            R3["18: D13 (GPIO13)"]
            R4["19: D12 (GPIO12)"]
            R5["20: D14 (GPIO14)"]
            R6["21: D27 (GPIO27)"]
            R7["22: D26 (GPIO26)"]
            R8["23: D25 (GPIO25)"]
            R9["24: D33 (GPIO33)"]
            R10["25: D32 (GPIO32)"]
            R11["26: D35 (GPIO35)"]
            R12["27: D34 (GPIO34)"]
            R13["28: VN (GPIO39)"]
            R14["29: VP (GPIO36)"]
            R15["30: EN"]
        end
    end

    %% --- Conexiones del Proyecto ---
    I1 --- VCC3V["<-- Alimentación 3.3V"]
    I2 --- GND_I2["<-- Tierra Común"]
    I11 --- SDA["<-- (SDA) I2C Bus"]
    I14 --- SCL["<-- (SCL) I2C Bus"]
    I15 --- Relay["<-- Control Relé"]

    R1 --- VCC5V["Alimentación 5V (VIN) -->"]
    R2 --- GND_R2["Tierra Común -->"]
    R5 --- PWM["PWM Ventilador -->"]
    R6 --- BtnOK["Botón OK/Enter -->"]
    R7 --- BtnPause["Botón Pausa -->"]
    R8 --- BtnBack["Botón Back/Cancel -->"]
    R9 --- EncB["Encoder B -->"]
    R10 --- EncA["Encoder A -->"]
    R12 --- MQ135["Sensor MQ135 (AOUT) -->"]

    %% --- Estilos de los Pines Usados ---
    style I1 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black
    style I2 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black
    style I11 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style I14 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style I15 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black

    style R1 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black
    style R2 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black
    style R5 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R6 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R7 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R8 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R9 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R10 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style R12 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black

    %% --- Estilos de las Etiquetas ---
    style VCC3V fill:#80CBC4,stroke:#004D40,stroke-width:1px,color:black
    style GND_I2 fill:#BDBDBD,stroke:#212121,stroke-width:1px,color:black
    style SDA fill:#90CAF9,stroke:#0D47A1,stroke-width:1px,color:black
    style SCL fill:#90CAF9,stroke:#0D47A1,stroke-width:1px,color:black
    style Relay fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black

    style VCC5V fill:#80CBC4,stroke:#004D40,stroke-width:1px,color:black
    style GND_R2 fill:#BDBDBD,stroke:#212121,stroke-width:1px,color:black
    style PWM fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style BtnOK fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style BtnPause fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style BtnBack fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style EncB fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style EncA fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black
    style MQ135 fill:#FFAB91,stroke:#BF360C,stroke-width:1px,color:black