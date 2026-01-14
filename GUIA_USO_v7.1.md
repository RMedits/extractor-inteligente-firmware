# 📱 GUÍA DE USO v7.1 - MODO INFINITO + MEJORAS

## ¿Qué es Nuevo?

### 🎯 Modo Manual INFINITO (Sin Límite de Tiempo)

Ahora puedes mantener el ventilador encendido INDEFINIDAMENTE a la velocidad que quieras, sin que se apague automáticamente pasado un tiempo.

**Ideal para:**
- Emergencias (humo, olor fuerte que requiere más tiempo)
- Ventilación continua a baja velocidad
- Pruebas/mantenimiento

---

## 🕹️ Cómo Usar el Nuevo Modo

### Pasos

1. **En modo AUTO**, gira el encoder → entra en **MANUAL SETUP**

2. **Paso 0: Selecciona TIEMPO**
   - Gira encoder: 15 min → 30 min → 45 min ... → 120 min
   - Click para continuar

3. **Paso 1: Selecciona VELOCIDAD**
   - Gira encoder: 25% → 50% → 75% → 100%
   - Click para continuar

4. **Paso 2: Selecciona MODO** ✨ NUEVO
   - Gira encoder: `"Limitado"` ↔ `"Infinito"`
   - **Limitado:** Se apagará cuando termine el timer
   - **Infinito:** Corre indefinidamente (tú controlas cuándo parar)
   - Click para confirmar

5. **El ventilador arranca** 🌀
   - Pantalla muestra velocidad actual
   - Si seleccionaste **"Limitado":** cuenta atrás del timer
   - Si seleccionaste **"Infinito":** muestra "SIN LIMITE"

### Detener (en cualquier momento)

- Presiona botón **BACK** (botón de la izquierda) → vuelve a AUTO

---

## 🔧 Mejoras Internas (NO requieren cambio de hardware)

### 1. PWM Más Suave
- El ventilador sube/baja velocidad GRADUALMENTE (no de repente)
- Menos ruido, menos esfuerzo para motor y electricidad

### 2. Sensores Más Confiables
- Si el sensor principal AHT20 falla, usa automáticamente el sensor BMP280
- Reintentos automáticos si hay problema de conexión I2C

### 3. Botones Más Precisos
- Debounce mejorado: no registra accidentes
- Requiere 3 pulsaciones confirmadas para activar

### 4. Pantalla Más Eficiente
- OLED se apaga automáticamente después de 5 minutos sin tocar nada
- Ahorra batería/energía

### 5. Sin Riesgo de Bloqueo
- Sistema NO se congela si hay error
- LED rojo parpadea si hay problema (pero sistema sigue responsivo)

---

## 🚨 Modo Pausa (Emergencia)

Sigue igual que antes:
- Presiona **PAUSA** (botón de la derecha) → ventilador se detiene, pantalla muestra "PAUSADO"
- Mantén presionado para reanudar automáticamente

---

## 💡 Ejemplos Prácticos

### Escenario 1: Cocina con mucho humo
1. Gira encoder (AUTO → MANUAL)
2. Selecciona 20 min
3. Selecciona 100% velocidad
4. **Selecciona "Limitado"** (para que se apague automáticamente)
5. ✓ Ventilador corre a máximo 20 minutos, después apaga

### Escenario 2: Ventilación nocturna lenta
1. Gira encoder (AUTO → MANUAL)
2. Selecciona 480 min (8 horas, pero...)
3. Selecciona 25% velocidad
4. **Selecciona "Infinito"** (no queremos límite temporal)
5. ✓ Ventilador corre a 25% hasta que presiones BACK

### Escenario 3: Prueba rápida
1. Gira encoder (AUTO → MANUAL)
2. Cualquier tiempo (será ignorado)
3. Selecciona velocidad que quieras probar
4. **Selecciona "Infinito"**
5. ✓ Prueba mientras quieras, BACK para salir

---

## 🔌 Cambios de Pinout (IMPORTANTE)

### LED Amarillo se MOVIÓ

Si montaste el hardware anteriormente:

- **Antes:** GPIO 4 (INCORRECTO - causa interferencia)
- **Ahora:** **GPIO 12** (CORRECTO)

**⚠️ Si tu hardware tiene LED amarillo en GPIO 4, necesitas trasladarlo a GPIO 12**

Diagrama:
```
ESP32
│
├─ GPIO 12 ← LED AMARILLO (NUEVO)
├─ GPIO 18 ← LED ROJO (sin cambios)
├─ GPIO 17 ← LED VERDE (sin cambios)
├─ GPIO 19 ← MOSFET PWM (sin cambios)
└─ ...
```

---

## ❌ Si Algo No Funciona

### El ventilador no responde
- [ ] Verificar MOSFET GPIO 19 tiene PWM
- [ ] Verificar pull-down 100k en gate
- [ ] Verificar resistor 100Ω en serie con gate

### Los botones no funcionan
- [ ] Verificar encoder en GPIO 32, 33, 27
- [ ] Verificar botones CONFIRM (P25) y BAK (P13)
- [ ] Intentar presionar 3 veces (debounce)

### Pantalla OLED apagada
- [ ] ¿Pasó más de 5 minutos sin pulsar nada?
- [ ] Presiona un botón → enciende
- [ ] Si no enciende, revisar conexión I2C

### Sensores no leen (ERROR en pantalla)
- [ ] Verificar AHT20 en I2C (dirección 0x38)
- [ ] Verificar BMP280 en I2C (dirección 0x77)
- [ ] Verificar pull-ups 10k en SDA/SCL

---

## 📊 Velocidades

| % | PWM | Tipo |
|----|-----|------|
| 25% | ~64 | Bajo (muy silencioso) |
| 50% | ~128 | Medio |
| 75% | ~192 | Medio-Alto |
| 100% | ~255 | Máximo (muy fuerte) |

---

## 🔮 Futuro (Servidor Debian)

En el futuro, cuando tengas el servidor Debian online:
- El ESP32 enviará logs de temperatura, humedad, calidad aire y potencia del ventilador
- Podrás ver histórico en dashboard web
- Sistema está preparado, falta conectar el servidor

---

**Versión:** v7.1  
**Última actualización:** 2024-12-20  
**Estado:** ✅ LISTO PARA USAR  

