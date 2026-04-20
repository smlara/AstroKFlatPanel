# AstroFlatPanel — Cableado y Pines

## Plataforma

**Arduino Nano** (ATmega328P) — comunicación serie USB a 9600 bps.

---

## Resumen de pines

| Pin Arduino | Señal       | Componente              | Notas                              |
|:-----------:|-------------|-------------------------|------------------------------------|
| D2          | CLK         | Encoder rotatorio       | INPUT_PULLUP, detección por polling |
| D3          | DT          | Encoder rotatorio       | INPUT_PULLUP                       |
| D4          | SW (botón)  | Encoder rotatorio       | INPUT_PULLUP, antirrebote software  |
| D9          | Señal PWM   | Módulo XY-MOS (tira de LED) | PWM ~62.5 kHz via Timer1 (OC1A)  |
| A4 (SDA)    | SDA         | OLED SSD1306 (I2C)      | Bus I2C                            |
| A5 (SCL)    | SCL         | OLED SSD1306 (I2C)      | Bus I2C                            |
| 3V3         | VCC         | OLED + Encoder          | Alimentación 3.3 V                 |
| GND         | GND         | Todos los componentes   | Masa común                         |
| RST         | RST         | Condensador ASCOM       | 10 µF a GND mediante puente — **retirar el puente para programar** |

---

## Componentes y cableado detallado

### 1. Panel LED — Módulo XY-MOS

El XY-MOS es un módulo driver MOSFET completo, elegido frente a un MOSFET convencional por su circuito driver de gate integrado. Esto garantiza transiciones de conmutación rápidas y limpias a la alta frecuencia de PWM (~62.5 kHz, Timer1) utilizada por el firmware, minimizando la disipación de calor y proporcionando una regulación suave y lineal en todo el rango 0–100 % sin necesidad de resistencias de gate externas ni componentes de protección adicionales.

| Pin XY-MOS  | Conexión                       |
|:-----------:|--------------------------------|
| SIG (PWM)   | Arduino D9 (PWM)               |
| VCC         | Alimentación del tira de LED (+) |
| GND         | GND común                      |
| OUT+        | Panel LED (+)                  |
| OUT−        | Panel LED (−) / GND            |

> El brillo (0–255) se persiste en EEPROM y se recupera al arrancar.

---

### 2. Pantalla OLED SSD1306 — 128×32 px (I2C)

Muestra el porcentaje de brillo, el estado (ON/OFF) y el incremento activo del encoder.

| Pin OLED | Señal | Conexión Arduino |
|:--------:|-------|-----------------|
| 1 — GND  | GND   | GND             |
| 2 — VCC  | +3.3V | 3V3             |
| 3 — SCL  | SCL   | A5              |
| 4 — SDA  | SDA   | A4              |

---

### 3. Encoder rotatorio con pulsador (EC11 o compatible)

Controla el brillo manualmente. El pulsador tiene dos comportamientos:

- **Pulsación corta** (< 1 s): alterna el paso de incremento entre **1 %** y **10 %**.
- **Pulsación larga** (≥ 1 s): enciende o apaga el panel.

| Pin Encoder | Señal | Conexión Arduino |
|:-----------:|-------|-----------------|
| 1 — GND     | GND   | GND             |
| 2 — VCC     | +3.3V | 3V3             |
| 3 — SW      | Botón | D4              |
| 4 — DT      | DT    | D3              |
| 5 — CLK     | CLK   | D2              |

---

## Protocolo serie (Alnitak Flat Panel Emulator)

Comunicación a **9600 bps**. Permite el control desde software de astrofotografía (N.I.N.A., Sequence Generator Pro, etc.).

| Comando | Descripción               | Respuesta        |
|---------|---------------------------|------------------|
| `>P`    | Ping / identificación     | `*P19000`        |
| `>S`    | Estado actual             | `*S190{L}0`      |
| `>L`    | Encender panel            | `*L19000`        |
| `>D`    | Apagar panel              | `*D19000`        |
| `>B{n}` | Fijar brillo (0–255)      | `*B19{nnn}`      |
| `>J`    | Consultar brillo actual   | `*J19{nnn}`      |
| `>V`    | Versión firmware          | `*V19001`        |

`{L}` = 1 si encendido, 0 si apagado. Device ID = **19** (Flat Man).

---

### 4. Condensador ASCOM (RST ↔ GND)

Un **condensador de 10 µF** está conectado entre el pin RST y GND mediante un **puente extraíble**. Cuando el driver ASCOM (o software compatible como N.I.N.A. o Sequence Generator Pro) abre el puerto serie, genera un pulso DTR que de otro modo resetearía el Arduino y cortaría la conexión. El condensador amortigua ese pulso, manteniendo el Arduino en marcha y permitiendo una comunicación estable con el driver.

> **Importante:** la programación del Arduino también usa un pulso DTR para activar el bootloader. Como el condensador suprime ese pulso, **hay que retirar el puente antes de cargar el firmware** y volver a colocarlo después para que ASCOM funcione correctamente.

| Conexión | Detalle |
|----------|---------|
| RST — C+ — puente — C− — GND | Condensador electrolítico 10 µF, respetar polaridad (+ hacia RST) |

---

## Notas de implementación

- El Timer1 se reconfigura en modo **Fast PWM 8-bit sin prescaler** para operar a ~62.5 kHz en el pin D9. Esto es incompatible con `analogWrite()` en ese pin tras llamar a `Alnitak::begin()`.
- Los tres pines del encoder usan `INPUT_PULLUP`; no se necesitan resistencias externas.
- La pantalla OLED solo se actualiza cuando cambia el estado (brillo, on/off o incremento), reduciendo la carga de CPU.
