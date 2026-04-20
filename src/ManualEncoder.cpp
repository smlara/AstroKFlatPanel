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
    // --- Lógica del Botón por Polling ---
    bool currentButtonState = digitalRead(_sw);
    
    if (currentButtonState == LOW && !_isPressed) {
        // Flanco de bajada (empieza pulsación)
        if (millis() - _lastButtonChangeTime > 50) { // Antirrebote inicial
            _isPressed = true;
            _buttonPressStartTime = millis();
            _actionTaken = false;
        }
    }
    else if (currentButtonState == LOW && _isPressed) {
        // Mantenido pulsado: chequear si llevamos > 1 segundo para pulsación larga (encender/apagar)
        if (!_actionTaken && (millis() - _buttonPressStartTime > 1000)) {
            _actionTaken = true;
            _longPressRequested = true;
        }
    }
    else if (currentButtonState == HIGH && _isPressed) {
        // Flanco de subida (soltado)
        _isPressed = false;
        _lastButtonChangeTime = millis();
        
        // Si soltamos antes del 1 segundo (pulsación corta), entonces es cambiar el incremento
        if (!_actionTaken) {
            if (millis() - _buttonPressStartTime > 50) { // Antirrebote mínimo
                _shortPressRequested = true;
                // Alternar incremento entre 1 y 10 internamente
                if (_incrementStep == 1) {
                    _incrementStep = 10;
                } else {
                    _incrementStep = 1;
                }
            }
        }
        _actionTaken = false;
    }

    // --- Lógica del Encoder por Polling ---
    int clkState = digitalRead(_clk);
    int dtState = digitalRead(_dt);
    int delta = 0;

    // Detectar un flanco válido de CLK (cambio de estado)
    if (clkState != _lastClkState) {
        // Solo evaluamos el sentido de giro cuando CLK ha cambiado
        // y CLK es distinto del estado inicial de reposo (por ejemplo LOW).
        // Evitamos así dobles saltos en cada paso del encoder.
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
