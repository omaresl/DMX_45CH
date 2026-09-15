#!/bin/bash
# Headless build for DMX_45CH using STM32CubeIDE CLI.
# Usage: ./build.sh [config]      (default: Debug)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}/DMX_45CH"
CUBEIDE_DIR="${CUBEIDE_DIR:-/home/deck/st/stm32cubeide_2.2.0}"
WORKSPACE_DIR="${WORKSPACE_DIR:-/tmp/opencode/cubeide-ws}"
CONFIG="${1:-Debug}"

if [ ! -f "${CUBEIDE_DIR}/stm32cubeide" ]; then
	echo "ERROR: STM32CubeIDE no encontrado en ${CUBEIDE_DIR}" >&2
	exit 1
fi

echo "== Headless build DMX_45CH [${CONFIG}] =="
echo "   Workspace temp: ${WORKSPACE_DIR}"

rm -rf "${WORKSPACE_DIR}"

"${CUBEIDE_DIR}/headless-build.sh" \
	-data "${WORKSPACE_DIR}" \
	-import "${PROJECT_DIR}" \
	-build "DMX_45CH/${CONFIG}" \
	2>&1

ELF="${PROJECT_DIR}/${CONFIG}/DMX_45CH.elf"
if [ -f "${ELF}" ]; then
	echo ""
	echo "== Build OK =="
	echo "   ELF: ${ELF}"
	"${CUBEIDE_DIR}/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32."*.linux64_*/tools/bin/arm-none-eabi-size "${ELF}"
else
	echo "ERROR: build fallido (no se genero ${ELF})" >&2
	exit 1
fi