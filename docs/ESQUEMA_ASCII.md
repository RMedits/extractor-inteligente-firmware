# ESQUEMA ELECTRICO ASCII - v7.1C FINAL (Labels Reales)
PROYECTO: Extractor Inteligente (Shield Morada 38P)

================================================================================
1. CONEXIONES DE CONTROL (Usa Etiquetas Serigrafiadas en el Shield)
================================================================================

   [ MODULO OLED + ENCODER ]             [ SHIELD ESP32 (Pines Macho S) ]
   -----------------------               --------------------------------
   VCC (3.3V-5V)          <----------->  Terminal 3V3 (Extremo Izq)
   GND                    <----------->  Fila G (Cualquier negro)
   OLED_SCL               <----------->  P22
   OLED_SDA               <----------->  P21
   ENCODER_TRA (CLK)      <----------->  P32
   ENCODER_TRB (DT)       <----------->  P33
   ENCODER_PUSH (OK)      <----------->  P27
   CONFIRM (BACK)         <----------->  P25
   BAK (PAUSE)            <----------->  P26

   [ SENSORES ]
   ------------
   AHT20+BMP280 VCC       <----------->  Terminal 3V3 (Extremo Izq)
   AHT20+BMP280 GND       <----------->  Fila G
   AHT20+BMP280 SCL       <----------->  P22 (I2C compartido)
   AHT20+BMP280 SDA       <----------->  P21 (I2C compartido)

   MQ135 VCC (5V)         <----------->  Fila V (Roja - Jumper 5V)
   MQ135 GND              <----------->  Fila G
   MQ135 AO/AD            <----------->  P34

   [ LEDS ESTADO ]
   -------------
   ROJO (+)      --[220R]-- P4
   VERDE (+)     --[220R]-- P15
   GND           ---------- Fila G

================================================================================
2. CIRCUITO DE POTENCIA (Ventilador Delta 12V)
================================================================================

   [ SHIELD PIN ]     [ MOSFET N-channel (Low-side) ]
   --------------     --------------------------------
   P19  ------------> Gate (PWM control)
   Fila V (5V)  ----> + (VCC)  (Nota: Alimentación del ventilador debe ser +12V desde fuente dedicada)
   Fila G (GND) ----> - (GND)

   [ SHIELD PIN ]     [ VENTILADOR (PWM 4 HILOS) ]
   --------------     ---------------------------
   P19  ------------> PWM (Cable de control / Gate via driver si aplica)

================================================================================
3. NOTAS CRITICAS
================================================================================
- JUMPER VOLTAJE: Debe estar en la posicion "5V".
- PWM PIN: Usamos P19 (GPIO 19) por seguridad en el arranque.
 - ALIMENTACION: Lógica (ESP32, sensores) desde USB/Vin; ventilador desde fuente +12V dedicada. Asegura GND común.
