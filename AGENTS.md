# DMX_45CH — STM32F302RB DMX receiver (54ch + 8ch AC dimmer + WS2811)

Nested layout: repo root / `DMX_45CH/` (CubeIDE project: `Core/`, `Application/`, `Drivers/`, `Libraries/`) / `Doc/`.
Bare-metal super-loop, no RTOS, no tests, no CI. `README.md` is a stub; `Doc/uart_commands.md` and `Doc/bootloader.md` are design-only (NOT implemented).

## Build (all 4 configs after any shared-code change)

- `./build.sh [Debug|Release|ConResistencia|SinResistencia]` (default `Debug`). Headless CubeIDE; needs `stm32cubeide_2.2.0` at `~/st/` (`CUBEIDE_DIR` override).
- Build outputs (`Debug/`, `Release/`, `*/Resistencia/`) are gitignored — never stage them.
- `ConResistencia` = `HW_CON_RESISTENCIA` (PA2 LOW); `SinResistencia` = `HW_SIN_RESISTENCIA` (PA2 HIGH). `OfficialRelease/` is a legacy folder, not a build config.

## CubeMX regeneration rules (hard-earned)

- Custom code lives ONLY in `USER CODE` sections — `HAL_UART_RxCplt/LINBreakCallback`, TIM6 reprogramming, PA2 block, IWDG call/refresh/FREEZE. Anything outside is lost on regenerate.
- After regenerating: normalize line endings to LF (`sed -i 's/\r$//'`) — Cube writes CRLF, repo is LF; otherwise every file shows as rewritten.
- `git diff --ignore-cr-at-eol --stat` must show only real changes. Revert pure-CRLF churn (`Drivers/CMSIS/*`, `*_msp.c`, `it.c/h`, `main.h`, `hal_def.h`).
- NEVER accept a regenerated `Drivers/.../stm32f3xx_hal_uart.c` blindly: newer Cube HAL dropped the `LBDF → HAL_UART_LINBreakCallback` dispatch in `HAL_UART_IRQHandler`, which kills DMX break detection. Revert it (`git checkout -- ...`) — verified regression.
- `.mxproject` churn (`\`→`/`, added driver entries) is expected — keep it.

## Watchdog (ported from EdisonPro, identical strategy)

- IWDG `Prescaler 4 / Reload 4095 / Window 4095` (~410ms nominal, 273–546ms LSI spread), window disabled, LSI on, `__HAL_DBGMCU_FREEZE_IWDG`.
- `HAL_IWDG_Refresh` goes at the TOP of `while(1)` — never in the TIM6 ISR (its 763ms DMX-loss mode would cause spurious resets).
- `MX_IWDG_Init()` AFTER `app_DMXCore_Init()` (first boot does blocking `Flash_Erase` on invalid EEPROM).

## Architecture notes (not obvious from filenames)

- TIM6 is dual-purpose, reprogrammed live from ISRs: 1ms sequence tick (`PSC 480/ARR 100`) vs 763ms DMX-loss detector (`PSC 3662/ARR 10000`, set in `RxCpltCallback`). 763ms of TIM6 silence is normal, not a hang.
- DMX RX: USART1 250kbps LIN-break (`app_LIN.c` arms `LBDIE`); `LINBreakCallback` + `Receive_IT` sized by `DMX_StartAddress`. Safe-state on reset comes from `MX_GPIO_Init` (LAMPs `SET`=OFF open-drain, PA2 per variant).
- `Application/`: `app_DMXCore` (54ch window over 513 raw bytes, start addr persisted at `0x08007000` via `lib_Flash`), `app_Sequence` (AC cascade + LED rainbow, clamped to `SEQUENCE_DMX_MAX_VALUE`), `app_ACControl` (8ch phase-cut on TIM2 + zero-cross EXTI), `app_WS2811` (SPI1+DMA circular, no double-buffer — tearing possible).

## Conventions

- Code comments in English. Commit messages: English imperative (`Add ...`), `develop` is integration, `feature/*` branches.
