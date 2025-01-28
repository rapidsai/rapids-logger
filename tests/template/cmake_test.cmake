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
include_guard(GLOBAL)

# Create a test that configures, builds, and runs a CMake project.
function(add_cmake_test source_or_dir)

  if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${source_or_dir}")
    set(src_dir "${CMAKE_CURRENT_LIST_DIR}/${source_or_dir}/")
  else()
    message(FATAL_ERROR "Unable to find a file or directory named: ${source_or_dir}")
  endif()

  cmake_path(GET source_or_dir STEM base_test_name)
  set(test_name "${base_test_name}")

  if(DEFINED ARGV1)
    set(test_name "${test_name}_${ARGV1}")
  endif()

  # If we are building the tests as part of the main build, each CMake tests needs to be told where
  # to find the rapids-logger build. If not, we assume that the caller is responsible for making
  # them discoverable (either by having them installed to the prefix path or by setting
  # CMAKE_PREFIX_PATH).
  set(extra_args "")
  if(NOT BUILDING_STANDALONE_TESTS)
    list(APPEND extra_args "-DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}")
  endif()

  # The third argument is a list of extra arguments to pass to CMake.
  if(DEFINED ARGV2)
    foreach(arg ${ARGV2})
      list(APPEND extra_args "${arg}")
    endforeach()
  endif()

  set(build_dir "${CMAKE_CURRENT_BINARY_DIR}/${test_name}-build")
  add_test(
    NAME ${test_name}_configure
    COMMAND
      ${CMAKE_COMMAND} -S ${src_dir} -B ${build_dir}
      # Hardcoding Ninja to simplify things. Assumes Ninja is available when running tests, which is
      # not a very onerous requirement.
      -G Ninja ${extra_args}
  )

  add_test(NAME ${test_name}_build COMMAND ${CMAKE_COMMAND} --build ${build_dir})
  set_tests_properties(${test_name}_build PROPERTIES DEPENDS ${test_name}_configure)

  add_test(NAME ${test_name}_run COMMAND ${build_dir}/${base_test_name})
  set_tests_properties(${test_name}_run PROPERTIES DEPENDS ${test_name}_build)
endfunction()
