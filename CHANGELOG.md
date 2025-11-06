# Changelog

## [6.1.0] - 2025-11-06

### ✨ Añadido
- **Integración del sensor AHT20**: El firmware ahora es compatible con el sensor AHT20 para temperatura y humedad.
- **Lecturas Redundantes y Promediadas**: El sistema lee datos tanto del BME280 como del AHT20 y utiliza el promedio de ambos para obtener mediciones de temperatura y humedad más precisas y fiables.
- **Manejo de Fallos de Sensor**: El código puede operar con solo uno de los dos sensores (BME280 o AHT20) si el otro no es detectado.

### 🔧 Mejorado
- **Precisión de Sensores**: La lógica de promediado mejora la robustez de las lecturas ambientales.
- **Logs de Debug**: Los mensajes del puerto serie ahora muestran las lecturas de ambos sensores y el valor promediado resultante.

## [6.0.0] - 2025-11-06

### ✨ Añadido
- **Nueva Lógica de Botones Ergonómica**:
  - **Encoder (Pulsar)** ahora es **OK/Confirmar**.
  - **Botón `CONFIRM`** ahora es **Back/Cancelar**.
  - **Botón `BAK`** ahora activa la **Pausa de Emergencia**.
- **Modo Pausa**:
  - Permite detener el ventilador y congelar el temporizador en cualquier momento.
  - Al reanudar, el sistema vuelve al estado exacto en el que estaba.
  - Ideal para interrupciones temporales sin cancelar la configuración manual.
- Nuevo estado `PAUSA` en la máquina de estados.

### 🔧 Cambiado
- **¡CAMBIO DE PINOUT CRÍTICO!**
  - El control del **Relé** se ha movido del `GPIO 27` al **`GPIO 23`**.
  - El **botón del encoder (OK)** ahora se conecta al `GPIO 27`.
- Toda la documentación (README, diagramas, guías) ha sido actualizada para reflejar la v6.0.

## [4.0.0] - 2024-11-04

### ✨ Añadido
- Nuevo estado MANUAL_SELECCION_VELOCIDAD
- 4 opciones de velocidad en modo manual (25%, 50%, 75%, 100%)
- Navegación con botón BACK entre menús
- Display actualizado con menú de velocidad
- Logs estructurados con emojis para mejor debugging
- Validación robusta de lecturas de sensores
- Documentación completa de instalación y calibración

### 🔧 Mejorado
- Máquina de estados expandida de 3 a 4 modos
- Interfaz OLED más informativa
- Anti-rebote mejorado (250ms)
- Rango PWM ajustado (80-255) para mejor control
- Comentarios detallados en el código

### 🐛 Corregido
- Prevención de parada del motor en velocidades bajas
- Manejo de valores NaN del BME280
- Validación de rango ADC del MQ135

## [3.0.0] - 2024-10-XX
- Versión inicial con 3 modos
