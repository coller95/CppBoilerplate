#include <Logger/Logger.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace logger {

Logger::Logger(const std::string& remoteHost, int remotePort)
    : remoteHost_(remoteHost)
    , remotePort_(remotePort)
    , logLevel_(LogLevel::INFO)
    , running_(false)
    , localDisplayEnabled_(false)
{
}

Logger::~Logger() {
    if (running_.load()) {
        stop();
    }
}

Logger::Logger(Logger&& other) noexcept
    : remoteHost_(std::move(other.remoteHost_))
    , remotePort_(other.remotePort_)
    , logLevel_(other.logLevel_.load())
    , running_(other.running_.load())
    , localDisplayEnabled_(other.localDisplayEnabled_.load())
{
    other.running_ = false;
}

Logger& Logger::operator=(Logger&& other) noexcept {
    if (this != &other) {
        if (running_.load()) {
            stop();
        }
        
        remoteHost_ = std::move(other.remoteHost_);
        remotePort_ = other.remotePort_;
        logLevel_ = other.logLevel_.load();
        running_ = other.running_.load();
        localDisplayEnabled_ = other.localDisplayEnabled_.load();
        
        other.running_ = false;
    }
    return *this;
}

void Logger::logDebug(std::string_view message) {
    log(LogLevel::DEBUG, message);
}

void Logger::logInfo(std::string_view message) {
    log(LogLevel::INFO, message);
}

void Logger::logWarning(std::string_view message) {
    log(LogLevel::WARNING, message);
}

void Logger::logError(std::string_view message) {
    log(LogLevel::ERROR, message);
}

void Logger::logCritical(std::string_view message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::log(LogLevel level, std::string_view message) {
    if (shouldLog(level)) {
        logMessage(level, message);
    }
}

void Logger::setLogLevel(LogLevel level) {
    logLevel_.store(level);
}

LogLevel Logger::getLogLevel() const {
    return logLevel_.load();
}

bool Logger::start() {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (!running_.load()) {
        running_.store(true);
        return true;
    }
    return true; // Already running
}

void Logger::stop() {
    std::lock_guard<std::mutex> lock(logMutex_);
    running_.store(false);
}

bool Logger::isRunning() const {
    return running_.load();
}

void Logger::setLocalDisplay(bool enabled) {
    localDisplayEnabled_.store(enabled);
}

bool Logger::isLocalDisplayEnabled() const {
    return localDisplayEnabled_.load();
}

void Logger::logMessage(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    if (localDisplayEnabled_.load()) {
        std::string formattedMessage = formatMessage(level, message);
        std::cout << formattedMessage << std::endl;
    }
}

std::string Logger::formatMessage(LogLevel level, std::string_view message) const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << " [" << logLevelToString(level) << "] " << message;
    
    return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

bool Logger::shouldLog(LogLevel level) const {
    return static_cast<int>(level) >= static_cast<int>(logLevel_.load());
}

} // namespace logger
