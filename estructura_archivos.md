🗂️ ESTRUCTURA Y CONTEXTO DE ARCHIVOS DEL PROYECTOAdemás del gemini_cli_prompt.md (que es la biblia del firmware), este proyecto se organiza con la siguiente estructura de archivos. Cuando te pida generar o modificar uno de estos archivos, sigue su propósito:ExtractorInteligente_ESP32/
├── 📁 src/
│   └── 📁 SmartExtractor/
│       └── 📄 SmartExtractor.ino  # (El código C++/Arduino principal)
├── 📁 docs/
│   ├── 📄 SCHEMATICS.md      # (Esquemas de conexión y hardware)
│   ├── 📄 CALIBRATION.md     # (Guía de calibración de sensores)
│   └── 📄 USAGE.md           # (Guía de usuario final)
│   └── 📄 prompt_estructura.md # (Este mismo archivo)
├── 📄 gemini_cli_prompt.md   # (La biblia del proyecto)
├── 📄 platformio.ini         # (Configuración de PlatformIO, librerías, etc.)
└── 📄 README.md              # (La portada pública del proyecto en GitHub)
Propósito de cada archivo:gemini_cli_prompt.md: Es la "Única Fuente de Verdad" para los requisitos y la lógica del firmware.src/SmartExtractor/SmartExtractor.ino: Es donde reside todo el código C++/Arduino que escribiremos.docs/SCHEMATICS.md: Contiene las descripciones de los pines, el circuito de potencia (MOSFET, relé) y las conexiones.docs/CALIBRATION.md: Explica el proceso paso a paso para calibrar el MQ135 y ajustar los umbrales de humedad/temperatura.docs/USAGE.md: Es el manual de usuario final. Explica cómo usar el sistema (Modo Auto, Modo Manual, etc.).README.md: Es la carta de presentación del proyecto en GitHub. Debe ser un resumen atractivo.