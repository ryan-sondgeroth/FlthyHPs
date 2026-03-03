#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC_LIB_DIR="${REPO_ROOT}/libraries/SlowServoPCA9685"
DST_BASE_DIR="${ARDUINO_LIB_DIR:-$HOME/Documents/Arduino/libraries}"
DST_LIB_DIR="${DST_BASE_DIR}/SlowServoPCA9685"

if [[ ! -d "${SRC_LIB_DIR}" ]]; then
  echo "Missing vendored library: ${SRC_LIB_DIR}" >&2
  exit 1
fi

mkdir -p "${DST_BASE_DIR}"
rm -rf "${DST_LIB_DIR}"
cp -R "${SRC_LIB_DIR}" "${DST_LIB_DIR}"

echo "Installed SlowServoPCA9685 to: ${DST_LIB_DIR}"
echo "Done."
