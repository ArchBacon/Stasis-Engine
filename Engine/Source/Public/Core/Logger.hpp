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
    std::shared_ptr<spdlog::logger> Log {};
    
public:
    explicit Logger(const std::string& Name);

    template <typename... TArgs>
    void Trace(const std::string& Format, TArgs&&... Args);

    template <typename... TArgs>
    void Info(const std::string& Format, TArgs&&... Args);

    template <typename... TArgs>
    void Warn(const std::string& Format, TArgs&&... Args);

    template <typename... TArgs>
    void Error(const std::string& Format, TArgs&&... Args);
};

template <typename... TArgs>
void Logger::Trace(const std::string& Format, TArgs&&... Args)
{
#ifndef NDEBUG
    Log->trace(fmt::runtime(Format), std::forward<TArgs>(Args)...);
#endif
}

template <typename... TArgs>
void Logger::Info(const std::string& Format, TArgs&&... Args)
{
#ifndef NDEBUG
    Log->info(fmt::runtime(Format), std::forward<TArgs>(Args)...);
#endif
}

template <typename... TArgs>
void Logger::Warn(const std::string& Format, TArgs&&... Args)
{
#ifndef NDEBUG
    Log->warn(fmt::runtime(Format), std::forward<TArgs>(Args)...);
#endif
}

template <typename... TArgs>
void Logger::Error(const std::string& Format, TArgs&&... Args)
{
#ifndef NDEBUG        
    Log->error(fmt::runtime(Format), std::forward<TArgs>(Args)...);
#endif
}
}

#define DECLARE_LOG_CATEGORY(CategoryName) \
    extern std::unique_ptr<Stasis::Logger> (CategoryName);

#define DEFINE_LOG_CATEGORY(CategoryName) \
    std::unique_ptr<Stasis::Logger> CategoryName = std::make_unique<Stasis::Logger>(#CategoryName)
