/*
 * SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rapids_logger/logger.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

struct LoggerTest : public ::testing::Test {
  LoggerTest()
    : oss{}, logger_{"logger_test", {std::make_shared<rapids_logger::ostream_sink_mt>(oss)}}
  {
    // Remove extra formatting to simplify validation of outputs.
    logger_.set_pattern("%v");
  }

  void clear_sink() { oss.str(""); }
  std::string sink_content() { return oss.str(); }

  std::ostringstream oss;
  rapids_logger::logger logger_;
};

TEST_F(LoggerTest, DefaultLevel)
{
  // The default level should not change without suitable warning to consumers.
  EXPECT_EQ(logger_.level(), rapids_logger::level_enum::info);
}

TEST_F(LoggerTest, DefaultLevelLogs)
{
  logger_.trace("trace");
  logger_.debug("debug");
  logger_.info("info");
  logger_.warn("warn");
  logger_.error("error");
  logger_.critical("critical");
  EXPECT_EQ(this->sink_content(), "info\nwarn\nerror\ncritical\n");
}

TEST_F(LoggerTest, DefaultLevelLogsLogFunction)
{
  logger_.log(rapids_logger::level_enum::trace, "trace");
  logger_.log(rapids_logger::level_enum::debug, "debug");
  logger_.log(rapids_logger::level_enum::info, "info");
  logger_.log(rapids_logger::level_enum::warn, "warn");
  logger_.log(rapids_logger::level_enum::error, "error");
  logger_.log(rapids_logger::level_enum::critical, "critical");
  EXPECT_EQ(this->sink_content(), "info\nwarn\nerror\ncritical\n");
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

std::string logged = "";
void example_callback(int lvl, const char* msg) { logged = std::string(msg); }

bool check_if_logged(rapids_logger::logger const& logger,
                     const std::string& msg,
                     rapids_logger::level_enum log_level_def)
{
  bool actually_logged  = logged.find(msg) != std::string::npos;
  bool should_be_logged = logger.level() <= log_level_def;
  return actually_logged == should_be_logged;
}

TEST_F(LoggerTest, TwoSinks)
{
  std::ostringstream oss2;
  logger_.sinks().push_back(std::make_shared<rapids_logger::ostream_sink_mt>(oss2));
  logger_.set_pattern("%v");

  logger_.info("info");
  EXPECT_EQ(this->sink_content(), "info\n");
  EXPECT_EQ(oss2.str(), "info\n");
}

TEST_F(LoggerTest, CallbackSink)
{
  std::string msg;
  logger_.sinks().clear();
  logger_.sinks().push_back(std::make_shared<rapids_logger::callback_sink_mt>(example_callback));

  msg = "This is a critical message";
  logger_.critical(msg.c_str());
  EXPECT_TRUE(check_if_logged(logger_, msg, rapids_logger::level_enum::critical));

  msg = "This is an error message";
  logger_.error(msg.c_str());
  EXPECT_TRUE(check_if_logged(logger_, msg, rapids_logger::level_enum::error));

  msg = "This is a warning message";
  logger_.warn(msg.c_str());
  EXPECT_TRUE(check_if_logged(logger_, msg, rapids_logger::level_enum::warn));

  msg = "This is an info message";
  logger_.info(msg.c_str());
  EXPECT_TRUE(check_if_logged(logger_, msg, rapids_logger::level_enum::info));

  msg = "This is a debug message";
  logger_.debug(msg.c_str());
  EXPECT_TRUE(check_if_logged(logger_, msg, rapids_logger::level_enum::debug));

  msg = "This is a trace message";
  logger_.trace(msg.c_str());
  EXPECT_TRUE(check_if_logged(logger_, msg, rapids_logger::level_enum::trace));
}

int flush_count = 0;
void example_flush() { ++flush_count; }

TEST_F(LoggerTest, FlushTest)
{
  logger_.sinks().clear();
  logger_.sinks().push_back(
    std::make_shared<rapids_logger::callback_sink_mt>(example_callback, example_flush));
  logger_.flush();
  EXPECT_EQ(1, flush_count);
}

TEST_F(LoggerTest, NullSink)
{
  logger_.info("info");
  logger_.critical("critical");
  EXPECT_EQ(this->sink_content(), "info\ncritical\n");
  this->clear_sink();
  logger_.sinks().clear();
  logger_.sinks().push_back(std::make_shared<rapids_logger::null_sink_mt>());
  logger_.info("info");
  logger_.critical("critical");
  EXPECT_EQ(this->sink_content(), "");
}

TEST_F(LoggerTest, LogLevelSetter)
{
  {
    rapids_logger::log_level_setter setter{logger_, rapids_logger::level_enum::trace};
    logger_.info("trace");
    logger_.critical("critical");
    EXPECT_EQ(this->sink_content(), "trace\ncritical\n");
  }
  this->clear_sink();
  {
    rapids_logger::log_level_setter setter{logger_, rapids_logger::level_enum::off};
    logger_.info("trace");
    logger_.critical("critical");
    EXPECT_EQ(this->sink_content(), "");
  }
}
