#pragma once

#include <Logger/ILogger.h>
#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace logger {

/**
 * NetworkLogger - Network/remote logging implementation
 * 
 * Provides thread-safe logging to remote server with configurable levels.
 * Implements ILogger interface for dependency injection and testability.
 */
class NetworkLogger : public ILogger {
public:
	/**
	 * Constructor with remote logging configuration
	 */
	NetworkLogger(const std::string& remoteHost, int remotePort);

	/**
	 * Destructor - ensures proper cleanup
	 */
	~NetworkLogger() override;

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
	NetworkLogger(const NetworkLogger&) = delete;
	NetworkLogger& operator=(const NetworkLogger&) = delete;
	
	// Allow move constructor and assignment
	NetworkLogger(NetworkLogger&&) noexcept;
	NetworkLogger& operator=(NetworkLogger&&) noexcept;

private:
	std::string remoteHost_;
	int remotePort_;
	std::atomic<LogLevel> logLevel_;
	std::atomic<bool> running_;
	std::atomic<bool> localDisplayEnabled_;
	std::atomic<bool> connected_;
	std::atomic<bool> timestampEnabled_;
	mutable std::mutex logMutex_;
	int socketFd_;
	struct sockaddr_in serverAddr_;
	
	void logMessage(LogLevel level, std::string_view message);
	std::string formatMessage(LogLevel level, std::string_view message) const;
	std::string logLevelToString(LogLevel level) const;
	bool shouldLog(LogLevel level) const;
	bool connectToRemote();
	void disconnectFromRemote();
	bool sendToRemote(const std::string& message);
	bool reconnect();
};

} // namespace logger