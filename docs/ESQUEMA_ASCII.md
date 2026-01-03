# ESQUEMA ELECTRICO ASCII - v7.1C FINAL (Labels Reales)
PROYECTO: Extractor Inteligente (Shield Morada 38P)

================================================================================
1. CONEXIONES DE CONTROL (Usa Etiquetas Serigrafiadas en el Shield)
================================================================================

   [ MODULO OLED + ENCODER ]             [ SHIELD ESP32 (Pines Macho S) ]
   -----------------------               --------------------------------
   VCC (3.3V-5V)          <----------->  Terminal 3V3 (Extremo Izq)
   GND                    <----------->  Fila G (Cualquier negro)
   OLED_SCL               <----------->  D22 (GPIO22)
   OLED_SDA               <----------->  D21 (GPIO21)
   ENCODER_TRA (CLK)      <----------->  D32 (GPIO32)
   ENCODER_TRB (DT)       <----------->  D33 (GPIO33)
   ENCODER_PUSH (OK)      <----------->  D27 (GPIO27)
   CONFIRM (BACK)         <----------->  D25 (GPIO25)
   BAK (PAUSE)            <----------->  D26 (GPIO26)

   [ SENSORES ]
   ------------
   AHT20+BMP280 VCC       <----------->  Terminal 3V3 (Extremo Izq)
   AHT20+BMP280 GND       <----------->  Fila G
   AHT20+BMP280 SCL       <----------->  D22 (I2C compartido)
   AHT20+BMP280 SDA       <----------->  D21 (I2C compartido)

   MQ135 VCC (5V)         <----------->  Fila V (Roja - Jumper 5V)
   MQ135 GND              <----------->  Fila G
   MQ135 AO/AD            <----------->  D34 (GPIO34)

   [ LEDS ESTADO ]
   -------------
   ROJO (+)      --[220R]-- D4 (GPIO4)
   VERDE (+)     --[220R]-- D15 (GPIO15)
   GND           ---------- Fila G

================================================================================
2. CIRCUITO DE POTENCIA (Ventilador Delta 12V PWM 4 hilos)
================================================================================

   [ SHIELD PIN ]     [ RELE KY-019 (Corte 12V, opcional) ]
   --------------     ------------------------------------
   D23  ------------> S (Signal)
   Fila V (5V)  ----> + (VCC)
   Fila G (GND) ----> - (GND)

   [ CONTROL PWM DIRECTO ]
   D19 (GPIO19) -------> Cable Azul PWM del Ventilador

================================================================================
3. NOTAS CRITICAS
================================================================================
- JUMPER VOLTAJE: Debe estar en la posicion "5V".
- PWM PIN: Usamos D19 (GPIO19) al cable azul del ventilador.
- ALIMENTACION: Todo el sistema (ESP32, Rele, Sensores) se alimenta desde el 
  puerto USB del ESP32 o Vin por ahora.
