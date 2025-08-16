#pragma once

#include <Logger/ILogger.h>
#include <vector>
#include <memory>
#include <string_view>
#include <mutex>

namespace logger {

/**
 * CompositeLogger - Composite pattern for multiple logger implementations
 * 
 * Allows logging to multiple destinations simultaneously (console, network, file, etc.)
 * Implements ILogger interface for seamless integration with existing code.
 */
class CompositeLogger : public ILogger {
public:
    /**
     * Default constructor
     */
    CompositeLogger();

    /**
     * Destructor - ensures proper cleanup of all loggers
     */
    ~CompositeLogger() override = default;

    /**
     * Add a logger implementation to the composite
     */
    void addLogger(std::shared_ptr<ILogger> logger);

    /**
     * Remove a specific logger implementation
     */
    void removeLogger(std::shared_ptr<ILogger> logger);

    /**
     * Clear all loggers
     */
    void clearLoggers();

    /**
     * Get count of registered loggers
     */
    size_t getLoggerCount() const;

    // ILogger interface implementation - delegates to all registered loggers
    void logDebug(std::string_view message) override;
    void logInfo(std::string_view message) override;
    void logWarning(std::string_view message) override;
    void logError(std::string_view message) override;
    void logCritical(std::string_view message) override;
    
    void log(LogLevel level, std::string_view message) override;
    
    void setLogLevel(LogLevel level) override;
    LogLevel getLogLevel() const override;
    
    bool start() override;
    void stop() override;
    bool isRunning() const override;
    
    void setLocalDisplay(bool enabled) override;
    bool isLocalDisplayEnabled() const override;
    
    void setTimestampEnabled(bool enabled) override;
    bool isTimestampEnabled() const override;

    // Delete copy constructor and assignment operator
    CompositeLogger(const CompositeLogger&) = delete;
    CompositeLogger& operator=(const CompositeLogger&) = delete;
    
    // Allow move constructor and assignment
    CompositeLogger(CompositeLogger&&) noexcept;
    CompositeLogger& operator=(CompositeLogger&&) noexcept;

private:
    std::vector<std::shared_ptr<ILogger>> loggers_;
    mutable std::mutex loggersMutex_;
    LogLevel logLevel_;
    bool localDisplayEnabled_;
    bool timestampEnabled_;
};

} // namespace logger