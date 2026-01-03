# 🧠 Decisión Técnica: Arquitectura de Control de Potencia - v7.1C FINAL

**Fecha:** Diciembre 2024
**Versión Firme:** v7.1C

---

## 1. Arquitectura Elegida: Control PWM 4-Hilos (Cable Azul)

Para este proyecto estandarizamos el control del ventilador usando su **entrada PWM nativa** (cable azul), eliminando el MOSFET de potencia.

### **Configuración:**
- **Actuador:** Ventilador Delta QFR1212GHE (12V, 2.70A) con señal PWM.
- **Driver:** Salida de **D19 (GPIO19)** directamente al cable azul PWM.
- **Conexiones Clave:**
    - +12V (rojo) y GND (negro) directos desde la fuente o pasando por el relé KY-019 en la línea de +12V.
    - Cable **PWM (azul)** -> D19 (GPIO19) del ESP32.
    - **GND común obligatorio** entre la fuente de 12V y el ESP32 para que la señal PWM tenga referencia.
- **Lógica PWM:**
    - `PWM 0` (0%) -> Ventilador Apagado.
    - `PWM 255` (100%) -> Ventilador a Máxima Potencia.

---

## 2. Arquitectura Descartada: Corte de Potencia con MOSFET

Se retira la topología de **Low-Side Switching** con MOSFET porque el ventilador ya acepta control PWM dedicado.

### **Motivos del Cambio:**
1. **Menos Componentes Críticos:** Se elimina el MOSFET, resistencias y disipación asociada.
2. **Compatibilidad Nativa:** El ventilador ofrece entrada PWM; controlar el cable azul simplifica el cableado.
3. **Mantenimiento:** Se reduce la probabilidad de fallas por soldaduras o calentamiento en el MOSFET.

---

## 3. Conclusión
Cualquier documentación futura o modificación de código debe respetar la lógica **PWM DIRECTA (0=OFF, 255=ON)** aplicada sobre el **cable azul en D19**. Mantener GND común entre las fuentes de 12V y 5V/USB. Si se añade corte de energía, usar el relé KY-019 en la línea de +12V.
