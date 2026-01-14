# 🏠 Integración Home Assistant + Modo Noche

## ❓ Tus Preguntas Respondidas

### 1️⃣ "¿El modo manual infinito te deja seleccionar la velocidad?"

**✅ SÍ, COMPLETAMENTE**

El flujo es:
```
1. Gira encoder (AUTO → MANUAL_SETUP)
2. Paso 0: Selecciona TIEMPO (15-120 min) ← aunque sea infinito, puedes seleccionarlo
3. Paso 1: Selecciona VELOCIDAD (25%-100%) ← ✅ AQUÍ seleccionas la velocidad
4. Paso 2: Elige modo ("Limitado" vs "Infinito")
5. Si seleccionas "Infinito":
   → Ventilador corre a la velocidad que elegiste EN PASO 1
   → Sin límite temporal
   → Mientras presiones BACK para salir
```

**Ejemplo:**
```
- Selecciono Velocidad: 35%
- Selecciono Modo: "Infinito"
- Ventilador CORRE A 35% INDEFINIDAMENTE ✅
```

---

### 2️⃣ "NO QUIERO QUE SE APAGUE LA OLED DESPUÉS DE 5 MIN EN MODO INFINITO"

**✅ YA ARREGLADO**

Acabo de modificar el código:
```cpp
// Antes:
if (oledOn && (millis() - oledLastActivity > OLED_TIMEOUT * 1000UL)) {
  display.ssd1306_command(0xAE); // APAGABA SIEMPRE
}

// Ahora:
bool keepOledOn = (currentMode == MODE_MANUAL_INFINITE) || nightModeEnabled;

if (!keepOledOn && oledOn && (millis() - oledLastActivity > OLED_TIMEOUT * 1000UL)) {
  display.ssd1306_command(0xAE); // SOLO apaga si NO estás en infinito
}
```

**Resultado:**
- ✅ Modo INFINITO: OLED **SIEMPRE ENCENDIDA** (no se apaga)
- ✅ Otros modos: OLED se apaga después 5 min (ahorro energía)

---

### 3️⃣ "¿Se pueden poner emojis en la pantalla OLED? ¿Es monocromática?"

**SÍ, tienes emojis (parcialmente) - La OLED ES MONOCROMÁTICA**

#### Sobre la pantalla:
- **Tipo:** SSD1306 128×64 píxeles
- **Color:** ⚪ Blanco sobre fondo negro (monocromática)
- **Caracteres soportados:** ASCII estándar (0-127) + caracteres extendidos (128-255)

#### Emojis/Símbolos disponibles:

```
SÍMBOLOS ÚTILES (desde tabla ASCII extendida):
────────────────────────────────────────────
☀ = 15 (sol)
♡ = 3 (corazón)
★ = 42 (estrella)
● = 249 (círculo)
■ = 254 (cuadrado)
→ = 26 (flecha derecha)
← = 27 (flecha izquierda)
▲ = 24 (triángulo arriba)
↓ = 25 (triángulo abajo)
┌ ┐ └ ┘ (esquinas)
│ ─ (líneas)
°C ° (grados)
```

#### EJEMPLOS QUE PUEDES USAR AHORA:

```cpp
// Pantalla de temperatura con símbolo
display.print("Temp: 22.5");
display.print((char)167); // °

// Pantalla de velocidad con flecha
display.print("Vel: ");
display.print((char)26); // →
display.print("50%");

// Pantalla de error con símbolo especial
display.print("ERROR!");
display.print((char)42); // ★

// Modo infinito con símbolo
display.print("Infinito ");
display.print((char)8734); // ∞ (si lo soporta)
```

#### LIMPIACIÓN:
No hay emojis coloridos como 😀 (eso es Bluetooth/WiFi/color que no soporta).

Pero **emojis simples de texto SÍ:**
```
[!] = Advertencia
[✓] = Confirmado (pero sería [√])
[*] = Importante
[∞] = Infinito
[◆] = Punto
```

---

## 🏠 Futuro: Home Assistant Integration

### Visión General

```
HOME ASSISTANT (en tu PC/Raspberry)
        ↕ (WiFi)
   ESP32 EXTRACTOR
        ↓
   Sensor Data + Control
```

### Lo que querías:

1. ✅ **Modo Noche:** Activable desde Home Assistant
2. ✅ **En modo noche:**
   - Pantalla encendida (para ver temperatura, estado)
   - ❌ LEDs apagados (EXCEPTO en caso de error)
   - En futuro: solo enciende pantalla por las noches

### Cómo se vería:

**En Home Assistant:**
```
┌─────────────────────┐
│  Extractor Control  │
├─────────────────────┤
│                     │
│ Velocidad: [----]   │ Slider
│ Modo Auto/Manual    │ Switch
│ 🌙 Modo Noche      │ Toggle ← NUEVO
│                     │
│ Temp: 22.5°C        │ Sensor
│ Humedad: 65%        │ Sensor
│                     │
└─────────────────────┘
```

---

## 🔧 Arquitectura Preparada (v7.1 - Ya incluido)

### Variable Global Añadida:
```cpp
bool nightModeEnabled = false;  // Activable desde HA
```

### Lógica Implementada:
```cpp
// En loop():
bool keepOledOn = (currentMode == MODE_MANUAL_INFINITE) || nightModeEnabled;
```

### Lo que falta para implementar (futuro):

1. **WiFi en setup():**
   ```cpp
   void setupWiFi() {
     WiFi.begin("SSID", "PASSWORD");
     // Conectar a red doméstica
   }
   ```

2. **MQTT o REST para recibir comandos:**
   ```cpp
   void handleHomeAssistant() {
     // Escuchar: /ha/extractor/night_mode
     // Si recibe true → nightModeEnabled = true
     // Si recibe false → nightModeEnabled = false
   }
   ```

3. **Apagar LEDs en modo noche (excepto error):**
   ```cpp
   void updateLEDs() {
     if (nightModeEnabled && currentMode != MODE_ERROR) {
       // Apagar todos los LEDs
       digitalWrite(LED_RED_PIN, LOW);
       digitalWrite(LED_YELLOW_PIN, LOW);
       digitalWrite(LED_GREEN_PIN, LOW);
     } else {
       // Lógica normal
       // ...
     }
   }
   ```

---

## 📱 Tipo de Integración (Opciones)

### Opción 1: MQTT (Recomendado para IoT) ⭐
```
Home Assistant ← MQTT Broker → ESP32
              (tema: home/extractor/night_mode)
```
- Más rápido
- Más confiable
- Ideal para automatizaciones

### Opción 2: REST API (HTTP)
```
Home Assistant → POST /api/config → ESP32
```
- Más simple
- Requiere menos dependencias

### Opción 3: Integración nativa con HA
```
Home Assistant ← ESPHome protocol → ESP32
```
- Más integrado
- Requiere ESPHome firmware

**Mi recomendación:** MQTT (Opción 1)

---

## 🌙 Cómo sería el Modo Noche

### Pantalla en Modo Noche:
```
┌─────────────────────┐
│ EXTRACTOR - NOCHE  │
├─────────────────────┤
│                     │
│ Temp: 21.2°C        │
│ Humedad: 62%        │
│ Calidad: BUENA      │
│                     │
│ Velocidad: AUTO     │
│                     │
│ [Pantalla blanca]   │
│ LEDs APAGADOS       │
│ (salvo ERROR)       │
│                     │
└─────────────────────┘
```

### Pantalla Normal:
```
┌─────────────────────┐
│ EXTRACTOR - AUTO    │
├─────────────────────┤
│ 🟢 🟡 🔴            │
│ Status: OK          │
│                     │
│ Temp: 21.2°C        │ 🔴 LED ROJO
│ Humedad: 62%        │ 🟡 LED AMARILLO
│ Calidad: BUENA      │ 🟢 LED VERDE
│                     │ (Visibles)
│ Velocidad: 50%      │
│                     │
└─────────────────────┘
```

---

## ✨ Mejora Visual para OLED

Aquí te propongo pantallas mejoradas con símbolos:

### OPCIÓN 1: Símbolos ASCII simples
```
drawManualInfiniteScreen():
┌──────────────────┐
│ [∞] INFINITO     │
├──────────────────┤
│ Vel: ▓▓▓▓░░░░░░ │
│ 50%              │
│                  │
│ [◆] Presiona <-- │
│ para salir       │
└──────────────────┘
```

### OPCIÓN 2: Símbolos de estado
```
┌──────────────────┐
│ [!] ERROR        │
├──────────────────┤
│ ★ Sensor AHT20   │
│ ★ OLED           │
│                  │
│ Status: CRÍTICO  │
└──────────────────┘
```

### OPCIÓN 3: Barra de progreso
```
┌──────────────────┐
│ Velocidad: 75%   │
│ ▓▓▓▓▓▓░░░░ 75%   │
│                  │
│ Temp: 22°C       │
│ Hum:  65%        │
└──────────────────┘
```

---

## 📝 Código para Símbolos en OLED

```cpp
void drawManualInfiniteScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  
  // Símbolo infinito + texto
  display.setCursor(0, 0);
  display.print("[");
  display.print((char)8734); // Intenta mostrar ∞
  display.print("] INFINITO");
  
  // Velocidad con barra
  display.setCursor(0, 15);
  display.print("Vel: ");
  display.print(manualSpeedSel);
  display.print("%");
  
  // Barra visual
  int barLength = map(manualSpeedSel, 0, 100, 0, 110);
  display.setCursor(0, 30);
  for (int i = 0; i < barLength / 10; i++) {
    display.print((char)254); // ■ Bloque lleno
  }
  for (int i = barLength / 10; i < 11; i++) {
    display.print((char)176); // ░ Bloque vacío
  }
  
  // Instrucción
  display.setCursor(0, 55);
  display.print((char)27); // Flecha izquierda
  display.print(" BACK para salir");
  
  display.display();
}
```

---

## 🔮 Roadmap Home Assistant

### Fase 1: Modo Noche Simple
- [ ] Variable `nightModeEnabled` (✅ YA ESTÁ)
- [ ] Apagar LEDs en modo noche
- [ ] Recibir comando simple (WiFi manual)

### Fase 2: Integración MQTT
- [ ] Library PubSubClient
- [ ] Conectar a MQTT broker
- [ ] Publicar: temperatura, humedad, estado
- [ ] Suscribirse: night_mode on/off

### Fase 3: Integración HA Completa
- [ ] Configuración automática en HA
- [ ] Automaciones (ej: activar noche a las 22:00)
- [ ] Historial de datos
- [ ] Dashboard HTML

### Fase 4: Emojis/UI Mejorada
- [ ] Fonts customizadas para OLED
- [ ] Animaciones suaves
- [ ] Indicadores visuales mejorados

---

## 💾 Estado Actual (v7.1.1)

| Feature | Status |
|---------|--------|
| Modo infinito con selección velocidad | ✅ DONE |
| OLED no se apaga en modo infinito | ✅ DONE |
| Variable `nightModeEnabled` | ✅ DONE |
| Soporte para emojis/símbolos | ✅ READY |
| Apagar LEDs en modo noche | 🔮 TODO |
| WiFi + MQTT | 🔮 TODO |
| Integración Home Assistant | 🔮 TODO |

---

## 🎯 Siguiente Paso

¿Quieres que ahora:

1. **Agregue la lógica para apagar LEDs en modo noche?** ← Fácil, 10 líneas
2. **Prepare la estructura WiFi/MQTT?** ← Intermedio, necesita dependencias
3. **Mejore la pantalla OLED con emojis?** ← Fácil, solo UI
4. **Todo lo anterior?** ← Completo

Responde y lo hago 👇

---

**Versión:** v7.1.1  
**Fecha:** 2026-01-12  
**Estado:** Modo infinito perfeccionado + preparado para HA  

