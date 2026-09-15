# Bootloader para actualizar firmware por el puerto DMX (UART)

> Nota de diseño / exploración. No implementado.

## Resumen

Es viable actualizar el SW por el mismo USART1 que usa DMX. Hay dos caminos:

1. **Bootloader propio** (flash + linker + protocolo custom).
2. **Bootloader ROM de ST** + `STM32CubeProgrammer` (sin escribir código propio).

## Contexto actual del firmware

- MCU: STM32F302RB (Cortex-M4, 128 KB flash, 32 KB RAM)
- App actual: fines en flash aprox. `0x080066F0` (text+data ~26 KB)
- EEPROM emulado del FW: página en `0x08007000` (2 KB)
- Puerto: USART1, 250 kbps (DMX), break LIN, transceiver RS-485 bidireccional (pines DE/RE)

## Opcion 1 — Bootloader propio

- Bootloader en `0x08000000` (donde arranca el MCU).
- App reubicada a `0x08008000` (o similar):
  - Ajustar linker script (`FLASH` start/length).
  - `SCB->VTOR = APP_BASE`;
  - reubicar / remapear vectores de interrupcion.
- Reusa USART1 tal cual: mismo puerto, mismo break LIN, misma linea RS-485.
- Modos de entrada a bootloader:
  - pin / boton (`BOOT0`-like),
  - o "magic frame" via un **start code** DMX vendor (por ejemplo start code > 0x00), que permite entrar en modo update sin hardware extra y sin interferir con la senal DMX cuando hay frames normales.
- Protocolo por bloques con CRC y ACK / reintentos (inspirado en XMODEM/YMODEM).
- Programacion flash usando el codigo existente `lib_Flash.c` (borra pagina + escribe word).

### Estimacion de tiempo

- ~26 KB de app a 250 kbps: aprox. 0.9 s de payload.
- Con ACKs y overhead del protocolo: algunos segundos en total.

## Opcion 2 — Bootloader ROM de ST (más simple)

- El STM32F302 trae un bootloader en ROM accesible por **USART1** con `BOOT0 = 1`.
- No se escribe codigo: se usa `STM32CubeProgrammer` (CLI) contra el puerto.
- Requisitos:
  - pin `BOOT0` accesible (jumper/switch),
  - transceiver RS-485 capaz de transmitir respuestas (pines DE/RE ya disponibles).

## Consideraciones de diseno

- **Layout de flash**: con bootloader + app reubicada, mover la zona EEPROM (`0x08007000`)
  a la parte alta o inicio del bootloader para no pisarla.
- **Seguridad**: corte de energia durante una escritura = riesgo de brick.
  Mitigaciones:
  - doble slot (A/B) e image good/recovery,
  - el bootloader garantiza que siempre se puede re-flashear.
- **Compatibilidad DMX**: DMX manda frames continuos a 250k/8N2 con start code `0x00`
  (dimmers). Los vendor codes libres (>0x00) permiten distinguir el modo bootloader
  sin romper la recepcion normal.

## Conclusión

Viable en ambas variantes. La complejidad real esta en:
- la particion de flash (linker + VTOR + EEPROM),
- y un protocolo de escritura fiable (CRC + ACK/reintento),
no en el puerto en si.