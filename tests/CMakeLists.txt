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

# This function takes in a test name and test source and handles setting all of the associated
# properties and linking to build the test
function(ConfigureTest CMAKE_TEST_NAME)
  set(options)
  set(one_value)
  set(multi_value)
  cmake_parse_arguments(_TEST "${options}" "${one_value}" "${multi_value}" ${ARGN})

  add_executable(${CMAKE_TEST_NAME} ${_TEST_UNPARSED_ARGUMENTS})
  set_target_properties(
    ${CMAKE_TEST_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "$<BUILD_INTERFACE:${RAPIDS_LOGGER_BINARY_DIR}/gtests>"
               CXX_STANDARD 17
               CXX_STANDARD_REQUIRED ON
  )
  target_link_libraries(
    ${CMAKE_TEST_NAME} PRIVATE rapids_logger::rapids_logger GTest::gmock GTest::gmock_main
                               GTest::gtest GTest::gtest_main $<TARGET_NAME_IF_EXISTS:conda_env>
  )
  gtest_discover_tests(${CMAKE_TEST_NAME})
endfunction()

ConfigureTest(BASIC_TEST basic_test.cpp)
