#include <Logger/NetworkLogger.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <cstring>
#include <errno.h>

namespace logger {

NetworkLogger::NetworkLogger(const std::string& remoteHost, int remotePort)
    : remoteHost_(remoteHost)
    , remotePort_(remotePort)
    , logLevel_(LogLevel::Info)
    , running_(false)
    , localDisplayEnabled_(false)  // Network logger defaults to disabled local display
    , connected_(false)
    , timestampEnabled_(true)      // Timestamps enabled by default
    , socketFd_(-1)
{
    // Initialize server address structure
    memset(&serverAddr_, 0, sizeof(serverAddr_));
    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(remotePort_);
    
    if (inet_pton(AF_INET, remoteHost_.c_str(), &serverAddr_.sin_addr) <= 0) {
        // Invalid IP address format
        serverAddr_.sin_addr.s_addr = INADDR_NONE;
    }
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
    , timestampEnabled_(other.timestampEnabled_.load())
    , socketFd_(other.socketFd_)
    , serverAddr_(other.serverAddr_)
{
    other.running_ = false;
    other.connected_ = false;
    other.socketFd_ = -1;
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
        timestampEnabled_ = other.timestampEnabled_.load();
        socketFd_ = other.socketFd_;
        serverAddr_ = other.serverAddr_;
        
        other.running_ = false;
        other.connected_ = false;
        other.socketFd_ = -1;
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

void NetworkLogger::setTimestampEnabled(bool enabled) {
    timestampEnabled_.store(enabled);
}

bool NetworkLogger::isTimestampEnabled() const {
    return timestampEnabled_.load();
}

void NetworkLogger::logMessage(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    std::string formattedMessage = formatMessage(level, message);
    
    // Try to send to remote server
    if (connected_.load()) {
        if (!sendToRemote(formattedMessage)) {
            // Connection lost, try to reconnect once
            connected_.store(false);
            if (reconnect()) {
                connected_.store(true);
                // Retry sending the message
                if (!sendToRemote(formattedMessage)) {
                    connected_.store(false);
                }
            }
        }
    } else if (running_.load()) {
        // Not connected but logger is running, try to reconnect
        if (reconnect()) {
            connected_.store(true);
            sendToRemote(formattedMessage);
        }
    }
    
    // Local display if enabled (for debugging or fallback)
    if (localDisplayEnabled_.load()) {
        std::cout << "[NETWORK] " << formattedMessage << std::endl;
    }
}

std::string NetworkLogger::formatMessage(LogLevel level, std::string_view message) const {
    std::ostringstream oss;
    
    if (timestampEnabled_.load()) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << " ";
    }
    
    oss << "[" << logLevelToString(level) << "] " << message;
    
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
    if (remoteHost_.empty() || remotePort_ <= 0 || serverAddr_.sin_addr.s_addr == INADDR_NONE) {
        return false;
    }
    
    // Create TCP socket
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ < 0) {
        if (localDisplayEnabled_.load()) {
            std::cerr << "[NETWORK] Failed to create socket: " << strerror(errno) << std::endl;
        }
        return false;
    }
    
    // Set socket options for better reliability
    int optval = 1;
    setsockopt(socketFd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    
    // Connect to remote server
    if (connect(socketFd_, (struct sockaddr*)&serverAddr_, sizeof(serverAddr_)) < 0) {
        if (localDisplayEnabled_.load()) {
            std::cerr << "[NETWORK] Failed to connect to " << remoteHost_ << ":" << remotePort_ 
                     << " - " << strerror(errno) << std::endl;
        }
        close(socketFd_);
        socketFd_ = -1;
        return false;
    }
    
    if (localDisplayEnabled_.load()) {
        std::cout << "[NETWORK] Connected to " << remoteHost_ << ":" << remotePort_ << std::endl;
    }
    
    return true;
}

void NetworkLogger::disconnectFromRemote() {
    if (socketFd_ >= 0) {
        close(socketFd_);
        socketFd_ = -1;
        
        if (localDisplayEnabled_.load()) {
            std::cout << "[NETWORK] Disconnected from " << remoteHost_ << ":" << remotePort_ << std::endl;
        }
    }
}

bool NetworkLogger::sendToRemote(const std::string& message) {
    if (socketFd_ < 0) {
        return false;
    }
    
    // Add newline for proper message termination
    std::string messageWithNewline = message + "\n";
    const char* data = messageWithNewline.c_str();
    size_t totalBytes = messageWithNewline.length();
    size_t bytesSent = 0;
    
    // Send all data (handle partial sends)
    while (bytesSent < totalBytes) {
        ssize_t result = send(socketFd_, data + bytesSent, totalBytes - bytesSent, MSG_NOSIGNAL);
        
        if (result < 0) {
            if (errno == EPIPE || errno == ECONNRESET) {
                // Connection lost, mark as disconnected
                connected_.store(false);
                if (localDisplayEnabled_.load()) {
                    std::cerr << "[NETWORK] Connection lost: " << strerror(errno) << std::endl;
                }
                return false;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Temporary failure, retry
                continue;
            } else {
                // Other error
                if (localDisplayEnabled_.load()) {
                    std::cerr << "[NETWORK] Send error: " << strerror(errno) << std::endl;
                }
                return false;
            }
        } else if (result == 0) {
            // Connection closed by peer
            connected_.store(false);
            if (localDisplayEnabled_.load()) {
                std::cerr << "[NETWORK] Connection closed by peer" << std::endl;
            }
            return false;
        }
        
        bytesSent += result;
    }
    
    return true;
}

bool NetworkLogger::reconnect() {
    disconnectFromRemote();
    return connectToRemote();
}

} // namespace logger