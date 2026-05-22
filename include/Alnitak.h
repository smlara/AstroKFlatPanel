#ifndef ALNITAK_H
#define ALNITAK_H

#include <Arduino.h>

#define MOSFET_PIN 9 // Do not change; documentation only

class Alnitak {
public:
    Alnitak(uint8_t pin);
    void begin();
    void process();

    // Methods for manual control and display
    int getBrightness();
    void setBrightness(int val);
    bool isOn();
    void turnOn();
    void turnOff();

private:
    uint8_t _pin;
    uint8_t _brightness;
    bool _isOn;

    char _buffer[20];

    void processCommand(char* cmd, char* data);
    void sendStatus();
    void updatePWM();
};

#endif
