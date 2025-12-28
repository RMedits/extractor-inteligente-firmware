## 2024-05-22 - Cuello de botella en velocidad I2C con OLED
**Learning:** La velocidad por defecto de I2C (100kHz) hace que `display.display()` tome ~100ms para enviar el buffer de 1KB de la OLED. En un bucle limitado a 150ms, esto consume el ~66% del tiempo disponible, saturando el ciclo si se suman lecturas de sensores bloqueantes.
**Action:** Siempre verificar y configurar explícitamente `Wire.setClock(400000)` al usar pantallas SSD1306 para reducir el tiempo de transferencia a ~25ms.
