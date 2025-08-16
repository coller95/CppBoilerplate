#include <Logger/NetworkLogger.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace logger {

NetworkLogger::NetworkLogger(const std::string& remoteHost, int remotePort)
    : remoteHost_(remoteHost)
    , remotePort_(remotePort)
    , logLevel_(LogLevel::Info)
    , running_(false)
    , localDisplayEnabled_(false)  // Network logger defaults to disabled local display
    , connected_(false)
{
}

NetworkLogger::~NetworkLogger() {
    if (running_.load()) {
        stop();
    }
}

NetworkLogger::NetworkLogger(NetworkLogger&& other) noexcept
    : remoteHost_(std::move(other.remoteHost_))
    , remotePort_(other.remotePort_)
    , logLevel_(other.logLevel_.load())
    , running_(other.running_.load())
    , localDisplayEnabled_(other.localDisplayEnabled_.load())
    , connected_(other.connected_.load())
{
    other.running_ = false;
    other.connected_ = false;
}

NetworkLogger& NetworkLogger::operator=(NetworkLogger&& other) noexcept {
    if (this != &other) {
        if (running_.load()) {
            stop();
        }
        
        remoteHost_ = std::move(other.remoteHost_);
        remotePort_ = other.remotePort_;
        logLevel_ = other.logLevel_.load();
        running_ = other.running_.load();
        localDisplayEnabled_ = other.localDisplayEnabled_.load();
        connected_ = other.connected_.load();
        
        other.running_ = false;
        other.connected_ = false;
    }
    return *this;
}

void NetworkLogger::logDebug(std::string_view message) {
    log(LogLevel::Debug, message);
}

void NetworkLogger::logInfo(std::string_view message) {
    log(LogLevel::Info, message);
}

void NetworkLogger::logWarning(std::string_view message) {
    log(LogLevel::Warning, message);
}

void NetworkLogger::logError(std::string_view message) {
    log(LogLevel::Error, message);
}

void NetworkLogger::logCritical(std::string_view message) {
    log(LogLevel::Critical, message);
}

void NetworkLogger::log(LogLevel level, std::string_view message) {
    if (shouldLog(level)) {
        logMessage(level, message);
    }
}

void NetworkLogger::setLogLevel(LogLevel level) {
    logLevel_.store(level);
}

LogLevel NetworkLogger::getLogLevel() const {
    return logLevel_.load();
}

bool NetworkLogger::start() {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (!running_.load()) {
        running_.store(true);
        // Try to connect to remote server
        connected_.store(connectToRemote());
        return true;
    }
    return true; // Already running
}

void NetworkLogger::stop() {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (running_.load()) {
        running_.store(false);
        if (connected_.load()) {
            disconnectFromRemote();
            connected_.store(false);
        }
    }
}

bool NetworkLogger::isRunning() const {
    return running_.load();
}

void NetworkLogger::setLocalDisplay(bool enabled) {
    localDisplayEnabled_.store(enabled);
}

bool NetworkLogger::isLocalDisplayEnabled() const {
    return localDisplayEnabled_.load();
}

void NetworkLogger::logMessage(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    std::string formattedMessage = formatMessage(level, message);
    
    // Try to send to remote server
    if (connected_.load()) {
        if (!sendToRemote(formattedMessage)) {
            // If remote send fails, optionally fallback to local display
            connected_.store(false);
        }
    }
    
    // Local display if enabled (for debugging or fallback)
    if (localDisplayEnabled_.load()) {
        std::cout << "[NETWORK] " << formattedMessage << std::endl;
    }
}

std::string NetworkLogger::formatMessage(LogLevel level, std::string_view message) const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << " [" << logLevelToString(level) << "] " << message;
    
    return oss.str();
}

std::string NetworkLogger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARNING";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

bool NetworkLogger::shouldLog(LogLevel level) const {
    return static_cast<int>(level) >= static_cast<int>(logLevel_.load());
}

bool NetworkLogger::connectToRemote() {
    // TODO: Implement actual network connection (TCP/UDP socket)
    // For now, simulate connection
    if (remoteHost_.empty() || remotePort_ <= 0) {
        return false;
    }
    
    // Simulate connection attempt
    // In real implementation: create socket, connect to remoteHost_:remotePort_
    return true; // Assume success for now
}

void NetworkLogger::disconnectFromRemote() {
    // TODO: Implement actual network disconnection
    // For now, just cleanup simulation
}

bool NetworkLogger::sendToRemote(const std::string& message) {
    // TODO: Implement actual network send (TCP/UDP)
    // For now, simulate send
    (void)message; // Suppress unused parameter warning
    
    // In real implementation: send message over socket
    // Return false if send fails (connection lost, etc.)
    return true; // Assume success for now
}

} // namespace logger