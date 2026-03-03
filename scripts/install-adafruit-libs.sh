#!/usr/bin/env bash
set -euo pipefail

# Installs Adafruit dependencies directly from upstream GitHub repos.
# Requires: arduino-cli with unsafe install enabled.

NEOPIXEL_REPO="https://github.com/adafruit/Adafruit_NeoPixel.git"
PWM_SERVO_REPO="https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library.git"

echo "Installing Adafruit NeoPixel from ${NEOPIXEL_REPO}"
arduino-cli lib install --git-url "${NEOPIXEL_REPO}"

echo "Installing Adafruit PWM Servo Driver Library from ${PWM_SERVO_REPO}"
arduino-cli lib install --git-url "${PWM_SERVO_REPO}"

echo "Done."
