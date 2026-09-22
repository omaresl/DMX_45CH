# Variantes Con/Sin Resistencia (gate drive de LEDs)

## Causa

El riel `LED_CONTROL_VCC` alimenta los resistores de gate de los 45 MOSFET
AO3400 con **12 V**. En reposo, aun con los WS2811 en alta impedancia, el
voltaje es suficiente para drenar corriente a los LEDs (brillo fantasma).

## Arreglo temporal (HW actual) = variante Con Resistencia

- Q7 apagado (sin 12 V a los gates).
- Resistor físico temporal de una terminal de **3.3 V** hacia los resistores
  de gate. Valor probado: **48 Ω**.
- Firmware: `HW_CON_RESISTENCIA`, `PA2 (LED_CONTROL_SIGNAL) = LOW`.
  Es el default desde `5c1d586`.

## Solución final (pendiente en esquemático)

Actualizar el esquemático para alimentar los gates con **3.3 V** en lugar de
12 V y retirar el resistor temporal. Sin cambios de firmware: `PA2 LOW`
sigue siendo el estado correcto.

## Variante Sin Resistencia (diseño original)

- Gates a 12 V (Q7 activo), sin resistor temporal.
- Firmware: `HW_SIN_RESISTENCIA`, `PA2 = HIGH`.
- Solo válida con el esquemático corregido o aceptando el brillo fantasma.

## Dirección final: variante única

Se eliminan las variantes; queda solo el diseño a 3.3 V (comportamiento
`PA2 LOW`). Pendiente: retirar `HW_SIN_RESISTENCIA` del firmware (ifdef en
`MX_GPIO_Init`, config `SinResistencia`) una vez validado el esquemático.
