# Changelog

## [7.1] - 2024-12-20 🚀 GRAN REFACTOR CON ARQUITECTURA NO-BLOQUEANTE

### ✨ Características Nuevas

#### 1. **Modo Manual Infinito (SIN LÍMITE DE TIEMPO)**
   - Nuevo `MODE_MANUAL_INFINITE` para modo manual sin restricción temporal
   - Selector en pantalla de configuración: "Limitado" vs "Infinito"
   - Ideal para fallos o control manual a baja velocidad prolongado
   - Usuario presiona BACK para salir

#### 2. **Arquitectura No-Bloqueante Completa**
   - ❌ Eliminados todos los `delay()` bloqueantes
   - ✅ Implementados timers no-bloqueantes con `millis()` para:
     - Rampa PWM suave (5ms steps, ±10 delta)
     - Lectura de sensores (cada 2 seg)
     - Timeout OLED (300 seg inactividad → apaga pantalla)
     - Debounce de botones (máquina de estados con 3 muestras)

#### 3. **Rampa PWM Suave**
   - Función `updateFanSpeedRamp()` ejecutada cada loop
   - Cambios graduales de velocidad: ±10 PWM cada 5ms
   - Evita saltos bruscos que dañan el ventilador/MOSFET
   - Mejora eficiencia energética y vida útil del hardware

#### 4. **Sensor Redundancia + I2C Reintentos**
   - Reintentos I2C: 3 intentos antes de fallar (configurable)
   - Fallback BMP280 → AHT20 (si AHT20 falla, usa BMP280 para temperatura)
   - Enum `SensorState`: OK, DEGRADED, FAILED
   - Calibración MQ135: baseline capture + 30s warmup check

#### 5. **Debounce Mejorado (Máquina de Estados)**
   - Reemplazó sistema de timestamps
   - Requiere N muestras consecutivas (DEBOUNCE_SAMPLES=3)
   - Elimina falsos positivos por ruido rápido
   - Variables: `encoderSwitchSamples`, `confirmButtonSamples`, `bakButtonSamples`

#### 6. **fatalError() Seguro (Sin Bucle Infinito)**
   - ❌ Elimina `while(1)` que triggers watchdog reboot infinito
   - ✅ Establece `MODE_ERROR` y retorna al loop
   - LED rojo parpadea cada 500ms (no bloqueante)
   - Pantalla OLED muestra error, sistema responsivo

#### 7. **Timeout OLED (Ahorro de Potencia)**
   - Apaga pantalla después de 5 minutos (300s) sin actividad
   - Resetea al presionar botones o leer sensores
   - `ssd1306_command(0xAE)` → off, `0xAF` → on
   - Reduce consumo de 30mA aprox

#### 8. **Setup Mejorado**
   - Reintentos I2C para sensor init
   - Captura baseline MQ135 después de 30s warmup
   - Manejo graceful de fallos (DEGRADED vs FAILED)

### 🔧 Cambios Técnicos

#### Constantes Nuevas/Actualizadas
```cpp
#define PWM_RAMP_STEP 5        // ms entre pasos de rampa
#define PWM_RAMP_DELTA 10      // ± PWM cambio por paso
#define OLED_TIMEOUT 300       // seg antes apagar pantalla
#define MQ135_WARMUP_TIME 30   // seg precalentamiento
#define I2C_RETRY_TIMES 3      // reintentos I2C
#define DEBOUNCE_SAMPLES 3     // muestras para confirmar presión
```

#### Enumeraciones Nuevas
```cpp
enum SensorState {
  SENSOR_OK,      // Todos sensores OK
  SENSOR_DEGRADED,// Al menos uno fallando
  SENSOR_FAILED   // Sistema inoperable
};

enum SystemMode {
  MODE_AUTO,
  MODE_MANUAL_SETUP,
  MODE_MANUAL_RUN,
  MODE_MANUAL_INFINITE,  // ← NUEVO
  MODE_PAUSE,
  MODE_ERROR
};
```

#### Variables Nuevas
```cpp
unsigned long lastPwmRampTime;     // Timer rampa PWM
unsigned long oledLastActivity;    // Actividad OLED
unsigned long setupTime;           // Para MQ135 warmup
int targetSpeed, currentSpeed;     // Para rampa suave
int temp_bmp;                      // BMP280 fallback temp
float mq135_baseline;              // Calibración MQ135
bool mq135_warmed;                 // Flag warmup completado
bool oledOn;                       // Estado OLED (on/off)
bool infiniteManualMode;           // Flag modo infinito
bool manualInfiniteSelected;       // Selector UI

// Debounce estado
volatile int encoderSwitchSamples;
volatile int confirmButtonSamples;
volatile int bakButtonSamples;
```

#### Funciones Nuevas/Refactorizadas

| Función | Cambio |
|---------|--------|
| `setup()` | Reintentos I2C, init MQ135 baseline, manejo DEGRADED |
| `loop()` | Llamadas no-bloqueantes: `updateFanSpeedRamp()`, OLED timeout, ERROR LED blink |
| `setFanSpeed()` | Solo asigna target, rampa en `updateFanSpeedRamp()` |
| `updateFanSpeedRamp()` | ✨ NUEVA: rampa PWM suave no-bloqueante |
| `readSensors()` | Reintentos I2C×3, BMP280 fallback, MQ135 warmup check, OLED activity reset |
| `checkButtons()` | Debounce máquina de estados, selector modo infinito en step 2 |
| `fatalError()` | Modo seguro sin bucle infinito, LED rojo blink no-bloqueante |
| `drawManualSetupScreen()` | Añade step 2: selector "Limitado" vs "Infinito" |
| `drawManualInfiniteScreen()` | ✨ NUEVA: pantalla modo infinito |

### 🔨 Hardware

#### GPIO Fix
- **LED_YELLOW_PIN**: GPIO 4 → **GPIO 12** (evita FLASH conflict)

#### Diagrama Actualizado
- MOSFET gate: 100Ω resistor documented
- Pull-down: 100k noted in Mermaid diagrams
- SVG/PNG rendered con mermaid-cli

### 📊 Resultados de Build

```
✅ Compilación exitosa (0 warnings)
✅ Todas las funciones non-blocking
✅ I2C con reintentos (robusto)
✅ PWM ramping suave
✅ Debounce state-machine
✅ Redundancia sensores
```

### 📝 Notas de Compatibilidad

- ⚠️ LED_YELLOW cambió GPIO → actualizar hardware
- ✅ MOSFET sigue GPIO 19 PWM (sin cambios)
- ✅ Backward compatible con existing hardware (solo 1 pin cambió)
- 🔮 Logging preparado para futuro servidor Debian (comments/structure)

### 🐛 Bugs Arreglados

1. ❌ Blocking `delay()` everywhere → ✅ Non-blocking timers
2. ❌ fatalError() infinite loop triggers watchdog → ✅ Safe error mode
3. ❌ Instant PWM jumps damage hardware → ✅ Soft ramp (5ms, ±10)
4. ❌ Weak debounce misses fast pulses → ✅ State-machine (3 samples)
5. ❌ MQ135 no calibration → ✅ Baseline + 30s warmup
6. ❌ No I2C retries → ✅ Loop 3× before fail
7. ❌ BMP280 unused → ✅ Fallback si AHT20 falla
8. ❌ LED GPIO 4 FLASH risk → ✅ GPIO 12 safe
9. ❌ OLED siempre on (potencia) → ✅ Timeout 300s
10. ❌ No manual mode sin límite → ✅ Infinite mode added

---

## [6.1.0] - 2025-11-06

### ✨ Añadido
- **Integración del sensor AHT20**: El firmware ahora es compatible con el sensor AHT20 para temperatura y humedad.
- **Lecturas Redundantes y Promediadas**: El sistema lee datos tanto del BME280 como del AHT20 y utiliza el promedio de ambos para obtener mediciones de temperatura y humedad más precisas y fiables.
- **Manejo de Fallos de Sensor**: El código puede operar con solo uno de los dos sensores (BME280 o AHT20) si el otro no es detectado.

### 🔧 Mejorado
- **Precisión de Sensores**: La lógica de promediado mejora la robustez de las lecturas ambientales.
- **Logs de Debug**: Los mensajes del puerto serie ahora muestran las lecturas de ambos sensores y el valor promediado resultante.

## [6.0.0] - 2025-11-06

### ✨ Añadido
- **Nueva Lógica de Botones Ergonómica**:
  - **Encoder (Pulsar)** ahora es **OK/Confirmar**.
  - **Botón `CONFIRM`** ahora es **Back/Cancelar**.
  - **Botón `BAK`** ahora activa la **Pausa de Emergencia**.
- **Modo Pausa**:
  - Permite detener el ventilador y congelar el temporizador en cualquier momento.
  - Al reanudar, el sistema vuelve al estado exacto en el que estaba.
  - Ideal para interrupciones temporales sin cancelar la configuración manual.
- Nuevo estado `PAUSA` en la máquina de estados.

### 🔧 Cambiado
- **¡CAMBIO DE PINOUT CRÍTICO!**
  - El control del **Relé** se ha movido del `GPIO 27` al **`GPIO 23`**.
  - El **botón del encoder (OK)** ahora se conecta al `GPIO 27`.
- Toda la documentación (README, diagramas, guías) ha sido actualizada para reflejar la v6.0.

## [4.0.0] - 2024-11-04

### ✨ Añadido
- Nuevo estado MANUAL_SELECCION_VELOCIDAD
- 4 opciones de velocidad en modo manual (25%, 50%, 75%, 100%)
- Navegación con botón BACK entre menús
- Display actualizado con menú de velocidad
- Logs estructurados con emojis para mejor debugging
- Validación robusta de lecturas de sensores
- Documentación completa de instalación y calibración

### 🔧 Mejorado
- Máquina de estados expandida de 3 a 4 modos
- Interfaz OLED más informativa
- Anti-rebote mejorado (250ms)
- Rango PWM ajustado (80-255) para mejor control
- Comentarios detallados en el código

### 🐛 Corregido
- Prevención de parada del motor en velocidades bajas
- Manejo de valores NaN del BME280
- Validación de rango ADC del MQ135

## [3.0.0] - 2024-10-XX
- Versión inicial con 3 modos
