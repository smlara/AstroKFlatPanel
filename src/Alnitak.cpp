#include "Alnitak.h"

Alnitak::Alnitak(uint8_t pin) : _pin(pin), _brightness(0), _isOn(false)
{
    memset(_buffer, 0, sizeof(_buffer));
}

void Alnitak::begin() {
    digitalWrite(_pin, LOW);
    pinMode(_pin, OUTPUT);

    // Timer 1 configuration for pin D9.
    // Fast PWM, 10-bit (mode 7, TOP=0x03FF), prescaler 1 -> ~15.625 kHz.
    // Above the stroboscopic perception threshold of the eye and high
    // enough to be invisible to a camera at any reasonable flat-frame
    // exposure (>= 1 ms = ~16 PWM cycles), while still slow enough to
    // keep the optocoupler on the XY-MOS driver module in a linear
    // operating range. Brightness (0-255, 8-bit) is scaled into the
    // 10-bit OCR1A range in updatePWM().
    TCCR1A = _BV(COM1A1) | _BV(WGM11) | _BV(WGM10);
    TCCR1B = _BV(WGM12) | _BV(CS10);

    updatePWM();
}

void Alnitak::process()
{
    // Only proceed if at least 2 bytes are available (minimum command, e.g. >S\n)
    if (Serial.available() > 0)
    {
        // Read until the end-of-line character
        String inputStr = Serial.readStringUntil('\n');

        // The protocol expects commands to start with '>'
        if (inputStr.length() >= 2 && inputStr.charAt(0) == '>') {
            // Convert the String into a char array for processCommand().
            // inputStr.charAt(1) is the command letter (S, P, L, B, ...)
            // inputStr.substring(2) holds the data (if any)

            char cmd = inputStr.charAt(1);
            String dataStr = inputStr.substring(2);
            char data[16];
            dataStr.toCharArray(data, sizeof(data));

            processCommand(&cmd, data);
        }
    }
}

void Alnitak::processCommand(char* cmd, char* data)
{
    int val = atoi(data);

    switch (*cmd)
    {
    case 'P': // Ping
        Serial.print("*P19000\n");
        break;

    case 'S': // Status
        sendStatus();
        break;

    case 'J': // Get brightness info
        {
            char temp[16];
            sprintf(temp, "*J19%03d\n", _brightness);
            Serial.print(temp);
        }
        break;

    case 'L': // Turn on light
        _isOn = true;
        updatePWM();
        Serial.print("*L19000\n");
        break;

    case 'D': // Turn off light
        _isOn = false;
        updatePWM();
        Serial.print("*D19000\n");
        break;

    case 'B': // Set brightness
        if (val >= 0 && val <= 255)
        {
            _brightness = val;
        }
        // Always refresh PWM and respond
        updatePWM();
        {
            char temp[16];
            sprintf(temp, "*B19%03d\n", _brightness);
            Serial.print(temp);
        }
        break;

    case 'V': // Version
        Serial.print("*V19001\n");
        break;

    default:
        break;
    }
}

void Alnitak::sendStatus()
{
    // Format: *SidMLC\n
    // id = 19 (FLAT_MAN)
    // M = motor status (0 stopped)
    // L = light status (0 off, 1 on)
    // C = cover status (0)
    char statusStr[16];
    sprintf(statusStr, "*S190%d0\n", _isOn ? 1 : 0);
    Serial.print(statusStr);
}

void Alnitak::updatePWM() {
    // When off, force the register to absolute zero.
    // When on, scale the 8-bit brightness (0-255) into the 10-bit
    // OCR1A range (0-1023). brightness=255 maps to 1020 (~99.6% duty),
    // which is indistinguishable from full-on for the panel.
    if (!_isOn) {
        OCR1A = 0;
    } else {
        OCR1A = (uint16_t)_brightness << 2;
    }
}

int Alnitak::getBrightness() {
    return _brightness;
}

void Alnitak::setBrightness(int val) {
    if (val < 0) val = 0;
    if (val > 255) val = 255;

    if (_brightness != val) {
        _brightness = val;
        if (_isOn) {
            updatePWM();
        }
    }
}

bool Alnitak::isOn() {
    return _isOn;
}

void Alnitak::turnOn() {
    if (!_isOn) {
        _isOn = true;
        updatePWM();
    }
}

void Alnitak::turnOff() {
    if (_isOn) {
        _isOn = false;
        updatePWM();
    }
}
