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

#include <rapids_logger/logger.hpp>

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

int main()
{
  std::ostringstream actual;
  rapids_logger::logger logger{"LOGGER_TEST",
                               {static_cast<rapids_logger::sink_ptr>(
                                 std::make_shared<rapids_logger::ostream_sink_mt>(actual))}};
  logger.set_pattern("%v");

  logger.trace("trace");
  logger.debug("debug");
  logger.info("info");
  logger.warn("warn");
  logger.error("error");
  logger.critical("critical");

  std::string expected{"info\nwarn\nerror\ncritical\n"};

  if (actual.str() == expected) {
    return 0;
  } else {
    // Print to make debugging easier with verbose ctest outputs in case of failure.
    std::cout << "The log output is: " << actual.str() << std::endl;
    return 1;
  }
}
