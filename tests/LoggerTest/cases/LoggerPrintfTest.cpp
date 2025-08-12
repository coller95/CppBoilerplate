#include <gtest/gtest.h>
#include <Logger/Logger.h>
#include <Logger/ILogger.h>

namespace {

class LoggerPrintfTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create logger for testing
        logger = std::make_unique<logger::Logger>("127.0.0.1", 9000);
        logger->setLocalDisplay(true);
    }

    void TearDown() override {
        if (logger) {
            logger->stop();
        }
    }

    std::unique_ptr<logger::Logger> logger;
};

} // anonymous namespace

TEST_F(LoggerPrintfTest, CanLogWithFormattedStringNoArgs) {
    // Test simple string without formatting
    EXPECT_NO_THROW(logger->log("Simple log message"));
}

TEST_F(LoggerPrintfTest, CanLogWithFormattedStringWithArgs) {
    // Test formatted string with arguments
    EXPECT_NO_THROW(logger->log("User %s logged in with ID %d", "john", 123));
    EXPECT_NO_THROW(logger->log("Temperature: %.2f°C", 23.456));
    EXPECT_NO_THROW(logger->log("Processing %d of %d items (%d%%)", 75, 100, 75));
}

TEST_F(LoggerPrintfTest, CanLogWithDifferentTypes) {
    // Test various data types
    EXPECT_NO_THROW(logger->log("String: %s", "test"));
    EXPECT_NO_THROW(logger->log("Integer: %d", 42));
    EXPECT_NO_THROW(logger->log("Float: %f", 3.14159));
    EXPECT_NO_THROW(logger->log("Character: %c", 'A'));
    EXPECT_NO_THROW(logger->log("Hex: 0x%x", 255));
}

TEST_F(LoggerPrintfTest, CanLogWithMultipleArguments) {
    // Test with many arguments
    EXPECT_NO_THROW(logger->log("User: %s, Age: %d, Score: %.2f, Active: %s", 
                                "alice", 25, 98.5, "yes"));
}

TEST_F(LoggerPrintfTest, InterfaceHasLogMethod) {
    // Test that the interface method exists and works
    logger::ILogger* interface = logger.get();
    EXPECT_NO_THROW(interface->log("Interface test: %s %d", "value", 123));
}

TEST_F(LoggerPrintfTest, LogMethodHandlesEmptyString) {
    // Test edge case: empty format string
    EXPECT_NO_THROW(logger->log(""));
}

TEST_F(LoggerPrintfTest, LogMethodHandlesLongString) {
    // Test with a longer formatted string
    std::string longFormat = "This is a very long log message with multiple parameters: %s, %d, %f, %c, and more text to make it longer";
    EXPECT_NO_THROW(logger->log(longFormat.c_str(), "string_param", 12345, 67.89, 'X'));
}
