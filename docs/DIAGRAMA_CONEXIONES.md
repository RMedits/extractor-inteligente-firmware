# Diagrama de Conexiones (Mermaid)

Este diagrama muestra las conexiones lógicas entre el ESP32 y todos los componentes periféricos, reflejando el uso de un módulo que integra la pantalla y los controles.

```mermaid
graph LR
    subgraph "Periféricos Externos"
        BME280((Sensor BME280))
        MQ135((Sensor MQ135))
    end

    subgraph "Módulo Integrado (OLED + Controles)"
        direction LR
        Pin1(oled_sda)
        Pin2(oled_scl)
        Pin3(encoder_tra)
        Pin4(encoder_trb)
        Pin5(confirm)
        Pin6(bak)
        Pin7(3v3-5v)
        Pin8(gnd)
    end

    subgraph potencia["Circuito de Potencia 12V"]
        Power12V(Fuente 12V) --> Relay{Relé};
        Relay -- COM/NO --> Fan(Ventilador DC);
        Fan --> MOSFET{MOSFET};
        MOSFET -- Source --> GND;
        Power12V -- "GND Común" --> GND;
    end

    ESP32[ESP32 DevKit]

    %% Conexiones al Módulo Integrado
    ESP32 -- "GPIO 21 (SDA)" --> Pin1;
    ESP32 -- "GPIO 22 (SCL)" --> Pin2;
    ESP32 -- "GPIO 32" --> Pin3;
    ESP32 -- "GPIO 33" --> Pin4;
    ESP32 -- "GPIO 25" --> Pin5;
    ESP32 -- "GPIO 26" --> Pin6;
    ESP32 -- "3.3V" --> Pin7;
    ESP32 -- "GND" --> Pin8;

    %% Conexiones a otros periféricos
    ESP32 -- "GPIO 21 (SDA)" --> BME280;
    ESP32 -- "GPIO 22 (SCL)" --> BME280;
    ESP32 -- "GPIO 34 (Analógico)" --> MQ135;
    
    %% Conexiones al circuito de potencia
    ESP32 -- "GPIO 27 (Control)" --> Relay;
    ESP32 -- "GPIO 14 (PWM)" --> MOSFET;

    style ESP32 fill:#4CAF50,stroke:#333,stroke-width:2px,color:#fff
    style "Periféricos Externos" fill:#2196F3,color:#fff
    style "Módulo Integrado (OLED + Controles)" fill:#FF9800,color:#fff
    style potencia fill:#F44336,color:#fff
```
