/*
 * Copyright (c) 2023-2025, NVIDIA CORPORATION.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <logger.hpp>

#include <memory>
#include <string>

struct LoggerTest : public ::testing::Test {
  LoggerTest()
    : oss{}, logger_{"logger_test", {std::make_shared<rapids_logger::ostream_sink_mt>(oss)}}
  {
    logger_.set_pattern("%v");
  }
  ~LoggerTest() override { logger_.sinks().pop_back(); }

  void clear_sink() { oss.str(""); }
  std::string sink_content() { return oss.str(); }

  std::ostringstream oss;
  rapids_logger::logger logger_;
};

TEST_F(LoggerTest, Basic)
{
  logger_.critical("crit msg");
  ASSERT_EQ(this->sink_content(), "crit msg\n");
}

TEST_F(LoggerTest, DefaultLevel)
{
  logger_.trace("trace");
  logger_.debug("debug");
  logger_.info("info");
  logger_.warn("warn");
  logger_.error("error");
  logger_.critical("critical");
  ASSERT_EQ(this->sink_content(), "info\nwarn\nerror\ncritical\n");
}

TEST_F(LoggerTest, CustomLevel)
{
  logger_.set_level(rapids_logger::level_enum::warn);
  logger_.info("info");
  logger_.warn("warn");
  ASSERT_EQ(this->sink_content(), "warn\n");

  this->clear_sink();

  logger_.set_level(rapids_logger::level_enum::debug);
  logger_.trace("trace");
  logger_.debug("debug");
  ASSERT_EQ(this->sink_content(), "debug\n");
}
