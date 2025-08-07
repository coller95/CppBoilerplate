
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockLogger.h"
#include "Logger/Logger.h"

namespace logger {

/**
 * Example test fixture for ILogger using Google Mock
 */
class LoggerTestFixture : public ::testing::Test {
protected:
    MockLogger mockLogger;
};

TEST_F(LoggerTestFixture, LogInfoCallsInterface) {
    EXPECT_CALL(mockLogger, logInfo(testing::StrEq("Test info message"))).Times(1);
    mockLogger.logInfo("Test info message");
}

TEST_F(LoggerTestFixture, LogErrorCallsInterface) {
    EXPECT_CALL(mockLogger, logError(testing::StrEq("Test error message"))).Times(1);
    mockLogger.logError("Test error message");
}

TEST_F(LoggerTestFixture, StartReturnsTrue) {
    EXPECT_CALL(mockLogger, start()).WillOnce(testing::Return(true));
    EXPECT_TRUE(mockLogger.start());
}

TEST_F(LoggerTestFixture, IsConnectedReturnsFalse) {
    EXPECT_CALL(mockLogger, isConnected()).WillOnce(testing::Return(false));
    EXPECT_FALSE(mockLogger.isConnected());
}

// Add more tests for other ILogger methods as needed

} // namespace logger
