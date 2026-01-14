# 🎨 EMOJIS ASCII - Guía Visual

## Los 4 Indicadores de Calidad del Aire

### 1. BUENA - [*_*]
```
 *   *
[*_*]
 Ojos felices, sonrisa
 Aire limpio y saludable
```
**Significado**: El usuario puede respirar tranquilo ✅

---

### 2. REGULAR - [-_-]
```
 -   -
[-_-]
 Ojos en línea, cara neutral
 Calidad aceptable
```
**Significado**: Todo está bien, pero podría mejorar ⚠️

---

### 3. MALA - [o_o]
```
 o   o
[o_o]
 Ojos sorprendidos/alarmados
 Contaminación significativa
```
**Significado**: ¡Atención! El aire no es bueno 🔴

---

### 4. CRÍTICA - [X_X]
```
 X   X
[X_X]
 Ojos tachados = Crítico
 Contaminación severa
```
**Significado**: ¡EMERGENCIA! Accionar de inmediato 🚨

---

## Pantalla Completa - Todos los Estados

### Estado BUENA
```
┌─────────────────────────────┐
│ EXTRACTOR TUNEADO BY RAUL   │
│ ═════════════════════════════│
│ [∞] MANUAL INFINITO         │
│ [█████░░░░░░░░░░░░] 25%    │
│ ★ T:20° H:40%              │
│ Aire: BUENA   [*_*]        │
└─────────────────────────────┘
```

### Estado REGULAR
```
┌─────────────────────────────┐
│ EXTRACTOR TUNEADO BY RAUL   │
│ ═════════════════════════════│
│ [∞] MANUAL INFINITO         │
│ [█████████░░░░░░░░] 50%    │
│ ★ T:24° H:55%              │
│ Aire: REGULAR [-_-]        │
└─────────────────────────────┘
```

### Estado MALA
```
┌─────────────────────────────┐
│ EXTRACTOR TUNEADO BY RAUL   │
│ ═════════════════════════════│
│ [∞] MANUAL INFINITO         │
│ [████████████░░░░░░] 70%   │
│ ★ T:26° H:68%              │
│ Aire: MALA    [o_o]        │
└─────────────────────────────┘
```

### Estado CRÍTICA
```
┌─────────────────────────────┐
│ EXTRACTOR TUNEADO BY RAUL   │
│ ═════════════════════════════│
│ [∞] MANUAL INFINITO         │
│ [██████████████████] 100%  │
│ ★ T:32° H:80%              │
│ Aire: CRITICA [X_X]        │
└─────────────────────────────┘
```

---

## Referencia de Rangos

```
Sensor: MQ135 (Calidad del Aire)
Unidad: ppm (partes por millón)

[*_*]  BUENA      <  300 ppm   ✅
[-_-]  REGULAR    300-599 ppm  ⚠️
[o_o]  MALA       600-899 ppm  🔴
[X_X]  CRÍTICA    ≥   900 ppm  🚨
```

---

## Tiempo de Actualización

Los emojis cambian **en tiempo real** cuando:

1. El sensor MQ135 detecta cambio de contaminación
2. Se actualiza la variable `airQuality` (cada 2 seg)
3. Se redibuja la pantalla OLED (cada frame)

**Ejemplo**:
```
Tiempo 0s:   Aire: BUENA   [*_*]  ← Usuario abre ventana
Tiempo 1s:   (sensor midiendo)
Tiempo 2s:   Aire: REGULAR [-_-]  ← Cambio detectado
Tiempo 3s:   (sensor midiendo)
Tiempo 4s:   Aire: MALA    [o_o]  ← Más contaminación
```

---

## Lógica de Decisión en el Código

```cpp
if (mq135_warmed) {  // Sensor calentado y listo
  if (airQuality < 300) {
    display.print("BUENA   [*_*]");  // ✅ VERDE
  } 
  else if (airQuality < 600) {
    display.print("REGULAR [-_-]");  // ⚠️ AMARILLO
  } 
  else if (airQuality < 900) {
    display.print("MALA    [o_o]");   // 🔴 ROJO
  } 
  else {
    display.print("CRITICA [X_X]");   // 🚨 EMERGENCIA
  }
}
else {
  display.print("CALENT. [...]");      // Esperando 30 seg
}
```

---

## Comportamiento del Sistema por Estado

### BUENA [*_*]
- ✅ Ventilador puede estar apagado o bajo
- ✅ No requiere mantenimiento urgente
- ✅ Bueno para dormir/descanso
- ✅ Usuario: "Puede relajarse"

### REGULAR [-_-]
- ⚠️ Ventilador activo a velocidad moderada
- ⚠️ Considerar aumentar velocidad
- ⚠️ Abrir ventana si es posible
- ⚠️ Usuario: "Monitoreando la situación"

### MALA [o_o]
- 🔴 Ventilador a alta velocidad
- 🔴 Requiere atención inmediata
- 🔴 Frenético cambio de aire recomendado
- 🔴 Usuario: "¡Hay un problema!"

### CRÍTICA [X_X]
- 🚨 Ventilador a MÁXIMA velocidad
- 🚨 ¡ALARMA! Nivel peligroso
- 🚨 Evacuar si es posible
- 🚨 Usuario: "¡EMERGENCIA!"

---

## Calibración del Sensor MQ135

El sensor se calibra automáticamente:

```
1. Inicio: setupTime = millis()
2. Espera: 30 segundos de precalentamiento
3. Línea base: Se establece en aire "normal"
4. Lectura: Media móvil de valores continuos

mq135_baseline = 400 (por defecto)
airQuality = (80% anterior) + (20% nueva lectura)
```

Esto asegura que:
- Los rangos sean consistentes
- No haya lecturas erráticas
- El sistema sea predecible y confiable

---

## Ejemplos en la Vida Real

### Escenario 1: Casa Limpia
```
Aire: BUENA [*_*]
- Casa recién limpiada
- Ventanas abiertas
- Sin humo ni químicos
→ Ventilador apagado, usuario relajado
```

### Escenario 2: Cocina Ocupada
```
Aire: REGULAR [-_-]
- Se está cocinando
- Vapor de agua presente
- Olor a comida en el aire
→ Ventilador activo al 50%, usuario atento
```

### Escenario 3: Incendio/Humo
```
Aire: MALA [o_o]
- Quema controlada cerca
- Humo entrado por ventilación
- Contaminación moderada
→ Ventilador al 70%, usuario activado
```

### Escenario 4: Gas/Químico Tóxico
```
Aire: CRÍTICA [X_X]
- Fuga de gas
- Químico industrial
- Contaminación severa
→ Ventilador 100%, ALARMA, evacuar
```

---

## Animación de los Emojis en la Pantalla

La pantalla se ve así mientras scrollea:

```
Frame 0: EXTRACTOR TUNEADO BY RAUL
         Aire: BUENA   [*_*]

Frame 1:  EXTRACTOR TUNEADO BY RA
          Aire: BUENA   [*_*]

Frame 2:   EXTRACTOR TUNEADO BY R
           Aire: BUENA   [*_*]

Frame 3:    EXTRACTOR TUNEADO BY
            Aire: BUENA   [*_*]

Frame 4:     EXTRACTOR TUNEADO B
             Aire: BUENA   [*_*]
```

Los emojis **NO parpadean**, simplemente se **actualizan con el sensor**.

---

## Comparativa Visual

### Antes (v7.1C)
```
Aire: 450 - NORMAL
     (solo número)
```

### Después (v7.2C) ⭐
```
Aire: REGULAR [-_-]
      (texto + emoji)
```

El nuevo sistema es:
- ✅ Más visual
- ✅ Más intuitivo
- ✅ Más informativo
- ✅ Menos técnico (mejor para no-especialistas)

---

## Referencia Rápida

```
[*_*] = Sonrisa feliz       = AIRE LIMPIO ✅
[-_-] = Cara neutral         = ACEPTABLE ⚠️
[o_o] = Sorpresa alarmada    = MALO 🔴
[X_X] = Crítico/Emergencia   = PELIGRO 🚨
```

**Recordar**: Los emojis son simples pero efectivos. El usuario sabrá de inmediato qué necesita hacer por la cara.

---

**Versión**: 7.2C  
**Emojis**: 4 estados  
**Actualización**: Tiempo real  
**Status**: ✅ Implementado
