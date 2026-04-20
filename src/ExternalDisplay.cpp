#include "ExternalDisplay.h"

ExternalDisplay::ExternalDisplay()
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), _enabled(false) {}

void ExternalDisplay::begin()
{
    // Intentamos inicializar. Si no responde en la dirección 0x3C, begin() devuelve false.
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("OLED no encontrado. Continuando sin pantalla..."));
        _enabled = false;
        return;
    }

    _enabled = true;
    display.clearDisplay();
    display.setRotation(2); // Rotate 180 degrees
    display.setTextColor(SSD1306_WHITE);
    display.display();
}

void ExternalDisplay::actualizar(int brightness, int increment, bool isOn)
{
    if (!_enabled)
        return;
        
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Valor principal en % (0-100%)
    float porcentaje = (brightness / 255.0) * 100.0;

    display.setCursor(10, 4); // Centrado verticalmente (32-24)/2 = 4
    display.setTextSize(3);   // Tamaño 3 (24px de alto) para que sea bien visible
    
    if (!isOn) {
        display.print(F("OFF"));
    } else {
        display.print((int)porcentaje);
        display.setTextSize(2);
        display.print(F("%"));
    }

    // Mostrar el incremento (x1 o x10) en la esquina inferior derecha
    display.setTextSize(1);
    display.setCursor(94, 24); // Corner inferior derecha, ajustado para [x10]
    display.print(F("[x"));
    display.print(increment);
    display.print(F("]"));

    display.display();
}


void ExternalDisplay::mostrarBienvenida(String title, String version)
{
    if (!_enabled)
        return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    // Título en grande (tamaño 2)
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println(title);
    
    // Versión en pequeño (tamaño 1)
    display.setTextSize(1);
   // display.setCursor(0, 20); 
    display.print(F("v"));
    display.print(version);
    
    display.display();
}
