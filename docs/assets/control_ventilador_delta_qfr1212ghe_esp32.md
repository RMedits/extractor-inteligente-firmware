# Control de Ventilador Industrial Delta QFR1212GHE con ESP32

## 1. Resumen del Hardware

- **Microcontrolador:** ESP32 (lógica 3.3 V)
- **Ventilador:** Delta QFR1212GHE
- **Alimentación:** 12 V DC
- **Corriente nominal:** 2.7 A
- **Tipo:** Ventilador industrial de 4 pines
- **Pines:**
  - VCC: +12 V
  - GND: Masa
  - PWM: Control de velocidad
  - TACH (FG): Señal de revoluciones
- **Frecuencia PWM recomendada:** ~25 kHz

---

## 2. Funcionamiento del PWM en Ventiladores Delta de 4 Pines

El Delta QFR1212GHE sigue el estándar de ventiladores PWM de 4 hilos (tipo Intel):

- El motor **NO** se controla modulando la alimentación.
- La corriente principal (2.7 A) fluye directamente desde la fuente de 12 V al ventilador.
- El pin PWM **solo es una entrada de control**, no maneja potencia.

### Características clave del pin PWM:

- Entrada **open-collector / open-drain**
- Pull-up **interno** en el ventilador (típicamente a ~5 V, ≈10 kΩ)
- Rango de señal admitido: **0–15 V**
- Nivel lógico esperado:
  - LOW → 0 V
  - HIGH → flotante (el ventilador lo eleva internamente a ~5 V)

⚠️ **Importante:** el controlador externo no debe forzar 5 V directamente.

---

## 3. ¿Es compatible el PWM de 3.3 V del ESP32?

### Teoría

- 3.3 V está dentro del rango válido de entrada PWM del ventilador.
- Algunos ventiladores Delta reconocen correctamente 3.3 V como nivel alto.

### Práctica (lo que importa)

- El ventilador tiene pull-up interno a ~5 V.
- El ESP32 **NO es 5 V tolerant**.
- Conectar un GPIO directamente:
  - Puede forzar corriente hacia el ESP32
  - Puede activar diodos de protección internos
  - Puede acortar la vida del GPIO
  - En algunos casos, el ventilador ignora el PWM y gira al 100%

### Conclusión

👉 **Funcionar puede funcionar. Ser correcto y robusto, no.**

---

## 4. Riesgos de Conectar GPIO Directamente al PWM

- Sobretensión potencial en el GPIO (5 V internos del ventilador)
- Corriente inversa hacia el ESP32
- Ruido inducido por un motor de 2.7 A
- Comportamiento errático del control de velocidad
- Daño acumulativo en el pin del microcontrolador

**No recomendado para un diseño serio.**

---

## 5. Uso de MOSFET o Transistor como Buffer (RECOMENDADO)

### Objetivo

- Adaptar niveles 3.3 V → 5 V
- Proteger el GPIO del ESP32
- Respetar el modo open-drain esperado por el ventilador

---

### Opción A: MOSFET Canal N (recomendada)

Ejemplos:
- 2N7000
- BSS138
- BS170
- FQP30N06L (sobredimensionado pero válido)

**Conexión:**

- Source → GND
- Drain → PWM del ventilador
- Gate → GPIO ESP32 (con resistencia 100–1kΩ opcional)

**Funcionamiento:**

- GPIO HIGH (3.3 V) → PWM a GND (LOW)
- GPIO LOW → PWM flotante → pull-up interno del ventilador (~5 V)

✔️ Solución limpia  
✔️ Rápida  
✔️ Sin estrés eléctrico  

---

### Opción B: Transistor NPN

Ejemplos:
- 2N3904
- BC547
- 2N2222

**Conexión:**

- Emisor → GND
- Colector → PWM ventilador
- Base → GPIO ESP32 con resistencia (4.7–10 kΩ)

⚠️ Recomendable añadir:
- Pull-up externo a 5 V (2.2–4.7 kΩ) para flancos rápidos a 25 kHz

Funciona, pero el MOSFET es mejor opción.

---

## 6. Corte Total de Energía con Relé

### Hecho importante

El Delta QFR1212GHE **NO se detiene completamente** con PWM = 0%.

- Mantiene un giro mínimo (~700 RPM)
- Es comportamiento normal del fabricante

### Solución

✔️ Usar un **relé** (o MOSFET de potencia) en la línea de +12 V

**Buenas prácticas:**

- Cortar solo el positivo de 12 V
- Mantener masa común
- Añadir diodo flyback en la bobina del relé
- Añadir condensadores en la línea de 12 V:
  - ≥4.7 µF (recomendado por Delta)
  - +100 nF cerámico

⚠️ Al cortar 12 V:
- Asegurar PWM en LOW o flotante
- Evitar alimentar el ventilador “por el pin PWM”

---

## 7. Tacómetro (FG / TACH)

- Salida open-collector desde el ventilador
- Dos pulsos por revolución

### Recomendación

- Pull-up externo a **3.3 V** (≈10 kΩ)
- Entrada a GPIO con interrupción en el ESP32
- NO usar pull-up a 5 V

Opcional:
- Condensador pequeño (≈100 nF) si el cable es largo o hay ruido

---

## 8. Frecuencia PWM

- Usar **25 kHz**
- Generar PWM con periférico LEDC del ESP32
- Evitar PWM por software

---

## 9. Arquitectura Recomendada (Resumen)

