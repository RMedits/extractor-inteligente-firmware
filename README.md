# 🌀 Extractor Inteligente

Sistema de ventilación inteligente para baño con ESP32, sensores ambientales, radar de presencia e integración con Home Assistant.

## 📂 Estructura del Proyecto

```
├── firmware/          ← Código ESP32 (PlatformIO)
│   ├── include/       ← Config.h (pinout) + Secrets.h (credenciales)
│   └── src/           ← main.cpp (firmware principal)
├── app/               ← App web (backend + frontend) — En desarrollo
├── docs/              ← Documentación técnica
└── CONTEXTO_PROYECTO.md  ← Referencia técnica completa
```

## ⚙️ Hardware

- **ESP32 DevKit V1** (30 pines)
- **Sensores**: AHT20 (temp/hum), BMP280 (presión), MQ135 (calidad aire), Radar LD2410C (presencia)
- **Pantalla**: OLED 1.3" SH1106G (I2C)
- **Actuadores**: Ventilador 12V + MOSFET IRLZ44N (PWM) + Relé JQC-3FF
- **Indicadores**: 5 LEDs (rojo, amarillo, verde estado + rojo/verde ocupación baño)
- **HMI**: Encoder rotativo + botón BACK

## 🔧 Setup Firmware

1. Instalar [PlatformIO](https://platformio.org/)
2. Copiar `firmware/include/Secrets.h.example` → `firmware/include/Secrets.h`
3. Editar `Secrets.h` con tus credenciales WiFi
4. Compilar y flashear desde `firmware/`

## 🌐 App Web

En desarrollo. Ver [app/README.md](app/README.md).

## 📡 Ramas

- `main` — Firmware estable + estructura del proyecto
- `antigravity/app-integration` — Desarrollo de la app web + integración HA
