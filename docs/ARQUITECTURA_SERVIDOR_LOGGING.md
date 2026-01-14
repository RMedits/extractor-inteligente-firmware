# Arquitectura de Logging para Servidor Debian

## 📋 Descripción

Preparación de arquitectura para envío de logs de sensores y consumo del ventilador a servidor Debian remoto.

**Estado Actual:** 🔮 Estructura preparada (servidor actualmente offline)
**Implementación:** Deferred hasta que servidor esté online

---

## 🏗️ Estructura de Datos

### Sensor Log Entry
```cpp
struct SensorLogEntry {
  unsigned long timestamp;      // ms desde boot (o time_t si NTP implementado)
  float humidity;               // % RH
  float temperature;            // °C
  int airQuality;              // PPM equivalente (MQ135)
  uint8_t fanSpeed;            // PWM 0-255
  uint8_t fanSpeedPercent;     // 0-100%
  uint8_t sensorState;         // SENSOR_OK=0, DEGRADED=1, FAILED=2
  char mode[16];               // "AUTO", "MANUAL_RUN", "MANUAL_INFINITE", etc
};
```

### Intervalo de Logging
- **Frecuencia:** Cada 30 segundos (configurable)
- **Buffer:** Almacenar últimos 100 registros en RAM (circular buffer)
- **Envío:** Cuando buffer ≥ 50 logs O timeout 5 minutos

---

## 🌐 Protocolos Soportados (Placeholder)

### Opción 1: HTTP POST (REST)
```cpp
// TODO: Implementar con HTTPClient library
POST /api/logs
Content-Type: application/json

{
  "device_id": "ESP32-EXTRACTOR-001",
  "logs": [
    {
      "timestamp": 123456789,
      "humidity": 65.3,
      "temperature": 22.5,
      "airQuality": 350,
      "fanSpeed": 200,
      "fanSpeedPercent": 78,
      "sensorState": 0,
      "mode": "AUTO"
    }
  ]
}
```

### Opción 2: MQTT (Recomendado para IoT)
```cpp
// TODO: Implementar con PubSubClient
Topic: home/extractor/logs
Payload: JSON array de logs
```

### Opción 3: InfluxDB (Time Series)
```cpp
// TODO: Implementar con InfluxDB API
Ideal para visualización con Grafana
```

---

## 📁 Estructura Propuesta

### En firmware
```
src/
├── main.cpp (existente)
├── logging.h (nuevo)
└── logging.cpp (nuevo)
    ├── SensorLogEntry struct
    ├── CircularBuffer<SensorLogEntry> buffer
    ├── logSensor(data)
    ├── sendLogsToServer()
    └── initNetworking()
```

### En servidor Debian
```
/opt/extractor-server/
├── app.py (Python Flask/FastAPI)
├── database/
│  └── logs.db (SQLite o PostgreSQL)
├── api/
│  └── /logs (POST endpoint)
└── dashboard/ (opcional Grafana/Grafikus)
```

---

## 🔄 Flujo de Datos

```
ESP32 Sensors
    ↓
readSensors() + loop()
    ↓
SensorLogEntry struct
    ↓
CircularBuffer (100 slots)
    ↓
[Buffer ≥ 50 OR timeout 5min]
    ↓
sendLogsToServer()
    ↓
HTTP/MQTT/InfluxDB
    ↓
Servidor Debian
    ↓
Almacenamiento (DB)
    ↓
Dashboard (opcional)
```

---

## 🔐 Seguridad

### Nivel 1 (Desarrollo)
- No autenticación (LAN local)
- HTTP plain (no HTTPS)

### Nivel 2 (Producción - Futuro)
- Token API key en headers
- HTTPS con certificado autofirmado
- Validación de SSL/TLS en ESP32

---

## 📊 Campos Capturados

| Campo | Tipo | Fuente | Rango |
|-------|------|--------|-------|
| timestamp | u32 | millis() | 0-4.3M |
| humidity | float | AHT20/BMP280 | 0-100% |
| temperature | float | AHT20/BMP280 | -40°C a +80°C |
| airQuality | int | MQ135 | 0-1023 (raw) |
| fanSpeed | u8 | PWM actual | 0-255 |
| fanSpeedPercent | u8 | Calculado | 0-100% |
| sensorState | enum | readSensors() | OK/DEGRADED/FAILED |
| mode | string | loop() switch | 16 bytes max |

---

## 📝 Notas de Implementación

### Libraries Necesarias (cuando se implemente)
- `WiFi.h` - Conectar a red
- `HTTPClient.h` (opción 1) - POST HTTP
- `PubSubClient.h` (opción 2) - MQTT
- `ArduinoJson.h` - Serializar JSON

### Consideraciones
1. **NTP Sync:** Sincronizar reloj ESP32 con NTP para timestamps precisos
2. **Fallback:** Si servidor está offline, buffer en RAM → escritura en SPIFFS si necesario
3. **Compresión:** Considerar compresión GZIP para logs grandes
4. **Rate Limiting:** Servidor debe validar max 1 POST/30seg por dispositivo

---

## 🚀 Roadmap

- [ ] Estructura preparada (✅ DONE v7.1)
- [ ] Implementar WiFi connect en setup()
- [ ] Agregar logSensor() a loop()
- [ ] Buffer circular en RAM
- [ ] sendLogsToServer() con reintentos
- [ ] Servidor Debian endpoint REST
- [ ] Almacenamiento en DB
- [ ] Dashboard Grafana/HTML

---

## 💡 Ejemplo de Uso Futuro (Pseudocódigo)

```cpp
// En setup()
initNetworking("SSID", "PASSWORD", "192.168.1.100:8000");

// En loop()
if (readSensorsInterval.ready()) {
  readSensors();
  
  SensorLogEntry log = {
    .timestamp = millis(),
    .humidity = hum,
    .temperature = temp,
    .airQuality = airQuality,
    .fanSpeed = currentSpeed,
    .fanSpeedPercent = map(currentSpeed, 0, 255, 0, 100),
    .sensorState = sensorState,
    .mode = "AUTO" // o según currentMode
  };
  
  logBuffer.push(log);
  
  if (logBuffer.size() >= 50 || logBuffer.timeOldest() > 300000) {
    sendLogsToServer();
  }
}
```

---

**Documento Preparado:** 2024-12-20  
**Estado:** 🔮 Estructura lista, implementación pending servidor online  
**Responsable:** Firmware v7.1  
