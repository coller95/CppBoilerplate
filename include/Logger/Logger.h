#pragma once

#include <Logger/ILogger.h>
#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>

namespace logger {

/**
 * Logger - Cross-cutting concern implementation
 * 
 * Provides thread-safe logging with configurable levels and output destinations.
 * Implements ILogger interface for dependency injection and testability.
 */
class Logger : public ILogger {
public:
	/**
	 * Constructor with remote logging configuration
	 */
	Logger(const std::string& remoteHost, int remotePort);

	/**
	 * Destructor - ensures proper cleanup
	 */
	~Logger() override;

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

	// Delete copy constructor and assignment operator (RAII best practice)
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	
	// Allow move constructor and assignment
	Logger(Logger&&) noexcept;
	Logger& operator=(Logger&&) noexcept;

private:
	std::string remoteHost_;
	int remotePort_;
	std::atomic<LogLevel> logLevel_;
	std::atomic<bool> running_;
	std::atomic<bool> localDisplayEnabled_;
	mutable std::mutex logMutex_;
	
	void logMessage(LogLevel level, std::string_view message);
	std::string formatMessage(LogLevel level, std::string_view message) const;
	std::string logLevelToString(LogLevel level) const;
	bool shouldLog(LogLevel level) const;
};

} // namespace logger
