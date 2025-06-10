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

cmake_minimum_required(VERSION 3.30.4 FATAL_ERROR)

project(
  RAPIDS_LOGGER_DYNAMIC_LIB_TESTS
  VERSION 1.0.0
  LANGUAGES CXX
)

# We must preempt the fmt find because rapids-cmake contains patches for fmt that always trigger
# downloading and we cannot get rid of those with an override file without also triggering a
# download because of how rapids-cpm decides when to download.
find_package(fmt 11.0.2 REQUIRED)
set_target_properties(fmt::fmt PROPERTIES IMPORTED_GLOBAL TRUE)
set_target_properties(fmt::fmt-header-only PROPERTIES IMPORTED_GLOBAL TRUE)

include(FetchContent)

# Force test to use the copy of rapids-logger in the source tree.
set(FETCHCONTENT_SOURCE_DIR_RAPIDS_LOGGER "${CMAKE_CURRENT_SOURCE_DIR}/../..")

FetchContent_Declare(
  rapids_logger
  GIT_REPOSITORY https://github.com/rapidsai/rapids_logger.git
  GIT_TAG main
)

# The purpose of this test is to verify that we can link against preexisting spdlog and fmt shared
# libraries, so we must set the appropriate options here.
set(BUILD_SHARED_LIBS ON)
set(BUILD_TESTS OFF)
set(RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS OFF)
set(RAPIDS_LOGGER_FMT_OPTION "EXTERNAL_FMT")

FetchContent_MakeAvailable(rapids_logger)

add_executable(LoggerTest test.cpp)
target_link_libraries(LoggerTest PUBLIC rapids_logger::rapids_logger)
set_target_properties(LoggerTest PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
