🔧 Guía de Calibración

Para que el modo automático funcione correctamente, necesitas calibrar los sensores para TU ambiente específico.

1. Calibración del Sensor MQ135 (Calidad de Aire)

Este sensor mide gases (CO2, Humo, Alcohol, etc.) y su valor "base" cambia según el ambiente. No da un valor en PPM directamente, sino una lectura analógica (0-4095 en el ESP32).

Requisito: El sensor MQ135 tiene una pequeña resistencia de calentamiento interna. Necesita estar encendido un tiempo para dar lecturas estables. Para la calibración inicial, déjalo encendido al menos 30 minutos, aunque idealmente 24-48 horas para una estabilización completa.

Proceso de Calibración:

Cargar el código: Carga el firmware en el ESP32.

Abrir Monitor Serial: Abre el Monitor Serial de Arduino/PlatformIO (velocidad 115200).

Ventilar la zona: Asegúrate de que la habitación (baño, galería) esté lo más "limpia" posible. Abre ventanas, ventila bien.

Observar el valor base: El Monitor Serial debería estar imprimiendo los valores de los sensores (si habilitaste el DEBUG). Observa el valor "A:" (Air Quality).

Anotar el valor "Limpio": Después de que el sensor se estabilice (ej. 30 min), anota el valor analógico que muestra. Este es tu "valor base limpio".

Ejemplo: El valor base en aire limpio es 350.

Establecer el umbral: El umbral en el código (AIR_QUALITY_THRESHOLD) debe ser un valor significativamente MÁS ALTO que tu valor base.

Ejemplo: Si tu base es 350, un buen punto de partida para el umbral es 600 o 700.

Prueba de "Estrés": Echa un poco de vapor (de un vaporizador) o acerca un trapo con alcohol isopropílico (con cuidado) cerca del sensor. Deberías ver cómo el valor analógico sube rápidamente. Si supera tu umbral (ej. 700), el ventilador debería activarse.

Ajuste Fino: Tendrás que ajustar este umbral (600 en el código) por prueba y error. Si el ventilador salta sin motivo, sube el umbral. Si no salta cuando hay mucho vapor, bájalo.

2. Ajuste de Umbrales (BME280)

Estos son más directos y dependen de tus preferencias.

Humedad (HUMIDITY_HIGH_THRESHOLD): El valor por defecto es 70%.

Si vives en una zona muy húmeda y el ventilador salta solo, súbelo a 75% o 80%.

Si después de una ducha no salta, bájalo a 65%.

Temperatura (TEMP_HIGH_THRESHOLD): El valor por defecto es 30°C. Es menos probable que este se active en un baño, pero es útil en galerías cerradas en verano. Ajústalo a tu gusto.