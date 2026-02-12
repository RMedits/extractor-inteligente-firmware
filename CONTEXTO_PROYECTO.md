# 🧠 CONTEXTO COMPLETO DEL PROYECTO: Extractor Inteligente
## Archivo de Referencia Permanente - NO REPETIR ESTAS PREGUNTAS

---

## ⚙️ DECISIONES TÉCNICAS CONFIRMADAS (NO CAMBIAR)

### 1. Control de Velocidad del Ventilador
- ✅ **DECISIÓN FINAL**: Usar **MOSFET EXTERNO IRLZ44N**
- ❌ **NO usar** el PWM interno del ventilador (cable amarillo)
- **Razón**: Investigación profunda en internet confirmó esta configuración
- **Cable amarillo del ventilador**: Se corta y se aísla con termorretráctil

### 2. Relé de Seguridad
- ✅ **Relé JQC-3FF** para corte completo de alimentación
- **Función**: Seguridad y ahorro energético
- **Control**: GPIO 23

### 3. Placa de Filtrado
- ✅ Condensador 100µF/25V
- **Ubicación**: Dentro de la caja de control (NO en la fuente)
- **Función**: Filtrar ruido eléctrico del motor

---

## 🔌 PINOUT COMPLETO Y CONFIRMADO

### ESP32 DevKit V1 (30 pines)

| GPIO | Componente | Cable/Función | Tipo | Notas |
|------|------------|---------------|------|-------|
| **GPIO 19** | MOSFET Gate | PWM Control Velocidad | Output | Señal PWM para controlar MOSFET |
| **GPIO 23** | Relé | Señal S (Control) | Output | Activa/desactiva relé |
| **GPIO 35** | Ventilador | Cable AZUL (TACH) | Input | **Lee feedback RPM del ventilador** |
| GPIO 21 | I2C | SDA (Sensores + OLED) | I2C | - |
| GPIO 22 | I2C | SCL (Sensores + OLED) | I2C | - |
| GPIO 32 | Encoder | CLK (A) | Input | - |
| GPIO 33 | Encoder | DT (B) | Input | - |
| GPIO 27 | Encoder | SW (OK) | Input | Botón pulsar encoder |
| GPIO 25 | Botón | CONFIRM (Back) | Input | - |

| GPIO 34 | MQ135 | Analog Input | Input | Sensor calidad aire |
| GPIO 18 | LED | Rojo | Output | Error/Standby |
| GPIO 5 | LED | Amarillo | Output | Manual/Config |
| GPIO 15 | LED | Verde | Output | OK (lógica invertida) |
| **GPIO 26** | **LED Puerta** | **Rojo** | Output | **Baño Ocupado** |
| **GPIO 14** | **LED Puerta** | **Verde** | Output | **Baño Libre** |
| GPIO 16 | Radar | TX (LD2410C) | UART | - |
| GPIO 17 | Radar | RX (LD2410C) | UART | - |
| 5V | Salida | Alimentación Relé VCC | Power | - |
| GND | Tierra | GND Común | Ground | Compartido con todo |

---

## 🎯 CABLES DEL VENTILADOR (4 CABLES)

| Color | Función Real | Conecta a | Estado |
|-------|--------------|-----------|---------|
| **ROJO** | +12V | Source MOSFET (modulado) | ✅ USADO |
| **NEGRO** | GND | GND Común | ✅ USADO |
| **AMARILLO** | PWM Input (interno) | **CORTADO Y AISLADO** | ❌ NO USADO |
| **AZUL** | TACH Output (RPM) | **GPIO 35 DIRECTO** | ✅ USADO |

**CRÍTICO**: 
- El cable **AZUL** es el TACH (tacómetro)
- Envía pulsos al ESP32 para calcular RPM reales
- Permite detectar si el ventilador está bloqueado (stall protection)
- **NO pasa por la caja**, va directo del ventilador a GPIO 35

---

## 📦 CONFIGURACIÓN DE LA CAJA DE CONTROL

### Componentes dentro:
1. MOSFET IRLZ44N
2. Relé JQC-3FF
3. Condensador 100µF (placa filtrado)

### Cables que ENTRAN (6):
1. 12V (+) desde fuente
2. GND (-) desde fuente  
3. GPIO 19 (señal PWM)
4. GPIO 23 (señal relé)
5. 5V desde ESP32
6. GND desde ESP32

### Cables que SALEN (2):
1. ROJO hacia ventilador
2. NEGRO hacia ventilador

### Cables que NO pasan por la caja:
- **AZUL** del ventilador → directo a GPIO 35 del ESP32

---

## 🔧 CONEXIONES INTERNAS DE LA CAJA

```
FLUJO DE CORRIENTE:

Fuente 12V (+) 
    → Relé COM
    → Relé NO (cuando GPIO 23 = HIGH)
    → Se divide en:
        ├─→ (+) Condensador filtrado
        └─→ Drain MOSFET
            → Gate MOSFET recibe PWM desde GPIO 19
            → Source MOSFET 
            → Cable ROJO ventilador
            → Ventilador
            → Cable NEGRO ventilador
            → GND Común
            → Fuente 12V (-)
```

**MOSFET actúa como interruptor variable:**
- GPIO 19 HIGH → MOSFET conduce → Pasa 12V al ventilador
- GPIO 19 LOW → MOSFET corta → No pasa corriente
- GPIO 19 PWM → Controla velocidad (duty cycle)

---

## 📡 COMUNICACIÓN Y SENSORES

### I2C Bus (GPIO 21 SDA, GPIO 22 SCL):
- **OLED SH1106G** (0x3C) - Pantalla 1.3"
- **AHT20** (0x38) - Temperatura + Humedad
- **BMP280** (0x77) - Presión atmosférica

### Analog:
- **MQ135** (GPIO 34) - Calidad de aire

### UART:
- **LD2410C** (GPIO 16 TX, GPIO 17 RX) - Radar detección presencia

### Digital Input:
- **Ventilador TACH** (GPIO 35) - **Cable AZUL - Feedback RPM**

---

## 🌐 CONFIGURACIÓN WIFI/MQTT

- **WiFi**: Credenciales en `src/Secrets.h`
- **WebSocket**: Puerto 80 (`/ws`)
- **NTP**: Sincronización hora para modo noche automático
- **Home Assistant**: Integración MQTT (futura)

---

## 🧮 LÓGICA DE CONTROL

### Modos de operación:
1. **AUTO (EST_IDLE)**: Ventilación continua ASHRAE (18%)
2. **SHOWER (EST_SHOWER)**: Detección ducha → 85% velocidad
3. **ODOR (EST_ODOR)**: Detección mal olor → 60% velocidad
4. **NIGHT (EST_NIGHT)**: Modo noche silencioso → máx 40%
5. **MANUAL_LIMITED**: Configurado por usuario con timer
6. **MANUAL_INFINITE**: Manual sin límite de tiempo
7. **MANUAL_CONFIG**: Asistente configuración (4 pasos)
8. **DIAGNOSTIC**: Modo debug
9. **ERROR**: Protección stall (ventilador bloqueado)

### Detección de ducha:
- **Rate-of-Rise**: >5%/min cambio humedad
- **Punto de rocío**: >20°C
- **Histéresis**: 1.5°C para salir

### Detección olor:
- **Sensibilidad**: AQI < 80% baseline
- **Recuperación**: AQI > 95% baseline + 5 min purga

### Modo noche:
- **Horario**: 23:00 - 07:00
- **Límite velocidad**: 40% máximo

### Protección:
- **Velocidad máxima global**: 90% (nunca superar)
- **Stall detection**: Si RPM=0 con velocidad >20% durante 5s → ERROR
- **GPIO 35** lee pulsos TACH cada 1 segundo para calcular RPM

---

## 📝 CÓDIGO CRÍTICO

### En `Config.h`:
```cpp
#define PIN_FAN_PWM 19      // MOSFET Gate
#define PIN_FAN_TACH 35     // Cable AZUL ventilador
#define PIN_RELAY 23        // Control relé
```

### En `main.cpp`:
```cpp
// ISR para contar pulsos TACH
void IRAM_ATTR tachISR() { tachPulses++; }

// Setup
pinMode(PIN_FAN_TACH, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(PIN_FAN_TACH), tachISR, FALLING);

// Cálculo RPM cada 1 segundo
sensors.fan_rpm = (tachPulses / 2) * 60;
tachPulses = 0;
```

**Fórmula RPM**:
- Ventilador envía 2 pulsos por revolución
- `RPM = (pulsos / 2) * 60`

---

## ⚡ ALIMENTACIÓN

### Fuente 12V:
- Alimenta: Ventilador (través relé y MOSFET)
- Positivo → Relé COM
- Negativo → GND común

### Fuente 5V (USB del ESP32):
- Alimenta: ESP32, Relé VCC, sensores 5V (MQ135)
- GND → GND común

### Salida 3.3V ESP32:
- Alimenta: OLED, AHT20, BMP280

**IMPORTANTE**: Todos los GND deben estar conectados (común)

---

## 🛡️ SEGURIDAD Y LÓGICA DEL RELÉ (CRÍTICO)

### Requisito de Seguridad FAIL-SAFE:

**OBJETIVO**: El ventilador debe estar **APAGADO** si:
- El ESP32 está apagado
- El ESP32 está arrancando
- Hay un fallo en el ESP32
- No se ha enviado señal de activación

### Configuración del Relé JQC-3FF:

**Terminal usado**: **NO (Normally Open)**

**Lógica**:
```
ESP32 Apagado / GPIO 23 = LOW:
    → Relé DESACTIVADO
    → Terminal NO está ABIERTO
    → NO pasa corriente
    → Ventilador APAGADO ✅ (FAIL-SAFE)

ESP32 Activo / GPIO 23 = HIGH:
    → Relé ACTIVADO  
    → Terminal NO está CERRADO
    → Pasa corriente (12V)
    → Ventilador puede funcionar ✅
```

### ⚠️ VERIFICACIÓN PENDIENTE:

**PROBAR**: Confirmar que la lógica del relé funciona como esperado:
1. Desconectar ESP32 → Ventilador debe estar parado
2. ESP32 arrancando → Ventilador parado hasta GPIO 23 = HIGH
3. GPIO 23 = LOW → Ventilador parado
4. GPIO 23 = HIGH → Ventilador puede funcionar

**Si el relé tiene lógica inversa** (sospecha del usuario):
- Considerar usar terminal **NC (Normally Closed)** en lugar de NO
- O invertir la señal en código (GPIO 23 = HIGH para apagar)





---

## 🚨 ERRORES COMUNES RESUELTOS

### ❌ NO confundir:
- Cable AZUL ≠ Cable AMARILLO
- AZUL = TACH (salida del ventilador, entrada al ESP32)
- AMARILLO = PWM Input interno (NO USADO, cortado)

### ❌ NO conectar:
- 12V directo al ESP32 (quemaría)
- Cable amarillo a nada (está aislado)
- MOSFET sin GND común (no funcionaría)

### ✅ VERIFICAR siempre:
- Todos los GND están unidos
- Cable AZUL va a GPIO 35 (no a GPIO 19)
- Relé recibe 5V (no 12V en VCC)
- MOSFET Gate recibe señal de GPIO 19

---

## 📂 ESTRUCTURA DEL PROYECTO

```
extractor-firmware/
├── platformio.ini          # Configuración PlatformIO
├── include/
│   └── Config.h           # Todos los #defines de pines
├── src/
│   ├── main.cpp           # Código principal
│   └── Secrets.h          # WiFi/MQTT credentials
└── CONTEXTO_PROYECTO.md   # ESTE ARCHIVO

extractor-app/
├── HARDWARE_SPECS.md      # Specs de hardware
├── src/
│   └── App.jsx           # WebUI React
└── package.json
```

---

## 🔄 HISTORIAL DE CAMBIOS EN DECISIONES

| Fecha | Cambio | Razón |
|-------|--------|-------|
| 2026-01-28 | Confirmado uso MOSFET externo | Investigación profunda, decisión final |
| 2026-01-28 | Cable amarillo cortado | No se usa PWM interno del ventilador |
| 2026-01-28 | Cable azul → GPIO 35 | TACH para feedback RPM (NO cambiar) |

---

## 💡 NOTAS IMPORTANTES PARA FUTURAS SESIONES

1. **NO sugerir** usar el PWM interno del ventilador (ya decidido usar MOSFET externo)
2. **Cable AZUL** siempre es TACH, va a GPIO 35
3. **Cable AMARILLO** está cortado y aislado, NO SE USA
4. La configuración actual es resultado de investigación profunda, NO improvisar
5. Este archivo es la **única fuente de verdad** del proyecto

---

## 📞 PREGUNTAS FRECUENTES RESPONDIDAS

**P: ¿Por qué no usar el PWM interno del ventilador?**  
R: Decisión técnica confirmada tras investigación. Usar MOSFET externo IRLZ44N.

**P: ¿Qué hago con el cable amarillo?**  
R: Cortarlo cerca del ventilador y aislarlo con termorretráctil.

**P: ¿El cable azul sirve para controlar velocidad?**  
R: NO. El azul es TACH (salida), lee RPM. No confundir con amarillo.

**P: ¿Necesito el MOSFET si el ventilador tiene 4 cables?**  
R: SÍ. Esa es la configuración decidida para este proyecto.

**P: ¿Puedo conectar el cable azul a GPIO 19?**  
R: NO. Cable azul va a GPIO 35 (TACH input). GPIO 19 controla el MOSFET.

---

**Última actualización**: 2026-01-28  
**Versión del documento**: 1.0  
**Estado**: Configuración CONFIRMADA Y FINAL
