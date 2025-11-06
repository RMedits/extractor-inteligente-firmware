# Diagrama de Pines - ESP32 DevKit (WROOM-32D) - Layout Fiel a la Realidad

Este diagrama muestra la disposición de los 38 pines de tu placa, imitando su apariencia física con dos columnas enfrentadas.

**Los pines utilizados en el proyecto "Extractor Inteligente v6.1" están resaltados en color.**

```mermaid
graph LR
    subgraph "ESP32 DevKit C (Tu Placa)"
        direction TB

        subgraph "Pines Izquierda"
            direction TB
            P1["1: GND"] --> P2["2: 3V3"] --> P3["3: EN"] --> P4["4: SENSOR_VP (GPIO36)"] --> P5["5: SENSOR_VN (GPIO39)"] --> P6["6: IO34 (GPIO34)"] --> P7["7: IO35 (GPIO35)"] --> P8["8: IO32 (GPIO32)"] --> P9["9: IO33 (GPIO33)"] --> P10["10: IO25 (GPIO25)"] --> P11["11: IO26 (GPIO26)"] --> P12["12: IO27 (GPIO27)"] --> P13["13: IO14 (GPIO14)"] --> P14["14: IO12 (GPIO12)"] --> P15["15: IO13 (GPIO13)"] --> P16["16: IO15 (GPIO15)"] --> P17["17: IO2 (GPIO2)"] --> P18["18: GND"] --> P19["19: IO4 (GPIO4)"] --> P20["20: IO0 (GPIO0)"]
        end

        subgraph " "
           direction TB
           ESP["<br/><br/><br/><br/><br/><br/><b>ESP32</b><br/>WROOM-32D<br/><br/><br/><br/><br/><br/><br/><br/>"]
        end

        subgraph "Pines Derecha"
            direction TB
            P21["21: IO17 (GPIO17)"] --> P22["22: IO16 (GPIO16)"] --> P23["23: IO5 (GPIO5)"] --> P24["24: IO18 (GPIO18)"] --> P25["25: IO19 (GPIO19)"] --> P26["26: IO21 (GPIO21)"] --> P27["27: RXD0 (GPIO3)"] --> P28["28: TXD0 (GPIO1)"] --> P29["29: IO22 (GPIO22)"] --> P30["30: IO23 (GPIO23)"] --> P31["31: NC"] --> P32["32: IO38 (GPIO38)"] --> P33["33: IO39 (GPIO39)"] --> P34["34: IO36 (GPIO36)"] --> P35["35: IO37 (GPIO37)"] --> P36["36: IO34 (GPIO34)"] --> P37["37: IO33 (GPIO33)"] --> P38["38: GND"]
        end
    end

    %% --- Estilos de los Pines Usados ---
    %% Columna Izquierda
    style P1 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black
    style P2 fill:#80CBC4,stroke:#004D40,stroke-width:2px,color:black
    style P6 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P8 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P9 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P10 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P11 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P12 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P13 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P18 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black

    %% Columna Derecha
    style P26 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style P29 fill:#90CAF9,stroke:#0D47A1,stroke-width:2px,color:black
    style P30 fill:#FFAB91,stroke:#BF360C,stroke-width:2px,color:black
    style P38 fill:#BDBDBD,stroke:#212121,stroke-width:2px,color:black

    %% Nota sobre el pin de 5V
    note["Nota: El pin de 5V (VIN) no está en tu lista.<br/>Si tu placa tiene un pin VIN, úsalo para la alimentación de 5V."]
    style note fill:#FFF9C4,stroke:#F57F17,stroke-width:1px,color:black
```

### Leyenda de Colores
- **Naranja**: Pines de control y sensores (GPIO).
- **Azul**: Pines del bus de comunicación I2C.
- **Verde**: Pines de alimentación (3.3V / 5V).
- **Gris**: Pines de tierra (GND).
- **Amarillo**: Nota importante.
