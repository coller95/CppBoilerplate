#pragma once

#include <gmock/gmock.h>
#include <string_view>
#include "Logger.h"

/**
 * Google Mock for ILogger for unit testing
 */
class MockLogger : public ILogger {
public:
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isConnected, (), (const, override));
    MOCK_METHOD(void, logInfo, (std::string_view message), (override));
    MOCK_METHOD(void, logWarning, (std::string_view message), (override));
    MOCK_METHOD(void, logError, (std::string_view message), (override));
    MOCK_METHOD(void, logDebug, (std::string_view message), (override));
    MOCK_METHOD(void, setInterceptStdStreams, (bool intercept), (override));
    MOCK_METHOD(void, setLocalDisplay, (bool enable), (override));
    MOCK_METHOD(bool, getLocalDisplay, (), (const, override));
};
