#pragma once

#include <gmock/gmock.h>
#include <Logger/ILogger.h>
#include <cstdarg>
#include <cstdio>

namespace logger {

/**
 * Mock implementation of ILogger for testing
 */
class MockLogger : public ILogger {
public:
    // Connection management
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
    
    // Logging methods
    MOCK_METHOD(void, logInfo, (std::string_view message), (override));
    MOCK_METHOD(void, logWarning, (std::string_view message), (override));
    MOCK_METHOD(void, logError, (std::string_view message), (override));
    MOCK_METHOD(void, logDebug, (std::string_view message), (override));
    
    // Printf-style logging method (variadic functions can't be mocked directly)
    void log(const char* format, ...) override {
        constexpr size_t BufferSize = 1024;
        char buffer[BufferSize];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, BufferSize, format, args);
        va_end(args);
        logInfo(buffer);  // Delegate to the mockable method
    }
    
    // Configuration methods
    MOCK_METHOD(void, setInterceptStdStreams, (bool intercept), (override));
    MOCK_METHOD(void, setLocalDisplay, (bool enable), (override));
    MOCK_METHOD(bool, getLocalDisplay, (), (const, override));
};

} // namespace logger
