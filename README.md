# 🌬️ Extractor de Aire Inteligente v6.4C FINAL

Sistema de extracción automatizado basado en ESP32 para baño o galería, optimizado para ventiladores industriales de alta potencia (Delta 12V 2.70A).

## 🚀 Características Finales
- **Modo Automático:** Control inteligente de velocidad basado en Humedad (AHT20/BMP280), Temperatura (AHT20/BMP280) y Calidad de Aire (MQ135).
- **Protección Total:**
  - **Relé de Seguridad:** Corte físico de la alimentación de 12V (0W consumo standby).
  - **Buffer PWM:** Etapa MOSFET para proteger el GPIO del ESP32 de voltajes peligrosos del ventilador.
- **Control Preciso:** Señal PWM invertida de 25kHz para gestión suave de velocidad.
- **Modo Manual:** Temporizador programable (30/60/90 min) con selección de potencia.
- **Interfaz OLED:** Pantalla de 1.3" (Driver SH1106) con encoder rotativo.
- **Robustez:** Watchdog Timer (WDT) activo.

## 🛠️ Hardware Confirmado
- **Microcontrolador:** ESP32-WROOM-32.
- **Sensores:** AHT20 + BMP280 + MQ135.
- **Control:** Módulo OLED 1.3" (SH1106) + Encoder EC11.
- **Actuadores:**
  - Relé KY-019 (Corte 12V).
  - MOSFET FQP30N06L (Buffer de señal PWM).
- **Ventilador:** Delta QFR1212GHE (12V, 2.70A, 4 Pines).

## 📌 Pinout Resumido
| Componente | Pin ESP32 |
| :--- | :--- |
| **I2C** | 21 (SDA), 22 (SCL) |
| **Encoder** | 32, 33, 27 |
| **Botones** | 25 (Back), 26 (Pause) |
| **Relé** | 23 |
| **PWM Buffer**| 14 |
| **MQ135** | 34 |

## 💻 Instalación
1. Clonar este repositorio.
2. Abrir con **PlatformIO**.
3. Compilar y subir al ESP32.
4. Las librerías necesarias se descargan automáticamente.

## 📜 Licencia
Este proyecto es de código abierto. Siéntete libre de mejorarlo.
