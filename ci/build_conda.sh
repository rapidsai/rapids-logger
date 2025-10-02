#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.

set -euo pipefail

source rapids-configure-sccache

export CMAKE_GENERATOR=Ninja

rapids-print-env

rapids-logger "Begin cpp build"

sccache --zero-stats

mamba install -y rattler-build

source rapids-rattler-channel-string

rattler-build build                      \
  --recipe conda/recipes/rapids-logger   \
  "${RATTLER_ARGS[@]}"                   \
  "${RATTLER_CHANNELS[@]}"

sccache --show-adv-stats
