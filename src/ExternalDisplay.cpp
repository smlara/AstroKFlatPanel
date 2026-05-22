#include "ExternalDisplay.h"

ExternalDisplay::ExternalDisplay()
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), _enabled(false) {}

void ExternalDisplay::begin()
{
    // Try to initialise. If nothing responds at 0x3C, begin() returns false.
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("OLED not found. Continuing without display..."));
        _enabled = false;
        return;
    }

    _enabled = true;
    display.clearDisplay();
    display.setRotation(2); // Rotate 180 degrees
    display.setTextColor(SSD1306_WHITE);
    display.display();
}

void ExternalDisplay::update(int brightness, int increment, bool isOn)
{
    if (!_enabled)
        return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Main value as % (0-100%)
    float percentage = (brightness / 255.0) * 100.0;

    display.setCursor(10, 4); // Vertically centred: (32-24)/2 = 4
    display.setTextSize(3);   // Size 3 (24 px tall) for high visibility

    if (!isOn) {
        display.print(F("OFF"));
    } else {
        display.print((int)percentage);
        display.setTextSize(2);
        display.print(F("%"));
    }

    // Show the increment (x1 or x10) in the bottom-right corner
    display.setTextSize(1);
    display.setCursor(94, 24); // Bottom-right, tuned for [x10]
    display.print(F("[x"));
    display.print(increment);
    display.print(F("]"));

    display.display();
}


void ExternalDisplay::showWelcome(String title, String version)
{
    if (!_enabled)
        return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Title in large font (size 2)
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println(title);

    // Version in small font (size 1)
    display.setTextSize(1);
    display.print(F("v"));
    display.print(version);

    display.display();
}
