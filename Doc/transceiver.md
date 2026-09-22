# RS-485 transceiver — YD3082EESA

Half-duplex RS-485/RS-422 transceiver (SOP8, 3.0–5.5 V) used for the DMX bus.
Source: manufacturer datasheet via LCSC (C269866, 12 pages, Chinese).

## DE/RE control (matches firmware usage)

| `/RE` | `DE` | State |
|---|---|---|
| 0 | 1 | Driving bus (`DI` → A/B) |
| 0 | 0 | Receiving (A/B → `RO`) |
| 1 | 0 | Shutdown (low power, unused) |

Firmware mapping: `DE = TRX_IN_DE (PA3)`, `/RE = TRX_IN_RE (PF4)`.
Normal state is RX (`DE=0`, `RE=0`); TX asserts `DE=1` only.

## Timing constraints for `app_CMD_Respond()`

- Driver enable to valid output: **2500 ns max**. Firmware waits `DELAY_US(10)`
  after `DE=1` before the first bit (4 µs @250 kbps), margin x4 over the max.
- Driver disable: 100 ns. Receiver enable/disable: 20–50 ns (negligible).

## Fail-safe (idle bus)

Open, shorted, or all-drivers-disabled bus → `RO` reads **high**
(thresholds −50 mV / −200 mV, EIA/TIA-485 compliant). Idle bus = UART idle.

## Multidrop budget

1/8 unit-load receiver (96 kΩ) → up to **256 nodes per bus**. Covers the
multi-board discovery scenario (`CMD_DISCOVER`).

## Rate and protection

Slew-rate limited driver, error-free up to 1 Mbps — 250 kbps DMX has margin.
ESD ±15 kV HBM (±12 kV contact / ±15 kV air IEC 61000-4-2) on A/B.
