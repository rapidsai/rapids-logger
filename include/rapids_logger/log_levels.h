/*
 * SPDX-FileCopyrightText: Copyright (c) 2025, NVIDIA CORPORATION.
 * SPDX-License-Identifier: Apache-2.0
 */

// This is a C rather than C++ header to support usage of these macros in
// C-only wrappers around C++ libraries that leverage rapids_logger.
#pragma once

// These values must be kept in sync with spdlog!
#define RAPIDS_LOGGER_LOG_LEVEL_TRACE    0
#define RAPIDS_LOGGER_LOG_LEVEL_DEBUG    1
#define RAPIDS_LOGGER_LOG_LEVEL_INFO     2
#define RAPIDS_LOGGER_LOG_LEVEL_WARN     3
#define RAPIDS_LOGGER_LOG_LEVEL_ERROR    4
#define RAPIDS_LOGGER_LOG_LEVEL_CRITICAL 5
#define RAPIDS_LOGGER_LOG_LEVEL_OFF      6
