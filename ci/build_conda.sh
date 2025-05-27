#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.

set -euo pipefail

source rapids-configure-sccache

export CMAKE_GENERATOR=Ninja

rapids-print-env

rapids-logger "Begin cpp build"

sccache --zero-stats

mamba install -y rattler-build

RAPIDS_PACKAGE_VERSION=$(rapids-generate-version) rattler-build build --recipe conda/recipes/rapids-logger --output-dir ${RAPIDS_CONDA_BLD_OUTPUT_DIR}

sccache --show-adv-stats
