#ifndef MANUAL_ENCODER_H
#define MANUAL_ENCODER_H

#include <Arduino.h>

// Default pin assignment for the encoder
#define ENCODER_CLK_PIN 2
#define ENCODER_DT_PIN 3
#define ENCODER_SW_PIN 4

class ManualEncoder {
public:
    ManualEncoder(uint8_t clkPin = ENCODER_CLK_PIN, uint8_t dtPin = ENCODER_DT_PIN, uint8_t swPin = ENCODER_SW_PIN);

    void begin();

    // Returns the encoder change (positive or negative), already scaled by the current increment. 0 if no change.
    int getDelta();

    // Returns true once a long press (1 second) has been detected — used to toggle increment size.
    bool checkLongPress();

    // Returns true once a short press has been detected — used to power on/off.
    bool checkShortPress();

    // Returns the current increment step (for display purposes).
    int getIncrement();

private:
    uint8_t _clk, _dt, _sw;

    bool _longPressRequested;
    bool _shortPressRequested;
    int _incrementStep; // 1 or 10

    // Encoder polling state
    int _lastClkState;

    // Button state
    unsigned long _buttonPressStartTime;
    bool _isPressed;
    bool _actionTaken;
    unsigned long _lastButtonChangeTime;
};

#endif
