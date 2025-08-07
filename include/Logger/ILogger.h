#pragma once

#include <string_view>

namespace logger {

/**
 * Logger interface for TDD and mocking
 */
class ILogger {
public:
    virtual ~ILogger() = default;

    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isConnected() const = 0;
    virtual void logInfo(std::string_view message) = 0;
    virtual void logWarning(std::string_view message) = 0;
    virtual void logError(std::string_view message) = 0;
    virtual void logDebug(std::string_view message) = 0;
    virtual void setInterceptStdStreams(bool intercept) = 0;
    virtual void setLocalDisplay(bool enable) = 0;
    virtual bool getLocalDisplay() const = 0;
};

} // namespace logger
