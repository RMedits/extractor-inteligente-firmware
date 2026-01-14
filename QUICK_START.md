# QUICK START - Nuevas Características v7.2C

## 🚀 En 3 Pasos

### Paso 1: Compilar el Código
```bash
# Ir a la carpeta del proyecto
cd extractor-inteligente-firmware

# Compilar (o compilar y cargar en ESP32)
pio run          # Solo compilar
pio run -t upload  # Compilar y cargar
```

### Paso 2: Disfrutar en el Dispositivo
El código ya está listo. Simplemente cargalo en tu ESP32.

### Paso 3: Usar las Nuevas Características
Ver sección "Cómo Usar" abajo.

---

## 💡 ¿Qué Hay Nuevo?

### ✨ 3 Nuevas Animaciones
1. **"EXTRACTOR TUNEADO"** - Título que se desplaza suavemente
2. **"BY RAUL"** - Parpadeo pulsante en la esquina
3. **Línea divisoria** - Efecto de respiración visual

### 🌙 Modo Noche
- Pantalla siempre encendida
- Ideal para monitoreo continuo
- Se activa desde el menú de configuración

---

## 📱 Cómo Usar

### Activar Animaciones

1. **Enciende el dispositivo** → Inicia en modo AUTO
2. **Gira el encoder** → Entra a MANUAL SETUP
3. **Selecciona Tiempo** (15-120 min) → Click
4. **Selecciona Velocidad** (25-100%) → Click
5. **Selecciona Modo** (Limitado o Infinito) → Click
6. **Selecciona Noche** (SI o NO) → ⭐ NUEVO
7. **Confirma** → Las animaciones comienzan

### ¿Quieres Modo Noche?

En el **Paso 6**:
- Gira izquierda → **NO** (pantalla se apaga tras 5 min)
- Gira derecha → **SI** (pantalla siempre encendida)

---

## 📊 Pantalla Resultante

Cuando entres a modo MANUAL INFINITO verás esto:

```
EXTRACTOR TUNEADO    ← Se mueve
─────────────────── ← Parpadea
[∞] MANUAL INFINITO
[████████░░░░░░░░] 50%
★ T:25° H:65%
Aire: 450 - NORMAL
                BY RA ← Pulsante
```

---

## ⚙️ Especificaciones Técnicas (TL;DR)

| Aspecto | Valor |
|--------|-------|
| **Compilación** | ✅ Exitosa |
| **RAM usado** | 6.9% |
| **Flash usado** | 26.3% |
| **Errores** | 0 |
| **Animaciones** | 3 simultáneas |
| **FPS animaciones** | 2-3 FPS (suave) |

---

## 🔧 Archivos Modificados

Solo cambió **1 archivo** en cada proyecto:
- `src/main.cpp`

Pero agregaron **3 documentos nuevos**:
1. `CAMBIOS_IMPLEMENTADOS.md` - Detalles técnicos
2. `VISUALIZACION_PANTALLAS.md` - Diagramas y mockups
3. `RESUMEN_IMPLEMENTACION.md` - Este resumen

---

## 🐛 Troubleshooting

### "No veo las animaciones"
- ✅ Verifica que seleccionaste modo INFINITO
- ✅ Pantalla debe estar encendida
- ✅ Mira la pantalla 5-10 segundos para apreciar el efecto

### "La pantalla se apaga"
- ✅ Normal si seleccionaste Noche = NO
- ✅ Presiona cualquier botón para reactivar
- ✅ O selecciona Noche = SI en la próxima vez

### "Los sensores se ven raros"
- ✅ Normal durante el primer minuto (calibración)
- ✅ Espera 30 segundos para que se estabilicen
- ✅ El sensor MQ135 necesita calentar 30 seg

---

## 📚 Documentación Completa

Para más detalles, consulta:

1. **CAMBIOS_IMPLEMENTADOS.md**
   - Explicación técnica detallada
   - Código de cada característica
   - Comportamiento esperado

2. **VISUALIZACION_PANTALLAS.md**
   - Diagramas ASCII de pantallas
   - Timing de animaciones
   - Flujos de navegación
   - Instrucciones de usuario detalladas

3. **RESUMEN_IMPLEMENTACION.md**
   - Visión general completa
   - Especificaciones técnicas
   - Próximas mejoras potenciales

---

## ✅ Checklist Rápido

Antes de cargar el código:

- [ ] Compilaste sin errores
- [ ] Tu ESP32 está conectado
- [ ] Tienes PlatformIO instalado
- [ ] Los sensores están conectados correctamente

Después de cargar:

- [ ] El dispositivo enciende normalmente
- [ ] LEDs parpadean como antes
- [ ] Pantalla OLED muestra datos correctamente
- [ ] Las nuevas animaciones aparecen en modo infinito

---

## 🎨 Personalización

### Cambiar velocidad de animaciones

En `src/main.cpp`, busca estos valores:

```cpp
// Línea 1: "EXTRACTOR TUNEADO"
if (millis() - animationTime > 500)  // Cambiar 500 a otro valor
  animationTime = millis();
  scrollPos = (scrollPos + 1) % 5;

// Línea 2: Línea divisoria
if (millis() - lineBlinkTime > 600)  // Cambiar 600 a otro valor
  lineBlinkTime = millis();
  lineVisible = !lineVisible;

// Línea 3: "BY RAUL"
if (millis() - raulBlinkTime > 300)  // Cambiar 300 a otro valor
  raulBlinkTime = millis();
  raulBlinkPhase = (raulBlinkPhase + 1) % 4;
```

**Valores recomendados**:
- Más lento: 700-900ms
- Normal: 300-600ms
- Más rápido: 100-200ms

---

## 🚨 Soporte

Si algo no funciona:

1. **Verifica compilación** → `pio run`
2. **Revisa los logs** → Serial Monitor a 115200 baud
3. **Consulta la documentación** → Lee CAMBIOS_IMPLEMENTADOS.md
4. **Reinicia el dispositivo** → Presiona RESET

---

## 🎯 Próximos Pasos

Después de disfrutar las nuevas características, considera:

1. **Experimenta** con diferentes velocidades
2. **Intenta** activar/desactivar modo noche
3. **Monitorea** los sensores en tiempo real
4. **Disfruta** del diseño mejorado

---

**¡Disfruta tu Extractor Inteligente Tuneado! 🔧**

Por RAUL - Versión 7.2C ✅
