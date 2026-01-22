## 2024-05-23 - Throttling de Sensores I2C
**Learning:** En sistemas embebidos, leer sensores I2C lentos (como AHT20 con ~80ms de delay interno) en cada iteración del bucle principal destruye la capacidad de respuesta de la UI.
**Action:** Siempre desacoplar la frecuencia de lectura de sensores (ej. 0.5Hz) de la frecuencia de refresco de la UI (ej. 30Hz o 5Hz) usando timers no bloqueantes (`millis()`). Esto libera ciclos de CPU para animaciones y detección de botones.
