# 🧠 Decisión Técnica: Arquitectura de Control de Potencia - v7.1C FINAL

**Fecha:** Diciembre 2024
**Versión Firme:** v7.1C

---

## 1. Arquitectura Elegida: Control PWM Directo (Ventilador 4 Hilos)

Para este proyecto, estandarizamos el control de velocidad utilizando la **línea PWM dedicada del ventilador (4 hilos)**. El relé solo se usa para cortar o dar alimentación al ventilador.

### **Configuración:**
- **Actuador:** Ventilador Delta QFR1212GHE (12V, 2.70A).
- **Control PWM:** GPIO 19 directo al cable PWM del ventilador. *GPIO 14 descartado por seguridad*.
- **Lógica PWM:** **DIRECTA**
    - `PWM 0` (0%) -> Ventilador detenido.
    - `PWM 255` (100%) -> Ventilador a máxima potencia.

---

## 2. Arquitectura Descartada: Corte de Potencia con MOSFET

Se descarta el uso de un MOSFET de potencia para modular el ventilador. El control de velocidad se realiza por el cable PWM del propio ventilador, y el relé queda únicamente para ON/OFF de alimentación.

---

## 3. Conclusión
Cualquier documentación futura o modificación de código debe respetar la lógica **PWM DIRECTA (0=OFF, 255=ON)** y el esquema de hardware con **PWM directo al ventilador + relé de corte de alimentación**.
