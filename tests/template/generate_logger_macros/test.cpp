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

#include <rapids_logger/logger.hpp>

#include <iostream>
#include <sstream>
#include <string>

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
  std::string msg{"Hello, World!"};
  auto expected = msg + "\n";
  RAPIDS_TEST_LOG_INFO(msg);
  if (default_stream().str() == expected) {
    return 0;
  } else {
    std::cerr << "Expected: " << expected << std::endl;
    std::cerr << "Actual: " << default_stream().str() << std::endl;
    return 1;
  }
}
