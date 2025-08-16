#pragma once

#include <string_view>

namespace logger {

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

class ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void logDebug(std::string_view message) = 0;
    virtual void logInfo(std::string_view message) = 0;
    virtual void logWarning(std::string_view message) = 0;
    virtual void logError(std::string_view message) = 0;
    virtual void logCritical(std::string_view message) = 0;
    
    virtual void log(LogLevel level, std::string_view message) = 0;
    
    virtual void setLogLevel(LogLevel level) = 0;
    virtual LogLevel getLogLevel() const = 0;
    
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    virtual void setLocalDisplay(bool enabled) = 0;
    virtual bool isLocalDisplayEnabled() const = 0;
};

} // namespace logger