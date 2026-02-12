"""
Extractor Inteligente — Backend API
FastAPI + MQTT subscriber + SQLite
"""
import asyncio
import json
import os
import sqlite3
import threading
import time
from contextlib import asynccontextmanager
from datetime import datetime, timedelta

import paho.mqtt.client as paho_mqtt
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware

# =====================================================
# CONFIG
# =====================================================
MQTT_HOST = os.getenv("MQTT_HOST", "localhost")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
DB_PATH = os.getenv("DATABASE_URL", "sqlite:///data/extractor.db").replace("sqlite:///", "")

# =====================================================
# DATABASE
# =====================================================
def init_db():
    os.makedirs(os.path.dirname(DB_PATH) or ".", exist_ok=True)
    conn = sqlite3.connect(DB_PATH)
    conn.executescript("""
        CREATE TABLE IF NOT EXISTS sensor_readings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL DEFAULT (datetime('now')),
            temp REAL,
            hum REAL,
            pressure REAL,
            aqi INTEGER,
            dew REAL
        );

        CREATE TABLE IF NOT EXISTS state_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL DEFAULT (datetime('now')),
            mode TEXT,
            fan_speed INTEGER,
            rpm INTEGER,
            occupied INTEGER
        );

        CREATE TABLE IF NOT EXISTS events (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL DEFAULT (datetime('now')),
            type TEXT NOT NULL,
            message TEXT
        );

        CREATE INDEX IF NOT EXISTS idx_readings_ts ON sensor_readings(timestamp);
        CREATE INDEX IF NOT EXISTS idx_state_ts ON state_log(timestamp);
        CREATE INDEX IF NOT EXISTS idx_events_ts ON events(timestamp);
    """)
    conn.close()

def db_insert_sensors(data: dict):
    try:
        conn = sqlite3.connect(DB_PATH)
        conn.execute(
            "INSERT INTO sensor_readings (temp, hum, pressure, aqi, dew) VALUES (?, ?, ?, ?, ?)",
            (data.get("temp"), data.get("hum"), data.get("pressure"), data.get("aqi"), data.get("dew"))
        )
        conn.commit()
        conn.close()
    except Exception as e:
        print(f"DB sensor error: {e}")

def db_insert_state(data: dict):
    try:
        conn = sqlite3.connect(DB_PATH)
        conn.execute(
            "INSERT INTO state_log (mode, fan_speed, rpm, occupied) VALUES (?, ?, ?, ?)",
            (data.get("mode"), data.get("fan_speed"), data.get("rpm"), 1 if data.get("occupied") else 0)
        )
        conn.commit()
        conn.close()
    except Exception as e:
        print(f"DB state error: {e}")

def db_insert_event(event_type: str, message: str):
    try:
        conn = sqlite3.connect(DB_PATH)
        conn.execute("INSERT INTO events (type, message) VALUES (?, ?)", (event_type, message))
        conn.commit()
        conn.close()
    except Exception as e:
        print(f"DB event error: {e}")

def db_query(query: str, params: tuple = ()):
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    rows = conn.execute(query, params).fetchall()
    conn.close()
    return [dict(r) for r in rows]

# =====================================================
# MQTT CLIENT
# =====================================================
latest_sensors = {}
latest_state = {}
ws_clients: list[WebSocket] = []
mqtt_client = None

def on_mqtt_connect(client, userdata, flags, rc, properties=None):
    print(f"MQTT connected (rc={rc})")
    client.subscribe("extractor/#")
    db_insert_event("system", "Backend conectado a MQTT")

def on_mqtt_message(client, userdata, msg):
    global latest_sensors, latest_state
    try:
        payload = json.loads(msg.payload.decode())
    except json.JSONDecodeError:
        return

    topic = msg.topic

    if topic == "extractor/sensors":
        latest_sensors = payload
        db_insert_sensors(payload)

    elif topic == "extractor/state":
        # Detectar cambio de modo para event log
        old_mode = latest_state.get("mode", "")
        new_mode = payload.get("mode", "")
        if old_mode and old_mode != new_mode:
            db_insert_event("mode_change", f"{old_mode} → {new_mode}")

        latest_state = payload
        db_insert_state(payload)

    elif topic == "extractor/availability":
        status = msg.payload.decode()
        db_insert_event("availability", f"ESP32 {status}")

    # Notificar WebSocket clients
    asyncio.run_coroutine_threadsafe(broadcast_ws(), loop)

async def broadcast_ws():
    data = json.dumps({**latest_sensors, **latest_state, "ts": datetime.now().isoformat()})
    disconnected = []
    for ws in ws_clients:
        try:
            await ws.send_text(data)
        except Exception:
            disconnected.append(ws)
    for ws in disconnected:
        ws_clients.remove(ws)

def start_mqtt():
    global mqtt_client
    mqtt_client = paho_mqtt.Client(paho_mqtt.CallbackAPIVersion.VERSION2)
    mqtt_client.on_connect = on_mqtt_connect
    mqtt_client.on_message = on_mqtt_message
    mqtt_client.connect(MQTT_HOST, MQTT_PORT, 60)
    mqtt_client.loop_start()

# =====================================================
# FASTAPI APP
# =====================================================
loop = None

@asynccontextmanager
async def lifespan(app: FastAPI):
    global loop
    loop = asyncio.get_event_loop()
    init_db()
    start_mqtt()
    db_insert_event("system", "Backend iniciado")
    yield
    if mqtt_client:
        mqtt_client.loop_stop()
        mqtt_client.disconnect()

app = FastAPI(title="Extractor Inteligente API", version="1.0.0", lifespan=lifespan)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# =====================================================
# ENDPOINTS
# =====================================================

@app.get("/api/status")
def get_status():
    """Datos actuales en tiempo real"""
    return {**latest_sensors, **latest_state, "ts": datetime.now().isoformat()}

@app.get("/api/history")
def get_history(hours: int = 24, limit: int = 500):
    """Historial de lecturas de sensores"""
    since = (datetime.now() - timedelta(hours=hours)).isoformat()
    rows = db_query(
        "SELECT * FROM sensor_readings WHERE timestamp > ? ORDER BY timestamp DESC LIMIT ?",
        (since, limit)
    )
    return rows

@app.get("/api/states")
def get_states(hours: int = 24, limit: int = 200):
    """Historial de estados"""
    since = (datetime.now() - timedelta(hours=hours)).isoformat()
    rows = db_query(
        "SELECT * FROM state_log WHERE timestamp > ? ORDER BY timestamp DESC LIMIT ?",
        (since, limit)
    )
    return rows

@app.get("/api/events")
def get_events(limit: int = 100):
    """Log de eventos"""
    return db_query("SELECT * FROM events ORDER BY timestamp DESC LIMIT ?", (limit,))

@app.get("/api/stats")
def get_stats():
    """Estadísticas generales"""
    total_readings = db_query("SELECT COUNT(*) as count FROM sensor_readings")[0]["count"]
    mode_counts = db_query(
        "SELECT mode, COUNT(*) as count FROM state_log GROUP BY mode ORDER BY count DESC"
    )
    events_today = db_query(
        "SELECT type, COUNT(*) as count FROM events WHERE timestamp > date('now') GROUP BY type"
    )
    avg_24h = db_query("""
        SELECT
            ROUND(AVG(temp), 1) as avg_temp,
            ROUND(AVG(hum), 1) as avg_hum,
            ROUND(AVG(pressure), 1) as avg_pressure,
            ROUND(AVG(aqi), 0) as avg_aqi
        FROM sensor_readings
        WHERE timestamp > datetime('now', '-24 hours')
    """)
    return {
        "total_readings": total_readings,
        "mode_distribution": mode_counts,
        "events_today": events_today,
        "averages_24h": avg_24h[0] if avg_24h else {}
    }

@app.post("/api/command/{cmd}")
def send_command(cmd: str, value: str = ""):
    """Enviar comando al ESP32 via MQTT"""
    topic_map = {
        "mode": "extractor/cmd/mode",
        "fan_speed": "extractor/cmd/fan_speed",
        "night_start": "extractor/cmd/night_start",
        "night_end": "extractor/cmd/night_end",
    }
    topic = topic_map.get(cmd)
    if not topic:
        return {"error": f"Comando desconocido: {cmd}"}

    if mqtt_client and mqtt_client.is_connected():
        mqtt_client.publish(topic, value)
        db_insert_event("command", f"{cmd} = {value}")
        return {"ok": True, "cmd": cmd, "value": value}
    return {"error": "MQTT no conectado"}

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    ws_clients.append(websocket)
    try:
        # Enviar estado actual inmediatamente
        if latest_sensors or latest_state:
            await websocket.send_text(
                json.dumps({**latest_sensors, **latest_state, "ts": datetime.now().isoformat()})
            )
        while True:
            data = await websocket.receive_text()
            try:
                cmd = json.loads(data)
                if "cmd" in cmd and "val" in cmd:
                    send_command(cmd["cmd"], str(cmd["val"]))
            except json.JSONDecodeError:
                pass
    except WebSocketDisconnect:
        ws_clients.remove(websocket)
