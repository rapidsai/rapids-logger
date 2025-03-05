# =============================================================================
# Copyright (c) 2024, NVIDIA CORPORATION.
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

include(rapids_config.cmake)
include(rapids-cmake)
include(rapids-cpm)
include(rapids-export)

file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" _version)
string(STRIP "${_version}" _version)
string(REGEX MATCH "[0-9]+\.[0-9]+\.[0-9]+" _version "${_version}")

project(
  RAPIDS_LOGGER
  VERSION "${_version}"
  LANGUAGES CXX
)

rapids_cmake_build_type(Release)

rapids_cpm_init()

option(BUILD_TESTS "Configure CMake to build tests" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

include(CMakeDependentOption)
# We cannot hide all spdlog symbols if we are building a static library.
cmake_dependent_option(
  RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS
  "Build and link to spdlog in a way that maximizes all symbol hiding" ON "BUILD_SHARED_LIBS" OFF
)

# If we are hiding all spdlog symbols then we need to use the bundled fmt library, so this option is
# only configurable if we are not hiding those symbols.
cmake_dependent_option(
  RAPIDS_LOGGER_FMT_OPTION "The fmt option to use when building spdlog." "EXTERNAL_FMT_HO"
  "NOT RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS" "BUNDLED"
)

# TODO: For Spark-RAPIDS we will have to support building this as a static library. We may need some
# additional testing to make sure that will work.
add_library(rapids_logger src/logger.cpp)
add_library(rapids_logger::rapids_logger ALIAS rapids_logger)
target_include_directories(
  rapids_logger PUBLIC "$<BUILD_INTERFACE:${RAPIDS_LOGGER_SOURCE_DIR}/include>"
                       "$<INSTALL_INTERFACE:include>"
)
set_target_properties(
  rapids_logger
  PROPERTIES BUILD_RPATH "\$ORIGIN"
             INSTALL_RPATH "\$ORIGIN"
             # set target compile options
             CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON
             CXX_VISIBILITY_PRESET hidden
             POSITION_INDEPENDENT_CODE ON
)

# Get spdlog
include(${rapids-cmake-dir}/cpm/spdlog.cmake)
if(RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS)
  set(CPM_DOWNLOAD_spdlog ON)
  # The override file sets EXCLUDE_FROM_ALL to ON. There is no way to do that directly via
  # rapids_cpm_spdlog right now because it explicitly passes in a value to the rapids_cpm_find call
  # that will override the same version of the argument passed here (or at least, it's
  # order-dependent which is selected so we're at the mercy of the rapids-cmake implementation). We
  # should generalize the rapids_cpm_* functions to support this natively.
  include(${rapids-cmake-dir}/cpm/package_override.cmake)
  rapids_cpm_package_override(${CMAKE_CURRENT_LIST_DIR}/cmake/spdlog_override.cmake)
  rapids_cpm_spdlog(
    FMT_OPTION ${RAPIDS_LOGGER_FMT_OPTION} CPM_ARGS OPTIONS "BUILD_SHARED_LIBS OFF"
                                           "SPDLOG_BUILD_SHARED OFF"
  )
  set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_link_options(rapids_logger PRIVATE "LINKER:--exclude-libs,libspdlog")
else()
  rapids_cpm_spdlog(
    FMT_OPTION ${RAPIDS_LOGGER_FMT_OPTION}
    BUILD_EXPORT_SET rapids-logger-exports
    INSTALL_EXPORT_SET
      rapids-logger-exports CPM_ARGS OPTIONS "BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS}"
                            "SPDLOG_BUILD_SHARED ${BUILD_SHARED_LIBS}"
  )
  if(spdlog_ADDED)
    set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
  endif()
endif()
target_link_libraries(rapids_logger PRIVATE spdlog::spdlog)

if(BUILD_TESTS)
  include(CTest)
  add_subdirectory(tests)
endif()

rapids_cmake_install_lib_dir(lib_dir)
install(
  TARGETS rapids_logger
  DESTINATION ${lib_dir}
  EXPORT rapids-logger-exports
)
install(DIRECTORY ${RAPIDS_LOGGER_SOURCE_DIR}/include/rapids_logger
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Both the function to create macros and the template must be installed so that they are available
# to use from the installed package. TODO: This file should probably go into share and not be right
# alongside the CMake file in lib, but this is easiest for now
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/logger_macros.hpp.in
        DESTINATION "${lib_dir}/cmake/rapids_logger"
)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/create_logger_macros.cmake
        DESTINATION "${lib_dir}/cmake/rapids_logger"
)

# cmake-lint: disable=E1126
file(COPY_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/logger_macros.hpp.in"
     "${CMAKE_CURRENT_BINARY_DIR}/logger_macros.hpp.in"
)
file(COPY_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/create_logger_macros.cmake"
     "${CMAKE_CURRENT_BINARY_DIR}/create_logger_macros.cmake"
)
# For e.g. CPM builds from source we need to make sure that the function is available.
include(cmake/create_logger_macros.cmake)

set(doc_string
    [=[
Provide targets for the rapids-logger library.

rapids-logger provides an ABI stable interface to spdlog-like logging that can
be safely embedded into complex environments where exposing spdlog symbols or
having it as a public dependency makes stable environment difficult to create
or maintain safely.

Imported Targets
^^^^^^^^^^^^^^^^

If rapids_logger is found, this module defines the following IMPORTED GLOBAL
targets:

 rapids_logger::rapids_logger             - The rapids_logger library.

    ]=]
)

set(code_string
    [=[
include("${CMAKE_CURRENT_LIST_DIR}/create_logger_macros.cmake")
]=]
)

rapids_export(
  INSTALL rapids_logger
  EXPORT_SET rapids-logger-exports
  GLOBAL_TARGETS rapids_logger
  NAMESPACE rapids_logger::
  DOCUMENTATION doc_string
  FINAL_CODE_BLOCK code_string
)
rapids_export(
  BUILD rapids_logger
  EXPORT_SET rapids-logger-exports
  GLOBAL_TARGETS rapids_logger
  NAMESPACE rapids_logger::
  DOCUMENTATION doc_string
  FINAL_CODE_BLOCK code_string
)
