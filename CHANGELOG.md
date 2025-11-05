# Changelog

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
