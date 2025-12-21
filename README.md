# 🌬️ Extractor de Aire Inteligente v6.5C FINAL (Dual 12V/24V)

Sistema de extracción automatizado basado en ESP32, diseñado para operar con ventiladores industriales Delta de alta potencia en dos configuraciones: 12V (Galería) y 24V (Baño).

## 🚀 Características Finales
- **Soporte Dual Hardware:**
  - **Variante A (12V):** Control vía MOSFET Buffer.
  - **Variante B (24V):** Control vía Optoacoplador (Aislamiento Total).
- **Lógica Unificada:** Firmware único con lógica PWM invertida compatible con ambos circuitos.
- **Modo Automático:** Control inteligente (Humedad/Temp/Aire).
- **Seguridad:**
  - **Relé:** Corte físico de energía.
  - **Watchdog:** Reinicio automático ante fallos.
  - **Protección GPIO:** Buffer hardware obligatorio para señal PWM.
- **Interfaz:** OLED 1.3" (SH1106) + Encoder EC11.

## ⚠️ Advertencia Importante: Colores Delta
Los ventiladores Delta tienen un código de colores NO ESTÁNDAR:
- **ROJO:** +12V/24V
- **NEGRO:** GND
- **AMARILLO:** PWM (Señal de control)
- **AZUL:** TACH (No usar para PWM)

**¡Conectar 24V al ESP32 destruirá el chip!** Siga estrictamente el `esquema_ conexiones.md`.

## 🛠️ Hardware Soportado
- **Microcontrolador:** ESP32-WROOM-32.
- **Sensores:** AHT20 (Humedad) + BMP280 (Temp/Presión) + MQ135 (Aire).
- **Control:** OLED 1.3" (SH1106).
- **Actuadores:**
  - Ventilador 12V: Delta QFR1212GHE (con MOSFET Buffer).
  - Ventilador 24V: Delta PFB1224UHE (con Optoacoplador).

## 💻 Instalación
1. Clonar este repositorio.
2. Abrir con **PlatformIO**.
3. Compilar y subir al ESP32.
4. Consultar `esquema_ conexiones.md` para elegir su variante de montaje.

## 📜 Licencia
Código abierto. Diseñado para alta fiabilidad y seguridad eléctrica.
