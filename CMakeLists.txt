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

# cmake-lint: disable=R0915

include_guard(GLOBAL)

cmake_minimum_required(VERSION 3.26.4 FATAL_ERROR)

include(rapids_config.cmake)

include(rapids-cmake)
include(rapids-cpm)
include(rapids-export)

project(
  RAPIDS_LOGGER
  # TODO: Determine appropriate versioning.
  VERSION 0.0.1
  LANGUAGES CXX)

rapids_cmake_build_type(Release)

rapids_cpm_init()

option(RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS "Build and link to spdlog in a way that maximizes all symbol hiding" ON)

include(CMakeDependentOption)
# If we are hiding all spdlog symbols then we need to use the bundled fmt library.
cmake_dependent_option(RAPIDS_LOGGER_FMT_OPTION "The fmt option to use when building spdlog." "EXTERNAL_FMT_HO" "NOT RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS" "BUNDLED")

# Get spdlog
if(RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS)
  set(CPM_DOWNLOAD_spdlog ON)
  #set(args "EXCLUDE_FROM_ALL ON OPTIONS \"BUILD_SHARED_LIBS OFF\" \"SPDLOG_BUILD_SHARED OFF\"")
else()
  message(FATAL_ERROR "RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS must be set to ON")
endif()
include(${rapids-cmake-dir}/cpm/spdlog.cmake)
rapids_cpm_spdlog(FMT_OPTION ${RAPIDS_LOGGER_FMT_OPTION}
    # TODO: Figure out how to get rid of this. We should never install these
    BUILD_EXPORT_SET rapids-logger-exports
    INSTALL_EXPORT_SET rapids-logger-exports
    # TODO: I don't really expect this to work with the way that
    # rapids_cpm_spdlog is set up right now, it's going to require
    # some tweaking of argument forwarding I suspect.
    CPM_ARGS
    EXCLUDE_FROM_ALL ON
    OPTIONS "BUILD_SHARED_LIBS OFF"
            "SPDLOG_BUILD_SHARED OFF"
 )

add_library(
    rapids_logger
    src/logger.cpp
)
add_library(rapids_logger::rapids_logger ALIAS rapids_logger)
target_include_directories(
  rapids_logger
  PUBLIC "$<BUILD_INTERFACE:${RAPIDS_LOGGER_SOURCE_DIR}/include>"
         "$<INSTALL_INTERFACE:include>")
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

target_link_libraries(rapids_logger PRIVATE spdlog::spdlog)
if(RAPIDS_LOGGER_HIDE_ALL_SPDLOG_SYMBOLS)
   target_link_options(rapids_logger PRIVATE "LINKER:--exclude-libs,libspdlog")
endif()

rapids_cmake_install_lib_dir(lib_dir)
install(
  TARGETS rapids_logger
  DESTINATION ${lib_dir}
  EXPORT rapids-logger-exports
)
install(DIRECTORY ${RAPIDS_LOGGER_SOURCE_DIR}/include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

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
rapids_export(
  INSTALL rapids_logger
  EXPORT_SET rapids-logger-exports
  GLOBAL_TARGETS rapids_logger
  NAMESPACE rapids_logger::
  DOCUMENTATION doc_string
)
rapids_export(
  BUILD rapids_logger
  EXPORT_SET rapids-logger-exports
  GLOBAL_TARGETS rapids_logger
  NAMESPACE rapids_logger::
  DOCUMENTATION doc_string
)
