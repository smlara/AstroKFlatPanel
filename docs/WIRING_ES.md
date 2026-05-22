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
| D9          | Señal PWM   | Módulo XY-MOS (tira de LED) | PWM ~15.6 kHz via Timer1 (OC1A)  |
| A4 (SDA)    | SDA         | OLED SSD1306 (I2C)      | Bus I2C                            |
| A5 (SCL)    | SCL         | OLED SSD1306 (I2C)      | Bus I2C                            |
| 3V3         | VCC         | OLED + Encoder          | Alimentación 3.3 V                 |
| GND         | GND         | Todos los componentes   | Masa común                         |
| RST         | RST         | Condensador ASCOM       | 10 µF a GND mediante puente — **retirar el puente para programar** |

---

## Componentes y cableado detallado

### 1. Panel LED — Módulo XY-MOS

El XY-MOS es un módulo driver MOSFET, elegido frente a un MOSFET convencional por su circuito de conmutación integrado. Muchos de estos módulos usan un optoacoplador en la entrada, así que el firmware genera el PWM a **~15.6 kHz** (Timer1, Fast PWM 10-bit, prescaler 1) — lo bastante alto para ser invisible al ojo y a la cámara en cualquier exposición razonable de flat, y lo bastante bajo para mantener al optoacoplador en su rango lineal (los 62.5 kHz originales lo saturaban).

### Alimentación de la tira por USB

El panel completo funciona desde el USB del Arduino, como todos los generadores de flats comerciales para astrofotografía. Usa una **tira LED de 5 V** dimensionada para que su consumo a brillo máximo quepa dentro del presupuesto USB (USB 2.0: 500 mA, USB 3.0: 900 mA) — para tiras típicas de 60 LED/m eso significa **aproximadamente 1 metro o menos a full**.

El `+` de la tira se toma del pin `+5V` del Arduino (expuesto en el conector `VOUT`); el `−` vuelve por el módulo MOSFET (conmutación low-side) a GND. Para uso como flat panel no hace falta ninguna fuente ni filtrado adicional: a 15.6 kHz de PWM el ripple residual del rail de 5 V queda muy por debajo de lo que cualquier exposición de astrofotografía (≥ 0.1 s) puede resolver.

> **Refinamiento opcional.** Si llegas a ver flicker visible — típicamente porque has usado una tira mucho más larga o un cable USB más largo/fino — añadir un electrolítico **220–470 µF / 10 V** en paralelo a la entrada de la tira (positivo a `+5V`, negativo a GND, montado lo más cerca posible de la tira) absorbe el pulso de corriente de cada ciclo y estabiliza el rail. En el diseño de referencia no es necesario.

| Pin XY-MOS  | Conexión                       |
|:-----------:|--------------------------------|
| SIG (PWM)   | Arduino D9 (PWM)               |
| VCC         | Alimentación del tira de LED (+) |
| GND         | GND común                      |
| OUT+        | Panel LED (+)                  |
| OUT−        | Panel LED (−) / GND            |

> El brillo arranca siempre en 0 (panel encendido, luz apagada); el valor anterior no se persiste.

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

- El Timer1 se reconfigura en modo **Fast PWM 10-bit, prescaler 1** para operar a ~15.6 kHz en el pin D9 (TOP=0x03FF). El brillo Alnitak de 8 bits (0–255) se escala al rango de 10 bits de OCR1A, así que un brillo de 255 corresponde a ~99.6 % de ciclo de trabajo. Esto es incompatible con `analogWrite()` en ese pin tras llamar a `Alnitak::begin()`.
- Los tres pines del encoder usan `INPUT_PULLUP`; no se necesitan resistencias externas.
- La pantalla OLED solo se actualiza cuando cambia el estado (brillo, on/off o incremento), reduciendo la carga de CPU.
