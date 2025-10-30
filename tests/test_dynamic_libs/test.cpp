/*
 * SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
 * SPDX-License-Identifier: Apache-2.0
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
