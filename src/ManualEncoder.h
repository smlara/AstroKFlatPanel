#ifndef MANUAL_ENCODER_H
#define MANUAL_ENCODER_H

#include <Arduino.h>

// Definición de pines por defecto para el encoder
#define ENCODER_CLK_PIN 2
#define ENCODER_DT_PIN 3
#define ENCODER_SW_PIN 4

class ManualEncoder {
public:
    ManualEncoder(uint8_t clkPin = ENCODER_CLK_PIN, uint8_t dtPin = ENCODER_DT_PIN, uint8_t swPin = ENCODER_SW_PIN);
    
    void begin();
    
    // Devuelve el cambio en el encoder (positivo o negativo), ya escalado por el incremento actual. 0 si no hay cambio.
    int getDelta();
    
    // Verifica si se ha detectado una pulsación larga (1 segundo) para cambiar los incrementos
    bool checkLongPress();
    
    // Verifica si se ha detectado una pulsación corta para encender/apagar
    bool checkShortPress();
    
    // Obtiene el incremento actual (para mostrarlo si es necesario)
    int getIncrement();

private:
    uint8_t _clk, _dt, _sw;
    
    bool _longPressRequested;
    bool _shortPressRequested;
    int _incrementStep; // 1 o 10
    
    // Variables para polling del encoder
    int _lastClkState;
    
    // Variables para el botón
    unsigned long _buttonPressStartTime;
    bool _isPressed;
    bool _actionTaken;
    unsigned long _lastButtonChangeTime;
};

#endif
