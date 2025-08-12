#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "Logger/ILogger.h"

namespace logger {

/**
 * Logger - Network-enabled logging module with local display support
 * 
 * Provides logging functionality with both local console output and 
 * remote network logging capabilities. Supports different log levels
 * and configurable output options.
 */
class Logger : public ILogger {
public:
    /**
     * Constructor with server configuration
     * @param serverIp IP address of the logging server
     * @param serverPort Port of the logging server
     */
    Logger(std::string_view serverIp, int serverPort);

    /**
     * Destructor - ensures proper cleanup
     */
    ~Logger() override;

    // Connection management
    bool start() override;
    void stop() override;
    bool isConnected() const override;
    
    // Logging methods
    void logInfo(std::string_view message) override;
    void logWarning(std::string_view message) override;
    void logError(std::string_view message) override;
    void logDebug(std::string_view message) override;
    
    // Printf-style logging method (implements interface)
    void log(const char* format, ...) override;
    
    // Configuration methods
    void setInterceptStdStreams(bool intercept) override;
    void setLocalDisplay(bool enable) override;
    bool getLocalDisplay() const override;

    // Delete copy constructor and assignment operator (RAII best practice)
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Allow move constructor and assignment
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;

private:
    // PIMPL idiom for implementation details
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace logger
