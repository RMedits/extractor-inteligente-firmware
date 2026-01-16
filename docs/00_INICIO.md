# 🌬️ Extractor de Aire Inteligente v7.2C - Guía de Inicio

## 📋 Índice Rápido

1. **[INICIO (este archivo)](#-extractor-de-aire-inteligente-v72c---guía-de-inicio)** - Bienvenida y orientación
2. **[INSTALACION_FIRMWARE.md](INSTALACION_FIRMWARE.md)** - Compilar y cargar el código en ESP32
3. **[HARDWARE.md](HARDWARE.md)** - Pinout, componentes y esquemáticos
4. **[MONTAJE_FISICO.md](MONTAJE_FISICO.md)** - Cómo armar el dispositivo
5. **[ALIMENTACION.md](ALIMENTACION.md)** - Esquemas de potencia y seguridad
6. **[CALIBRACION.md](CALIBRACION.md)** - Ajustar sensores
7. **[ARQUITECTURA.md](ARQUITECTURA.md)** - Lógica del firmware y modos

---

## 🎯 ¿Por Dónde Empezar?

### 👤 Soy Usuario
1. Ir a [MONTAJE_FISICO.md](MONTAJE_FISICO.md)
2. Conectar componentes según [HARDWARE.md](HARDWARE.md)
3. Cargar firmware: [INSTALACION_FIRMWARE.md](INSTALACION_FIRMWARE.md)

### 👨‍💻 Soy Desarrollador
1. Revisar [ARQUITECTURA.md](ARQUITECTURA.md) para entender la lógica
2. Ver [HARDWARE.md](HARDWARE.md) para pinout exacto
3. Modificar `/src/main.cpp`
4. Compilar y probar

### 🔧 Tengo Problemas
- Revisar [HARDWARE.md](HARDWARE.md) → Verificación de I2C
- Revisar [MONTAJE_FISICO.md](MONTAJE_FISICO.md) → Conexiones físicas
- Revisar [ALIMENTACION.md](ALIMENTACION.md) → Potencia y voltajes

---

## ✨ Características Principales

- ✅ **Modo Automático**: Control inteligente de velocidad basado en sensores
- ✅ **Modo Manual**: Temporizador + Modo Infinito con control de potencia
- ✅ **Diagnóstico Visual**: Pantalla OLED de 1.3" con navegación por encoder
- ✅ **Modo Noche**: Pantalla con sleep/wake, LEDs deshabilitados
- ✅ **Seguridad**: Watchdog, failover de sensores, modo ciego

---

## 🛠️ Hardware Confirmado

| Componente | Especificación |
|---|---|
| **Microcontrolador** | ESP32-WROOM-32 (30 pins, sin shield) |
| **Pantalla** | OLED SH110X 1.3" (I2C 0x3C) |
| **Sensores** | AHT20 (temp/humedad), BMP280 (presión), MQ135 (aire) |
| **Controles** | Encoder rotativo + 2 botones físicos |
| **Actuador** | MOSFET + Ventilador Delta 12V 2.7A |
| **Protección** | Diodo 1N5408, resistencias pulldown |

---

## 📝 Notas Importantes

⚠️ **El ESP32 es de 30 pines sin shield** - No es compatible con placas de 38 pines

⚠️ **GPIO exactos en [HARDWARE.md](HARDWARE.md)** - Não cambies sin documentar

⚠️ **I2C bus en GPIO 21/22** - Escáner disponible en `src/main_i2c_scanner.cpp`

---

## 🚀 Próximos Pasos

- [ ] Instalar firmware base
- [ ] Verificar sensores con escáner I2C
- [ ] Calibrar AHT20/MQ135 si es necesario
- [ ] Montar ventilador y MOSFET
- [ ] Implementar lógica AUTO MODE
- [ ] Implementar PAUSE MODE

---

**Última actualización**: 16 de enero de 2026
**Versión**: 7.2C (Sistema de modos completo + pantalla noche)
