#pragma once

#include <gmock/gmock.h>
#include <Logger/ILogger.h>

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
    
    // Configuration methods
    MOCK_METHOD(void, setInterceptStdStreams, (bool intercept), (override));
    MOCK_METHOD(void, setLocalDisplay, (bool enable), (override));
    MOCK_METHOD(bool, getLocalDisplay, (), (const, override));
};

} // namespace logger
