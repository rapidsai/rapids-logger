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

include(cmake_test.cmake)

add_cmake_test(generate_logger_macros)

add_cmake_test(
  generate_logger_macros level_critical
  "-DCMAKE_CXX_FLAGS=-DRAPIDS_TEST_LOG_ACTIVE_LEVEL=RAPIDS_LOGGER_LOG_LEVEL_CRITICAL"
)
