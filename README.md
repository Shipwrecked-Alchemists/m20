# m20

Any resemblance to AI generated or slop code is purely coincidental.

| Component     | Status | Description     |
|---------------|--------|-----------------|
| GPS M10M      | ✔️     | Pos Speed Time  |
| LPS22HB B&T   | ✔️     | Pressure Temp   |
| Vbat          | ✔️     | Battery Voltage |
| Radio AD7012B | 🟥     |                 |
| Humidity      | ✔️     | Calibrated Hum  |
| NTC           | ✔️     | Calibrated Temp |
| IR            | 🟥     |                 |
| UART          | ✔️     |                 |


# Components

Components can be customized in `Makefile`.

- Components sizes are given as a guide only. They are approximate.
- `Printf` is not reliable, the compiler optimizes the other components as theyre values are never used.
- All builds are done with all other options enabled.

| Module | Code | Data | BSS | **Total** |
| ------ | ---- | ---- | --- | --------- |
| Humidity | 3476 | 0 | 8 | **3484** |
| VBat | 220 | 0 | 0 | **220** |
| Temp | 604 | 0 | 0 | **604** |
| Pressure | 460 | 0 | 0 | **460** |
| GPS | 1328 | 0 | 152 | **1480** |
| Printf | 12300 | 80 | 312 | **12692** |
