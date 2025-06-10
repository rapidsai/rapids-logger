#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.

set -eEuo pipefail

echo "checking for symbol visibility issues"

LIBRARY="${1}"

echo ""
echo "Checking exported symbols in '${LIBRARY}'"
symbol_file="./symbols.txt"
readelf --dyn-syms --wide "${LIBRARY}" \
    | c++filt \
    > "${symbol_file}"

echo "Checking for spdlog symbols..."
if grep -E "spdlog\:\:" "${symbol_file}"; then
    echo "ERROR: Found some exported symbols in ${LIBRARY} matching the pattern spdlog::."
    exit 1
fi

echo "No symbol visibility issues found in ${LIBRARY}"
