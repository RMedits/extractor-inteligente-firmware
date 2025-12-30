# 🛒 Lista de Materiales: Extractor #2 (Versión 24V Industrial)

Esta es la lista de compra exacta para montar la segunda unidad del Extractor Inteligente, basada en un ventilador industrial de 24V con control PWM (4 hilos) y sistema de doble alimentación.

---

## 🌪️ Sistema de Ventilación (Potencia 24V)
Estos componentes manejan la alta potencia.

- [ ] **Ventilador:** Delta Serie QFR o similar (Especificaciones: **24V DC**, PWM 4-Hilos, ~2.40A).
- [ ] **Fuente de Alimentación:** **24V DC** con capacidad mínima de **3 Amperios** (72W).
- [ ] **Protección Anti-Retorno:** 1x Diodo **1N5408** (3A, 1000V). *Se suelda en los cables del ventilador.*
- [ ] **Filtrado de Ruido (Agudos):** 1x Condensador Cerámico **100nF / 50V** (o superior).
- [ ] **Estabilización de Arranque:** 1x Condensador Electrolítico **470µF / 50V** (¡Importante que sea de 50V!).
- [ ] **Corte de Seguridad:** 1x Módulo Relé **KY-019** de 5V (El estándar azul).

---

## 🧠 Sistema de Control (Lógica 5V)
Estos componentes forman el cerebro y la interfaz.

- [ ] **Cerebro:** 1x **ESP32 DevKit V1** (Modelo de **30 Pines** WROOM-32D).
- [ ] **Base:** 1x Shield de Expansión para ESP32 de 30 Pines (Con bornes de tornillo morada/verde).
- [ ] **Alimentación Lógica:** 1x Cargador de Móvil USB (5V / 2A) + Cable Micro-USB de calidad.
- [ ] **Interfaz:**
    - 1x Pantalla OLED 1.3" (Controlador SH1106 o SSD1306, I2C).
    - 1x Módulo Encoder Rotativo KY-040 o Estardyn (con pulsador).
    - 2x Pulsadores momentáneos (para funciones BACK y PAUSA).
    - 3x LEDs (Rojo, Amarillo, Verde) de 5mm o 3mm.
    - 3x Resistencias de 220Ω o 330Ω (para los LEDs).
- [ ] **Sensores:**
    - 1x Módulo Dual **AHT20 + BMP280** (Temperatura/Humedad/Presión por I2C).
    - 1x Módulo **MQ135** (Calidad de Aire, Analógico).

---

## 🔌 Material Vario y Conexiones
- [ ] **Caja estanca/proyecto:** Tamaño suficiente para alojar la shield y el relé.
- [ ] **Cables:**
    - Cables Dupont (Hembra-Hembra y Hembra-Macho) para sensores y lógica.
    - Cable eléctrico de 18-20 AWG (grosor medio) para la línea de 24V y Ventilador.
- [ ] **Placa de Prototipos (Perfboard):** Pequeño trozo para soldar el diodo y los condensadores del ventilador de forma robusta.
- [ ] **Termorretráctil:** Para aislar empalmes.

---

## ⚠️ NOTAS CRÍTICAS DE MONTAJE (24V)

1.  **Condensador Electrolítico:** Debe ser de **50V** mínimo. Si pones uno de 16V o 25V en una línea de 24V, **explotará**.
2.  **GND Común:** Al usar dos fuentes (Cargador 5V + Fuente 24V), es **OBLIGATORIO** conectar un cable negro entre el negativo de la fuente de 24V y un pin GND del ESP32. Sin esto, el control de velocidad no funcionará.
3.  **Relé:** El cable rojo de 24V pasa por los bornes **COM** y **NO** del relé. La electrónica del relé (pines) se conecta a los 5V del ESP32.
