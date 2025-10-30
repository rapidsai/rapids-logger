# =============================================================================
# cmake-format: off
# SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
# SPDX-License-Identifier: Apache-2.0
# cmake-format: on
# =============================================================================

# The tests may be built as a standalone project or as part of the main project.
if(NOT DEFINED PROJECT_NAME)
  cmake_minimum_required(VERSION 3.30.4 FATAL_ERROR)

  include(../rapids_config.cmake)
  include(rapids-cpm)
  rapids_cpm_init()

  file(READ "${CMAKE_CURRENT_LIST_DIR}/../VERSION" _version)
  string(STRIP "${_version}" _version)
  string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+" _version "${_version}")

  project(
    RAPIDS_LOGGER_TESTS
    VERSION "${_version}"
    LANGUAGES CXX
  )

  include(CTest)
  find_package(rapids_logger "${_version}" EXACT REQUIRED)
  set(BUILDING_STANDALONE_TESTS ON)
else()
  set(BUILDING_STANDALONE_TESTS OFF)
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
