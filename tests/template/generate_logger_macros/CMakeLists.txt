# =============================================================================
# Copyright (c) 2025, NVIDIA CORPORATION.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
# in compliance with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License
# is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
# or implied. See the License for the specific language governing permissions and limitations under
# the License.
# =============================================================================

cmake_minimum_required(VERSION 3.26.4 FATAL_ERROR)

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
