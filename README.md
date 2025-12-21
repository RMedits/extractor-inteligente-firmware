# 🌬️ Extractor de Aire Inteligente v6.3C FINAL

Sistema de extracción automatizado basado en ESP32 para baño o galería, optimizado para ventiladores industriales de alta potencia (Delta 12V 2.70A - PWM 4 pines).

## 🚀 Características Finales
- **Modo Automático:** Control inteligente de velocidad basado en Humedad (AHT20/BMP280), Temperatura (AHT20/BMP280) y Calidad de Aire (MQ135).
- **Control PWM Preciso:** Uso de la entrada PWM del ventilador (4 pines) para un control de velocidad suave y eficiente (25kHz).
- **Corte de Energía:** Relé de seguridad para apagado total (0W consumo en reposo).
- **Modo Manual:** Temporizador programable (30/60/90 min) con selección de potencia (25/50/75/100%).
- **Interfaz OLED:** Pantalla de 1.3" (Driver SH1106) con navegación mediante Encoder rotativo y botones físicos.
- **Robustez:** Watchdog Timer (WDT) activo para reinicio automático en caso de fallo y redundancia de sensores.

## 🛠️ Hardware Confirmado
- **Microcontrolador:** ESP32-WROOM-32.
- **Sensores:**
  - **AHT20:** Humedad y Temperatura (Principal).
  - **BMP280:** Presión y Temperatura (Auxiliar/Respaldo).
  - **MQ135:** Calidad de Aire (Analógico).
- **Control:** Módulo OLED 1.3" (SH1106) + Encoder EC11 + 2 Botones.
- **Actuadores:**
  - **Relé KY-019:** Corte de alimentación 12V.
  - **PWM Directo:** Control de velocidad en ventilador de 4 pines.
- **Ventilador:** Delta QFR1212GHE (12V, 2.70A, 4 Pines).

## 📌 Pinout Resumido
| Componente | Pin ESP32 |
| :--- | :--- |
| **I2C (OLED/Sensors)** | SDA: 21 / SCL: 22 |
| **Encoder** | TRA: 32 / TRB: 33 / PUSH: 27 |
| **Botón BACK** | 25 |
| **Botón PAUSA** | 26 |
| **MQ135** | 34 |
| **Relé (Power)** | 23 |
| **PWM (Speed)** | 14 |

## 💻 Instalación
1. Clonar este repositorio.
2. Abrir con **PlatformIO**.
3. Compilar y subir al ESP32.
4. Las librerías necesarias (Adafruit SH110X, AHTX0, BMP280, etc.) se descargan automáticamente.

## 📜 Licencia
Este proyecto es de código abierto. Siéntete libre de mejorarlo.
