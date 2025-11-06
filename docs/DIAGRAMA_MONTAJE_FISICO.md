# Diagrama de Montaje Físico (Layout)

Este diagrama representa una disposición física sugerida para los componentes dentro de una carcasa, destacando la separación de zonas y las consideraciones de cableado.

```mermaid
graph TD
    subgraph "Carcasa / Caja de Montaje"
        direction LR

        subgraph "Zona de Control (Bajo Voltaje, 3.3V/5V)"
            id1[ESP32 DevKit]
            id2(Placa de Conexiones / Protoboard)
            id1 -- "Cableado Corto y Ordenado" --> id2
        end

        subgraph "Zona de Potencia (Alto Voltaje, 12V)"
            id3(Terminales Fuente 12V)
            id4(Módulo Relé)
            id5(MOSFET + Diodo)
            id3 --> id4 --> id5
        end

        %% Conexiones entre zonas
        id1 -- "GPIO 27 (Control Relé)" --> id4
        id1 -- "GPIO 14 (PWM MOSFET)" --> id5
    end

    subgraph "Panel Frontal (Accesible al Usuario)"
        id6["Módulo Integrado (OLED + Controles)"]
    end

    subgraph "Rejilla de Ventilación (Expuesto al Aire)"
        id9[Sensor BME280]
        id10[Sensor MQ135]
        id9 -. "IMPORTANTE: Separar al menos 1-2 cm" .-> id10
        note["Nota: Ambos sensores deben estar\nlejos de fuentes de calor\n(ESP32, MOSFET, MQ135 mismo)"]
        style note fill:#f9f,stroke:#333,stroke-width:2px
    end

    %% Conexiones a periféricos
    id2 -- "Cableado a Módulo" --> id6
    id2 -- "Cable I2C (< 30cm)" --> id9
    id2 -- "Cable Analógico (< 20cm)" --> id10
    id5 -- "Cable de Potencia (Calibre Adecuado)" --> id11((Ventilador Externo))

    %% Estilos para claridad
    style id1,id2 fill:#4CAF50,color:#fff
    style id3,id4,id5 fill:#F44336,color:#fff
    style id6 fill:#FF9800,color:#fff
    style id9,id10 fill:#2196F3,color:#fff
```

