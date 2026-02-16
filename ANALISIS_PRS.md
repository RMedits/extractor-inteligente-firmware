# Análisis de Pull Requests - extractor-inteligente-firmware

## Resumen

- **Total PRs abiertos**: 30
- **PRs obsoletos (Bolt)**: 29 (del #41 al #70, excepto #43)
- **PR obsoleto (Codex)**: 1 (#43 - diagrama MOSFET simple, reemplazado por docs/MONTAJE_SSR_MOSFET.md)

**Optimizaciones válidas extraídas:**
- ✅ **Display throttling (5Hz/200ms)**: Aplicado en commit e1ec384
- ✅ **Sensor throttling (2s)**: Ya existía desde antes

**Conclusión:** TODOS los 30 PRs pueden ser cerrados de forma segura.

---

## PRs Obsoletos de Bolt (Cerrar todos)

Todos estos PRs fueron creados por **Bolt.new** (IA automática) entre **18 enero y 15 febrero 2026**, con cambios duplicados sobre:
- Throttling de sensores (2000ms)
- Throttling de pantalla OLED (5Hz)
- Desacoplamiento de lecturas I2C

**Razón para cerrar:**
1. El proyecto fue **completamente reestructurado el 12 de febrero** (monorepo + MQTT + app web)
2. El firmware actual es **incompatible** con estos cambios (arquitectura diferente)
3. Los cambios propuestos **ya están implementados** o son irrelevantes en la versión actual
4. Son **29 PRs duplicados** haciendo básicamente lo mismo

### Lista completa de PRs Bolt a cerrar:

| PR | Título | Fecha | Acción |
|---|---|---|---|
| #70 | ⚡ Bolt: Throttle display updates to 5Hz | 15 Feb | **CERRAR** |
| #69 | ⚡ Bolt: Throttling de pantalla a 5Hz | 14 Feb | **CERRAR** |
| #68 | ⚡ Bolt: throttle display updates to 5Hz | 13 Feb | **CERRAR** |
| #67 | ⚡ Bolt: Throttle OLED updates to 5Hz | 12 Feb | **CERRAR** |
| #66 | ⚡ Bolt: Desacoplar lectura de sensores del bucle UI | 11 Feb | **CERRAR** |
| #65 | ⚡ Bolt: Desacoplar lecturas de sensores a bucle de 2000ms | 10 Feb | **CERRAR** |
| #64 | ⚡ Bolt: Decouple sensor readings to 2000ms loop | 9 Feb | **CERRAR** |
| #63 | ⚡ Bolt: Throttle sensor readings to improve UI responsiveness | 8 Feb | **CERRAR** |
| #62 | ⚡ Bolt: Decouple sensor readings from UI loop | 7 Feb | **CERRAR** |
| #61 | ⚡ Bolt: Desacoplar lecturas de sensores del bucle UI | 6 Feb | **CERRAR** |
| #60 | ⚡ Bolt: Optimización de lectura de sensores | 5 Feb | **CERRAR** |
| #59 | ⚡ Bolt: Throttle sensor polling to 2000ms | 4 Feb | **CERRAR** |
| #58 | ⚡ Bolt: Desacoplar lecturas de sensores del bucle UI | 3 Feb | **CERRAR** |
| #57 | ⚡ Bolt: Decouple sensor readings from UI loop | 2 Feb | **CERRAR** |
| #56 | ⚡ Bolt: Optimización de lectura de sensores (2000ms) | 1 Feb | **CERRAR** |
| #55 | ⚡ Bolt: Decouple sensor reads from UI loop | 31 Ene | **CERRAR** |
| #54 | ⚡ Bolt: Throttle sensors & boost I2C | 30 Ene | **CERRAR** |
| #53 | ⚡ Bolt: Optimización lecturas sensores I2C a 2000ms | 29 Ene | **CERRAR** |
| #52 | ⚡ Bolt: Throttle sensor polling to 2000ms | 28 Ene | **CERRAR** |
| #51 | ⚡ Bolt: throttle sensor readings to 2000ms | 27 Ene | **CERRAR** |
| #50 | ⚡ Bolt: Decouple sensor I/O from UI loop | 26 Ene | **CERRAR** |
| #49 | ⚡ Bolt: Decouple sensor reading from UI loop (200ms -> 2000ms) | 25 Ene | **CERRAR** |
| #48 | ⚡ Bolt: Optimize Sensor Reading Frequency | 24 Ene | **CERRAR** |
| #47 | ⚡ Bolt: Throttle sensor readings to 2000ms | 23 Ene | **CERRAR** |
| #46 | ⚡ Bolt: Throttling de lectura de sensores a 2000ms | 22 Ene | **CERRAR** |
| #45 | ⚡ Bolt: Throttle sensor reads to 2s | 21 Ene | **CERRAR** |
| #44 | ⚡ Bolt: Limitar lectura de sensores a 2s | 20 Ene | **CERRAR** |
| #42 | ⚡ Bolt: Limitar lecturas de sensores a 2s para evitar bloqueo de UI | 19 Ene | **CERRAR** |
| #41 | ⚡ Bolt: Throttle sensor reads to 2s | 18 Ene | **CERRAR** |

---

## PR a Revisar Manualmente

### PR #43: Add MOSFET wiring visual diagram
- **Branch**: `codex/instrucciones-para-montar-el-mosfet`
- **Creado**: 19 enero 2026
- **Descripción**: Añade diagrama visual de cableado del MOSFET

**Análisis:**
- ✅ Es legítimo (creado por Codex, no Bolt)
- ⚠️ **Probablemente obsoleto**: acabamos de crear `docs/MONTAJE_SSR_MOSFET.md` que es mucho más completo
- 🔍 **Acción recomendada**: Revisar si tiene algún diagrama o contenido útil que no esté en la nueva guía. Si no, cerrar también.

---

## Script para Cerrar PRs en Masa

```bash
#!/bin/bash
# Cerrar todos los PRs obsoletos de Bolt

REPO="RMedits/extractor-inteligente-firmware"
PRS_TO_CLOSE="41 42 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70"
COMMENT="Este PR ya no es relevante tras la reestructuración completa del proyecto (monorepo + MQTT + app web) del 12 de febrero. Los cambios propuestos ya están implementados o son incompatibles con la nueva arquitectura. Cerrando para limpiar el historial de PRs."

for PR in $PRS_TO_CLOSE; do
  echo "Cerrando PR #$PR..."
  gh pr close $PR -R $REPO -c "$COMMENT"
  sleep 1
done

echo "✅ PRs cerrados. Revisar manualmente PR #43."
```

---

## Recomendación Final

1. **Cerrar PRs #41-#70** (excepto #43) con el script anterior
2. **Revisar PR #43** para ver si tiene contenido útil
3. **Eliminar ramas remotas** asociadas a los PRs cerrados para limpiar el repo
