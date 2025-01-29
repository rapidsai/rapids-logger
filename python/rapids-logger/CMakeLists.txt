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
