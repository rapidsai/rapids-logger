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

# The tests may be built as a standalone project or as part of the main project.
if(NOT DEFINED PROJECT_NAME)
  cmake_minimum_required(VERSION 3.26.4 FATAL_ERROR)

  include(../rapids_config.cmake)
  include(rapids-cpm)
  rapids_cpm_init()

  file(READ "${CMAKE_CURRENT_LIST_DIR}/../VERSION" _version)
  string(STRIP "${_version}" _version)
  project(
    RAPIDS_LOGGER_TESTS
    VERSION "${_version}"
    LANGUAGES CXX
  )

  include(CTest)
  find_package(rapids_logger "${_version}" EXACT REQUIRED)
endif()

include(${rapids-cmake-dir}/cpm/gtest.cmake)
rapids_cpm_gtest(BUILD_STATIC)
include(GoogleTest)
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

# This function takes in a test name and test source and handles setting all of the associated
# properties and linking to build the test
function(ConfigureTest CMAKE_TEST_NAME)
  list(POP_FRONT ARGV)
  add_executable(${CMAKE_TEST_NAME} ${ARGV})
  set_target_properties(
    ${CMAKE_TEST_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/gtests>"
               CXX_STANDARD 17
               CXX_STANDARD_REQUIRED ON
  )
  target_link_libraries(
    ${CMAKE_TEST_NAME} PRIVATE rapids_logger::rapids_logger GTest::gmock GTest::gmock_main
                               GTest::gtest GTest::gtest_main
  )
  gtest_discover_tests(${CMAKE_TEST_NAME})
endfunction()

ConfigureTest(BASIC_TEST basic_test.cpp)

add_subdirectory(template)
