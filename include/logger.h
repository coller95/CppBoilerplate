#pragma once

#include <memory>
#include <string>
#include <string_view>

/**
 * Network-based logger that sends log messages to a TCP server
 * and optionally intercepts stdout/stderr for comprehensive logging
 */
class Logger {
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
     * Destructor - ensures proper cleanup
     */
    ~Logger();
    
    /**
     * Starts the logger and establishes connection to server
     * Optionally intercepts stdout/stderr if connection succeeds
     * @return true if connection was established successfully
     */
    bool start();
    
    /**
     * Stops the logger and closes connection
     */
    void stop();
    
    /**
     * Checks if the logger is connected to the server
     * @return true if connected, false otherwise
     */
    bool isConnected() const;
    
    /**
     * Logs a formatted message to the server
     * @param format printf-style format string
     * @param ... variable arguments for the format string
     */
    void log(const char* format, ...) __attribute__((format(printf, 2, 3)));
    
    /**
     * Logs an informational message
     * @param message The message to log
     */
    void logInfo(std::string_view message);
    
    /**
     * Logs a warning message
     * @param message The warning message to log
     */
    void logWarning(std::string_view message);
    
    /**
     * Logs an error message
     * @param message The error message to log
     */
    void logError(std::string_view message);
    
    /**
     * Logs a debug message
     * @param message The debug message to log
     */
    void logDebug(std::string_view message);
    
    /**
     * Sets whether to intercept stdout and stderr
     * Must be called before start() to take effect
     * @param intercept true to intercept stdout/stderr, false otherwise
     */
    void setInterceptStdStreams(bool intercept);
    
    /**
     * Sets whether to display logs locally (to console) in addition to TCP
     * @param enable true to enable local display, false to disable
     */
    void setLocalDisplay(bool enable);
    
    /**
     * Gets the current local display setting
     * @return true if local display is enabled
     */
    bool getLocalDisplay() const;
    
    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Allow move constructor and assignment
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
};
