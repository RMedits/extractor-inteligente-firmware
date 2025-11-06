# Diagrama de Pines - ESP32 DevKit (30 Pines) - Layout Fiel a tu Placa

Este diagrama muestra la disposición de los 30 pines de tu placa, imitando su apariencia física con dos columnas enfrentadas y usando la numeración que proporcionaste.

**Los pines utilizados en el proyecto "Extractor Inteligente v6.1" están resaltados en color.**

```mermaid
graph LR
    subgraph "ESP32 DevKit (30 Pines)"
        direction TB

        subgraph "Pines Izquierda"
            direction TB
            I1["1: 3V3"] --> I2["2: GND"] --> I3["3: D15 (GPIO15)"] --> I4["4: D2 (GPIO2)"] --> I5["5: D4 (GPIO4)"] --> I6["6: D16 (GPIO16)"] --> I7["7: D17 (GPIO17)"] --> I8["8: D5 (GPIO5)"] --> I9["9: D18 (GPIO18)"] --> I10["10: D19 (GPIO19)"] --> I11["11: D21 (GPIO21)"] --> I12["12: RX0 (GPIO3)"] --> I13["13: TX0 (GPIO1)"] --> I14["14: D22 (GPIO22)"] --> I15["15: D23 (GPIO23)"]
        end

        subgraph " "
           direction TB
           ESP["<br/><br/><br/><br/><b>ESP32</b><br/>WROOM-32D<br/>(30 Pines)<br/><br/><br/><br/><br/>"]
        end

        subgraph "Pines Derecha"
            direction TB
            R1["16: VIN"] --> R2["17: GND"] --> R3["18: D13 (GPIO13)"] --> R4["19: D12 (GPIO12)"] --> R5["20: D14 (GPIO14)"] --> R6["21: D27 (GPIO27)"] --> R7["22: D26 (GPIO26)"] --> R8["23: D25 (GPIO25)"] --> R9["24: D33 (GPIO33)"] --> R10["25: D32 (GPIO32)"] --> R11["26: D35 (GPIO35)"] --> R12["27: D34 (GPIO34)"] --> R13["28: VN (GPIO39)"] --> R14["29: VP (GPIO36)"] --> R15["30: EN"]
        end
    end

    %% --- Estilos de los Pines Usados ---
    %% Columna Izquierda (1-15)
    style I1 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black  %% 3.3V
    style I2 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black  %% GND
    style I11 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black %% GPIO21 (SDA)
    style I14 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black %% GPIO22 (SCL)
    style I15 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black %% GPIO23 (Relay)

    %% Columna Derecha (16-30)
    style R1 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black  %% VIN (5V)
    style R2 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black  %% GND
    style R5 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black  %% GPIO14 (PWM)
    style R6 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black  %% GPIO27 (OK/Enter)
    style R7 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black  %% GPIO26 (Pause)
    style R8 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black  %% GPIO25 (Back)
    style R9 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black  %% GPIO33 (Encoder B)
    style R10 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black %% GPIO32 (Encoder A)
    style R12 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black %% GPIO34 (MQ135)

```

### Leyenda de Colores
- **Naranja**: Pines de control y sensores (GPIO).
- **Azul**: Pines del bus de comunicación I2C.
- **Verde**: Pines de alimentación (3.3V / 5V).
- **Gris**: Pines de tierra (GND).
