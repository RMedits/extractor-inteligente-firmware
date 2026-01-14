# 🧠 Decisión Técnica: Arquitectura de Control de Potencia - v7.1C FINAL

**Fecha:** Diciembre 2024
**Versión Firme:** v7.1C

---

## 1. Arquitectura Elegida: Control PWM Directo (Ventilador 4 Hilos)

Para este proyecto, estandarizamos el control de velocidad utilizando la **línea PWM dedicada del ventilador (4 hilos)**. El corte/encendido de alimentación se realizará mediante un **MOSFET de potencia (low-side)** cuando sea necesario; no se usará relé.

### **Configuración:**
- **Actuador:** Ventilador Delta QFR1212GHE (12V, 2.70A).
- **Control PWM:** GPIO 19 directo al cable PWM del ventilador. *GPIO 14 descartado por seguridad*.
- **Lógica PWM:** **DIRECTA**
    - `PWM 0` (0%) -> Ventilador detenido.
    - `PWM 255` (100%) -> Ventilador a máxima potencia.

---

## 2. Arquitectura Adoptada: Corte/Bypass con MOSFET

Se adopta el uso de un MOSFET N-channel (configuración low-side) para cualquier función de corte/encendido de la alimentación del ventilador. La modulación de velocidad se mantiene en la línea PWM del ventilador; el MOSFET ofrece una solución segura y compacta en lugar del relé mecánico.

---

## 3. Conclusión
Cualquier documentación futura o modificación de código debe respetar la lógica **PWM DIRECTA (0=OFF, 255=ON)** y el esquema de hardware con **PWM directo al ventilador + MOSFET de corte (si aplica)**.
