# AstroFlatPanel — Wiring & Pin Reference

## Platform

**Arduino Nano** (ATmega328P) — USB serial communication at 9600 bps.

---

## Pin summary

| Arduino Pin | Signal      | Component               | Notes                              |
|:-----------:|-------------|-------------------------|------------------------------------|
| D2          | CLK         | Rotary encoder          | INPUT_PULLUP, polling-based detection |
| D3          | DT          | Rotary encoder          | INPUT_PULLUP                       |
| D4          | SW (button) | Rotary encoder          | INPUT_PULLUP, software debounce    |
| D9          | PWM Signal  | XY-MOS module (LED strip) | PWM ~62.5 kHz via Timer1 (OC1A) |
| A4 (SDA)    | SDA         | OLED SSD1306 (I2C)      | I2C bus                            |
| A5 (SCL)    | SCL         | OLED SSD1306 (I2C)      | I2C bus                            |
| 3V3         | VCC         | OLED + Encoder          | 3.3 V power supply                 |
| GND         | GND         | All components          | Common ground                      |
| RST         | RST         | ASCOM capacitor         | 10 µF to GND via jumper — **remove jumper to program** |

---

## Components & detailed wiring

### 1. LED Panel — XY-MOS module

The XY-MOS is a complete MOSFET driver module chosen over a bare MOSFET for its integrated gate driver circuit. This ensures fast, clean switching transitions at the high PWM frequency (~62.5 kHz, Timer1) used by the firmware, minimising heat dissipation and delivering smooth, linear dimming across the full 0–100 % range without external gate resistors or protection components.

| XY-MOS Pin | Connection              |
|:----------:|-------------------------|
| SIG (PWM)  | Arduino D9 (PWM)        |
| VCC        | LED strip power supply (+) |
| GND        | Common GND              |
| OUT+       | LED strip (+)           |
| OUT−       | LED strip (−) / GND     |

> Brightness value (0–255) is stored in EEPROM and restored on boot.

---

### 2. OLED Display SSD1306 — 128×32 px (I2C)

Shows brightness percentage, panel state (ON/OFF), and the active encoder step.

| OLED Pin | Signal | Arduino Connection |
|:--------:|--------|--------------------|
| 1 — GND  | GND    | GND                |
| 2 — VCC  | +3.3V  | 3V3                |
| 3 — SCL  | SCL    | A5                 |
| 4 — SDA  | SDA    | A4                 |

---

### 3. Rotary encoder with push button (EC11 or compatible)

Controls brightness manually. The button has two behaviours:

- **Short press** (< 1 s): toggles the increment step between **1 %** and **10 %**.
- **Long press** (≥ 1 s): turns the panel on or off.

| Encoder Pin | Signal | Arduino Connection |
|:-----------:|--------|--------------------|
| 1 — GND     | GND    | GND                |
| 2 — VCC     | +3.3V  | 3V3                |
| 3 — SW      | Button | D4                 |
| 4 — DT      | DT     | D3                 |
| 5 — CLK     | CLK    | D2                 |

---

## Serial protocol (Alnitak Flat Panel Emulator)

Communication at **9600 bps**. Allows control from astrophotography software (N.I.N.A., Sequence Generator Pro, etc.).

| Command | Description              | Response         |
|---------|--------------------------|------------------|
| `>P`    | Ping / identify          | `*P19000`        |
| `>S`    | Current status           | `*S190{L}0`      |
| `>L`    | Turn panel on            | `*L19000`        |
| `>D`    | Turn panel off           | `*D19000`        |
| `>B{n}` | Set brightness (0–255)   | `*B19{nnn}`      |
| `>J`    | Query current brightness | `*J19{nnn}`      |
| `>V`    | Firmware version         | `*V19001`        |

`{L}` = 1 if on, 0 if off. Device ID = **19** (Flat Man).

---

### 4. ASCOM capacitor (RST ↔ GND)

A **10 µF capacitor** is connected between the RST pin and GND through a **removable jumper**. When the ASCOM driver (or compatible software such as N.I.N.A. or Sequence Generator Pro) opens the serial port, it asserts a DTR pulse that would otherwise reset the Arduino and drop the connection. The capacitor dampens this pulse, keeping the board running and allowing stable communication with the driver.

> **Important:** programming the Arduino also relies on a DTR pulse to trigger the bootloader. Because the capacitor suppresses that pulse, **the jumper must be removed before uploading firmware** and reconnected afterwards for ASCOM to work correctly.

| Connection | Detail |
|------------|--------|
| RST — C+ — jumper — C− — GND | 10 µF electrolytic, observe polarity (+ to RST) |

---

## Implementation notes

- Timer1 is reconfigured in **Fast PWM 8-bit, no prescaler** mode to run at ~62.5 kHz on pin D9. This is incompatible with `analogWrite()` on that pin after `Alnitak::begin()` is called.
- All three encoder pins use `INPUT_PULLUP`; no external pull-up resistors are needed.
- The OLED display is only refreshed when state changes (brightness, on/off, or increment step), reducing CPU load.
