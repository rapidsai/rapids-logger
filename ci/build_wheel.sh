#!/bin/bash
# SPDX-FileCopyrightText: Copyright (c) 2025-2026, NVIDIA CORPORATION.
# SPDX-License-Identifier: Apache-2.0

set -euo pipefail

package_name="rapids_logger"
package_dir="python/rapids-logger"
dist_dir="${package_dir}/dist"
final_dir="${RAPIDS_WHEEL_BLD_OUTPUT_DIR}"

source rapids-configure-sccache
source rapids-init-pip

rapids-logger "Building '${package_name}' wheel"
sccache --zero-stats

RAPIDS_PIP_WHEEL_ARGS=(
  -w "${dist_dir}"
  -v
  --no-deps
  --disable-pip-version-check
)

# Only use --build-constraint when build isolation is enabled.
#
# Passing '--build-constraint' and '--no-build-isolation` together results in an error from 'pip',
# but we want to keep environment variable PIP_CONSTRAINT set unconditionally.
# PIP_NO_BUILD_ISOLATION=0 means "add --no-build-isolation" (ref: https://github.com/pypa/pip/issues/573
if [[ "${PIP_NO_BUILD_ISOLATION:-}" != "0" ]]; then
    RAPIDS_PIP_WHEEL_ARGS+=(--build-constraint="${PIP_CONSTRAINT}")
fi

# unset PIP_CONSTRAINT (set by rapids-init-pip)... it doesn't affect builds as of pip 25.3, and
# results in an error from 'pip wheel' when set and --build-constraint is also passed
unset PIP_CONSTRAINT
python -m pip wheel \
    "${RAPIDS_PIP_WHEEL_ARGS[@]}" \
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
