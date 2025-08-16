#pragma once

#include <string>
#include <string_view>

namespace logger {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
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