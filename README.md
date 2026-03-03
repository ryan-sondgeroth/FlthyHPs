# FlthyHPs - Flthy Holoprojectors v2.1

Arduino-based control system for R2-D2 holoprojector units. Combines servo movement and NeoPixel LED display into a single unified sketch, supporting all 3 holoprojectors (Front, Rear, Top) with auto-twitch behavior, multiple LED sequences, and smooth servo easing.

**Author:** Ryan Sondgeroth (aka FlthyMcNsty)
**Released:** 01-23-2026

---

## Features

- Control 3 holoprojectors (Front, Rear, Top) from a single sketch
- 7 NeoPixel LEDs per HP for life-like hologram projection effects
- Servo control via Adafruit PCA9685 I2C driver (avoids NeoPixel library conflicts)
- Smooth servo acceleration/deceleration with 3 easing curve options *(new in v2.1)*
- Auto-twitch behavior with configurable intervals for both LEDs and servos
- 10-color palette with 10 shading variants per color
- 7 LED animation sequences (Leia mode, rainbow, pulse, cycle, short circuit, etc.)
- Command input via Serial (9600 baud) or I2C (address 0x19)
- Optional RC receiver control
- PROGMEM-optimized for AVR/Pro Mini boards

---

## Hardware Requirements

| Component | Quantity | Notes |
|-----------|----------|-------|
| Arduino (AVR/Pro Mini compatible) | 1 | |
| Adafruit PCA9685 16-Channel PWM/Servo Driver | 1 | I2C address 0x40 |
| Holoprojector units | 3 | Front, Rear, Top |
| Servo motors | 6 | 2 per HP (X/Y axis) |
| NeoPixel LED boards | 21 | 7 per HP (RGB or RGBW) |
| Status LED | 1 | On pin 13 |
| RC receiver (optional) | 1 | On pin 9 |

---

## Wiring

### Arduino Pin Assignments

| Pin | Function |
|-----|----------|
| 2 | Front HP NeoPixel data |
| 3 | Rear HP NeoPixel data |
| 4 | Top HP NeoPixel data |
| 9 | RC receiver input (optional) |
| 10 | Output Enable (OE) — servo power control |
| 13 | Status LED |
| A4/A5 | I2C SDA/SCL (to PCA9685) |

### PCA9685 Servo Channels

| Channel | Servo |
|---------|-------|
| 0 | Front HP — X-axis |
| 1 | Front HP — Y-axis |
| 2 | Rear HP — X-axis |
| 3 | Rear HP — Y-axis |
| 4 | Top HP — X-axis |
| 5 | Top HP — Y-axis |

### I2C Addresses

| Device | Address |
|--------|---------|
| Arduino (slave) | 0x19 |
| PCA9685 servo driver | 0x40 |

---

## Required Libraries

- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [Adafruit PWM Servo Driver (PCA9685)](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library)
- [Big Happy Dude's Slow Servo Library](https://astromech.net) — required for compatibility with NeoPixel
- `Wire.h` — built-in Arduino I2C library
- `avr/pgmspace.h` — built-in PROGMEM support

> **Note:** The standard Arduino servo library conflicts with Adafruit NeoPixel. Big Happy Dude's Slow Servo Library resolves this.

---

## Installation

1. **Wire the hardware** per the wiring table above.
2. **Install required libraries** via the Arduino Library Manager or manually.
3. **Configure the sketch** — adjust the defines at the top of the file:

```cpp
#define ENABLEBASICHP true    // true = basic mode (recommended for HP kits)
                              // false = 9 preset positions per HP
// #define ENABLERC           // Uncomment to enable RC receiver control
// #define DEBUG              // Uncomment for Serial Monitor debug output
// #define NEO_JEWEL_RGBW     // Uncomment if using RGBW NeoPixels
```

4. **Upload** to your Arduino.
5. **Open Serial Monitor** at 9600 baud to send test commands.

---

## Configuration

Key constants you may want to adjust:

| Constant | Default | Description |
|----------|---------|-------------|
| `BRIGHTNESS` | 150 | NeoPixel brightness (0–255) |
| `DIM_PULSE_SPEED` | 5 | Pulse speed (0=slow, 9=fast) |
| `DEFAULT_EASING_DURATION` | 600 ms | Servo movement duration |
| `ENABLE_SERVO_EASING` | true | Smooth servo motion on/off |
| `SERVO_SPEED` | 150–400 ms | Auto-twitch movement speed range |

### LED Easing Types

| Type | Behavior |
|------|----------|
| `EASE_IN_OUT` | Smooth S-curve (default) |
| `EASE_IN` | Slow start, fast end |
| `EASE_OUT` | Fast start, slow end |
| `LINEAR` | No easing |

### Color Palette (0–9)

| Code | Color |
|------|-------|
| 0 | Random |
| 1 | Red |
| 2 | Yellow |
| 3 | Green |
| 4 | Cyan / Aqua |
| 5 | Blue |
| 6 | Magenta |
| 7 | Orange |
| 8 | Purple |
| 9 | White |

Each color has 10 shading variants used for animation effects.

---

## Command Reference

### Device Designators

| Code | Target |
|------|--------|
| `F` | Front HP |
| `R` | Rear HP |
| `T` | Top HP |
| `A` | All 3 HPs |
| `X` | Front + Rear |
| `Y` | Front + Top |
| `Z` | Rear + Top |
| `S` | Sequence commands |

### LED Commands (Type 0)

| Command | Description |
|---------|-------------|
| `D001` | Leia Mode — random blue shades |
| `D002C` | Color Projector — uses specified color |
| `D003CS` | Dim Pulse — color pulses on/off (S = speed 0–9) |
| `D004C` | Cycle — light travels around LED ring |
| `D005C` | Solid Color — all LEDs set to one color |
| `D006` | Rainbow — cycling rainbow effect |
| `D007C` | Short Circuit — flashing with slowing interval |
| `D096` | Clear (no auto-sequence) |
| `D0971` | Clear + enable auto LED (default sequences) |
| `D0972` | Clear + enable auto LED (random sequences) |
| `D098` | Clear + enable "Off Color" |

### Servo Commands (Type 1)

| Command | Description |
|---------|-------------|
| `D101P` | Move to preset position (P = 0–8) |
| `D102` | Enable RC control (left/right) |
| `D103` | Enable RC control (up/down) |
| `D104` | Move to random position |
| `D105` | Wag left/right 5 times |
| `D106` | Wag up/down 5 times |
| `D198` | Disable auto-twitch |
| `D199` | Enable auto-twitch |

### Preset Positions (for D101P)

| Code | Position |
|------|----------|
| 0 | Down |
| 1 | Center |
| 2 | Up |
| 3 | Left |
| 4 | Upper Left |
| 5 | Lower Left |
| 6 | Right |
| 7 | Upper Right |
| 8 | Lower Right |

### Sequence Commands (Device S)

| Command | Description |
|---------|-------------|
| `S1` | Leia Mode — Front HP down + Leia LED, others off |
| `S4` | All off, auto-twitch disabled |
| `S5` | All on with default sequences + auto-twitch |
| `S9` | All on with random sequences + auto-twitch |
| `S7` | All off, enable "Off Color" |
| `S8` | All on default + "Off Color" + auto-twitch |

### Timed Commands

Append `|seconds` to any command to run it for a set duration:

```
A006|25    // Rainbow on all HPs for 25 seconds
F001|60    // Leia mode on Front HP for 60 seconds
```

### Examples

```
F005       // Front HP: solid blue (default color)
A104       // All HPs: move to random positions
R001       // Rear HP: Leia mode
T101 2     // Top HP: move to preset position 2 (Up)
A006|30    // All HPs: rainbow for 30 seconds
S1         // Leia sequence
```

Type `help` or `?` in the Serial Monitor for on-device command help.

---

## Auto-Twitch

Each HP independently triggers random LED and servo movements at configurable intervals:

| Setting | Default Range |
|---------|--------------|
| LED twitch trigger | 30–90 seconds |
| LED effect run time | 5–25 seconds |
| Front HP servo twitch | 45–120 seconds |
| Rear HP servo twitch | 60–180 seconds |
| Top HP servo twitch | 60–180 seconds |

Auto-twitch can be disabled per-HP with `D198` or globally with `S4`.

---

## Version History

### v2.1 — 01-23-2026
- Added smooth servo easing (ease-in-out, ease-in, ease-out, linear)
- Configurable easing duration and master enable/disable

### v2.0
- Major refactor by Nobser — improved efficiency and readability
- Duration/pipe syntax for timed commands
- Replaced `String` class with `char` arrays (prevents heap fragmentation)
- Magic numbers moved to named constants
- PROGMEM usage for large color tables
- Integer math replacing floating-point
- Timer classes for non-blocking timing
- Help command in Serial Monitor
- Bounds checking on all array accesses

### Earlier Versions
- v1.8: Custom "Off Colors" per HP (per IOIIOOO/Jason Cross request)
- v1.7: Bug fixes
- v1.6: Random LED sequences, debug feedback, compiler directives, PROGMEM
- v1.5: Improved servo pulse length mapping accuracy
- v1.1–v1.4: Progressive feature additions

---

## Credits

- **LostRebel & Knightshade** — input on system functions and code improvements
- **Nobser** — major refactoring and improvements in v2.0
