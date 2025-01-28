/*
 * Copyright (c) 2024, NVIDIA CORPORATION.
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

#pragma once

#include "log_levels.h"

#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

#define RAPIDS_LOGGER_EXPORT __attribute__((visibility("default")))

namespace rapids_logger {

/**
 * @brief The log levels supported by the logger.
 *
 * These levels correspond to the levels defined by spdlog.
 */
enum class RAPIDS_LOGGER_EXPORT level_enum : int32_t {
  trace    = RAPIDS_LOGGER_LOG_LEVEL_TRACE,
  debug    = RAPIDS_LOGGER_LOG_LEVEL_DEBUG,
  info     = RAPIDS_LOGGER_LOG_LEVEL_INFO,
  warn     = RAPIDS_LOGGER_LOG_LEVEL_WARN,
  error    = RAPIDS_LOGGER_LOG_LEVEL_ERROR,
  critical = RAPIDS_LOGGER_LOG_LEVEL_CRITICAL,
  off      = RAPIDS_LOGGER_LOG_LEVEL_OFF,
  n_levels
};

// Macro to define an operator for an enum class
#define DEFINE_ENUM_CLASS_OPERATOR(Enum, Op)              \
  constexpr bool operator Op(Enum lhs, Enum rhs)          \
  {                                                       \
    return static_cast<std::underlying_type_t<Enum>>(lhs) \
      Op static_cast<std::underlying_type_t<Enum>>(rhs);  \
  }

DEFINE_ENUM_CLASS_OPERATOR(level_enum, <);
DEFINE_ENUM_CLASS_OPERATOR(level_enum, <=);
DEFINE_ENUM_CLASS_OPERATOR(level_enum, >);
DEFINE_ENUM_CLASS_OPERATOR(level_enum, >=);

namespace detail {
// Forward declare the implementation classes.
class logger_impl;
class sink_impl;
}  // namespace detail

// Forward declare for the sink for the logger to use.
class RAPIDS_LOGGER_EXPORT sink;
using sink_ptr = std::shared_ptr<sink>;

/**
 * @class logger
 * @brief A logger class that either uses the real implementation (via spdlog) or performs no-ops
 * if not supported.
 */
class RAPIDS_LOGGER_EXPORT logger {
 public:
  logger()                         = delete;  ///< Not default constructible
  logger(logger const&)            = delete;  ///< Not copy constructible
  logger& operator=(logger const&) = delete;  ///< Not copy assignable

  logger(logger&& other);             ///< @default_move_constructor
  logger& operator=(logger&& other);  ///< @default_move_assignment{logger}

  /**
   * @brief A class to manage a vector of sinks.
   *
   * This class is used internally by the logger class to manage its sinks. It handles
   * synchronization of the sinks with the sinks in the underlying spdlog logger such that all
   * vector-like operations performed on this class are reflected in the underlying spdlog
   * logger's set of sinks.
   */
  class sink_vector {
   public:
    using Iterator      = std::vector<sink_ptr>::iterator;        ///< The iterator type
    using ConstIterator = std::vector<sink_ptr>::const_iterator;  ///< The const iterator type

    /**
     * @brief Construct a new sink_vector object
     *
     * @param parent The logger whose sinks are being managed
     * @param sinks The sinks to manage
     */
    explicit sink_vector(logger& parent, std::vector<sink_ptr> sinks = {})
      : parent{parent}, sinks_{sinks}
    {
    }

    /**
     * @brief Add a sink to the vector.
     *
     * @param sink The sink to add
     */
    void push_back(sink_ptr const& sink);

    /**
     * @brief Add a sink to the vector.
     *
     * @param sink The sink to add
     */
    void push_back(sink_ptr&& sink);

    /**
     * @brief Remove the last sink from the vector.
     */
    void pop_back();

    /**
     * @brief Remove all sinks from the vector.
     */
    void clear();

    /**
     * @brief Get an iterator to the beginning of the vector.
     *
     * @return Iterator The iterator
     */
    Iterator begin() { return sinks_.begin(); }

    /**
     * @brief Get an iterator to the end of the vector.
     *
     * @return Iterator The iterator
     */
    Iterator end() { return sinks_.end(); }

    /**
     * @brief Get a const iterator to the beginning of the vector.
     *
     * @return ConstIterator The const iterator
     */
    ConstIterator begin() const { return sinks_.begin(); }

    /**
     * @brief Get a const iterator to the end of the vector.
     *
     * @return ConstIterator The const iterator
     */
    ConstIterator end() const { return sinks_.end(); }

    /**
     * @brief Get a const iterator to the beginning of the vector.
     *
     * @return ConstIterator The const iterator
     */
    ConstIterator cbegin() const { return sinks_.cbegin(); }

    /**
     * @brief Get a const iterator to the end of the vector.
     *
     * @return ConstIterator The const iterator
     */
    ConstIterator cend() const { return sinks_.cend(); }

   private:
    logger& parent;                ///< The logger this vector belongs to
    std::vector<sink_ptr> sinks_;  ///< The sinks
  };

  // TODO: When we migrate to C++20 we can use std::format and format strings
  // instead of the printf-style printing used here.
  /**
   * @brief Format and log a message at the specified level.
   *
   * This function performs printf-style formatting to avoid the need for fmt
   * or spdlog's own templated APIs (which would require exposing spdlog
   * symbols publicly) and then invokes the base implementation with the
   * preformatted string.
   *
   * @param lvl The log level
   * @param format The format string
   * @param args The format arguments
   */
  template <typename... Args>
  void log(level_enum lvl, std::string const& format, Args&&... args)
  {
    auto convert_to_c_string = [](auto&& arg) -> decltype(auto) {
      using ArgType = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<ArgType, std::string>) {
        return arg.c_str();
      } else {
        return std::forward<decltype(arg)>(arg);
      }
    };

    // NOLINTBEGIN(cppcoreguidelines-pro-type-vararg)
    auto formatted_size =
      std::snprintf(nullptr, 0, format.c_str(), convert_to_c_string(std::forward<Args>(args))...);
    if (formatted_size < 0) { throw std::runtime_error("Error during formatting."); }
    if (formatted_size == 0) { log(lvl, {}); }
    auto size = static_cast<std::size_t>(formatted_size) + 1;  // for null terminator
    // NOLINTNEXTLINE(modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays)
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(
      buf.get(), size, format.c_str(), convert_to_c_string(std::forward<Args>(args))...);
    // NOLINTEND(cppcoreguidelines-pro-type-vararg)
    log(lvl, {buf.get(), buf.get() + size - 1});  // drop '\0'
  };

  /**
   * @brief Log a message at the TRACE level.
   *
   * @param format The format string
   * @param args The format arguments
   */
  template <typename... Args>
  void trace(std::string const& format, Args&&... args)
  {
    log(level_enum::trace, format, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a message at the DEBUG level.
   *
   * @param format The format string
   * @param args The format arguments
   */
  template <typename... Args>
  void debug(std::string const& format, Args&&... args)
  {
    log(level_enum::debug, format, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a message at the INFO level.
   *
   * @param format The format string
   * @param args The format arguments
   */
  template <typename... Args>
  void info(std::string const& format, Args&&... args)
  {
    log(level_enum::info, format, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a message at the WARN level.
   *
   * @param format The format string
   * @param args The format arguments
   */
  template <typename... Args>
  void warn(std::string const& format, Args&&... args)
  {
    log(level_enum::warn, format, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a message at the ERROR level.
   *
   * @param format The format string
   * @param args The format arguments
   */
  template <typename... Args>
  void error(std::string const& format, Args&&... args)
  {
    log(level_enum::error, format, std::forward<Args>(args)...);
  }

  /**
   * @brief Log a message at the CRITICAL level.
   *
   * @param format The format string
   * @param args The format arguments
   */
  template <typename... Args>
  void critical(std::string const& format, Args&&... args)
  {
    log(level_enum::critical, format, std::forward<Args>(args)...);
  }

  /**
   * @brief Construct a new logger object
   *
   * @param name The name of the logger
   * @param filename The name of the log file
   */
  logger(std::string name, std::string filename);

  /**
   * @brief Construct a new logger object
   *
   * @param name The name of the logger
   * @param stream The stream to log to
   */
  logger(std::string name, std::ostream& stream);

  /**
   * @brief Construct a new logger object
   *
   * @param name The name of the logger
   * @param sinks The sinks to log to
   *
   * Note that we must use a vector because initializer_lists are not flexible
   * enough to support programmatic construction in callers, and an
   * iterator-based API would require templating and thus exposing spdlog
   * types.
   */
  logger(std::string name, std::vector<sink_ptr> sinks);

  /**
   * @brief Destroy the logger object
   */
  ~logger();

  /**
   * @brief Log a message at the specified level.
   *
   * This is the core logging routine that dispatches to spdlog.
   *
   * @param lvl The log level
   * @param message The message to log
   */
  void log(level_enum lvl, std::string const& message);

  /**
   * @brief Get the sinks for the logger.
   *
   * @return The sinks
   */
  const sink_vector& sinks() const;

  /**
   * @brief Get the sinks for the logger.
   *
   * @return The sinks
   */
  sink_vector& sinks();

  /**
   * @brief Get the current log level.
   *
   * @return The current log level
   */
  level_enum level() const;

  /**
   * @brief Set the log level.
   *
   * @param log_level The new log level
   */
  void set_level(level_enum log_level);

  /**
   * @brief Flush the logger.
   */
  void flush();

  /**
   * @brief Flush all writes on the specified level or above.
   */
  void flush_on(level_enum log_level);

  /**
   * @brief Get the current flush level.
   */
  level_enum flush_level() const;

  /**
   * @brief Check if the logger should log a message at the specified level.
   *
   * @param msg_level The level of the message
   * @return true if the message should be logged, false otherwise
   */
  bool should_log(level_enum msg_level) const;

  /**
   * @brief Set the pattern for the logger.
   *
   * @param pattern The pattern to use
   */
  void set_pattern(std::string pattern);

 private:
  std::unique_ptr<detail::logger_impl> impl;  ///< The logger implementation
  sink_vector sinks_;                         ///< The sinks for the logger
};

/**
 * @brief A sink for the logger.
 *
 * These sinks are wrappers around the spdlog sinks that allow us to keep the
 * spdlog types private and avoid exposing them in the public API.
 */
class RAPIDS_LOGGER_EXPORT sink {
 public:
  ~sink();

 protected:
  explicit sink(std::unique_ptr<detail::sink_impl> impl);
  std::unique_ptr<detail::sink_impl> impl;
  // The sink vector needs to be able to pass the underlying sink to the spdlog logger.
  friend class logger::sink_vector;
};

/**
 * @brief A sink that writes to a file.
 *
 * See spdlog::sinks::basic_file_sink_mt for more information.
 */
class RAPIDS_LOGGER_EXPORT basic_file_sink_mt : public sink {
 public:
  basic_file_sink_mt(std::string const& filename, bool truncate = false);
};

/**
 * @brief A sink that writes to an ostream.
 *
 * See spdlog::sinks::ostream_sink_mt for more information.
 */
class RAPIDS_LOGGER_EXPORT ostream_sink_mt : public sink {
 public:
  ostream_sink_mt(std::ostream& stream, bool force_flush = false);
};

/**
 * @brief A sink that writes nothing.
 *
 * See spdlog::sinks::null_sink_mt for more information.
 */
class RAPIDS_LOGGER_EXPORT null_sink_mt : public sink {
 public:
  null_sink_mt();
};

/**
 * @brief A sink that writes to stderr.
 *
 * See spdlog::sinks::stderr_sink_mt for more information.
 */
class RAPIDS_LOGGER_EXPORT stderr_sink_mt : public sink {
 public:
  stderr_sink_mt();
};

typedef void (*log_callback_t)(int lvl, const char* msg);
typedef void (*flush_callback_t)();

/**
 * @brief A sink that executes a callback whenever a message is logged.
 */
class RAPIDS_LOGGER_EXPORT callback_sink_mt : public sink {
 public:
  explicit callback_sink_mt(const log_callback_t& callback,
                            const flush_callback_t& flush = nullptr);
};

/**
 * @brief An object used for scoped log level setting
 *
 * Instances will set the logging to the level indicated on construction and
 * will revert to the previous set level on destruction.
 */
struct RAPIDS_LOGGER_EXPORT log_level_setter {
  explicit log_level_setter(logger& logger_, level_enum level) : logger_{logger_}
  {
    prev_level_ = logger_.level();
    logger_.set_level(level);
  }
  ~log_level_setter() { logger_.set_level(prev_level_); }

 private:
  logger& logger_;
  level_enum prev_level_;
};

}  // namespace rapids_logger
