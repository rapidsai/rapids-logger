#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.

set -euo pipefail

package_name="rapids_logger"
package_dir="python/rapids-logger"

rapids-logger "Building '${package_name}' wheel"
sccache --zero-stats
python -m pip wheel \
    -w "${package_dir}/dist" \
    -v \
    --no-deps \
    --disable-pip-version-check \
    "${package_dir}"
sccache --show-adv-stats

mkdir -p "${package_dir}/final_dist"
python -m auditwheel repair \
    -w "${package_dir}/final_dist" \
    ${package_dir}/dist/*

RAPIDS_PY_WHEEL_NAME="${package_name}" rapids-upload-wheels-to-s3 cpp "${package_dir}/final_dist"
