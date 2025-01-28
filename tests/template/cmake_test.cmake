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

  cmake_path(GET source_or_dir STEM test_name)

  if(IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${source_or_dir}")
    set(src_dir "${CMAKE_CURRENT_LIST_DIR}/${source_or_dir}/")
  else()
    message(FATAL_ERROR "Unable to find a file or directory named: ${source_or_dir}")
  endif()

  set(build_dir "${CMAKE_CURRENT_BINARY_DIR}/${test_name}-build")

  add_test(
    NAME ${test_name}_configure
    COMMAND
      ${CMAKE_COMMAND} -S ${src_dir} -B ${build_dir}
      # TODO: Not sure if really need to hard code this, maybe just use whatever default generator
      # is discovered.
      -G "Ninja"
      # TODO: This needs to also work when we build the tests separately. In that case, probably
      # want "rapids_logger_BINARY_DIR".
      -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}
  )

  add_test(NAME ${test_name}_build COMMAND ${CMAKE_COMMAND} --build ${build_dir})
  set_tests_properties(${test_name}_build PROPERTIES DEPENDS ${test_name}_configure)

  add_test(NAME ${test_name}_run COMMAND ${build_dir}/${test_name})
  set_tests_properties(${test_name}_run PROPERTIES DEPENDS ${test_name}_build)
endfunction()
