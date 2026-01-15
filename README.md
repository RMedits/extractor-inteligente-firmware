# 🌬️ Extractor de Aire Inteligente v7.2C

Sistema de extracción automatizado basado en **ESP32 de 30 pines (sin shield)** para baño o galería, optimizado para ventiladores industriales de alta potencia (Delta 12V 2.70A) con seguridad redundante y diagnóstico avanzado.

⚠️ **Configuración de pines definitiva en:** [PINES_DEFINITIVOS.md](PINES_DEFINITIVOS.md)

## 🚀 Características Finales
- **Modo Automático:** Control inteligente de velocidad basado en Humedad (AHT20/BMP280), Temperatura (AHT20/BMP280) y Calidad de Aire (MQ135).
- **Modo Manual:** Temporizador programable (30/60/90 min) + Modo Infinito con selección de potencia.
- **Diagnóstico Visual:** Pantalla de error detallada indicando qué sensor específico ha fallado.
- **Seguridad Mejorada:**
    - **Watchdog Timer:** Reinicio automático si el sistema se bloquea por 8 segundos.
    - **Sensor Failover:** Si un sensor I2C falla, el sistema intenta usar el otro. Si ambos fallan, bloqueo seguro.
    - **Modo Ciego:** Si la pantalla OLED falla, el sistema sigue funcionando indicando estado por LEDs.
- **Interfaz OLED:** Pantalla SSD1306 con navegación mediante Encoder rotativo y botones físicos.
- **LEDs de Estado:** Verde (Normal), Amarillo (Manual), Rojo (Error/Standby).
- **Animaciones:** Efectos visuales en modo infinito (scroll, parpadeo).

## 🛠️ Hardware Confirmado
- **Microcontrolador:** ESP32-WROOM-32 (30 pines, sin shield).
- **Sensores:** Módulo Dual AHT20+BMP280 (I2C) + MQ135 (Analógico).
- **Control:** Módulo OLED con Encoder y 2 botones extra.
- **Actuadores:** MOSFET N-channel + Control PWM directo del ventilador (4 hilos).
- **Ventilador:** Delta QFR1212GHE (12V, 2.70A).
- **Protección:** Diodo 1N5408 + Resistencias Pulldown.

## 📌 Pinout Resumido
| Componente | GPIO | Función |
| :--- | :--- | :--- |
| **I2C** | SDA: 21 / SCL: 22 | Sensores + OLED |
| **Encoder** | CLK: 32 / DT: 33 / SW: 27 | Control Usuario |
| **Botones** | BACK: 25 / PAUSA: 13 | Control Usuario |
| **MQ135** | 34 | Calidad Aire (Analógico) |
| **Ventilador** | PWM: 19 / TACH: 16 | Control + Lectura RPM |
| **LEDs** | Rojo: 18 / Amarillo: 5 / Verde: 17 | Estado Sistema |

## 💻 Instalación
1. Clonar este repositorio.
2. Abrir con **PlatformIO**.
3. Compilar y subir al ESP32.
4. Las librerías necesarias se gestionan automáticamente en `platformio.ini`.

## 📚 Documentación
- **[PINES_DEFINITIVOS.md](PINES_DEFINITIVOS.md)** - Configuración de hardware (LEER PRIMERO)
- **[EMPIEZA_AQUI.md](EMPIEZA_AQUI.md)** - Guía de inicio rápido
- **[docs/](docs/)** - Diagramas, calibración, montaje
- **[QUICK_START.md](QUICK_START.md)** - Nuevas características v7.2C

## 📜 Licencia
Este proyecto es de código abierto. Siéntete libre de mejorarlo.

---

**Versión:** v7.2C  
**Autor:** RAUL  
**Hardware:** ESP32 30 pines (sin shield)

```
