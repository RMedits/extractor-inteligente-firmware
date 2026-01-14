# 🟢🟡🔴 Lógica de LEDs - EXTRACTOR TUNEADO

## Estado Actual de los LEDs

El sistema tiene **3 LEDs**:
- 🔴 **LED_RED** (GPIO 18) - Rojo
- 🟡 **LED_YELLOW** (GPIO 12) - Amarillo  
- 🟢 **LED_GREEN** (GPIO 17) - Verde

---

## Lógica de Funcionamiento (v7.1.2)

### 1️⃣ MODO AUTO (Normal)

```
Condición: currentMode == MODE_AUTO
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔴 RED:    OFF  (apagado)
🟡 YELLOW: OFF  (apagado)
🟢 GREEN:  ON/OFF según ventilador

  • Si ventilador está ENCENDIDO   → 🟢 VERDE encendida
  • Si ventilador está APAGADO     → 🟢 VERDE apagada
```

**Visual:**
```
Ventilador activo:  ........................ 🟢
Ventilador parado:  ..........................
```

---

### 2️⃣ MODO MANUAL (Setup + Run)

```
Condición: currentMode == MODE_MANUAL_SETUP || MODE_MANUAL_RUN
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔴 RED:    OFF  (apagado)
🟡 YELLOW: ON   (encendida)
🟢 GREEN:  OFF  (apagado)
```

**Visual:**
```
Configurando velocidad/tiempo: .... 🟡 ....
Ventilador en manual:          .... 🟡 ....
```

**Nota:** El amarillo indica que estás en MODO MANUAL (configuración o ejecución)

---

### 3️⃣ MODO PAUSA (Emergencia)

```
Condición: currentMode == MODE_PAUSE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔴 RED:    ON   (encendida) ⚠️
🟡 YELLOW: OFF  (apagado)
🟢 GREEN:  OFF  (apagado)
```

**Visual:**
```
Ventilador pausado: 🔴 .... ....  (¡ATENCIÓN!)
```

**Nota:** El rojo indica que hay una PAUSA activa

---

### 4️⃣ MODO ERROR

```
Condición: currentMode == MODE_ERROR
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔴 RED:    ON   (parpadeando cada 500ms) ⚠️⚠️
🟡 YELLOW: OFF  (apagado)
🟢 GREEN:  OFF  (apagado)
```

**Visual:**
```
Error crítico: 🔴 ......... (PARPADEA)
               .... 🔴 ....
               .... .... 🔴
```

**Nota:** El rojo parpadeante es MÁXIMA ALERTA (sensor fallo, etc)

---

### 5️⃣ MODO INFINITO

```
Condición: currentMode == MODE_MANUAL_INFINITE
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

🔴 RED:    OFF  (apagado)
🟡 YELLOW: ON   (encendida)  ← Indica MANUAL
🟢 GREEN:  OFF  (apagado)
```

**Visual:**
```
Modo infinito: .... 🟡 ....  (amarillo = manual)
```

**Nota:** Es igual a MANUAL_RUN porque es un tipo de manual

---

## Tabla Resumen

| Modo | 🔴 RED | 🟡 YELLOW | 🟢 GREEN | Significado |
|------|--------|-----------|----------|------------|
| AUTO | OFF | OFF | ON/OFF* | Normal, ventilador según sensor |
| MANUAL_SETUP | OFF | ON | OFF | Configurando parámetros |
| MANUAL_RUN | OFF | ON | OFF | Ventilador manual con timer |
| **MANUAL_INFINITE** | OFF | ON | OFF | Ventilador manual sin límite |
| PAUSA | ON | OFF | OFF | ⚠️ Emergencia activada |
| ERROR | ON(blink) | OFF | OFF | ⚠️⚠️ Fallo crítico |

*El verde depende de si el ventilador está corriendo

---

## Código Actual

```cpp
void updateLEDs() {
  if (currentMode == MODE_PAUSE || currentMode == MODE_ERROR) {
    // PAUSA o ERROR: Rojo
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    
  } else if (currentMode == MODE_MANUAL_RUN || currentMode == MODE_MANUAL_SETUP) {
    // MANUAL (cualquier tipo): Amarillo
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, HIGH);
    
  } else { // AUTO
    // AUTO: Verde si ventilador ON
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, fanRunning ? HIGH : LOW);
  }
}
```

---

## 🌙 Futuro: Modo Noche (Home Assistant)

Cuando implementemos modo noche desde HA, la lógica será:

```cpp
void updateLEDs() {
  // Si está en modo noche, apagar LEDs (excepto error)
  if (nightModeEnabled && currentMode != MODE_ERROR) {
    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    return;  // Salir, no hacer nada más
  }
  
  // Lógica normal (como está ahora)
  if (currentMode == MODE_PAUSE || currentMode == MODE_ERROR) {
    digitalWrite(LED_RED_PIN, HIGH);
    // ...
  }
  // ... resto igual
}
```

**Comportamiento noche:**
- 🔴🟡🟢 TODOS APAGADOS (oscuro)
- EXCEPTO en ERROR: 🔴 rojo parpadeante (para saber que hay fallo)

---

## ¿Te Parece Bien?

La lógica actual es:

✅ **Intuitiva:**
- 🟢 Verde = Normal (AUTO)
- 🟡 Amarillo = Manual (control del usuario)
- 🔴 Rojo = Problema (PAUSA o ERROR)

✅ **Segura:**
- El rojo en error alerta al usuario
- El amarillo diferencia modos manuales
- El verde muestra actividad en AUTO

✅ **Extensible:**
- Fácil agregar modo noche
- Fácil agregar nuevas alertas

---

¿Quieres cambiar algo?

1. **Agregar parpadeo a los LEDs** (para distinguir mejor estados)
2. **Cambiar colores según velocidad del ventilador** (ej: verde oscuro 25%, verde brillante 100%)
3. **Dejar LEDs apagados en modo infinito** (solo error visible)
4. **Mantener como está** ✅

---

**Versión:** v7.1.2  
**Estado:** FUNCIONANDO ✅  
**Build:** SUCCESS  

