#include <Logger/Logger.h>
#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <string_view>
#include <cstdarg>
#include <cstdio>
#include <sys/select.h>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>

namespace logger {

// Pimpl idiom implementation for Logger
class Logger::Impl {
public:
    std::string serverIp;
    int serverPort;
    int sockfd = -1;
    bool connected = false;
    bool interceptStdStreams = false;
    bool localDisplay = true;

    Impl(std::string_view ip, int port)
        : serverIp(ip), serverPort(port) {}

    ~Impl() {
        if (connected && sockfd != -1) {
            close(sockfd);
        }
    }
};

Logger::Logger(std::string_view serverIp, int serverPort)
    : _impl(std::make_unique<Impl>(serverIp, serverPort)) {}

Logger::~Logger() = default;

bool Logger::start() {
    if (_impl->connected) return true;
    _impl->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_impl->sockfd < 0) {
        _impl->connected = false;
        return false;
    }
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_impl->serverPort);
    if (inet_pton(AF_INET, _impl->serverIp.c_str(), &serv_addr.sin_addr) <= 0) {
        _impl->connected = false;
        return false;
    }
    if (connect(_impl->sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        _impl->connected = false;
        return false;
    }
    _impl->connected = true;
    return true;
}

void Logger::stop() {
    if (_impl->connected && _impl->sockfd != -1) {
        close(_impl->sockfd);
        _impl->sockfd = -1;
        _impl->connected = false;
    }
}

bool Logger::isConnected() const {
    return _impl->connected;
}

void Logger::logInfo(std::string_view message) {
    if (_impl->localDisplay) {
        std::cout << "[INFO] " << message << std::endl;
    }
    if (_impl->connected && _impl->sockfd != -1) {
        std::string msg = "INFO: " + std::string(message) + "\n";
        send(_impl->sockfd, msg.c_str(), msg.size(), 0);
    }
}

void Logger::logWarning(std::string_view message) {
    if (_impl->localDisplay) {
        std::cout << "[WARNING] " << message << std::endl;
    }
    if (_impl->connected && _impl->sockfd != -1) {
        std::string msg = "WARNING: " + std::string(message) + "\n";
        send(_impl->sockfd, msg.c_str(), msg.size(), 0);
    }
}

void Logger::logError(std::string_view message) {
    if (_impl->localDisplay) {
        std::cerr << "[ERROR] " << message << std::endl;
    }
    if (_impl->connected && _impl->sockfd != -1) {
        std::string msg = "ERROR: " + std::string(message) + "\n";
        send(_impl->sockfd, msg.c_str(), msg.size(), 0);
    }
}

void Logger::logDebug(std::string_view message) {
    if (_impl->localDisplay) {
        std::cout << "[DEBUG] " << message << std::endl;
    }
    if (_impl->connected && _impl->sockfd != -1) {
        std::string msg = "DEBUG: " + std::string(message) + "\n";
        send(_impl->sockfd, msg.c_str(), msg.size(), 0);
    }
}

void Logger::setInterceptStdStreams(bool intercept) {
    _impl->interceptStdStreams = intercept;
    // Not implemented: actual interception of std streams
}

void Logger::setLocalDisplay(bool enable) {
    _impl->localDisplay = enable;
}

bool Logger::getLocalDisplay() const {
    return _impl->localDisplay;
}

Logger::Logger(Logger&& other) noexcept = default;
Logger& Logger::operator=(Logger&& other) noexcept = default;

void Logger::log(std::string_view format, ...) {
    constexpr size_t BufferSize = 1024;
    char buffer[BufferSize];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BufferSize, format.data(), args);
    va_end(args);
    logInfo(buffer);
}

} // namespace logger
