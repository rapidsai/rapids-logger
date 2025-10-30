# =============================================================================
# cmake-format: off
# SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
# SPDX-License-Identifier: Apache-2.0
# cmake-format: on
# =============================================================================

include_guard(GLOBAL)

# Function to generate logger macros for a project.
function(create_logger_macros macro_prefix default_logger header_dir)
  list(APPEND CMAKE_MESSAGE_CONTEXT "create_logger_macros")

  set(_RAPIDS_LOGGER_MACRO_PREFIX "${macro_prefix}")
  set(_RAPIDS_LOGGER_DEFAULT_LOGGER "${default_logger}")

  set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/${header_dir})
  set(INSTALL_DIR ${header_dir})

  set(MACROS_OUTPUT_FILE ${BUILD_DIR}/logger_macros.hpp)
  configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/logger_macros.hpp.in ${MACROS_OUTPUT_FILE})
  install(FILES ${MACROS_OUTPUT_FILE} DESTINATION ${INSTALL_DIR})
endfunction()
