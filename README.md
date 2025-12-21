# 🌬️ Extractor de Aire Inteligente v6.0C FINAL

Sistema de extracción automatizado basado en ESP32 para baño o galería, optimizado para ventiladores industriales de alta potencia (Delta 12V 2.70A).

## 🚀 Características Finales
- **Modo Automático:** Control inteligente de velocidad basado en Humedad (BME280), Temperatura (BME280) y Calidad de Aire (MQ135).
- **Modo Manual:** Temporizador programable (30/60/90 min) con selección de potencia (25/50/75/100%).
- **Función Pausa:** Botón de emergencia para detener el sistema instantáneamente.
- **Interfaz OLED:** Pantalla de 1.3" con navegación mediante Encoder rotativo y 3 botones físicos.
- **Control PWM de Alta Frecuencia (25kHz):** Evita ruidos eléctricos en el motor del ventilador.

## 🛠️ Hardware Confirmado
- **Microcontrolador:** ESP32-WROOM-32.
- **Sensores:** BME280 (I2C) + MQ135 (Analógico).
- **Control:** Módulo OLED Estardyn con Encoder y 2 botones extra.
- **Actuadores:** Relé KY-019 + MOSFET FQP30N06L.
- **Ventilador:** Delta QFR1212GHE (12V, 2.70A).
- **Protección:** Diodo 1N5408 + Resistencias Pulldown.

## 📌 Pinout Resumido
| Componente | Pin ESP32 |
| :--- | :--- |
| **I2C (OLED/BME280)** | SDA: 21 / SCL: 22 |
| **Encoder** | TRA: 32 / TRB: 33 / PUSH: 27 |
| **Botón BACK (Confirm)** | 25 |
| **Botón PAUSA (Bak)** | 26 |
| **MQ135** | 34 |
| **Relé KY-019** | 23 |
| **MOSFET PWM** | 14 |

## 💻 Instalación
1. Clonar este repositorio.
2. Abrir con **PlatformIO**.
3. Compilar y subir al ESP32.
4. Las librerías necesarias se gestionan automáticamente en `platformio.ini`.

## 📜 Licencia
Este proyecto es de código abierto. Siéntete libre de mejorarlo.