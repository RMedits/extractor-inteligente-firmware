## 2024-05-22 - Bloqueo I2C en Bucle UI
**Learning:** La librería `Adafruit_AHTX0` usa un `delay(80)` bloqueante en `getEvent()`. Llamar a esto dentro de un bucle de UI de alta frecuencia (200ms) destruye la capacidad de respuesta.
**Action:** Siempre desacoplar lecturas de sensores I2C lentos (especialmente los que tienen tiempos de integración) del bucle de visualización/interacción. Usar timers independientes (e.g., 2000ms para clima).
