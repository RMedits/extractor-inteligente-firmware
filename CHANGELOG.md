# Changelog

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
