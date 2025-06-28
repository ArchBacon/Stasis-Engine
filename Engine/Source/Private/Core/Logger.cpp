#include "Core/Logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

Stasis::Logger::Logger(const std::string& Name)
{
    spdlog::set_pattern("%^ %n [%T] %v%$");
    Log = spdlog::stdout_color_mt(Name);
    Log->set_level(spdlog::level::trace);
}
