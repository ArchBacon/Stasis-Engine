#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Stasis
{
/**
 * Logger class for outputting color-coded messages to the console using spdlog.
 *
 * Usage:
 *   DECLARE_LOG_CATEGORY(LogName);  // In header
 *   DEFINE_LOG_CATEGORY(LogName);   // In source
 *   LogName->Info("Value: {}", x);  // Logging call
 *
 * Log levels:
 * - Trace: Grey   — for detailed tracing (e.g., function calls or step-by-step flow).
 * - Info:  Green  — for general informative messages.
 * - Warn:  Yellow — for warnings that aren't fatal but might require attention.
 * - Error: Red    — for serious issues that require immediate attention.
 *
 * Each logger is tied to a named category shown in the console. Use the DECLARE/DEFINE macros
 * to create named loggers per system or module.
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
