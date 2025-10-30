# =============================================================================
# cmake-format: off
# SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
# SPDX-License-Identifier: Apache-2.0
# cmake-format: on
# =============================================================================

include(cmake_test.cmake)

add_cmake_test(generate_logger_macros)

add_cmake_test(
  generate_logger_macros level_critical
  "-DCMAKE_CXX_FLAGS=-DRAPIDS_TEST_LOG_ACTIVE_LEVEL=RAPIDS_LOGGER_LOG_LEVEL_CRITICAL"
)
