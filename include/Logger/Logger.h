
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "Logger/ILogger.h"


namespace logger {

/**
 * Network-based logger that sends log messages to a TCP server
 * and optionally intercepts stdout/stderr for comprehensive logging
 */
class Logger : public ILogger {
private:
    class Impl;
    std::unique_ptr<Impl> _impl;

public:
    /**
     * Constructs a Logger with the specified server connection
     * @param serverIp IP address of the log server
     * @param serverPort Port number of the log server
     */
    Logger(std::string_view serverIp, int serverPort);

    /**
     * Logs a formatted message (printf-style)
     * @param format Format string
     * @param ... Arguments for format string
     */
    void log(std::string_view format, ...) override;

    /**
     * Destructor - ensures proper cleanup
     */
    ~Logger() override;

    bool start() override;
    void stop() override;
    bool isConnected() const override;
    void logInfo(std::string_view message) override;
    void logWarning(std::string_view message) override;
    void logError(std::string_view message) override;
    void logDebug(std::string_view message) override;
    void setInterceptStdStreams(bool intercept) override;
    void setLocalDisplay(bool enable) override;
    bool getLocalDisplay() const override;

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    // Allow move constructor and assignment
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
};

} // namespace logger