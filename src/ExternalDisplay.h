#ifndef EXTERNAL_DISPLAY_H
#define EXTERNAL_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

class ExternalDisplay
{
public:
    ExternalDisplay();
    void begin();
    void actualizar(int brightness, int increment, bool isOn);
    void mostrarBienvenida(String title, String version);

private:
    Adafruit_SSD1306 display;
    bool _enabled;
};

#endif
