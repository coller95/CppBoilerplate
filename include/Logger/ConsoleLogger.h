#pragma once

#include <Logger/ILogger.h>
#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>

namespace logger {

/**
 * ConsoleLogger - Console output implementation
 * 
 * Provides thread-safe logging to console/stdout with configurable levels.
 * Implements ILogger interface for dependency injection and testability.
 */
class ConsoleLogger : public ILogger {
public:
	/**
	 * Default constructor
	 */
	ConsoleLogger();

	/**
	 * Destructor - ensures proper cleanup
	 */
	~ConsoleLogger() override;

	// ILogger interface implementation
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

	// Delete copy constructor and assignment operator (RAII best practice)
	ConsoleLogger(const ConsoleLogger&) = delete;
	ConsoleLogger& operator=(const ConsoleLogger&) = delete;
	
	// Allow move constructor and assignment
	ConsoleLogger(ConsoleLogger&&) noexcept;
	ConsoleLogger& operator=(ConsoleLogger&&) noexcept;

private:
	std::atomic<LogLevel> logLevel_;
	std::atomic<bool> running_;
	std::atomic<bool> localDisplayEnabled_;
	std::atomic<bool> timestampEnabled_;
	mutable std::mutex logMutex_;
	
	void logMessage(LogLevel level, std::string_view message);
	std::string formatMessage(LogLevel level, std::string_view message) const;
	std::string logLevelToString(LogLevel level) const;
	bool shouldLog(LogLevel level) const;
};

} // namespace logger