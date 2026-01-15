#include <Wire.h>
#include <Arduino.h>

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== I2C SCANNER ===");
  Serial.print("Inicializando I2C en SDA="); Serial.print(I2C_SDA_PIN);
  Serial.print(" SCL="); Serial.println(I2C_SCL_PIN);
  
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);
  
  delay(2000);
  
  Serial.println("\nEscaneando I2C...");
  Serial.println("Dirección | Dispositivo");
  Serial.println("-----------+-------------");
  
  int count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    
    if (error == 0) {
      count++;
      Serial.print("0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print("      | ");
      
      // Identificar dispositivos comunes
      switch(address) {
        case 0x3C:
        case 0x3D:
          Serial.println("OLED SSD1306");
          break;
        case 0x38:
          Serial.println("AHT20 (Temp/Hum)");
          break;
        case 0x77:
          Serial.println("BMP280 (Presión)");
          break;
        case 0x76:
          Serial.println("BMP280 ALT (Presión)");
          break;
        default:
          Serial.println("Desconocido");
      }
    }
  }
  
  Serial.println("-----------+-------------");
  Serial.print("Total encontrados: ");
  Serial.println(count);
  
  if (count == 0) {
    Serial.println("\n⚠️  ¡NO SE ENCONTRÓ NINGÚN DISPOSITIVO!");
    Serial.println("Verificar:");
    Serial.println("- Conexiones SDA (GPIO 21) y SCL (GPIO 22)");
    Serial.println("- Pull-ups de 10k ohm en SDA y SCL");
    Serial.println("- Voltaje en los módulos (3.3V)");
  }
}

void loop() {
  delay(5000);
  Serial.println("--- Escaneo cada 5 segundos ---");
  
  int count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) count++;
  }
  Serial.print("Dispositivos I2C activos: ");
  Serial.println(count);
}
