# =============================================================================
# Copyright (c) 2018-2024, NVIDIA CORPORATION.
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
# TODO: Do we want to version the logger in the same way as the rest of RAPIDS? I expect it to be
# relatively static and not something we need to re-release often. Furthermore, we won't be building
# packages of it since we only ever need it cloned by CPM during the builds of other packages. On
# the other hand we still need a way to get a suitable rapids-cmake version. Either that, or we need
# to avoid using rapids-cmake.
set(_rapids_version 25.02.00)
if(_rapids_version MATCHES [[^([0-9][0-9])\.([0-9][0-9])\.([0-9][0-9])]])
  set(RAPIDS_VERSION_MAJOR "${CMAKE_MATCH_1}")
  set(RAPIDS_VERSION_MINOR "${CMAKE_MATCH_2}")
  set(RAPIDS_VERSION_PATCH "${CMAKE_MATCH_3}")
  set(RAPIDS_VERSION_MAJOR_MINOR "${RAPIDS_VERSION_MAJOR}.${RAPIDS_VERSION_MINOR}")
  set(RAPIDS_VERSION "${RAPIDS_VERSION_MAJOR}.${RAPIDS_VERSION_MINOR}.${RAPIDS_VERSION_PATCH}")
else()
  string(REPLACE "\n" "\n  " _rapids_version_formatted "  ${_rapids_version}")
  message(
    FATAL_ERROR
      "Could not determine RAPIDS version. Contents of VERSION file:\n${_rapids_version_formatted}"
  )
endif()

if(NOT EXISTS
   "${CMAKE_CURRENT_BINARY_DIR}/RAPIDS_LOGGER_RAPIDS-${RAPIDS_VERSION_MAJOR_MINOR}.cmake"
)
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/rapidsai/rapids-cmake/branch-${RAPIDS_VERSION_MAJOR_MINOR}/RAPIDS.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/RAPIDS_LOGGER_RAPIDS-${RAPIDS_VERSION_MAJOR_MINOR}.cmake"
  )
endif()
include("${CMAKE_CURRENT_BINARY_DIR}/RAPIDS_LOGGER_RAPIDS-${RAPIDS_VERSION_MAJOR_MINOR}.cmake")
