# =============================================================================
# cmake-format: off
# SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
# SPDX-License-Identifier: Apache-2.0
# cmake-format: on
# =============================================================================

cmake_minimum_required(VERSION 3.30.4 FATAL_ERROR)

include(../../rapids_config.cmake)

file(READ "${CMAKE_CURRENT_LIST_DIR}/rapids_logger/VERSION" _version)
string(STRIP "${_version}" _version)
string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+" _version "${_version}")

project(
  rapids-logger-python
  VERSION "${_version}"
  LANGUAGES CXX
)

# Check if rapids-logger is already available. If so, it is the user's responsibility to ensure that
# the CMake package is also available when building any Python packages that may depend on the
# rapids-logger Python package (e.g. cudf).
find_package(rapids_logger "${_version}")

if(rapids_logger_FOUND)
  return()
endif()

unset(rapids_logger_FOUND)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(BUILD_TESTS OFF)
add_subdirectory(../../ rapids-logger-cpp)
