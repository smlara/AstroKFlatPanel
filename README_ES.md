# AstroKFlat — Panel de Calibración DIY para Astrofotografía

[![License: GPL v3](https://img.shields.io/badge/Firmware-GPL%20v3-blue.svg)](LICENSE)
[![License: CERN OHL v2-S](https://img.shields.io/badge/Hardware-CERN%20OHL%20v2--S-orange.svg)](LICENSE-HARDWARE)
[![Platform: Arduino Nano](https://img.shields.io/badge/Plataforma-Arduino%20Nano-teal.svg)](https://www.arduino.cc/)
[![Protocol: Alnitak](https://img.shields.io/badge/Protocolo-Alnitak%20Flat%20Panel-lightgrey.svg)]()

> **[English version](README.md)**

Panel de luz plana de calibración DIY totalmente open-source para astrofotografía, basado en un **Arduino Nano** (ATmega328P). Compatible con el protocolo **Alnitak Flat Panel Emulator**, lo que permite la integración directa con software de captura como N.I.N.A., Sequence Generator Pro y otros.

---

## Características

- **Protocolo Alnitak** — reconocido de forma nativa por los principales programas de astrofotografía
- **Control manual de brillo** mediante encoder rotatorio (pasos de 1 % o 10 %)
- **Encendido/apagado** mediante pulsación larga en el botón del encoder
- **Pantalla OLED** (128×32) que muestra porcentaje de brillo, estado del panel y paso activo
- **PWM de alta frecuencia** (~62.5 kHz, Timer1) — elimina el parpadeo visible en exposiciones largas
- **Firmware versión 1.1**

---

## Hardware

| Componente | Descripción |
|------------|-------------|
| Arduino Nano | Microcontrolador ATmega328P |
| Panel LED | Panel plano electroluminiscente o LED |
| Módulo XY-MOS | Driver MOSFET con gate driver integrado |
| OLED SSD1306 | Pantalla I2C 128×32 px |
| Encoder rotatorio | EC11 o compatible, con pulsador |

Para el cableado completo, consulta [docs/WIRING_ES.md](docs/WIRING_ES.md) y el esquema en [docs/SCH_Schematic1_2026-04-19.pdf](docs/SCH_Schematic1_2026-04-19.pdf).

---

## Lista de materiales (BOM)

| Cant. | Componente | Especificaciones |
|:-----:|------------|-----------------|
| 1 | Arduino Nano | ATmega328P, USB-C |
| 1 | Módulo XY-MOS | Driver MOSFET con gate driver integrado |
| 1 | Tira de LED | 12 V (adaptar al tamaño del panel) |
| 1 | Pantalla OLED | SSD1306, 128×32 px, I2C, 3.3 V |
| 1 | Encoder rotatorio | EC11 o compatible, con pulsador |
| 1 | Condensador electrolítico | 10 µF, para la línea RST (ASCOM) |
| 1 | Puente / header pin | Para puentear el condensador RST al programar |
| — | Cableado | Cables Dupont o equivalente |

---

## Asignación de pines (Arduino Nano)

| Pin  | Señal       | Componente           |
|:----:|-------------|----------------------|
| D2   | CLK         | Encoder rotatorio    |
| D3   | DT          | Encoder rotatorio    |
| D4   | SW (botón)  | Encoder rotatorio    |
| D9   | Señal PWM   | XY-MOS (driver tira de LED) |
| A4   | SDA         | OLED (I2C)           |
| A5   | SCL         | OLED (I2C)           |

---

## Protocolo serie — Alnitak Flat Panel Emulator

Comunicación a **9600 bps** por USB serie.

| Comando | Descripción               | Respuesta   |
|---------|---------------------------|-------------|
| `>P`    | Ping / identificación     | `*P19000`   |
| `>S`    | Estado actual             | `*S190{L}0` |
| `>L`    | Encender panel            | `*L19000`   |
| `>D`    | Apagar panel              | `*D19000`   |
| `>B{n}` | Fijar brillo (0–255)      | `*B19{nnn}` |
| `>J`    | Consultar brillo actual   | `*J19{nnn}` |
| `>V`    | Versión firmware          | `*V19001`   |

Device ID = **19** (Flat Man).

---

## Compilación y carga

Este proyecto usa [PlatformIO](https://platformio.org/).

```bash
# Clonar el repositorio
git clone https://github.com/smlara/AstroFlatPanel.git
cd AstroFlatPanel

# Compilar y cargar (Arduino Nano bootloader nuevo)
pio run -e nanoatmega328new -t upload

# O para bootloader antiguo
pio run -e nanoatmega328 -t upload
```

### Dependencias (instaladas automáticamente por PlatformIO)

- `adafruit/Adafruit SSD1306 ^2.5.9`
- `adafruit/Adafruit GFX Library ^1.11.9`
- `Wire`
- `EEPROM`

---

## Controles del encoder

| Acción | Efecto |
|--------|--------|
| Girar a la derecha | Aumentar brillo en el paso activo |
| Girar a la izquierda | Disminuir brillo en el paso activo |
| Pulsación corta (< 1 s) | Alternar paso entre 1 % y 10 % |
| Pulsación larga (≥ 1 s) | Encender / apagar el panel |

---

## Licencia

Este proyecto utiliza un modelo de **doble licencia**:

### Uso no comercial (open source)

- **Firmware** (todo el código fuente en `src/` e `include/`): [GNU General Public License v3.0](LICENSE)
- **Hardware** (esquemas, diseños PCB y archivos mecánicos en `docs/`): [CERN Open Hardware Licence v2 - Strongly Reciprocal](LICENSE-HARDWARE)

Eres libre de usar, estudiar, modificar y compartir este proyecto para **uso personal, educativo o no comercial**, siempre que cualquier trabajo derivado permanezca abierto bajo las mismas licencias.

### Uso comercial

**Cualquier uso comercial — incluyendo, entre otros, la fabricación, venta o integración de este diseño en un producto o servicio — está estrictamente prohibido sin el permiso previo y por escrito del autor.**

Si estás interesado en una licencia comercial, abre una [consulta comercial](../../issues/new?labels=commercial-inquiry&title=Consulta+Licencia+Comercial) en este repositorio.

---

## Autor

**Sergio Martínez** — Entusiasta de la astrofotografía  
Preguntas, contribuciones y consultas comerciales: abre un [issue](../../issues) o inicia una [discusión](../../discussions).
