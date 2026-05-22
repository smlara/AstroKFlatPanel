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
| D9          | PWM Signal  | XY-MOS module (LED strip) | PWM ~15.6 kHz via Timer1 (OC1A) |
| A4 (SDA)    | SDA         | OLED SSD1306 (I2C)      | I2C bus                            |
| A5 (SCL)    | SCL         | OLED SSD1306 (I2C)      | I2C bus                            |
| 3V3         | VCC         | OLED + Encoder          | 3.3 V power supply                 |
| GND         | GND         | All components          | Common ground                      |
| RST         | RST         | ASCOM capacitor         | 10 µF to GND via jumper — **remove jumper to program** |

---

## Components & detailed wiring

### 1. LED Panel — XY-MOS module

The XY-MOS is a MOSFET driver module chosen over a bare MOSFET for its built-in switching circuitry. Many of these modules use an optocoupler on the input, so the firmware drives PWM at **~15.6 kHz** (Timer1, Fast PWM 10-bit, prescaler 1) — high enough to be invisible to both the eye and an astrophotography camera at any reasonable flat-frame exposure, while still slow enough to keep a cheap optocoupler in its linear range (the original 62.5 kHz overdrove it).

### Powering the LED strip from USB

The whole panel runs off the Arduino's USB connection, like every commercial astrophotography flat generator. Use a **5 V LED strip** sized so that its full-on current stays inside the USB budget (USB 2.0: 500 mA, USB 3.0: 900 mA) — for typical 60 LED/m strips that means **roughly 1 metre or less at full brightness**.

The strip's `+` is taken from the Arduino's `+5V` pin (exposed on the `VOUT` connector); the strip's `−` returns through the MOSFET module (low-side switching) to GND. No external supply or filtering is required for a flat-frame use case: at 15.6 kHz PWM the residual ripple on the 5 V rail is well below what any astrophotography exposure (≥ 0.1 s) can resolve.

> **Optional refinement.** If you do see visible flicker — typically because you've used a much longer LED strip or a longer/thinner USB cable — adding a **220–470 µF / 10 V electrolytic** in parallel with the strip's input (positive lead to `+5V`, negative to GND, mounted as close to the strip as possible) absorbs the per-cycle current pulse and stabilises the rail. Not necessary in the reference design.

| XY-MOS Pin | Connection              |
|:----------:|-------------------------|
| SIG (PWM)  | Arduino D9 (PWM)        |
| VCC        | LED strip power supply (+) |
| GND        | Common GND              |
| OUT+       | LED strip (+)           |
| OUT−       | LED strip (−) / GND     |

> Brightness always boots at 0 (panel on, light off); the previous value is not persisted.

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

- Timer1 is reconfigured in **Fast PWM 10-bit, prescaler 1** mode to run at ~15.6 kHz on pin D9 (TOP=0x03FF). The 8-bit Alnitak brightness (0–255) is scaled into the 10-bit OCR1A range, so a brightness of 255 maps to ~99.6 % duty cycle. This is incompatible with `analogWrite()` on that pin after `Alnitak::begin()` is called.
- All three encoder pins use `INPUT_PULLUP`; no external pull-up resistors are needed.
- The OLED display is only refreshed when state changes (brightness, on/off, or increment step), reducing CPU load.
