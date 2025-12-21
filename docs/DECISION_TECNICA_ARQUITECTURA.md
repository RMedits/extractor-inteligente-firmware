# 🧠 Decisión Técnica: Arquitectura de Control de Potencia

**Fecha:** Diciembre 2024
**Versión Firme:** v6.6C

---

## 1. Arquitectura Elegida: Corte de Potencia Directo (Low-Side Switching)

Para este proyecto, hemos decidido estandarizar el control del ventilador utilizando una topología de **Corte de Negativo** mediante un MOSFET de Potencia.

### **Configuración:**
- **Actuador:** Ventilador Delta QFR1212GHE (12V, 2.70A).
- **Driver:** MOSFET FQP30N06L (N-Channel, Logic Level).
- **Conexión:**
    - Drain MOSFET -> Negativo del Ventilador.
    - Source MOSFET -> GND Común.
    - Gate MOSFET -> GPIO 14 (vía 220Ω).
- **Lógica PWM:** **DIRECTA**
    - `PWM 0` (0%) -> MOSFET OFF -> Ventilador Apagado.
    - `PWM 255` (100%) -> MOSFET ON -> Ventilador a Máxima Potencia.

---

## 2. Arquitectura Descartada: Control de Señal PWM (4-Hilos)

Se evaluó y **descartó** una propuesta alternativa (visible en ramas anteriores como `jules-review`) que sugería usar el cable de control PWM del ventilador con lógica invertida.

### **Motivos del Descarte:**
1.  **Riesgo de Seguridad:** La lógica invertida implica que si el microcontrolador falla o se reinicia (GPIO en estado Low/Input), el ventilador podría arrancar a máxima potencia (Fail-On) dependiendo del driver. Preferimos un sistema **Fail-Off** (Si falla el control, el ventilador se apaga).
2.  **Complejidad Innecesaria:** El control por señal requiere asegurar niveles de voltaje compatibles en el cable PWM o usar optoacopladores/transistores adicionales.
3.  **Robustez:** El corte de potencia físico (Low-Side Switching) con el MOSFET de 30A garantiza que cuando el sistema dice "OFF", el ventilador no recibe energía, eliminando consumos parásitos o ruidos eléctricos en reposo.

---

## 3. Conclusión
Cualquier documentación futura o modificación de código debe respetar la lógica **PWM DIRECTA (0=OFF, 255=ON)** y el esquema de hardware de **Corte de Potencia**. No reintroducir lógica invertida sin cambiar el hardware físico.
