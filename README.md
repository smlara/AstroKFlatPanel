# AstroKFlat — DIY Flat Panel for Astrophotography

[![License: GPL v3](https://img.shields.io/badge/Firmware-GPL%20v3-blue.svg)](LICENSE)
[![License: CERN OHL v2-S](https://img.shields.io/badge/Hardware-CERN%20OHL%20v2--S-orange.svg)](LICENSE-HARDWARE)
[![Platform: Arduino Nano](https://img.shields.io/badge/Platform-Arduino%20Nano-teal.svg)](https://www.arduino.cc/)
[![Protocol: Alnitak](https://img.shields.io/badge/Protocol-Alnitak%20Flat%20Panel-lightgrey.svg)]()

> **[Versión en Español](README_ES.md)**

A fully open-source DIY flat panel calibration light for astrophotography, based on an **Arduino Nano** (ATmega328P). Compatible with the **Alnitak Flat Panel Emulator** protocol, allowing seamless integration with capture software such as N.I.N.A., Sequence Generator Pro, and others.

---

## Features

- **Alnitak protocol** — recognized natively by major astrophotography software
- **Manual brightness control** via rotary encoder (1 % or 10 % steps)
- **On/Off toggle** via long press on the encoder button
- **OLED display** (128×32) showing brightness percentage, panel state, and active step
- **High-frequency PWM** (~62.5 kHz, Timer1) — eliminates flicker visible in long exposures
- **Firmware version 1.1**

---

## Hardware

| Component | Description |
|-----------|-------------|
| Arduino Nano | ATmega328P microcontroller |
| LED Strip | LED strip |
| XY-MOS module | MOSFET driver module with integrated gate driver |
| OLED SSD1306 | 128×32 px I2C display |
| Rotary encoder | EC11 or compatible, with push button |

For full wiring details see [docs/WIRING_EN.md](docs/WIRING_EN.md) and the schematic at [docs/SCH_Schematic1_2026-04-19.pdf](docs/SCH_Schematic1_2026-04-19.pdf).

---

## Bill of Materials (BOM)

| Qty | Component | Specs |
|:---:|-----------|-------|
| 1 | Arduino Nano | ATmega328P, USB-C |
| 1 | XY-MOS module | MOSFET driver with integrated gate driver |
| 1 | LED strip | 12 V (adapt to your panel size) |
| 1 | OLED display | SSD1306, 128×32 px, I2C, 3.3 V |
| 1 | Rotary encoder | EC11 or compatible, with push button |
| 1 | Electrolytic capacitor | 10 µF, for ASCOM RST line |
| 1 | Jumper / header pin | To bypass RST capacitor during programming |
| — | Wiring | Dupont cables or equivalent |

---

## Pin Assignment (Arduino Nano)

| Pin  | Signal      | Component        |
|:----:|-------------|------------------|
| D2   | CLK         | Rotary encoder   |
| D3   | DT          | Rotary encoder   |
| D4   | SW (button) | Rotary encoder   |
| D9   | PWM Signal  | XY-MOS (LED strip driver) |
| A4   | SDA         | OLED (I2C)       |
| A5   | SCL         | OLED (I2C)       |

---

## Serial Protocol — Alnitak Flat Panel Emulator

Communication at **9600 bps** over USB serial.

| Command | Description              | Response    |
|---------|--------------------------|-------------|
| `>P`    | Ping / identify          | `*P19000`   |
| `>S`    | Current status           | `*S190{L}0` |
| `>L`    | Turn panel on            | `*L19000`   |
| `>D`    | Turn panel off           | `*D19000`   |
| `>B{n}` | Set brightness (0–255)   | `*B19{nnn}` |
| `>J`    | Query brightness         | `*J19{nnn}` |
| `>V`    | Firmware version         | `*V19001`   |

Device ID = **19** (Flat Man).

---

## Building & Flashing

This project uses [PlatformIO](https://platformio.org/).

```bash
# Clone the repository
git clone https://github.com/smlara/AstroFlatPanel.git
cd AstroFlatPanel

# Build and upload (Arduino Nano new bootloader)
pio run -e nanoatmega328new -t upload

# Or for old bootloader
pio run -e nanoatmega328 -t upload
```

### Dependencies (auto-installed by PlatformIO)

- `adafruit/Adafruit SSD1306 ^2.5.9`
- `adafruit/Adafruit GFX Library ^1.11.9`
- `Wire`
- `EEPROM`

---

## Encoder Controls

| Action | Effect |
|--------|--------|
| Rotate CW | Increase brightness by current step |
| Rotate CCW | Decrease brightness by current step |
| Short press (< 1 s) | Toggle step between 1 % and 10 % |
| Long press (≥ 1 s) | Turn panel on / off |

---

## License

This project uses a **dual licensing** model:

### Non-commercial use (open source)

- **Firmware** (all source code in `src/` and `include/`): [GNU General Public License v3.0](LICENSE)
- **Hardware** (schematics, PCB designs, mechanical files in `docs/`): [CERN Open Hardware Licence v2 - Strongly Reciprocal](LICENSE-HARDWARE)

You are free to use, study, modify, and share this project for **personal, educational, or non-commercial purposes**, provided that any derivative work remains open under the same licenses.

### Commercial use

**Any commercial use — including but not limited to manufacturing, selling, or integrating this design into a product or service — is strictly prohibited without prior written permission from the author.**

If you are interested in a commercial license, please open a [commercial inquiry issue](../../issues/new?labels=commercial-inquiry&title=Commercial+License+Inquiry) in this repository.

---

## Author

**Sergio Martínez** — Astrophotography enthusiast  
Questions, contributions, and commercial inquiries: open an [issue](../../issues) or start a [discussion](../../discussions).
