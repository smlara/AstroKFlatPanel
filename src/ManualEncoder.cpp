#include "ManualEncoder.h"

ManualEncoder::ManualEncoder(uint8_t clkPin, uint8_t dtPin, uint8_t swPin)
    : _clk(clkPin), _dt(dtPin), _sw(swPin), _longPressRequested(false), _shortPressRequested(false),
      _incrementStep(10), _lastClkState(HIGH), _buttonPressStartTime(0), _isPressed(false), _actionTaken(false),
      _lastButtonChangeTime(0)
{
}

void ManualEncoder::begin()
{
    pinMode(_clk, INPUT_PULLUP);
    pinMode(_dt, INPUT_PULLUP);
    pinMode(_sw, INPUT_PULLUP);

    _lastClkState = digitalRead(_clk);
}

bool ManualEncoder::checkLongPress() {
    if (_longPressRequested) {
        _longPressRequested = false;
        return true;
    }
    return false;
}

bool ManualEncoder::checkShortPress() {
    if (_shortPressRequested) {
        _shortPressRequested = false;
        return true;
    }
    return false;
}

int ManualEncoder::getIncrement() {
    return _incrementStep;
}

int ManualEncoder::getDelta()
{
    // --- Button polling ---
    bool currentButtonState = digitalRead(_sw);

    if (currentButtonState == LOW && !_isPressed) {
        // Falling edge (press starts)
        if (millis() - _lastButtonChangeTime > 50) { // Initial debounce
            _isPressed = true;
            _buttonPressStartTime = millis();
            _actionTaken = false;
        }
    }
    else if (currentButtonState == LOW && _isPressed) {
        // Held down: check whether we've crossed 1 second for a long press (power on/off).
        if (!_actionTaken && (millis() - _buttonPressStartTime > 1000)) {
            _actionTaken = true;
            _longPressRequested = true;
        }
    }
    else if (currentButtonState == HIGH && _isPressed) {
        // Rising edge (released)
        _isPressed = false;
        _lastButtonChangeTime = millis();

        // Released before 1 second: short press -> toggle the increment step.
        if (!_actionTaken) {
            if (millis() - _buttonPressStartTime > 50) { // Minimum debounce
                _shortPressRequested = true;
                // Toggle the increment between 1 and 10 internally.
                if (_incrementStep == 1) {
                    _incrementStep = 10;
                } else {
                    _incrementStep = 1;
                }
            }
        }
        _actionTaken = false;
    }

    // --- Encoder polling ---
    int clkState = digitalRead(_clk);
    int dtState = digitalRead(_dt);
    int delta = 0;

    // Detect a valid CLK edge (state change).
    if (clkState != _lastClkState) {
        // Only evaluate direction when CLK changes and is different from the
        // idle state (LOW). This avoids double-counting each detent.
        if (clkState == LOW) {
            if (dtState != clkState) {
                delta = _incrementStep;
            } else {
                delta = -_incrementStep;
            }
        }
    }

    _lastClkState = clkState;

    return delta;
}
