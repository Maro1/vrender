#pragma once

#include "spdlog/spdlog.h"

#define V_LOG_LEVEL_DEBUG spdlog::level::debug
#define V_LOG_LEVEL_INFO spdlog::level::info
#define V_LOG_LEVEL_WARNING spdlog::level::warn
#define V_LOG_LEVEL_ERROR spdlog::level::err

#define V_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define V_LOG_INFO(...)  spdlog::info(__VA_ARGS__)
#define V_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define V_LOG_WARNING(...) spdlog::warn(__VA_ARGS__)

#define V_LOG_LEVEL(lvl) spdlog::set_level(lvl)


