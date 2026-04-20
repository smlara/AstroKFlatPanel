#include "Alnitak.h"
#include <EEPROM.h>

// Dirección de memoria EEPROM para el brillo
const int EEPROM_ADDRESS = 0;

Alnitak::Alnitak(uint8_t pin) : _pin(pin), _brightness(0), _isOn(false)
{
    memset(_buffer, 0, sizeof(_buffer));
}

void Alnitak::begin() {
    digitalWrite(_pin, LOW);
    pinMode(_pin, OUTPUT);
    
    // Configuración Alta Frecuencia fija para Pin 9 (Timer 1)
    // Fast PWM, 8-bit, No prescaling (62.5 kHz)
    TCCR1A = _BV(COM1A1) | _BV(WGM10);
    TCCR1B = _BV(WGM12) | _BV(CS10);
    
    loadBrightness();
    updatePWM();
}

void Alnitak::process()
{
    // Solo procesamos si hay al menos 2 bytes (un comando mínimo como >S\n)
    if (Serial.available() > 0) 
    {
        // Leer hasta encontrar el final de línea
        String inputStr = Serial.readStringUntil('\n');
        
        // El protocolo espera que el comando empiece por '>'
        if (inputStr.length() >= 2 && inputStr.charAt(0) == '>') {
            // Convertimos el String a un array de caracteres para nuestra función
            // inputStr.charAt(1) es la letra del comando (S, P, L, B...)
            // inputStr.substring(2) son los datos (si los hay)
            
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
            saveBrightness();
        }
        // Siempre actualizamos el PWM y respondemos
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
    // Formato: *SidMLC\n
    // id = 19 (FLAT_MAN)
    // M = motor status (0 stopped)
    // L = light status (0 off, 1 on)
    // C = Cover status (0)
    char statusStr[16];
    sprintf(statusStr, "*S190%d0\n", _isOn ? 1 : 0);
    Serial.print(statusStr);
}

void Alnitak::updatePWM() {
    // Si la luz está apagada, forzamos el registro a 0 absoluto.
    // Si está encendida, cargamos el valor de brillo.
    if (!_isOn) {
        OCR1A = 0;
    } else {
        OCR1A = _brightness;
    }
}

void Alnitak::saveBrightness()
{
    if (EEPROM.read(EEPROM_ADDRESS) != _brightness)
    {
        EEPROM.write(EEPROM_ADDRESS, _brightness);
    }
}

void Alnitak::loadBrightness()
{
    _brightness = EEPROM.read(EEPROM_ADDRESS);
}

int Alnitak::getBrightness() {
    return _brightness;
}

void Alnitak::setBrightness(int val) {
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    
    if (_brightness != val) {
        _brightness = val;
        saveBrightness();
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