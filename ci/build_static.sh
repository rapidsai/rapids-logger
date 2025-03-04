#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.

set -euo pipefail

source rapids-date-string

rapids-logger "Static cpp build"

# Make sure we have an updated CMake
python -m pip install -U cmake
pyenv rehash

cmake -S . -B build -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
