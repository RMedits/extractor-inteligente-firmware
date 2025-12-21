# 📖 Guía de Uso - Extractor Inteligente v6.0C

Tu sistema de extracción de aire tiene dos modos principales de funcionamiento y una función de seguridad.

---

## 1. Modo Automático (Por defecto)
Al encender el sistema, siempre arrancará en este modo.

- **¿Qué hace?**: Monitoriza constantemente temperatura, humedad y calidad del aire.
- **Funcionamiento**: El ventilador se activa solo si se superan los umbrales (ej. ducha, humo, calor). La velocidad varía automáticamente entre 40% y 100%.
- **Pantalla**: Muestra los valores de los sensores y el estado "STANDBY" o la potencia del ventilador.

---

## 2. Modo Manual (Temporizador)
Para activar el ventilador manualmente por un tiempo fijo.

1. **Entrar**: Desde el modo Automático, **Pulsa la Rueda (Encoder)**.
2. **Tiempo**: Gira la rueda para elegir entre 30, 60 o 90 min. **Pulsa la Rueda** para confirmar.
3. **Velocidad**: Gira la rueda para elegir entre 25%, 50%, 75% o 100%. **Pulsa la Rueda** para iniciar.
4. **En Marcha**: Verás la cuenta regresiva y la velocidad fijada.

---

## 3. Función de Pausa de Emergencia
Si necesitas detener el ventilador instantáneamente sin apagar el ESP32.

- **Activar**: Mantén pulsado el botón **BAK (GPIO 26)** durante 2 segundos.
- **Efecto**: El ventilador se apaga y el temporizador se detiene (si estaba activo).
- **Reanudar**: Mantén pulsado **BAK** otros 2 segundos para volver al estado anterior.

---

## 4. Controles y Botones
- **Girar Rueda**: Navegar por las listas de los menús.
- **Pulsar Rueda (OK)**: Confirmar la selección y avanzar al siguiente paso.
- **Botón CONFIRM (BACK)**: Volver atrás en el menú o cancelar el temporizador manual para volver al modo automático.
- **Botón BAK (PAUSE)**: Pausa de emergencia (Pulsación larga 2s).

---

## 5. Recomendaciones de Calibración
El sensor **MQ135** (aire) realiza una limpieza de 30 segundos cada vez que el sistema arranca. Para lecturas más precisas de aire, el sistema debe llevar encendido al menos 24 horas.
