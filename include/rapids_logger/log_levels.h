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
