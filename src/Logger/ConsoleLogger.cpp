#include <Logger/ConsoleLogger.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace logger {

ConsoleLogger::ConsoleLogger()
    : logLevel_(LogLevel::Info)
    , running_(false)
    , localDisplayEnabled_(true)  // Console logger defaults to enabled
{
}

ConsoleLogger::~ConsoleLogger() {
    if (running_.load()) {
        stop();
    }
}

ConsoleLogger::ConsoleLogger(ConsoleLogger&& other) noexcept
    : logLevel_(other.logLevel_.load())
    , running_(other.running_.load())
    , localDisplayEnabled_(other.localDisplayEnabled_.load())
{
    other.running_ = false;
}

ConsoleLogger& ConsoleLogger::operator=(ConsoleLogger&& other) noexcept {
    if (this != &other) {
        if (running_.load()) {
            stop();
        }
        
        logLevel_ = other.logLevel_.load();
        running_ = other.running_.load();
        localDisplayEnabled_ = other.localDisplayEnabled_.load();
        
        other.running_ = false;
    }
    return *this;
}

void ConsoleLogger::logDebug(std::string_view message) {
    log(LogLevel::Debug, message);
}

void ConsoleLogger::logInfo(std::string_view message) {
    log(LogLevel::Info, message);
}

void ConsoleLogger::logWarning(std::string_view message) {
    log(LogLevel::Warning, message);
}

void ConsoleLogger::logError(std::string_view message) {
    log(LogLevel::Error, message);
}

void ConsoleLogger::logCritical(std::string_view message) {
    log(LogLevel::Critical, message);
}

void ConsoleLogger::log(LogLevel level, std::string_view message) {
    if (shouldLog(level)) {
        logMessage(level, message);
    }
}

void ConsoleLogger::setLogLevel(LogLevel level) {
    logLevel_.store(level);
}

LogLevel ConsoleLogger::getLogLevel() const {
    return logLevel_.load();
}

bool ConsoleLogger::start() {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (!running_.load()) {
        running_.store(true);
        return true;
    }
    return true; // Already running
}

void ConsoleLogger::stop() {
    std::lock_guard<std::mutex> lock(logMutex_);
    running_.store(false);
}

bool ConsoleLogger::isRunning() const {
    return running_.load();
}

void ConsoleLogger::setLocalDisplay(bool enabled) {
    localDisplayEnabled_.store(enabled);
}

bool ConsoleLogger::isLocalDisplayEnabled() const {
    return localDisplayEnabled_.load();
}

void ConsoleLogger::logMessage(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    if (localDisplayEnabled_.load()) {
        std::string formattedMessage = formatMessage(level, message);
        std::cout << formattedMessage << std::endl;
    }
}

std::string ConsoleLogger::formatMessage(LogLevel level, std::string_view message) const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << " [" << logLevelToString(level) << "] " << message;
    
    return oss.str();
}

std::string ConsoleLogger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO";
        case LogLevel::Warning:  return "WARNING";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

bool ConsoleLogger::shouldLog(LogLevel level) const {
    return static_cast<int>(level) >= static_cast<int>(logLevel_.load());
}

} // namespace logger