#include <Arduino.h>
#include "Alnitak.h"
#include "ManualEncoder.h"
#include "ExternalDisplay.h"

/*
 * ==========================================
 * WIRING SUMMARY (Arduino pins)
 * ==========================================
 * - LED PANEL (MOSFET):    Pin D9 (PWM)
 *
 * - OLED DISPLAY (I2C):
 *   - GND(1):               Pin GND
 *   - VCC(2):               Pin 3V3
 *   - SCL(3):               Pin A5
 *   - SDA(4):               Pin A4
 *
 * - ROTARY ENCODER:
 *   - GND(1):               Pin GND
 *   - VCC(2):               Pin 3V3
 *   - SW(3) (Button):       Pin D4
 *   - DT(4):                Pin D3
 *   - CLK(5):               Pin D2
 * ==========================================
 */
/*
 * ==========================================
 * MOSFET HW-517 pinout
 * ==========================================
 *   - PWM pin: D9 (PWM)
 *
 */

Alnitak alnitak(MOSFET_PIN); // Must be D9 to drive the PWM properly
ManualEncoder encoder;       // Uses the default pins defined in the .h (2, 3, 4)
ExternalDisplay display;

// Variables to control when to refresh the display without blocking
int lastBrightness = -1;
bool lastIsOn = false;
int lastIncrement = -1;
unsigned long lastDisplayUpdate = 0;

void setup() {
    Serial.begin(9600);

    // Initialise components
    alnitak.begin();

    // Force initial brightness to 0 and panel on
    alnitak.setBrightness(0);
    alnitak.turnOn();

    encoder.begin();
    display.begin();

    // Initial welcome message on the display (5 seconds with version info)
    String version = "";
#ifdef FW_VERSION
    version = FW_VERSION;
#endif
    display.showWelcome("ASTROKFLAT", version);
    delay(5000);

    // Force initial refresh
    display.update(alnitak.getBrightness(), encoder.getIncrement(), alnitak.isOn());
}

void loop() {
    // 1. Serial communication (Alnitak protocol)
    alnitak.process();

    // 2. Encoder rotation
    int delta = encoder.getDelta();

    if (delta != 0) {
        // Current percentage (0 to 100)
        int currentPct = round((alnitak.getBrightness() / 255.0) * 100.0);

        // The encoder delta is treated as a percentage increment (1% or 10%)
        int newPct = currentPct + delta;

        // Clamp between 0 and 100
        if (newPct < 0) newPct = 0;
        if (newPct > 100) newPct = 100;

        // Convert back to 0-255 range for the driver
        int newBrightness = round((newPct / 100.0) * 255.0);

        alnitak.setBrightness(newBrightness);
    }

    // 3. Encoder button
    if (encoder.checkLongPress()) {
        if (alnitak.isOn()) {
            alnitak.turnOff();
        } else {
            alnitak.turnOn();
        }
    } else if (encoder.checkShortPress()) {
        // ManualEncoder already toggled the increment internally (1 <-> 10) on the short click.
    }

    // 4. Refresh the display only when state has changed
    int currentBrightness = alnitak.getBrightness();
    bool currentIsOn = alnitak.isOn();
    int currentIncrement = encoder.getIncrement();

    if (currentBrightness != lastBrightness || currentIsOn != lastIsOn || currentIncrement != lastIncrement) {
        // Always pass the real brightness value; currentIsOn decides whether OFF or the percentage is shown.
        display.update(currentBrightness, currentIncrement, currentIsOn);

        lastBrightness = currentBrightness;
        lastIsOn = currentIsOn;
        lastIncrement = currentIncrement;
    }
}
