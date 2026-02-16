# 🔧 Guía de Montaje: SSR y MOSFET

## 📦 Materiales Necesarios

| Componente | Cantidad | Especificación |
|------------|----------|----------------|
| SSR BSSR-1DD | 1 | 25A DC-DC |
| MOSFET IRLZ44N | 1 | Logic Level |
| Resistencia 220Ω | 1 | 1/4W (SSR) |
| Resistencia 10kΩ | 1 | 1/4W (Pull-down MOSFET) |
| Resistencia 220Ω | 3 | 1/4W (LEDs - si no las tienes ya) |
| Cable dupont M-M | 10 | Conexiones |
| Protoboard pequeña | 1 | Opcional (organización) |

---

## 📐 Paso 1: Identificar los Bornes del SSR

El SSR BSSR-1DD tiene **4 bornes** marcados:

```
┌──────────────┐
│  BSSR-1DD    │
│              │
│  -4      +3  │  ← INPUT (Control 3-32VDC)
│              │
│  -1      +2  │  ← OUTPUT (Carga 5-220VDC)
└──────────────┘
```

### Verificación visual:
- **INPUT** (-4, +3): lado con LED pequeño o marcas "INPUT"
- **OUTPUT** (-1, +2): lado con tornillos más grandes

---

## 🔌 Paso 2: Montar el SSR (Control desde ESP32)

### 2.1. Preparar la resistencia 220Ω

Conecta la resistencia entre GPIO 23 y el borne +3:

```
ESP32 GPIO 23 ──┬── Resistencia 220Ω ──► Tornillo +3 (SSR)
                │
                └─────────────────────► Tornillo -4 (SSR)
ESP32 GND ──────────────────────────────┘
```

### 2.2. Conexiones físicas:

1. **Cable desde GPIO 23:**
   - Inserta un extremo en el pin GPIO 23 del ESP32
   - Conecta el otro extremo a una pata de la resistencia 220Ω

2. **Resistencia 220Ω al borne +3:**
   - Conecta la otra pata de la resistencia al tornillo **+3** del SSR
   - Aprieta el tornillo con destornillador

3. **GND al borne -4:**
   - Cable desde GND del ESP32 al tornillo **-4** del SSR
   - Aprieta el tornillo

### ✅ Verificación:
- GPIO 23 → 220Ω → +3 ✓
- GND → -4 ✓

---

## ⚡ Paso 3: Conexión de Alimentación al SSR (Salida)

### 3.1. Conectar la fuente 12V

```
Fuente 12V (+) ──────► Tornillo +2 (SSR)
Tornillo -1 (SSR) ───► Cable ROJO del ventilador
Cable NEGRO ventilador ─► GND común (fuente 12V -)
```

### 3.2. Conexiones físicas:

1. **Cable 12V+ al borne +2:**
   - Desde el positivo de tu fuente 12V al tornillo **+2**
   - Aprieta bien

2. **Cable ROJO del ventilador al borne -1:**
   - Desde el tornillo **-1** al cable ROJO (+) del ventilador
   - Aprieta bien

3. **Cable NEGRO del ventilador a GND:**
   - Cable NEGRO (-) del ventilador a GND de la fuente 12V

### ⚠️ IMPORTANTE:
- **GND común**: El GND del ESP32 y el GND de la fuente 12V **DEBEN estar conectados**
- Sin GND común, el SSR no funcionará correctamente

### ✅ Verificación:
- 12V+ → +2 ✓
- -1 → Ventilador ROJO ✓
- Ventilador NEGRO → GND común ✓

---

## 🔧 Paso 4: Montar el MOSFET IRLZ44N

### 4.1. Identificar los pines del MOSFET

Mirando el MOSFET de frente (con la etiqueta visible y las patas hacia abajo):

```
    ┌─────────┐
    │ IRLZ44N │
    │         │
    └──┬─┬─┬──┘
       │ │ │
       G D S
       A R O
       T A U
       E I R
         N C
           E
```

- **GATE** (G) = izquierda
- **DRAIN** (D) = centro
- **SOURCE** (S) = derecha

### 4.2. Conexiones del MOSFET

```
GPIO 19 (PWM) ────┬──► GATE (izquierda)
                  │
               10kΩ (pull-down)
                  │
GND ──────────────┴──► SOURCE (derecha)

Cable AZUL ventilador ──► DRAIN (centro)
```

### 4.3. Montaje físico:

**Opción A: Con protoboard (recomendado)**

1. Inserta el MOSFET en la protoboard con las patas hacia abajo
2. **Pull-down 10kΩ:**
   - Una pata al GATE (fila del pin izquierdo)
   - Otra pata a GND (fila negativa)
3. **GPIO 19 al GATE:**
   - Cable desde GPIO 19 del ESP32 al GATE (pin izquierdo)
4. **SOURCE a GND:**
   - Cable desde SOURCE (pin derecho) a GND común
5. **DRAIN al cable AZUL:**
   - Cable desde DRAIN (pin central) al cable AZUL del ventilador

**Opción B: Soldadura directa (más compacto)**

1. Solda la resistencia 10kΩ entre GATE y SOURCE
2. Solda cable desde GPIO 19 al GATE
3. Solda cable desde SOURCE a GND
4. Solda cable desde DRAIN al cable AZUL del ventilador
5. Protege con termoretráctil

### ✅ Verificación:
- GPIO 19 → GATE ✓
- 10kΩ entre GATE-SOURCE ✓
- SOURCE → GND ✓
- DRAIN → Cable AZUL ventilador ✓

---

## 🔴 Paso 5: LEDs de Estado (Opcional)

Si aún no los has montado:

```
GPIO 18 ──── 220Ω ──── LED ROJO (+) ──── GND
GPIO 5  ──── 220Ω ──── LED AMARILLO (+) ── GND
GPIO 17 ──── 220Ω ──── LED VERDE (+) ──── GND
```

**Polaridad del LED:**
- Pata larga (+) hacia la resistencia
- Pata corta (-) hacia GND

---

## 🧪 Paso 6: Prueba de Montaje

### Test 1: SSR (sin MOSFET conectado primero)

Sube este código al ESP32:

```cpp
#define PIN_RELAY 23

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW);
  Serial.println("Test SSR");
}

void loop() {
  Serial.println("SSR ON");
  digitalWrite(PIN_RELAY, HIGH);
  delay(2000);
  
  Serial.println("SSR OFF");
  digitalWrite(PIN_RELAY, LOW);
  delay(2000);
}
```

**Qué esperar:**
- El ventilador debería encenderse a máxima velocidad durante 2s
- Luego apagarse durante 2s
- Repetir indefinidamente

**Si NO funciona:**
1. Verifica que la resistencia 220Ω esté bien conectada
2. Confirma que los tornillos del SSR están bien apretados
3. Mide con multímetro: entre +3 y -4 deberías ver ~3V cuando GPIO23=HIGH

---

### Test 2: MOSFET + SSR (control de velocidad)

Una vez confirmado que el SSR funciona, conecta el cable AZUL al DRAIN del MOSFET y sube:

```cpp
#define PIN_RELAY 23
#define PIN_FAN_PWM 19
#define FAN_PWM_CHANNEL 0
#define FAN_PWM_FREQ 25000
#define FAN_PWM_RES 8

void setup() {
  Serial.begin(115200);
  
  // SSR
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, HIGH);  // ON siempre para este test
  
  // PWM
  ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQ, FAN_PWM_RES);
  ledcAttachPin(PIN_FAN_PWM, FAN_PWM_CHANNEL);
  
  Serial.println("Test PWM");
}

void loop() {
  for (int speed = 0; speed <= 255; speed += 25) {
    Serial.printf("Velocidad: %d%%\n", (speed * 100) / 255);
    ledcWrite(FAN_PWM_CHANNEL, speed);
    delay(2000);
  }
}
```

**Qué esperar:**
- El ventilador debería aumentar gradualmente de velocidad
- Desde mínimo (0%) hasta máximo (100%)
- En incrementos cada 2 segundos

**Si NO funciona:**
1. Verifica la orientación del MOSFET (G-D-S)
2. Confirma que el pull-down 10kΩ está conectado
3. Mide voltaje en GATE: debería variar entre 0V y 3.3V

---

## 📸 Diagrama Final de Conexiones

```
┌──────────────────────────────────────────┐
│              ESP32                        │
│                                          │
│  GPIO 23 ──┬── 220Ω ──► +3 (SSR)        │
│            │                              │
│  GND ──────┴───────────► -4 (SSR)        │
│                                          │
│  GPIO 19 (PWM) ──┬──► GATE (MOSFET)     │
│                  │                       │
│  GND ────────────┼──► SOURCE (MOSFET)   │
│                  │                       │
│                10kΩ (pull-down)          │
│                  │                       │
│  GND ────────────┘                       │
│                                          │
│  GPIO 18 ──────────────► TACH ventilador│
└──────────────────────────────────────────┘
         │           │
         │      MOSFET IRLZ44N
         │      DRAIN → Cable AZUL
         │
    SSR BSSR-1DD
    +2 ← 12V+
    -1 → Cable ROJO ventilador
    
    Cable NEGRO ventilador → GND común
```

---

## ⚠️ Precauciones de Seguridad

| ⚠️ | Precaución |
|---|------------|
| 🔴 | **GND común obligatorio**: ESP32 GND = Fuente 12V GND |
| 🔴 | **Polaridad SSR**: +3 a GPIO (con resistencia), -4 a GND. **NO invertir** |
| 🟡 | **Apriete de tornillos**: Los bornes del SSR deben estar bien apretados |
| 🟡 | **Ventilación SSR**: A 25A se calienta. Tu ventilador no llega a eso (~1-2A) |
| 🟢 | **Resistencia 220Ω obligatoria**: sin ella destruyes el GPIO 23 |
| 🟢 | **Pull-down 10kΩ recomendado**: evita activaciones fantasma en boot |

---

## ✅ Checklist Final

- [ ] SSR montado: GPIO 23 → 220Ω → +3, GND → -4
- [ ] SSR alimentación: 12V+ → +2, -1 → ROJO ventilador
- [ ] MOSFET: GPIO 19 → GATE, SOURCE → GND, DRAIN → AZUL
- [ ] Pull-down 10kΩ: GATE → SOURCE
- [ ] Cable NEGRO ventilador → GND común
- [ ] Cable AMARILLO ventilador → GPIO 18 (TACH)
- [ ] GND ESP32 = GND fuente 12V
- [ ] Test SSR: ventilador ON/OFF ✓
- [ ] Test MOSFET: variación de velocidad ✓

---

**¡Listo para integrar con el firmware completo!** 🚀
