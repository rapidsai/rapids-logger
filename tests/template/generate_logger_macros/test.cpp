/*
 * Copyright (c) 2025, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "logger_macros.hpp"

#include <rapids_logger/log_levels.h>
#include <rapids_logger/logger.hpp>

#include <iostream>
#include <memory>
#include <sstream>

inline std::ostringstream& default_stream()
{
  static std::ostringstream oss;
  return oss;
}

inline rapids_logger::logger& default_logger()
{
  static rapids_logger::logger logger_ = []() {
    rapids_logger::logger logger_{
      "LOGGER_TEST",
      {static_cast<rapids_logger::sink_ptr>(
        std::make_shared<rapids_logger::ostream_sink_mt>(default_stream()))}};
    logger_.set_pattern("%v");
    return logger_;
  }();
  return logger_;
}

int main()
{
  RAPIDS_TEST_LOG_TRACE("trace");
  RAPIDS_TEST_LOG_DEBUG("debug");
  RAPIDS_TEST_LOG_INFO("info");
  RAPIDS_TEST_LOG_WARN("warn");
  RAPIDS_TEST_LOG_ERROR("error");
  RAPIDS_TEST_LOG_CRITICAL("critical");
  std::ostringstream expected;
  if (RAPIDS_TEST_LOG_ACTIVE_LEVEL <= RAPIDS_LOGGER_LOG_LEVEL_TRACE) { expected << "trace\n"; }
  if (RAPIDS_TEST_LOG_ACTIVE_LEVEL <= RAPIDS_LOGGER_LOG_LEVEL_DEBUG) { expected << "debug\n"; }
  if (RAPIDS_TEST_LOG_ACTIVE_LEVEL <= RAPIDS_LOGGER_LOG_LEVEL_INFO) { expected << "info\n"; }
  if (RAPIDS_TEST_LOG_ACTIVE_LEVEL <= RAPIDS_LOGGER_LOG_LEVEL_WARN) { expected << "warn\n"; }
  if (RAPIDS_TEST_LOG_ACTIVE_LEVEL <= RAPIDS_LOGGER_LOG_LEVEL_ERROR) { expected << "error\n"; }
  if (RAPIDS_TEST_LOG_ACTIVE_LEVEL <= RAPIDS_LOGGER_LOG_LEVEL_CRITICAL) {
    expected << "critical\n";
  }
  if (default_stream().str() == expected.str()) {
    return 0;
  } else {
    // Print to make debugging easier with verbose ctest outputs in case of failure.
    std::cout << "The log output is: " << default_stream().str() << std::endl;
    return 1;
  }
}
