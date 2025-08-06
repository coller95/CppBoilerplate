#include "logger.h"
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <sys/select.h>
#include <thread>
#include <unistd.h>

namespace {
    constexpr int LogBufferSize = 2048;
    constexpr int ReadBufferSize = 512;
    constexpr int FlushTimeoutSeconds = 1;
    constexpr int SelectTimeoutMicroseconds = 100000; // 100ms
}

/**
 * Private implementation class for Logger (PIMPL idiom)
 */
class Logger::Impl {
private:
    std::string _serverIp;
    int _serverPort;
    int _logSocket;
    struct sockaddr_in _serverAddress;
    bool _isConnected;
    bool _interceptStdStreams;
    bool _localDisplay;
    
    /**
     * Initializes the socket connection to the log server
     */
    void initializeSocket();
    
    /**
     * Sends a raw message to the log server
     * @param message The message to send
     */
    void sendMessage(const char* message);
    
    /**
     * Sends a message to both local display and TCP server
     * @param message The message to send
     */
    void sendDualMessage(const char* message);
    
    /**
     * Intercepts file descriptor output and forwards to logger
     * @param fd File descriptor to intercept (1 for stdout, 2 for stderr)
     * @param tag Tag to prefix intercepted messages with
     */
    void interceptFileDescriptor(int fd, const char* tag);
    
    /**
     * Sends initial connection information to the log server
     */
    void sendConnectionInfo();

public:
    /**
     * Constructor
     */
    Impl(std::string_view serverIp, int serverPort);
    
    /**
     * Destructor
     */
    ~Impl();
    
    /**
     * Starts the logger connection
     * @return true if successful
     */
    bool start();
    
    /**
     * Stops the logger and closes connection
     */
    void stop();
    
    /**
     * Checks connection status
     * @return true if connected
     */
    bool isConnected() const;
    
    /**
     * Logs a formatted message
     * @param format printf-style format string
     * @param args variable arguments
     */
    void logFormatted(const char* format, va_list args);
    
    /**
     * Logs a simple message with level prefix
     * @param level Log level prefix
     * @param message The message to log
     */
    void logWithLevel(const char* level, std::string_view message);
    
    /**
     * Sets stream interception preference
     * @param intercept whether to intercept std streams
     */
    void setInterceptStdStreams(bool intercept);
    
    /**
     * Sets local display preference
     * @param enable whether to display logs locally
     */
    void setLocalDisplay(bool enable);
    
    /**
     * Gets local display setting
     * @return true if local display is enabled
     */
    bool getLocalDisplay() const;
};

Logger::Impl::Impl(std::string_view serverIp, int serverPort)
    : _serverIp(serverIp)
    , _serverPort(serverPort)
    , _logSocket(-1)
    , _isConnected(false)
    , _interceptStdStreams(true)
    , _localDisplay(true) {
    std::memset(&_serverAddress, 0, sizeof(_serverAddress));
}

Logger::Impl::~Impl() {
    stop();
}

void Logger::Impl::initializeSocket() {
    _logSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_logSocket < 0) {
        return;
    }
    
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(static_cast<uint16_t>(_serverPort));
    _serverAddress.sin_addr.s_addr = inet_addr(_serverIp.c_str());
    
    if (connect(_logSocket, reinterpret_cast<struct sockaddr*>(&_serverAddress), 
                sizeof(_serverAddress)) < 0) {
        close(_logSocket);
        _logSocket = -1;
        return;
    }
    
    _isConnected = true;
}

bool Logger::Impl::start() {
    if (_isConnected) {
        return true;
    }
    
    initializeSocket();
    
    if (_isConnected) {
        sendConnectionInfo();
        
        if (_interceptStdStreams) {
            interceptFileDescriptor(1, "STDOUT");
            interceptFileDescriptor(2, "STDERR");
        }
    }
    
    return _isConnected;
}

void Logger::Impl::stop() {
    if (_isConnected && _logSocket >= 0) {
        close(_logSocket);
        _logSocket = -1;
        _isConnected = false;
    }
}

bool Logger::Impl::isConnected() const {
    return _isConnected;
}

void Logger::Impl::sendMessage(const char* message) {
    // Send to TCP server if connected
    if (_isConnected && _logSocket >= 0) {
        ssize_t messageLength = static_cast<ssize_t>(std::strlen(message));
        ssize_t sent = send(_logSocket, message, static_cast<size_t>(messageLength), 0);
        if (sent > 0) {
            send(_logSocket, "", 1, 0); // Send null terminator
        }
    }
}

void Logger::Impl::sendDualMessage(const char* message) {
    // Always display locally if enabled
    if (_localDisplay) {
        std::cout << message << std::flush;
    }
    
    // Send to TCP server if connected
    sendMessage(message);
}

void Logger::Impl::logFormatted(const char* format, va_list args) {
    char logBuffer[LogBufferSize];
    vsnprintf(logBuffer, sizeof(logBuffer), format, args);
    sendDualMessage(logBuffer);
}

void Logger::Impl::logWithLevel(const char* level, std::string_view message) {
    char logBuffer[LogBufferSize];
    snprintf(logBuffer, sizeof(logBuffer), "[%s] %.*s\n", 
             level, static_cast<int>(message.length()), message.data());
    sendDualMessage(logBuffer);
}

void Logger::Impl::setInterceptStdStreams(bool intercept) {
    _interceptStdStreams = intercept;
}

void Logger::Impl::setLocalDisplay(bool enable) {
    _localDisplay = enable;
}

bool Logger::Impl::getLocalDisplay() const {
    return _localDisplay;
}

void Logger::Impl::interceptFileDescriptor(int fd, const char* tag) {
    int pipeFileDescriptors[2];
    if (pipe(pipeFileDescriptors) != 0) {
        return;
    }
    
    int savedFileDescriptor = dup(fd);
    dup2(pipeFileDescriptors[1], fd);
    close(pipeFileDescriptors[1]);
    
    std::thread([this, readFd = pipeFileDescriptors[0], savedFd = savedFileDescriptor, 
                 tagStr = std::string(tag)]() {
        char buffer[ReadBufferSize];
        std::string currentLine;
        
        // Make read file descriptor non-blocking
        int flags = fcntl(readFd, F_GETFL, 0);
        fcntl(readFd, F_SETFL, flags | O_NONBLOCK);
        
        fd_set readFileDescriptors;
        struct timeval timeout;
        time_t lastFlushTime = time(nullptr);
        
        bool isRunning = true;
        while (isRunning) {
            FD_ZERO(&readFileDescriptors);
            FD_SET(readFd, &readFileDescriptors);
            
            timeout.tv_sec = 0;
            timeout.tv_usec = SelectTimeoutMicroseconds;
            
            int selectResult = select(readFd + 1, &readFileDescriptors, nullptr, nullptr, &timeout);
            
            if (selectResult > 0) {
                ssize_t bytesRead = read(readFd, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    for (ssize_t i = 0; i < bytesRead; ++i) {
                        if (buffer[i] == '\n') {
                            // Display locally if enabled (restored file descriptor)
                            if (_localDisplay) {
                                write(savedFd, currentLine.c_str(), currentLine.length());
                                write(savedFd, "\n", 1);
                            }
                            // Send to TCP server if connected (intercepted messages already displayed locally)
                            char tcpBuffer[LogBufferSize];
                            snprintf(tcpBuffer, sizeof(tcpBuffer), "%.*s\n", 
                                   static_cast<int>(currentLine.length()), currentLine.data());
                            sendMessage(tcpBuffer);
                            currentLine.clear();
                        } else {
                            currentLine += buffer[i];
                        }
                    }
                    lastFlushTime = time(nullptr);
                } else if (bytesRead <= 0) {
                    isRunning = false;
                }
            } else {
                // Check if we need to flush due to timeout
                if (!currentLine.empty() && 
                    (time(nullptr) - lastFlushTime >= FlushTimeoutSeconds)) {
                    // Display locally if enabled
                    if (_localDisplay) {
                        write(savedFd, currentLine.c_str(), currentLine.length());
                        write(savedFd, "\n", 1);
                    }
                    // Send to TCP server if connected (intercepted messages already displayed locally)
                    char tcpBuffer[LogBufferSize];
                    snprintf(tcpBuffer, sizeof(tcpBuffer), "[%s] %.*s\n", 
                           tagStr.c_str(), static_cast<int>(currentLine.length()), currentLine.data());
                    sendMessage(tcpBuffer);
                    currentLine.clear();
                    lastFlushTime = time(nullptr);
                }
            }
        }
        
        // Flush any remaining content
        if (!currentLine.empty()) {
            // Display locally if enabled
            if (_localDisplay) {
                write(savedFd, currentLine.c_str(), currentLine.length());
                write(savedFd, "\n", 1);
            }
            // Send to TCP server if connected (intercepted messages already displayed locally)
            char tcpBuffer[LogBufferSize];
            snprintf(tcpBuffer, sizeof(tcpBuffer), "[%s] %.*s\n", 
                   tagStr.c_str(), static_cast<int>(currentLine.length()), currentLine.data());
            sendMessage(tcpBuffer);
        }
        
        close(readFd);
        
        // Restore original file descriptor
        int targetFd = (tagStr == "STDERR") ? 2 : 1;
        dup2(savedFd, targetFd);
        close(savedFd);
    }).detach();
}

void Logger::Impl::sendConnectionInfo() {
    // Send connection message with timestamp
    time_t currentTime = time(nullptr);
    char timestampBuffer[64];
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timestampBuffer, sizeof(timestampBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    
    // Get machine details
    char hostname[256] = {0};
    gethostname(hostname, sizeof(hostname));
    
    const char* username = getenv("USER");
    if (!username) {
        username = "unknown";
    }
    
    // Log connection with machine details
    logWithLevel("SYSTEM", std::string("Connected to log server at ") + timestampBuffer);
    
    char machineInfo[512];
    snprintf(machineInfo, sizeof(machineInfo), "Host: %s, User: %s", hostname, username);
    logWithLevel("SYSTEM", machineInfo);
    
    // Try to get CPU information
    FILE* cpuInfoFile = fopen("/proc/cpuinfo", "r");
    if (cpuInfoFile) {
        char line[512];
        while (fgets(line, sizeof(line), cpuInfoFile)) {
            if (strncmp(line, "model name", 10) == 0) {
                char* newlineChar = strchr(line, '\n');
                if (newlineChar) {
                    *newlineChar = '\0';
                }
                char* cpuName = strchr(line, ':');
                if (cpuName) {
                    cpuName += 2; // Skip ': '
                    char cpuInfo[512];
                    snprintf(cpuInfo, sizeof(cpuInfo), "CPU: %s", cpuName);
                    logWithLevel("SYSTEM", cpuInfo);
                }
                break;
            }
        }
        fclose(cpuInfoFile);
    }
}

// Logger public interface implementation

Logger::Logger(std::string_view serverIp, int serverPort)
    : _impl(std::make_unique<Impl>(serverIp, serverPort)) {
}

Logger::~Logger() = default;

Logger::Logger(Logger&&) noexcept = default;
Logger& Logger::operator=(Logger&&) noexcept = default;

bool Logger::start() {
    return _impl->start();
}

void Logger::stop() {
    _impl->stop();
}

bool Logger::isConnected() const {
    return _impl->isConnected();
}

void Logger::log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    _impl->logFormatted(format, args);
    va_end(args);
}

void Logger::logInfo(std::string_view message) {
    _impl->logWithLevel("INFO", message);
}

void Logger::logWarning(std::string_view message) {
    _impl->logWithLevel("WARNING", message);
}

void Logger::logError(std::string_view message) {
    _impl->logWithLevel("ERROR", message);
}

void Logger::logDebug(std::string_view message) {
    _impl->logWithLevel("DEBUG", message);
}

void Logger::setInterceptStdStreams(bool intercept) {
    _impl->setInterceptStdStreams(intercept);
}

void Logger::setLocalDisplay(bool enable) {
    _impl->setLocalDisplay(enable);
}

bool Logger::getLocalDisplay() const {
    return _impl->getLocalDisplay();
}
