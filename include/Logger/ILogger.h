#pragma once

#include <string_view>

namespace logger {

/**
 * Interface for Logger
 * Define the contract that implementations must follow
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Connection management
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isConnected() const = 0;
    
    // Logging methods
    virtual void logInfo(std::string_view message) = 0;
    virtual void logWarning(std::string_view message) = 0;
    virtual void logError(std::string_view message) = 0;
    virtual void logDebug(std::string_view message) = 0;
    
    // Configuration methods
    virtual void setInterceptStdStreams(bool intercept) = 0;
    virtual void setLocalDisplay(bool enable) = 0;
    virtual bool getLocalDisplay() const = 0;
};

} // namespace logger
