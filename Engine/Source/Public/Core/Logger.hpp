#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace blackbox
{
/**
 * Logger class for outputting color-coded messages to the console using spdlog.
 *
 * Usage:
 *   inline auto LogName = std::make_unique<blackbox::Logger>("LogName");
 *   LogName->Info("Value: {}", x);  // Logging call
 *   LogName [13:31:58] Value: 10  // Example output
 *
 * Log levels:
 * - Trace: Grey   — for detailed tracing (e.g., function calls or step-by-step flow).
 * - Info:  Green  — for general informative messages.
 * - Warn:  Yellow — for warnings that aren't fatal but might require attention.
 * - Error: Red    — for serious issues that require immediate attention.
 */
class Logger
{
    std::shared_ptr<spdlog::logger> log {};
    
public:
    explicit Logger(const std::string& name);

    template <typename... TArgs>
    void Trace(const std::string& format, TArgs&&... args);

    template <typename... TArgs>
    void Info(const std::string& format, TArgs&&... args);

    template <typename... TArgs>
    void Warn(const std::string& format, TArgs&&... args);

    template <typename... TArgs>
    void Error(const std::string& format, TArgs&&... args);
};

template <typename... TArgs>
void Logger::Trace(const std::string& format, TArgs&&... args)
{
#ifndef NDEBUG
    log->trace(fmt::runtime(format), std::forward<TArgs>(args)...);
#endif
}

template <typename... Args>
void Logger::Info(const std::string& format, Args&&... args)
{
#ifndef NDEBUG
    log->info(fmt::runtime(format), std::forward<Args>(args)...);
#endif
}

template <typename... Args>
void Logger::Warn(const std::string& format, Args&&... args)
{
#ifndef NDEBUG
    log->warn(fmt::runtime(format), std::forward<Args>(args)...);
#endif
}

template <typename... Args>
void Logger::Error(const std::string& format, Args&&... args)
{
#ifndef NDEBUG        
    log->error(fmt::runtime(format), std::forward<Args>(args)...);
#endif
}
}
