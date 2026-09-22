# Comandos por UART (protocolo binario) — Diseño

> Documento de diseño. **No implementado.**

## Objetivo

Agregar comandos de configuración/diagnóstico por el mismo puerto USART1 usado
por DMX (RS-485, 250 kbps): actualizar start address, leer ID único del chip,
ajustar límites de potencia, consultar estado, etc.

## Transporte (mismo cable DMX/RX)

- Se usa el **start code DMX** como selector:
  - `0x00` → datos de canales normales (camino DMX actual, sin cambios)
  - `0xEE` (`CMD_START_CODE`) → trama de comando (vendor start code, DMX-compliant)
- Detección: si `raw_DMX_Channels[0] == CMD_START_CODE` → procesar comando y
  **no** copiar a `DMX_Channels`; si no, camino DMX normal.
- Los comandos llegan como una trama DMX (break + start code + datos), así el
  host puede ser cualquier consola/controlador DMX o un adaptador USB-RS485.

### Formato de trama

```
0xEE (start code) | CMD (1B) | LEN (1B) | payload (LEN..) | checksum (1B)
```

- `checksum` = suma (uint8, sin acarreo) de `CMD + LEN + payload`.
- Opcional más adelante: elevar a CRC8.

## Comandos iniciales

| CMD | ID | Payload | Respuesta |
|---|---|---|---|
| `CMD_GET_INFO` | `0x01` | — | ID único 96 bits (`0x1FFFF7AC`), versión FW, modelo, start address |
| `CMD_SET_DMX_ADDR` | `0x02` | addr u16 (big-endian) | ACK/NACK |
| `CMD_SET_POWER_LIMIT` | `0x03` | limite secuencias u8, limite AC u8 | ACK/NACK |
| `CMD_GET_STATUS` | `0x04` | — | limites activos, addr, estado |

### Respuestas

- Invertir dirección del transceiver (`DE=1`, `RE=0`), configurar TX invert
  (misma técnica que el viejo `app_CMD`), `HAL_UART_Transmit` bloqueante,
  restaurar `DE/RE` y rearmar `HAL_LIN_WaitBreak_IT`.

## Límites de potencia → runtime en EEPROM

- `SEQUENCE_DMX_MAX_VALUE` (100) pasa a variable runtime `Sequence_DMX_MaxValue`.
- `ACCONTROL_VALUE_MAX` (200) pasa a variable runtime `ACControl_ValueMax`.
- EEPROM (4 palabras ya disponibles en `0x08007000`):

```
[0] = start address
[1] = limite secuencias (Sequence_DMX_MaxValue)
[2] = limite AC (ACControl_ValueMax)
[3] = reservado / checksum
```

- Backward-compat: si un slot leído es `0xFFFFFFFF` → usar default.
- `CMD_SET_DMX_ADDR`: validar rango 1..459 (512 - 54 + 1), persistir y aplicar.
- Persistencia usando `lib_Flash` existente.

## Archivos afectados

- Nuevos: `Application/app_CMD.{c,h}`
- Modificados:
  - `app_DMXCore.c/h` — setter de addr validado
  - `app_Sequence.c` — límite como variable runtime
  - `app_ACControl.c` — revisar uso de `ACCONTROL_VALUE_MAX`
  - `main.c` — dispatch del start code en el RX complejo / main loop

## Estado

- [ ] Definición del protocolo (este documento)
- [x] `CMD_GET_INFO`
- [ ] `CMD_SET_DMX_ADDR` + EEPROM
- [ ] `CMD_SET_POWER_LIMIT` + EEPROM
- [ ] `CMD_GET_STATUS`
- [x] `CMD_DISCOVER` (slotted backoff, 16 x 5 ms)
- [x] `CMD_SELECT_UID` / `CMD_DESELECT` (RAM selection, selective writes)
- [x] Respuestas por el transceiver
- [ ] Pruebas en hardware

## Selección (RAM, se pierde al reset)
- Sin selección activa (boot): escrituras en broadcast.
- `SELECT` lo ve todo el bus: activa selección global, solo el UID coincidente
  queda seleccionado (único que responde `ACK` — los demás callan para no
  colisionar). `DESELECT` limpia en todas.
- `SET_*` solo ejecuta la seleccionada cuando hay selección activa.
  Lecturas (`GET_INFO`/`STATUS`, `DISCOVER`) responden siempre; con N>1 el
  flujo es DISCOVER → SELECT → configurar.

## Visual ACK (1 s, solo LEDs)

Cada comando válido destella los 45 canales LED (`app_CmdBlink`, no
bloqueante, `DMX_Channels` intacto): `GET_INFO` cian, `DISCOVER` magenta,
`SELECT` verde (solo la coincidente), `DESELECT` azul. Tramas inválidas no
destellan. Simulación y timeline: `Doc/blink-simulation.py` (genera
`Doc/blink-simulation.svg`; `--animate` para replay en terminal).