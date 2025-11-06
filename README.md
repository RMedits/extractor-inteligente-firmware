# 🌪️ Extractor Inteligente - Firmware v6.0

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Ready-orange.svg)](https://platformio.org/)
[![ESP32](https://img.shields.io/badge/ESP32-Compatible-blue.svg)](https://www.espressif.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

Sistema inteligente de extracción de aire para baño/galería con control automático basado en sensores y modo manual con selección de tiempo, velocidad y función de pausa.

## ✨ Características v6.0

- ✅ **Control Automático**: Activación basada en humedad, temperatura y calidad del aire.
- ✅ **4 Velocidades Manuales**: 25%, 50%, 75%, 100%.
- ✅ **Temporizador**: 30, 60 o 90 minutos.
- ✅ **Función de Pausa**: Detiene el ventilador y congela el temporizador sin cancelar el programa.
- ✅ **Controles Ergonómicos**: Lógica de botones optimizada para un uso más intuitivo.
- ✅ **Interfaz Clara**: OLED 128x64 + Encoder rotativo + botones.
- ✅ **Monitoreo en Tiempo Real**: Temperatura, humedad y calidad del aire.

## 🚀 Inicio Rápido

```bash
git clone https://github.com/RMedits/extractor-inteligente-firmware.git
cd extractor-inteligente-firmware
pio run --target upload
pio device monitor
```

📖 Ver [Guía de Instalación Completa](docs/INSTALACION.md)

## 🎮 Controles v6.0 (Lógica Optimizada)

| Botón Físico | Acción | Función Principal | Icono |
| :--- | :--- | :--- | :---: |
| **Encoder** | Girar | Navegar Opciones | 🔄 |
| **Encoder** | Pulsar | OK / Confirmar | ✓ |
| **Botón `CONFIRM`** | Pulsar | Back / Cancelar | ⬅️ |
| **Botón `BAK`** | Pulsar | Pausa / Reanudar | ⏸️ |

---
crear un sistema de extracción de aire inteligente para baño/galería con control automático y manual. El sistema debe monitorear las condiciones ambientales y activar un ventilador de forma automática, además de permitir control manual con temporizador.

🔧 COMPONENTES DISPONIBLES

Hardware Principal

Microcontrolador: ESP32 DevKit HW-394 (USB-C, ESP32-WROOM-32 con WiFi/Bluetooth integrado)

Pantalla: OLED 128x64 píxeles con interfaz I2C

Sensores:

BME280: Sensor de temperatura y humedad (I2C)

MQ135: Sensor de calidad de aire (salida analógica)

Controles:

Encoder rotativo (rueda giratoria con 2 pines: CLK y DT)

2 botones físicos independientes (tipo pulsador)

Actuadores:

Módulo relé de 1 canal (5V)

Ventilador DC 12V

Alimentación:

Fuente de alimentación 12V para el ventilador

Puerto USB para el ESP32 (o alimentación desde 5V externo)

### 📟 Placa ESP32 utilizada

La placa base es el **ESP32 DevKit HW-394** (también comercializado como *ESP32 Development Board Type-C 5V*):

- USB-C nativo con conversor CH340C/CP2102 según la serigrafía recibida.
- Regulador integrado AMS1117 que entrega **3.3V** a los periféricos desde la entrada USB/VIN de **5V**.
- Dos hileras de 15 pines (30 en total) con serigrafía tipo `DXX` que corresponde al número de GPIO real.
- Pines VIN (5V) y GND ubicados junto al conector USB para alimentar relé y sensores que requieren 5V.
- Compatible con WiFi 2.4 GHz + Bluetooth/BLE del módulo ESP32-WROOM-32.

> 📘 En `docs/ESP32_DEVKIT_HW-394.md` se documentan las especificaciones completas, modos de alimentación y recomendaciones de conexión específicas para esta placa.

🎯 FUNCIONALIDAD REQUERIDA

MODO AUTOMÁTICO (Modo por defecto)

El sistema debe:

Monitorear continuamente los sensores de temperatura, humedad y calidad del aire.

Activar automáticamente el ventilador cuando se detecten condiciones que requieran ventilación:

Humedad alta (típica después de una ducha)

Temperatura elevada

Mala calidad del aire (gases, humo, vapor)

Ajustar la velocidad del ventilador proporcionalmente a las condiciones detectadas (requiere control PWM).

Mostrar en pantalla: valores de sensores y estado "MODO AUTOMÁTICO".

MODO MANUAL

El sistema debe permitir:

Entrar al modo manual presionando el botón ENTER desde el modo automático.

Al entrar al modo manual, mostrar un menú de selección de TIEMPO con 3 opciones:

30 minutos

60 minutos

90 minutos

Navegar entre las opciones de tiempo usando el encoder rotativo.

Confirmar la selección de tiempo con el botón ENTER.

NUEVO PASO: Tras confirmar el tiempo, mostrar un menú de selección de VELOCIDAD con 4 opciones (ej: 25%, 50%, 75%, 100%).

Navegar entre las opciones de velocidad usando el encoder rotativo.

Confirmar la selección de velocidad con el botón ENTER → activar ventilador a la velocidad seleccionada durante el tiempo seleccionado.

Cancelar la selección (ver Lógica de Botones).

Una vez activo el temporizador:

Mostrar cuenta regresiva en pantalla (formato MM:SS) y la velocidad seleccionada.

Mantener ventilador encendido a la velocidad fijada.

Permitir cancelación anticipada con ENTER o BACK.

Al terminar el tiempo, volver automáticamente al modo automático.

INTERFAZ DE PANTALLA

La pantalla OLED debe mostrar siempre:

Línea superior: Temperatura, humedad y calidad del aire (valores numéricos).

Área principal: Contenido según el modo actual:

En AUTO: "MODO AUTOMÁTICO" + velocidad del ventilador si está activo (ej. "VENTILADOR: 70%").

En SELECCIÓN (Tiempo): "SELECCIONA TIEMPO" + lista de opciones de tiempo (ej. "> 30 MIN").

En SELECCIÓN (Velocidad): "SELECCIONA VELOCIDAD" + tiempo seleccionado + lista de opciones de velocidad (ej. "> 50%").

En MANUAL ACTIVO: "MANUAL ACTIVO" + tiempo restante + velocidad (ej. "Tiempo: 28:47 | 50%").

LÓGICA DE BOTONES

Botón ENTER:

En AUTO → Ir a SELECCIÓN (Tiempo).

En SELECCIÓN (Tiempo) → Confirmar tiempo, ir a SELECCIÓN (Velocidad).

En SELECCIÓN (Velocidad) → Confirmar velocidad, iniciar MANUAL ACTIVO.

En MANUAL ACTIVO → Cancelar y volver a AUTO.

Botón BACK:

En AUTO → Sin función.

En SELECCIÓN (Tiempo) → Cancelar y volver a AUTO.

En SELECCIÓN (Velocidad) → Volver a SELECCIÓN (Tiempo) (permite corregir el tiempo).

En MANUAL ACTIVO → Cancelar y volver a AUTO.

⚙️ ESPECIFICACIONES TÉCNICAS

Control del Ventilador

Usar el relé para encendido/apagado general (como un interruptor de seguridad/principal).

Usar PWM para controlar la velocidad del ventilador (requiere circuito adicional con MOSFET).

Frecuencia PWM: 25kHz (ideal para motores DC).

Resolución: 8 bits (0-255).

Umbrales Sugeridos (ajustables)

Humedad alta: ≥70%

Humedad media: ≥65%

Temperatura alta: ≥30°C

Calidad de aire: valor analógico ≥600 (requiere calibración experimental).

Consideraciones del MQ135

Requiere alimentación de 5V.

Necesita tiempo de precalentamiento (mínimo 30 segundos en setup(), pero idealmente 24-48h para lecturas estables. Asumir que el sistema estará siempre encendido).

Los valores varían según ambiente, requiere calibración.

Usar salida analógica (AOUT), no digital (DOUT).

Comunicación I2C

Bus compartido entre OLED y BME280.

Direcciones típicas: OLED (0x3C o 0x3D), BME280 (0x76 o 0x77).

Pines estándar ESP32: SDA=GPIO21, SCL=GPIO22.

📊 REQUISITOS DE CÓDIGO

Librerías Necesarias

Wire.h - Comunicación I2C

Adafruit_GFX.h - Gráficos para OLED

Adafruit_SSD1306.h - Driver OLED

Adafruit_BME280.h - Driver sensor BME280

ESP32Encoder.h - Manejo del encoder rotativo (o una librería similar/implementación manual).

Funcionalidades del Código

Inicialización: Completa con mensajes de estado (Serial y OLED) si los sensores se detectan.

Controles: Manejo de anti-rebote (debounce) para botones.

Sensores: Lectura periódica (no usar delay() largos).

Lógica: Máquina de estados clara para los 4 modos (AUTO, SELECCION_TIEMPO, SELECCION_VELOCIDAD, MANUAL_ACTIVO).

Control Fan: Lógica inteligente de control de velocidad (PWM) basada en los umbrales de los sensores.

Control Fan: Lógica inteligente de control de velocidad (PWM) basada en los umbrales de los sensores.

Display: Actualización continua de la pantalla OLED sin parpadeos (evitar display.clear() excesivos).

Debug: Mensajes de debug por puerto serial para calibración y diagnóstico.

Errores: Manejo de sensores no detectados o lecturas inválidas.

Estructura Deseada

Variables globales claramente organizadas (pines, umbrales, objetos de librería).

Funciones separadas para cada tarea:

handleControls() - Lectura de encoder y botones.

readSensors() - Lectura de BME280 y MQ135.

runLogic() - Lógica de control y cambio de estados (máquina de estados).

controlFan() - Control del relé y PWM del ventilador.

updateDisplay() - Actualización de la pantalla OLED según el estado.

## 📍 Diagrama de Pines de la Placa

Para evitar confusiones con el cableado, puedes consultar un diagrama de pines detallado de la placa ESP32 DevKit, con los pines usados en este proyecto resaltados.

➡️ **[Ver Diagrama de Pines del ESP32](docs/DIAGRAMA_PINOUT_ESP32.md)**

## 🔌 Pinout Detallado (v6.0)

**Importante**: Esta versión requiere cambiar el cable del relé del GPIO 27 al **GPIO 23**.

| Pin (ESP32) | Pin físico / serigrafía | Componente Principal    | Conexión en Módulo      | Función Lógica v6.0 |
| :---------- | :---------------------- | :---------------------- | :---------------------- | :------------------ |
| **GPIO 21** | Pin 11 · D21            | Módulo Integrado / BME280 | `oled_sda`              | Datos I2C           |
| **GPIO 22** | Pin 14 · D22            | Módulo Integrado / BME280 | `oled_scl`              | Reloj I2C           |
| **GPIO 34** | Pin 27 · D34            | Sensor Calidad de Aire  | `AOUT`                  | Lectura Analógica   |
| **GPIO 32** | Pin 25 · D32            | Módulo Integrado        | `encoder_tra`           | Encoder A (Giro)    |
| **GPIO 33** | Pin 24 · D33            | Módulo Integrado        | `encoder_trb`           | Encoder B (Giro)    |
| **GPIO 27** | Pin 21 · D27            | Módulo Integrado        | `encoder_push`          | **OK / Confirmar**  |
| **GPIO 25** | Pin 23 · D25            | Módulo Integrado        | `confirm`               | **Back / Cancelar** |
| **GPIO 26** | Pin 22 · D26            | Módulo Integrado        | `bak`                   | **Pausa / Reanudar**|
| **GPIO 23** | Pin 15 · D23            | Módulo Relé             | `IN`                    | Control del Relé    |
| **GPIO 14** | Pin 20 · D14            | MOSFET                  | `Gate`                  | Control PWM         |
| **3.3V**    | Pin 1 · 3V3             | Alimentación            | `3v3-5v` (Módulo), `VCC` | Alimentación Lógica |
| **GND**     | Pin 2 / 17 · GND        | Tierra Común            | `gnd` (Módulo), `GND`   | Tierra Común        |
| **VIN (5V)**| Pin 16 · VIN            | Alimentación            | `VIN`                   | Alimentación Relé/MQ135 |



🎨 DISEÑO DE INTERFAZ (Mockups)

Pantalla en Modo Automático

T:24C H:55% A:387 ───────────────────── MODO AUTOMÁTICO

VENTILADOR: 70%

Pantalla en Selección Manual (Tiempo)

T:24C H:55% A:387 ───────────────────── SELECCIONA TIEMPO ─────────────────────

30 MIN 60 MIN 90 MIN ENTER=OK BACK=X

Pantalla en Selección Manual (Velocidad)

T:24C H:55% A:387 ───────────────────── SELEC. VELOCIDAD (Tiempo: 30 MIN) ───────────────────── 25% (Baja)

50% (Media) 75% (Alta) 100% (Máx) ENTER=OK BACK=Atrás

Pantalla en Manual Activo

T:24C H:55% A:387 ─────────────────────

 MANUAL ACTIVO
Vel: 50% | T: 28:47

⚠️ CONSIDERACIONES IMPORTANTES

Seguridad:

El ventilador (12V) y el ESP32 (3.3V/5V) deben tener sus alimentaciones separadas, pero GND común.

Usar un MOSFET de Nivel Lógico (ej. IRLZ44N) para el control PWM, que se active con 3.3V del ESP32.

El relé debe manejar la corriente del ventilador.

Indispensable: Incluir un diodo flyback (ej. 1N4007) en paralelo con el ventilador (cátodo al positivo de 12V) para proteger el MOSFET de picos de tensión inversa.

Rendimiento:

El loop() principal debe ser rápido. Evitar delay() a toda costa. Usar millis() para temporizadores y lecturas periódicas.

Anti-rebote efectivo en los botones (por software).

Experiencia de Usuario:

Respuesta inmediata a los controles.

Información clara en pantalla.

🎯 OBJETIVO FINAL

Un sistema completamente funcional y autónomo que:

Mejore automáticamente la calidad del aire.

Permita control manual cuando se necesite.

Sea intuitivo de usar.

Muestre información relevante en tiempo real.

Sea confiable y seguro en su operación.

💡 EXTRAS OPCIONALES (Para el futuro, no implementar ahora)

Almacenar umbrales en EEPROM/Preferences.

Conectividad WiFi para monitoreo remoto.

Hosting de Dashboard: Se dispone de un servidor doméstico (Home Server) que podría usarse para hostear un dashboard web o una API si se implementa la conectividad WiFi.