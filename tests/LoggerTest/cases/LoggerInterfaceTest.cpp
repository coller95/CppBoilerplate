#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Logger/Logger.h>
#include "../MockLogger.h"

using ::testing::_;
using ::testing::Return;

namespace {

class LoggerInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockModule = std::make_unique<logger::MockLogger>();
    }

    std::unique_ptr<logger::MockLogger> mockModule;
};

} // anonymous namespace

TEST_F(LoggerInterfaceTest, ImplementsInterface) {
    // Verify that Logger implements ILogger
    logger::Logger module("127.0.0.1", 8080);
    logger::ILogger* interface = &module;
    EXPECT_NE(interface, nullptr);
}

TEST_F(LoggerInterfaceTest, MockCanBeUsedForTesting) {
    // Example of how to use the mock for testing other components
    EXPECT_CALL(*mockModule, start())
        .WillOnce(Return(true));
    EXPECT_CALL(*mockModule, isConnected())
        .WillOnce(Return(true));
    EXPECT_CALL(*mockModule, logInfo("Test message"));
    
    bool result = mockModule->start();
    EXPECT_TRUE(result);
    EXPECT_TRUE(mockModule->isConnected());
    mockModule->logInfo("Test message");
}

TEST_F(LoggerInterfaceTest, MockSupportsAllLoggingMethods) {
    EXPECT_CALL(*mockModule, logInfo("Info message"));
    EXPECT_CALL(*mockModule, logWarning("Warning message"));
    EXPECT_CALL(*mockModule, logError("Error message"));
    EXPECT_CALL(*mockModule, logDebug("Debug message"));
    
    mockModule->logInfo("Info message");
    mockModule->logWarning("Warning message");
    mockModule->logError("Error message");
    mockModule->logDebug("Debug message");
}

TEST_F(LoggerInterfaceTest, MockSupportsConfiguration) {
    EXPECT_CALL(*mockModule, setLocalDisplay(false));
    EXPECT_CALL(*mockModule, getLocalDisplay())
        .WillOnce(Return(false));
    EXPECT_CALL(*mockModule, setInterceptStdStreams(true));
    
    mockModule->setLocalDisplay(false);
    EXPECT_FALSE(mockModule->getLocalDisplay());
    mockModule->setInterceptStdStreams(true);
}
