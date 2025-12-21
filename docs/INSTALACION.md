# 📖 Guía de Instalación del Firmware

Esta guía te ayudará a cargar el firmware del Extractor Inteligente en tu placa ESP32.

## Requisitos Previos

Tienes dos opciones principales para compilar y subir el código.

### Opción 1: Visual Studio Code + PlatformIO (Recomendado)
- **Editor de Código**: [Visual Studio Code](https://code.visualstudio.com/)
- **Extensión PlatformIO IDE**: Instálala desde el marketplace de extensiones de VS Code. PlatformIO gestionará automáticamente las librerías y la compilación.

### Opción 2: Arduino IDE
- **Software**: [Arduino IDE 2.x](https://www.arduino.cc/en/software)
- **Soporte para ESP32**: Necesitarás añadir el gestor de tarjetas de ESP32. Ve a `Archivo > Preferencias` y en "Gestor de URLs de Tarjetas Adicionales" añade: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
- **Librerías**: Deberás instalar manualmente las siguientes librerías desde el "Gestor de Librerías":
  - `Adafruit GFX Library`
  - `Adafruit SSD1306`
  - `Adafruit BME280 Library`
  - `ESP32Encoder`

---

## Instalación Paso a Paso (PlatformIO)

1. **Clona el Repositorio**:
   ```bash
   git clone https://github.com/RMedits/extractor-inteligente-firmware.git
   cd extractor-inteligente-firmware
   ```

2. **Abre el Proyecto**:
   - Abre Visual Studio Code.
   - Haz clic en el icono de PlatformIO en la barra lateral izquierda.
   - Selecciona "Open Project" y elige la carpeta que acabas de clonar.

3. **Compila y Sube el Firmware**:
   - Conecta tu placa ESP32 DevKit a tu ordenador por USB.
   - En la barra de estado inferior de VS Code, haz clic en el icono de la flecha (→) que dice "PlatformIO: Upload".
   - PlatformIO compilará el proyecto, instalará las dependencias y subirá el firmware a la placa.

4. **Monitorea la Salida**:
   - Una vez subido, haz clic en el icono del enchufe (🔌) que dice "PlatformIO: Monitor" para ver los logs del sistema en tiempo real.

---

## Configuración Inicial y Verificación

### Escaneo de Direcciones I2C
Es crucial verificar las direcciones de tus componentes I2C (OLED y BME280).
1. Carga un sketch de "I2C Scanner" en tu ESP32. Puedes encontrar ejemplos en `Archivo > Ejemplos > Wire > i2c_scanner`.
2. Abre el Monitor Serial. Deberías ver algo como:
   ```
   I2C device found at address 0x3C  !
   I2C device found at address 0x76  !
   ```
3. **Verifica las direcciones en el código**:
   - En `src/main.cpp`, comprueba que `I2C_ADDRESS` coincida con la de tu OLED (normalmente `0x3C`).
   - El código ya busca el BME280 en `0x76` y `0x77`, por lo que no necesitas cambiar nada para el sensor.

### Primera Prueba
Al iniciar por primera vez, observa el Monitor Serial. Deberías ver:
```
╔════════════════════════════════╗
║  EXTRACTOR INTELIGENTE v4.0   ║
╚════════════════════════════════╝

Iniciando BME280... ✓ OK
Calibrando sensor MQ135 (30s)...
✓ Sistema listo.
```
En la pantalla OLED, verás una pantalla de bienvenida y luego la de calibración, antes de pasar al modo automático.

---

##  Troubleshooting (Solución de Problemas)

- **OLED no detectado**:
  - **Causa**: Dirección I2C incorrecta o mala conexión.
  - **Solución**: Ejecuta el I2C Scanner. Asegúrate de que los pines SDA y SCL estén conectados correctamente (GPIO 21 y 22).

- **Error BME280**:
  - **Causa**: Mala conexión o sensor defectuoso.
  - **Solución**: Revisa las conexiones de VCC, GND, SDA y SCL.

- **Valores de MQ135 muy bajos/altos**:
  - **Causa**: El sensor necesita tiempo para calentarse y estabilizarse.
  - **Solución**: Deja el sistema encendido durante al menos 30 minutos para obtener lecturas fiables. Sigue la `CALIBRACION.md`.

- **Ventilador no arranca**:
  - **Causa**: Conexión incorrecta del relé/MOSFET, falta de alimentación externa de 12V.
  - **Solución**: Asegúrate de que la fuente de 12V esté conectada y que el GND sea común. Verifica el cableado del relé y el pin PWM al gate del MOSFET.

- **ESP32 se reinicia constantemente**:
  - **Causa**: Alimentación insuficiente desde el puerto USB, especialmente si hay muchos componentes.
  - **Solución**: Usa un cable USB de buena calidad o alimenta el ESP32 con una fuente externa de 5V.

- **Encoder no responde o salta valores**:
  - **Causa**: Mala conexión o ruido eléctrico.
  - **Solución**: Revisa los pines CLK y DT. Asegúrate de que el GND esté conectado.

---

## ✅ Checklist de Instalación
- [ ] VS Code y PlatformIO instalados.
- [ ] Repositorio clonado.
- [ ] Placa ESP32 conectada.
- [ ] Direcciones I2C verificadas.
- [ ] Firmware compilado y subido sin errores.
- [ ] Monitor Serial muestra logs de inicio correctos.

## Soporte
Si encuentras un problema que no está en esta guía, por favor, abre un "Issue" en el repositorio de GitHub y proporciona la siguiente información:
- Versión del firmware.
- Logs del Monitor Serial.
- Descripción del problema y cómo reproducirlo.
