#include <Logger/CompositeLogger.h>
#include <algorithm>

namespace logger {

CompositeLogger::CompositeLogger()
    : logLevel_(LogLevel::Info)
    , localDisplayEnabled_(true)
{
}

CompositeLogger::CompositeLogger(CompositeLogger&& other) noexcept
    : loggers_(std::move(other.loggers_))
    , logLevel_(other.logLevel_)
    , localDisplayEnabled_(other.localDisplayEnabled_)
{
}

CompositeLogger& CompositeLogger::operator=(CompositeLogger&& other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(loggersMutex_);
        loggers_ = std::move(other.loggers_);
        logLevel_ = other.logLevel_;
        localDisplayEnabled_ = other.localDisplayEnabled_;
    }
    return *this;
}

void CompositeLogger::addLogger(std::shared_ptr<ILogger> logger) {
    if (logger) {
        std::lock_guard<std::mutex> lock(loggersMutex_);
        loggers_.push_back(logger);
        
        // Sync the new logger with current settings
        logger->setLogLevel(logLevel_);
        logger->setLocalDisplay(localDisplayEnabled_);
    }
}

void CompositeLogger::removeLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    loggers_.erase(
        std::remove(loggers_.begin(), loggers_.end(), logger),
        loggers_.end()
    );
}

void CompositeLogger::clearLoggers() {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    loggers_.clear();
}

size_t CompositeLogger::getLoggerCount() const {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    return loggers_.size();
}

void CompositeLogger::logDebug(std::string_view message) {
    log(LogLevel::Debug, message);
}

void CompositeLogger::logInfo(std::string_view message) {
    log(LogLevel::Info, message);
}

void CompositeLogger::logWarning(std::string_view message) {
    log(LogLevel::Warning, message);
}

void CompositeLogger::logError(std::string_view message) {
    log(LogLevel::Error, message);
}

void CompositeLogger::logCritical(std::string_view message) {
    log(LogLevel::Critical, message);
}

void CompositeLogger::log(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    
    // Delegate to all registered loggers
    for (auto& logger : loggers_) {
        if (logger) {
            logger->log(level, message);
        }
    }
}

void CompositeLogger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    logLevel_ = level;
    
    // Apply to all registered loggers
    for (auto& logger : loggers_) {
        if (logger) {
            logger->setLogLevel(level);
        }
    }
}

LogLevel CompositeLogger::getLogLevel() const {
    return logLevel_;
}

bool CompositeLogger::start() {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    bool allStarted = true;
    
    // Start all registered loggers
    for (auto& logger : loggers_) {
        if (logger) {
            if (!logger->start()) {
                allStarted = false;
            }
        }
    }
    
    return allStarted;
}

void CompositeLogger::stop() {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    
    // Stop all registered loggers
    for (auto& logger : loggers_) {
        if (logger) {
            logger->stop();
        }
    }
}

bool CompositeLogger::isRunning() const {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    
    // Return true if any logger is running
    for (const auto& logger : loggers_) {
        if (logger && logger->isRunning()) {
            return true;
        }
    }
    
    return false;
}

void CompositeLogger::setLocalDisplay(bool enabled) {
    std::lock_guard<std::mutex> lock(loggersMutex_);
    localDisplayEnabled_ = enabled;
    
    // Apply to all registered loggers
    for (auto& logger : loggers_) {
        if (logger) {
            logger->setLocalDisplay(enabled);
        }
    }
}

bool CompositeLogger::isLocalDisplayEnabled() const {
    return localDisplayEnabled_;
}

} // namespace logger