# FlthyHPs (Flthy Holoprojectors)

This repo contains the `FlthyHPs_v2.1.ino` sketch and supporting documentation for controlling 3 R2 holoprojectors (Front/Rear/Top) with:

- NeoPixel LED effects (7 LEDs per projector)
- Servo articulation through an Adafruit PCA9685 I2C board
- Auto-twitch behavior for both LEDs and servos
- Serial/I2C command control (plus optional RC input)

This README is based on the manual in [`docs/Guide to Using Your FlthyHPs v1.8 .docx`](docs/Guide%20to%20Using%20Your%20FlthyHPs%20v1.8%20.docx), with updates aligned to the current `v2.1` sketch.

## At a Glance

- **Sketch:** `FlthyHPs_v2.1.ino`
- **Arduino I2C address:** `0x19` (decimal 25)
- **PCA9685 I2C address:** `0x40` (default)
- **Serial baud:** `9600`
- **HP count:** 3 (Front, Rear, Top)

## Hardware

Typical system components (from the manual):

- Arduino Pro Mini (5V, 16MHz) on Flthy breakout
- Adafruit 16-channel PCA9685 servo driver
- 3 HP NeoPixel assemblies (7 LED Jewel each)
- 6 servos (2 per HP)
- 5V supply with sufficient current headroom

## Wiring (Default)

### Arduino pins

- `D2` Front HP LED data
- `D3` Rear HP LED data
- `D4` Top HP LED data
- `D9` RC input (optional, if enabled)
- `D10` PCA9685 `OE` (servo output enable control)
- `D13` status LED
- `A4/A5` I2C SDA/SCL

### PCA9685 channels

- `0/1` Front HP servos
- `2/3` Rear HP servos
- `4/5` Top HP servos

## Required Libraries

- `Wire` (built-in)
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [Adafruit PWM Servo Driver Library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library)
- Big Happy Dude Slow Servo library (`Servos.h`) vendored in this repo at `libraries/SlowServoPCA9685`

Note: the standard Arduino Servo library is not used due to NeoPixel timing conflicts.

### Vendored slow-servo library

This repo includes `libraries/SlowServoPCA9685` so the required `Servos.h` dependency is versioned with the sketch.

- Source in this repo: `libraries/SlowServoPCA9685`
- License in upstream headers: GNU GPL v3 or later
- Original copyright: Graham Short (2013)

## Setup and Flashing

1. Install Arduino IDE.
2. Install required libraries.
3. Install Adafruit libraries directly from upstream repos:

```bash
./scripts/install-adafruit-libs.sh
```

4. Install vendored slow-servo library from this repo:

```bash
./scripts/install-libs.sh
```

Optional: set `ARDUINO_LIB_DIR` if your Arduino libraries path is not `~/Documents/Arduino/libraries`.

5. Open `FlthyHPs_v2.1.ino`.
6. Set board to **Arduino Pro or Pro Mini** and select your FTDI serial port.
7. Adjust configuration values (if needed).
8. Upload and verify boot behavior.

## Key Sketch Settings

Common settings called out in the manual and present in the current sketch:

- `Config::I2C_ADDRESS` (`0x19`)
- `Config::SERVO_I2C_ADDRESS` (`0x40`)
- `OUTPUT_ENABLED_PIN` (`10`)
- `BRIGHTNESS` (`150`)
- `ENABLEBASICHP` (basic vs 9-point preset coordinate mode)
- `enableTwitchLED[]` and `enableTwitchHP[]`
- `LEDTwitchInterval[][]` and `HPTwitchInterval[][]`
- `SERVO_SPEED[]`
- `Config::ENABLE_SERVO_EASING` and easing duration/update interval (v2.1)

For RC control, enable the RC option in the sketch and confirm RC pulse range values match your radio.

## Command Model

Command format:

- `DT##`
- `DT##C`
- `DT##CS`
- `DT##P`
- Optional runtime suffix: `|seconds`

Where:

- `D` = device designator: `F`, `R`, `T`, `A`, `X`, `Y`, `Z`, or `S` (special sequence set)
- `T` = type: `0` LED, `1` servo
- `##` = 2-digit sequence ID
- `C` = color (0-9)
- `S` = speed (dim pulse speed 0-9)
- `P` = position (0-8 preset)

### Color values

- `0` random
- `1` red
- `2` yellow
- `3` green
- `4` cyan
- `5` blue
- `6` magenta
- `7` orange
- `8` purple
- `9` white

### LED sequences (Type `0`)

- `D001` Leia
- `D002C` Color projector
- `D003CS` Dim pulse
- `D004C` Cycle
- `D005C` Solid color
- `D006` Rainbow
- `D007C` Short circuit
- `D096` Clear LEDs, disable auto LED, disable off-color
- `D0971` Clear LEDs, auto LED default, disable off-color
- `D0972` Clear LEDs, auto LED random, disable off-color
- `D098` Clear LEDs, disable auto LED, enable off-color
- `D0991` Clear LEDs, auto LED default, enable off-color
- `D0992` Clear LEDs, auto LED random, enable off-color

### Servo sequences (Type `1`)

- `D101P` Go to preset position (`0..8`)
- `D102` RC control left/right
- `D103` RC control up/down
- `D104` Random position
- `D105` Wag left/right
- `D106` Wag up/down
- `D198` Disable auto servo twitch
- `D199` Enable auto servo twitch

### Special `S` sequences

- `S1` Leia mode (front down + Leia LEDs, others disabled)
- `S4` Clear + disable auto, off-color disabled
- `S5` Clear + enable auto (default LED twitch), off-color disabled
- `S6` Clear + enable auto (random LED twitch), off-color disabled
- `S7` Clear + disable auto, off-color enabled
- `S8` Clear + enable auto (default LED twitch), off-color enabled
- `S9` Clear + enable auto (random LED twitch), off-color enabled

### Timed command examples

```text
A006|25     # Rainbow on all HPs for 25s
F001|60     # Leia on Front HP for 60s
T1012|10    # Move Top HP to preset 2 for 10s
```

## MarcDuino Integration (from manual)

The manual includes MarcDuino `v1.5/v2` usage over I2C. Core idea:

- Connect SDA/SCL/GND between MarcDuino and FlthyHPs controller.
- Send a string command to FlthyHPs address (`25` decimal by default).
- Example:

```text
&25,"A006|10\r
```

If another device already uses address `25`, change `Config::I2C_ADDRESS` in the sketch and update MarcDuino command address accordingly.

## Notes

- The manual in `docs/` is `v1.8`; this repo sketch is `v2.1`.
- If details conflict, treat `FlthyHPs_v2.1.ino` as source of truth.
