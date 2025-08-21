#include "Logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>

blackbox::Logger::Logger(const std::string& name)
{
    spdlog::set_pattern("%^ %n [%T] %v%$");
    log = spdlog::stdout_color_mt(name);
    log->set_level(spdlog::level::trace);
}
