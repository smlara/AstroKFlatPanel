#include <Arduino.h>
#include "Alnitak.h"
#include "ManualEncoder.h"
#include "ExternalDisplay.h"

/*
 * ==========================================
 * RESUMEN DE CONEXIONES (Pines del Arduino)
 * ==========================================
 * - PANEL LED (MOSFET):  Pin D9 (PWM)
 * 
 * - PANTALLA OLED (I2C):
 *   - GND(1):               Pin GND
 *   - VCC(2):               Pin 3V3
 *   - SCL(3):               Pin A5
 *   - SDA(4):               Pin A4
 * 
 * - ENCODER ROTATORIO:
 *   - GND(1):               Pin GND
 *   - VCC(2):               Pin 3V3
 *   - SW(3) (Botón):        Pin D4 
 *   - DT(4):                Pin D3
 *   - CLK(5):               Pin D2
 * ==========================================
 */
/*
 * ==========================================
 * Pinout  MOSFET HW 517
 *==========================================
 *   - Pin PWM : Pin 9 (PWM)
 *
 */

Alnitak alnitak(MOSFET_PIN); // Debe ser el 9 para poder manejar el pwm como dios manda
ManualEncoder encoder; // Usa los pines por defecto definidos en el .h (2, 3, 4)
ExternalDisplay display;

// Variables para controlar cuándo actualizar la pantalla sin bloquear
int lastBrightness = -1;
bool lastIsOn = false;
int lastIncrement = -1;
unsigned long lastDisplayUpdate = 0;

void setup() {
    Serial.begin(9600);
    
    // Inicializar componentes
    alnitak.begin();
    
    // Forzar brillo inicial a 0 y panel encendido
    alnitak.setBrightness(0);
    alnitak.turnOn();
    
    encoder.begin();
    display.begin();

    // Mensaje de bienvenida inicial en el display (5 segundos con la versión)
    String version = "";
#ifdef FW_VERSION
    version = FW_VERSION;
#endif
    display.mostrarBienvenida("ASTROKFLAT", version);
    delay(5000);
    
    // Forzamos actualización inicial
    display.actualizar(alnitak.getBrightness(), encoder.getIncrement(), alnitak.isOn());
}

void loop() {
    // 1. Procesar comunicación serie (Alnitak Protocol)
    alnitak.process();

    // 2. Procesar el estado del Encoder
    int delta = encoder.getDelta();
    
    if (delta != 0) {
        // Obtenemos el porcentaje actual (0 a 100)
        int currentPct = round((alnitak.getBrightness() / 255.0) * 100.0);
        
        // El delta del encoder ahora lo tratamos como incremento de porcentaje (1% o 10%)
        int newPct = currentPct + delta;
        
        // Limitar entre 0 y 100
        if (newPct < 0) newPct = 0;
        if (newPct > 100) newPct = 100;
        
        // Convertir de vuelta a rango 0-255 para el driver
        int newBrightness = round((newPct / 100.0) * 255.0);
        
        alnitak.setBrightness(newBrightness);
    }

    // 3. Procesar pulsación del Botón del Encoder
    if (encoder.checkLongPress()) {
        if (alnitak.isOn()) {
            alnitak.turnOff();
        } else {
            alnitak.turnOn();
        }
    } else if (encoder.checkShortPress()) {
        // La clase ManualEncoder ya ha alternado el incremento internamente (1 <-> 10) al hacer clic corto.
    }

    // 4. Actualizar pantalla solo si ha habido cambios en el estado
    int currentBrightness = alnitak.getBrightness();
    bool currentIsOn = alnitak.isOn();
    int currentIncrement = encoder.getIncrement();

    if (currentBrightness != lastBrightness || currentIsOn != lastIsOn || currentIncrement != lastIncrement) {
        // Mostrar siempre el valor real de brillo, y usar currentIsOn para mostrar OFF o el porcentaje
        display.actualizar(currentBrightness, currentIncrement, currentIsOn);
        
        lastBrightness = currentBrightness;
        lastIsOn = currentIsOn;
        lastIncrement = currentIncrement;
    }
}
