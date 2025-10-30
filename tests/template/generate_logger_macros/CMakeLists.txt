# =============================================================================
# cmake-format: off
# SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
# SPDX-License-Identifier: Apache-2.0
# cmake-format: on
# =============================================================================

cmake_minimum_required(VERSION 3.30.4 FATAL_ERROR)

project(
  generate_logger_macros
  VERSION 1.0.0
  LANGUAGES CXX
)

# rapids_logger_version must be set by the calling file.
find_package(rapids_logger ${rapids_logger_version} EXACT REQUIRED)

add_executable(generate_logger_macros test.cpp)
target_link_libraries(generate_logger_macros PUBLIC rapids_logger::rapids_logger)
create_logger_macros("RAPIDS_TEST" "default_logger()" "include")
target_include_directories(
  generate_logger_macros PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
                                "$<INSTALL_INTERFACE:include>"
)
set_target_properties(generate_logger_macros PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
