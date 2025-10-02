#!/bin/bash
# Copyright (c) 2025, NVIDIA CORPORATION.

set -euo pipefail

package_name="rapids_logger"
package_dir="python/rapids-logger"
dist_dir="${package_dir}/dist"
final_dir="${RAPIDS_WHEEL_BLD_OUTPUT_DIR}"

source rapids-configure-sccache
source rapids-init-pip

rapids-logger "Building '${package_name}' wheel"
sccache --zero-stats
python -m pip wheel \
    -w "${dist_dir}" \
    -v \
    --no-deps \
    --disable-pip-version-check \
    "${package_dir}"
sccache --show-adv-stats

mkdir -p "${final_dir}"
python -m auditwheel repair \
    -w "${final_dir}" \
    "${dist_dir}/"*

# Check that no undefined symbols are present in the shared library
WHEEL_EXPORT_DIR="$(mktemp -d)"
unzip -d "${WHEEL_EXPORT_DIR}" "${final_dir}/*"
LOGGER_LIBRARY=$(find "${WHEEL_EXPORT_DIR}" -type f -name 'librapids_logger.so')
./ci/check_symbols.sh "${LOGGER_LIBRARY}"
