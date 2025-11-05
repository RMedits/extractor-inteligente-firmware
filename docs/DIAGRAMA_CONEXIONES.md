# Diagrama de Conexiones (Mermaid)

Este diagrama muestra las conexiones lógicas entre el ESP32 y todos los componentes periféricos. GitHub renderizará automáticamente este código como un diagrama de bloques.

```mermaid
graph LR
    subgraph Sensores
        BME280((BME280))
        MQ135((MQ135))
    end

    subgraph Controles
        Encoder((Encoder Rotativo))
        BtnEnter((Botón ENTER))
        BtnBack((Botón BACK))
    end

    subgraph "Circuito de Potencia 12V"
        Power12V(Fuente 12V) --> Relay{Relé};
        Relay -- COM/NO --> Fan(Ventilador DC);
        Fan --> MOSFET{MOSFET};
        MOSFET -- Source --> GND;
        Power12V -- GND --> GND;
    end

    ESP32[ESP32 DevKit]

    subgraph "Bus I2C"
        ESP32 -- GPIO 21 / SDA --> BME280;
        ESP32 -- GPIO 22 / SCL --> BME280;
        ESP32 -- GPIO 21 / SDA --> OLED((OLED Display));
        ESP32 -- GPIO 22 / SCL --> OLED;
    end

    ESP32 -- GPIO 34 / Analógico --> MQ135;
    
    ESP32 -- GPIO 32 / CLK --> Encoder;
    ESP32 -- GPIO 33 / DT --> Encoder;
    
    ESP32 -- GPIO 25 / PULLUP --> BtnEnter;
    ESP32 -- GPIO 26 / PULLUP --> BtnBack;

    ESP32 -- GPIO 27 / Control --> Relay;
    ESP32 -- GPIO 14 / PWM --> MOSFET;

    style ESP32 fill:#4CAF50,stroke:#333,stroke-width:2px,color:#fff
    style Sensores fill:#2196F3,color:#fff
    style Controles fill:#FF9800,color:#fff
    style "Circuito de Potencia 12V" fill:#F44336,color:#fff
```
