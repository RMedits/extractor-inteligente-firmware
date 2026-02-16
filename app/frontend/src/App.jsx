import { useState, useEffect, useRef, useCallback } from 'react'
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer, AreaChart, Area } from 'recharts'

const API = window.location.hostname === 'localhost' ? '' : `http://${window.location.hostname}:8000`
const WS_URL = `ws://${window.location.hostname}:${window.location.port || 8000}/ws`

const MODES = {
    IDLE: { label: 'Auto', icon: '🔄', color: '#69f0ae' },
    SHOWER: { label: 'Ducha', icon: '🚿', color: '#4d8df7' },
    ODOR: { label: 'Olores', icon: '💨', color: '#ff8a50' },
    NIGHT: { label: 'Noche', icon: '🌙', color: '#b388ff' },
    CONFIG: { label: 'Config', icon: '⚙️', color: '#ffd740' },
    MANUAL_LIM: { label: 'Manual', icon: '⏱️', color: '#ffd740' },
    MANUAL_INF: { label: 'Manual ∞', icon: '🔧', color: '#ffd740' },
    DIAG: { label: 'Diagnóstico', icon: '🔍', color: '#00d4aa' },
    ERROR: { label: 'Error', icon: '❌', color: '#ff5252' },
}

function SensorCard({ icon, label, value, unit }) {
    return (
        <div className="card">
            <div className="card-icon">{icon}</div>
            <div className="card-label">{label}</div>
            <div className="card-value">
                {typeof value === 'number' ? (Number.isInteger(value) ? value : value.toFixed(1)) : value ?? '--'}
                <span className="card-unit">{unit}</span>
            </div>
        </div>
    )
}

function EventRow({ event }) {
    const time = event.timestamp ? new Date(event.timestamp).toLocaleTimeString('es-ES', { hour: '2-digit', minute: '2-digit' }) : '--'
    return (
        <div className="event-row">
            <span className="event-time">{time}</span>
            <span className={`event-type event-type-${event.type}`}>{event.type}</span>
            <span className="event-msg">{event.message}</span>
        </div>
    )
}

export default function App() {
    const [theme, setTheme] = useState(() => localStorage.getItem('theme') || 'dark')
    const [tab, setTab] = useState('dashboard')
    const [data, setData] = useState({})
    const [connected, setConnected] = useState(false)
    const [history, setHistory] = useState([])
    const [events, setEvents] = useState([])
    const [stats, setStats] = useState(null)
    const [historyFilter, setHistoryFilter] = useState(24)
    const [fanOverride, setFanOverride] = useState(null)
    const wsRef = useRef(null)
    const reconnectTimer = useRef(null)

    // Theme
    useEffect(() => {
        document.documentElement.setAttribute('data-theme', theme)
        localStorage.setItem('theme', theme)
    }, [theme])

    // WebSocket
    const connectWs = useCallback(() => {
        if (wsRef.current?.readyState === WebSocket.OPEN) return

        const ws = new WebSocket(WS_URL)
        ws.onopen = () => { setConnected(true); clearTimeout(reconnectTimer.current) }
        ws.onclose = () => {
            setConnected(false)
            reconnectTimer.current = setTimeout(connectWs, 3000)
        }
        ws.onerror = () => ws.close()
        ws.onmessage = (e) => {
            try {
                const d = JSON.parse(e.data)
                setData(d)
                if (fanOverride === null && d.fan_speed !== undefined) {
                    setFanOverride(d.fan_speed)
                }
            } catch { }
        }
        wsRef.current = ws
    }, [fanOverride])

    useEffect(() => {
        connectWs()
        return () => { wsRef.current?.close(); clearTimeout(reconnectTimer.current) }
    }, [])

    // Fetch data for non-dashboard tabs
    useEffect(() => {
        if (tab === 'history') {
            fetch(`${API}/api/history?hours=${historyFilter}&limit=300`)
                .then(r => r.json())
                .then(d => setHistory(d.reverse()))
                .catch(() => { })
        }
        if (tab === 'log') {
            fetch(`${API}/api/events?limit=50`)
                .then(r => r.json())
                .then(setEvents)
                .catch(() => { })
        }
        if (tab === 'stats') {
            fetch(`${API}/api/stats`)
                .then(r => r.json())
                .then(setStats)
                .catch(() => { })
        }
    }, [tab, historyFilter])

    const sendCommand = (cmd, value) => {
        fetch(`${API}/api/command/${cmd}?value=${value}`, { method: 'POST' }).catch(() => { })
    }

    const mode = data.mode || 'IDLE'
    const modeInfo = MODES[mode] || MODES.IDLE

    const tabs = [
        { id: 'dashboard', label: '📊 Dashboard' },
        { id: 'history', label: '📈 Historial' },
        { id: 'stats', label: '📉 Stats' },
        { id: 'log', label: '📋 Log' },
        { id: 'config', label: '⚙️ Config' },
    ]

    return (
        <div className="app">
            {/* HEADER */}
            <header className="header">
                <div className="header-left">
                    <span className="header-logo">🌀</span>
                    <div>
                        <div className="header-title">Extractor Inteligente</div>
                        <div className="header-subtitle">Sistema de Ventilación IoT</div>
                    </div>
                </div>
                <div className="header-controls">
                    <div className={`status-badge ${connected ? 'status-online' : 'status-offline'}`}>
                        <span className="status-dot" />
                        {connected ? 'ONLINE' : 'OFFLINE'}
                    </div>
                    <button className="theme-toggle" onClick={() => setTheme(t => t === 'dark' ? 'light' : 'dark')}>
                        {theme === 'dark' ? '☀️' : '🌙'}
                    </button>
                </div>
            </header>

            {/* TABS */}
            <nav className="tabs">
                {tabs.map(t => (
                    <button key={t.id} className={`tab ${tab === t.id ? 'active' : ''}`} onClick={() => setTab(t.id)}>
                        {t.label}
                    </button>
                ))}
            </nav>

            {/* ===== DASHBOARD TAB ===== */}
            {tab === 'dashboard' && (
                <>
                    {/* Status Bar */}
                    <div className="status-bar">
                        <div className="status-card">
                            <div className="status-icon-box bg-blue">
                                <span>{modeInfo.icon}</span>
                            </div>
                            <div className="status-info">
                                <h3 style={{ color: modeInfo.color }}>{modeInfo.label}</h3>
                                <p>Modo actual</p>
                            </div>
                        </div>
                        <div className="status-card">
                            <div className={`status-icon-box ${data.occupied ? 'bg-red' : 'bg-green'}`}>
                                <span>{data.occupied ? '🔴' : '🟢'}</span>
                            </div>
                            <div className="status-info">
                                <h3>{data.occupied ? 'Ocupado' : 'Libre'}</h3>
                                <p>Baño (Radar)</p>
                            </div>
                        </div>
                        <div className="status-card">
                            <div className="status-icon-box bg-orange">
                                <span>🕐</span>
                            </div>
                            <div className="status-info">
                                <h3>{data.hour !== undefined ? `${data.hour}:00` : '--'}</h3>
                                <p>Hora ESP32</p>
                            </div>
                        </div>
                    </div>

                    {/* Sensor Cards */}
                    <div className="grid">
                        <SensorCard icon="🌡️" label="Temperatura" value={data.temp} unit="°C" />
                        <SensorCard icon="💧" label="Humedad" value={data.hum} unit="%" />
                        <SensorCard icon="🌬️" label="Calidad Aire" value={data.aqi} unit="" />
                        <SensorCard icon="📊" label="Presión" value={data.pressure} unit="hPa" />
                        <SensorCard icon="🌫️" label="Punto Rocío" value={data.dew || data.dewPoint} unit="°C" />
                        <SensorCard icon="🔄" label="RPM" value={data.rpm} unit="" />
                    </div>

                    {/* Mode Selector */}
                    <div className="mode-section">
                        <div className="mode-title">Control de Modo</div>
                        <div className="mode-buttons">
                            {['IDLE', 'MANUAL_INF', 'NIGHT', 'DIAG'].map(m => (
                                <button
                                    key={m}
                                    className={`mode-btn ${mode === m ? 'active' : ''}`}
                                    onClick={() => sendCommand('mode', m)}
                                >
                                    {MODES[m].icon} {MODES[m].label}
                                </button>
                            ))}
                        </div>
                    </div>

                    {/* Fan Speed */}
                    <div className="slider-section">
                        <div className="slider-header">
                            <div className="mode-title" style={{ margin: 0 }}>Velocidad Ventilador</div>
                            <div className="slider-value">{fanOverride ?? data.fan_speed ?? data.fan ?? 0}%</div>
                        </div>
                        <input
                            type="range"
                            min="0"
                            max="90"
                            step="5"
                            value={fanOverride ?? data.fan_speed ?? data.fan ?? 0}
                            onChange={(e) => {
                                const v = Number(e.target.value)
                                setFanOverride(v)
                                sendCommand('fan_speed', v)
                            }}
                        />
                    </div>
                </>
            )}

            {/* ===== HISTORY TAB ===== */}
            {tab === 'history' && (
                <>
                    <div className="chart-section">
                        <div className="chart-header">
                            <div className="chart-title">Temperatura y Humedad</div>
                            <div className="chart-filters">
                                {[1, 6, 24, 168].map(h => (
                                    <button
                                        key={h}
                                        className={`filter-btn ${historyFilter === h ? 'active' : ''}`}
                                        onClick={() => setHistoryFilter(h)}
                                    >
                                        {h < 24 ? `${h}h` : h === 24 ? '24h' : '7d'}
                                    </button>
                                ))}
                            </div>
                        </div>
                        <ResponsiveContainer width="100%" height={300}>
                            <AreaChart data={history}>
                                <defs>
                                    <linearGradient id="gradTemp" x1="0" y1="0" x2="0" y2="1">
                                        <stop offset="0%" stopColor="#ff8a50" stopOpacity={0.3} />
                                        <stop offset="100%" stopColor="#ff8a50" stopOpacity={0} />
                                    </linearGradient>
                                    <linearGradient id="gradHum" x1="0" y1="0" x2="0" y2="1">
                                        <stop offset="0%" stopColor="#4d8df7" stopOpacity={0.3} />
                                        <stop offset="100%" stopColor="#4d8df7" stopOpacity={0} />
                                    </linearGradient>
                                </defs>
                                <CartesianGrid strokeDasharray="3 3" stroke="rgba(255,255,255,0.06)" />
                                <XAxis dataKey="timestamp" tick={{ fontSize: 10, fill: '#636980' }}
                                    tickFormatter={v => v ? new Date(v).toLocaleTimeString('es-ES', { hour: '2-digit', minute: '2-digit' }) : ''} />
                                <YAxis tick={{ fontSize: 10, fill: '#636980' }} />
                                <Tooltip contentStyle={{ background: '#1e2130', border: '1px solid rgba(255,255,255,0.1)', borderRadius: '12px', fontSize: '12px' }} />
                                <Area type="monotone" dataKey="temp" stroke="#ff8a50" fill="url(#gradTemp)" strokeWidth={2} name="Temp °C" />
                                <Area type="monotone" dataKey="hum" stroke="#4d8df7" fill="url(#gradHum)" strokeWidth={2} name="Humedad %" />
                            </AreaChart>
                        </ResponsiveContainer>
                    </div>

                    <div className="chart-section">
                        <div className="chart-header">
                            <div className="chart-title">Calidad del Aire (AQI)</div>
                        </div>
                        <ResponsiveContainer width="100%" height={200}>
                            <LineChart data={history}>
                                <CartesianGrid strokeDasharray="3 3" stroke="rgba(255,255,255,0.06)" />
                                <XAxis dataKey="timestamp" tick={{ fontSize: 10, fill: '#636980' }}
                                    tickFormatter={v => v ? new Date(v).toLocaleTimeString('es-ES', { hour: '2-digit', minute: '2-digit' }) : ''} />
                                <YAxis tick={{ fontSize: 10, fill: '#636980' }} />
                                <Tooltip contentStyle={{ background: '#1e2130', border: '1px solid rgba(255,255,255,0.1)', borderRadius: '12px' }} />
                                <Line type="monotone" dataKey="aqi" stroke="#69f0ae" strokeWidth={2} dot={false} name="AQI" />
                            </LineChart>
                        </ResponsiveContainer>
                    </div>
                </>
            )}

            {/* ===== STATS TAB ===== */}
            {tab === 'stats' && stats && (
                <div className="stats-grid">
                    <div className="stat-card">
                        <div className="stat-label">Total Lecturas</div>
                        <div className="stat-value">{stats.total_readings?.toLocaleString() ?? 0}</div>
                        <div className="stat-sub">Desde el inicio</div>
                    </div>
                    {stats.averages_24h && (
                        <>
                            <div className="stat-card">
                                <div className="stat-label">Temp Media 24h</div>
                                <div className="stat-value" style={{ color: '#ff8a50' }}>
                                    {stats.averages_24h.avg_temp ?? '--'}<span className="card-unit">°C</span>
                                </div>
                            </div>
                            <div className="stat-card">
                                <div className="stat-label">Humedad Media 24h</div>
                                <div className="stat-value" style={{ color: '#4d8df7' }}>
                                    {stats.averages_24h.avg_hum ?? '--'}<span className="card-unit">%</span>
                                </div>
                            </div>
                            <div className="stat-card">
                                <div className="stat-label">AQI Medio 24h</div>
                                <div className="stat-value" style={{ color: '#69f0ae' }}>
                                    {stats.averages_24h.avg_aqi ?? '--'}
                                </div>
                            </div>
                        </>
                    )}
                    {stats.mode_distribution?.map(m => (
                        <div key={m.mode} className="stat-card">
                            <div className="stat-label">{MODES[m.mode]?.icon} {MODES[m.mode]?.label || m.mode}</div>
                            <div className="stat-value">{m.count}</div>
                            <div className="stat-sub">transiciones</div>
                        </div>
                    ))}
                </div>
            )}
            {tab === 'stats' && !stats && (
                <div className="stat-card"><div className="stat-label">Cargando...</div></div>
            )}

            {/* ===== LOG TAB ===== */}
            {tab === 'log' && (
                <div className="events-section">
                    <div className="events-title">Últimos Eventos</div>
                    {events.length === 0 && <p style={{ color: 'var(--text-muted)', fontSize: '13px' }}>Sin eventos registrados</p>}
                    {events.map((e, i) => <EventRow key={i} event={e} />)}
                </div>
            )}

            {/* ===== CONFIG TAB ===== */}
            {tab === 'config' && (
                <>
                    <div className="mode-section">
                        <div className="mode-title">Velocidades por Modo (solo lectura)</div>
                        <div className="stats-grid">
                            <div className="stat-card"><div className="stat-label">Auto (IDLE)</div><div className="stat-value">18%</div></div>
                            <div className="stat-card"><div className="stat-label">Ducha</div><div className="stat-value">85%</div></div>
                            <div className="stat-card"><div className="stat-label">Olores</div><div className="stat-value">60%</div></div>
                            <div className="stat-card"><div className="stat-label">Noche Max</div><div className="stat-value">40%</div></div>
                            <div className="stat-card"><div className="stat-label">Vel. Máxima</div><div className="stat-value">90%</div></div>
                        </div>
                    </div>
                    <div className="mode-section">
                        <div className="mode-title">Horario Noche</div>
                        <p style={{ color: 'var(--text-secondary)', fontSize: '14px' }}>23:00 — 07:00 (configurado en firmware)</p>
                    </div>
                    <div className="mode-section">
                        <div className="mode-title">Umbrales de Detección</div>
                        <div className="stats-grid">
                            <div className="stat-card"><div className="stat-label">Punto Rocío Ducha</div><div className="stat-value">20°C</div></div>
                            <div className="stat-card"><div className="stat-label">Histéresis</div><div className="stat-value">1.5°C</div></div>
                            <div className="stat-card"><div className="stat-label">Sensibilidad Olor</div><div className="stat-value">80%</div></div>
                            <div className="stat-card"><div className="stat-label">Purga Adicional</div><div className="stat-value">5 min</div></div>
                        </div>
                    </div>
                </>
            )}
        </div>
    )
}
