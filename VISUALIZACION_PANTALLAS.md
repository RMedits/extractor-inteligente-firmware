# Visualización de Pantallas - v7.2C

## 1. PANTALLA DE CONFIGURACIÓN MANUAL (NUEVO)

```
┌──────────────────────┐
│ CONFIG MANUAL        │
│ > Tiempo: 30 min    │
│   Veloc:  50%       │
│   Modo:   Limitado  │
│   Noche:  NO        │
│ Click=OK Back=Auto  │
└──────────────────────┘
```

### Navegación con Encoder
- **Paso 0**: Seleccionar tiempo (15-120 minutos)
- **Paso 1**: Seleccionar velocidad (25-100%)
- **Paso 2**: Seleccionar modo (Limitado o Infinito)
- **Paso 3**: Seleccionar modo noche (SI o NO) ⭐ NUEVO

### Comportamiento del Cursor
El símbolo `>` se mueve entre opciones conforme se presiona el botón de confirmación.

---

## 2. PANTALLA DE MODO INFINITO CON ANIMACIONES

```
┌──────────────────────┐
│ EXTRACTOR TUNEADO    │ ← Scroll suave cada 500ms
│ ─────────────────── │ ← Parpadea cada 600ms
│ [∞] MANUAL INFINITO  │
│ [████████░░░░░░░░]50%│ ← Barra de velocidad
│ ★ T:25° H:65%       │
│ Aire: 450 - NORMAL  │
│                BY RA │ ← Pulsante cada 300ms
└──────────────────────┘
```

### Animaciones Detalladas

#### 2.1 "EXTRACTOR TUNEADO" (Scroll Suave)
```
Frame 0: EXTRACTOR TUNEADO
Frame 1:  EXTRACTOR TUNEADO
Frame 2:   EXTRACTOR TUNEADO
Frame 3:    EXTRACTOR TUNEADO
Frame 4:     EXTRACTOR TUNEADO
(ciclo cada 500ms)
```

#### 2.2 Línea Divisoria (Parpadeo Elegante)
```
Visible:  ─────────────────────
Oculto:   (espacio en blanco)
Visible:  ─────────────────────
(ciclo cada 600ms)
```

#### 2.3 "BY RAUL" (Parpadeo Pulsante)
```
Frame 0: BY RAUL (visible)
Frame 1: BY RAUL (visible)
Frame 2:         (oculto)
Frame 3:         (oculto)
(ciclo cada 300ms)
```

---

## 3. COMPARATIVA DE CARACTERÍSTICAS

### Versión Anterior (v7.1C)
```
┌──────────────────────┐
│ EXTRACTOR TUNEADO    │
│ ─────────────────── │
│ [∞] MANUAL INFINITO  │
│ [████████░░░░░░░░]50%│
│ ★ T:25° H:65%       │
│ Aire: 450 - NORMAL  │
│                     │
└──────────────────────┘
```

### Versión Mejorada (v7.2C) ⭐
```
┌──────────────────────┐
│EXTRACTOR TUNEADO  ↔️ │ Animado
│ ─────────────────── │ Animado
│ [∞] MANUAL INFINITO  │
│ [████████░░░░░░░░]50%│
│ ★ T:25° H:65%       │
│ Aire: 450 - NORMAL  │
│          BY RAUL 💫│ Animado
└──────────────────────┘

Menú extendido:
✅ Tiempo
✅ Velocidad
✅ Modo (Limitado/Infinito)
✅ Modo Noche (NUEVO)
```

---

## 4. DIAGRAMA DE FLUJO DE MODOS

```
┌─────────┐
│  AUTO   │
│ MODE    │
└────┬────┘
     │ Girar encoder
     ↓
┌──────────────────────┐
│   MANUAL SETUP       │
│  (4 pasos ahora)     │
│                      │
│ 1. Tiempo            │
│ 2. Velocidad         │
│ 3. Modo              │
│ 4. Noche (⭐ NUEVO)  │
└────┬────────────────┘
     │ Confirmar
     ├─→ Infinito ──→ ┌──────────────────────┐
     │                 │   MANUAL INFINITO    │
     │                 │  (Animaciones)       │
     │                 │                      │
     │                 │ - Título animado     │
     │                 │ - Línea parpadeante  │
     │                 │ - "BY RAUL" pulsante │
     │                 │ - Pantalla siempre   │
     │                 │   ON si Noche=SI     │
     │                 └──────────────────────┘
     │
     └─→ Limitado ──→ ┌──────────────────────┐
                      │   MANUAL RUN         │
                      │  (con cronómetro)    │
                      └──────────────────────┘
```

---

## 5. TABLA DE ANIMACIONES

| Elemento | Tipo | Velocidad | Estado |
|----------|------|-----------|--------|
| EXTRACTOR TUNEADO | Scroll | 500ms | ✅ Activo |
| Línea divisoria | Parpadeo | 600ms | ✅ Activo |
| BY RAUL | Pulsante | 300ms | ✅ Activo |
| Barra velocidad | Dinámica | Tiempo real | ✅ Activo |
| Datos sensores | Dinámica | 2 segundos | ✅ Activo |

---

## 6. MODO NOCHE - IMPLICACIONES

### Cuando Modo Noche = SI
```
┌─────────────────────────────────┐
│ ✅ Pantalla siempre encendida   │
│ ✅ Visibilidad total en oscuridad│
│ ✅ Animaciones visibles          │
│ ⚠️  Consumo de energía aumentado │
└─────────────────────────────────┘
```

### Cuando Modo Noche = NO
```
┌─────────────────────────────────┐
│ ✅ Apagado automático en 5 min  │
│ ✅ Ahorro de energía            │
│ ✅ Menos contaminación visual   │
│ ⚠️  Requiere reactivación manual │
└─────────────────────────────────┘
```

---

## 7. TIMING DE ANIMACIONES (DIAGRAMA)

```
Tiempo (ms)    0  300  600  900 1200 1500 1800 2100 2400
               |   |    |    |    |    |    |    |    |
"BY RAUL"      VIS VIS  OC   OC   VIS  VIS  OC   OC   VIS
(300ms)        

Línea divis.   VIS ---VIS ---VIS ---VIS ---VIS ---VIS ---
(600ms)        

Scroll TUNEAD  P0  P0  P1   P1   P2   P2   P3   P3   P4
(500ms)        

Leyenda:
VIS = Visible
OC  = Oculto
P0-P4 = Posiciones (0-4 píxeles)
```

---

## 8. ESPECIFICACIONES TÉCNICAS

### Uso de Memoria
```
Variables estáticas por animación: 
- animationTime: 4 bytes (unsigned long)
- scrollPos: 1 byte (int)
- lineBlinkTime: 4 bytes (unsigned long)
- lineVisible: 1 byte (bool)
- raulBlinkTime: 4 bytes (unsigned long)
- raulBlinkPhase: 1 byte (int)

Total adicional: ~15 bytes
```

### Eficiencia CPU
```
- No usa delay() bloqueante
- Basado en millis() para precisión
- Variables static evitan re-inicializaciones
- Compatible con watchdog (8 segundos)
- Sin impacto en readSensors() o control PWM
```

### Rendimiento OLED
```
RAM usado:  22,716 bytes (6.9%)
Flash usado: 345,009 bytes (26.3%)
Capacidad sobrante:
- RAM:  304,964 bytes (93.1%)
- Flash: 965,711 bytes (73.7%)
```

---

## 9. INSTRUCCIONES DE USUARIO

### Activar Modo Noche

```
1. En AUTO MODE
   ↓ Girar encoder
   
2. MANUAL SETUP - Paso 1: TIEMPO
   ↓ Girar encoder para cambiar (15-120 min)
   ↓ Click encoder
   
3. MANUAL SETUP - Paso 2: VELOCIDAD
   ↓ Girar encoder para cambiar (25-100%)
   ↓ Click encoder
   
4. MANUAL SETUP - Paso 3: MODO
   ↓ Girar encoder para cambiar
   ↓ (← Izq = Limitado, Der → = Infinito)
   ↓ Click encoder
   
5. MANUAL SETUP - Paso 4: NOCHE ⭐ NUEVO
   ↓ Girar encoder para cambiar
   ↓ (← Izq = NO, Der → = SI)
   ↓ Click encoder para CONFIRMAR
   
6. PANTALLA INFINITA CON ANIMACIONES
   ↓ Pantalla ON continuo si seleccionó SI
```

### Cancelar Configuración
```
En cualquier momento presionar BACK (botón lateral)
para volver a AUTO sin aplicar cambios
```

---

**Versión**: 7.2C  
**Fecha**: 2024  
**Animaciones**: 3 nuevas  
**Estados**: Todo compilado ✅
