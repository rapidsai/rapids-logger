#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.

set -euo pipefail

. /opt/conda/etc/profile.d/conda.sh

export CMAKE_GENERATOR=Ninja

rapids-logger "Build a rapids-logger consumer that builds rapids-logger and dynamically links against spdlog"

ENV_YAML_DIR="$(mktemp -d)"
ENV_FILE="${ENV_YAML_DIR}/env.yaml"
rapids-dependency-file-generator --file-key test_dynamic_linking --output conda --matrix "" | tee "${ENV_FILE}"

rapids-mamba-retry env create --yes -f "${ENV_FILE}" -n test

# Temporarily allow unbound variables for conda activation.
set +u
conda activate test
set -u

pushd tests/test_dynamic_libs

cmake -S . -B build/
cmake --build build/

# Check that ldd on build/LoggerTest includes spdlog and that neither
# library was cloned into build/_deps
shopt -s nullglob
ldd build/LoggerTest | grep -q spdlog || (echo "spdlog not found in ldd output" && exit 1)
dirs=(build/_deps/spdlog*)
(( ${#dirs[@]} > 0 )) && (echo "spdlog found in build/_deps" && exit 1)
echo "LoggerTest linked against spdlog as expected"
