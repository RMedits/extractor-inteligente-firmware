# 🌬️ Extractor de Aire Inteligente v7.2C

Sistema de extracción automatizado basado en **ESP32 de 30 pines (sin shield)** para baño o galería, optimizado para ventiladores industriales de alta potencia (Delta 12V 2.70A).

⚠️ **Ver documentación completa en carpeta [docs/](docs/)** - Empezar en [docs/00_INICIO.md](docs/00_INICIO.md)

## 🚀 Características

- **Modo Automático**: Control inteligente de velocidad basado en sensores
- **Modo Manual**: Temporizador configurable + Modo Infinito con control de potencia
- **Diagnóstico Visual**: Pantalla OLED 1.3" con navegación por encoder
- **Modo Noche**: Pantalla con sleep/wake inteligente
- **Seguridad**: Watchdog, failover de sensores, protección contra picos

## 🛠️ Hardware

| Componente | Especificación |
|:---|:---|
| **MCU** | ESP32-WROOM-32 (30 pines, sin shield) |
| **Pantalla** | OLED SH110X 1.3" (I2C 0x3C) |
| **Sensores** | AHT20, BMP280, MQ135 |
| **Interfaz** | Encoder + 2 botones |
| **Actuador** | MOSFET + Ventilador Delta 12V 2.7A |
| **Protección** | Diodo 1N5408 |
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
