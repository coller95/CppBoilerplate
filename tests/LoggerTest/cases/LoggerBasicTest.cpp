#include <gtest/gtest.h>
#include <Logger/Logger.h>
#include <sstream>
#include <iostream>

namespace {

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }

    void TearDown() override {
        // Cleanup code for each test
    }
};

} // anonymous namespace

TEST_F(LoggerTest, ConstructorCreatesValidInstance) {
    logger::Logger module("127.0.0.1", 8080);
    EXPECT_FALSE(module.isConnected()); // Should not be connected initially
}

TEST_F(LoggerTest, MoveConstructorWorks) {
    logger::Logger original("127.0.0.1", 8080);
    logger::Logger moved(std::move(original));
    
    // The moved logger should inherit the configuration
    EXPECT_FALSE(moved.isConnected()); // Should not be connected initially
}

TEST_F(LoggerTest, StartReturnsFalseWhenServerNotAvailable) {
    logger::Logger logger("127.0.0.1", 9999); // Use a port that's likely not in use
    EXPECT_FALSE(logger.start()); // Should fail to connect
    EXPECT_FALSE(logger.isConnected());
}

TEST_F(LoggerTest, StopWorksWhenNotConnected) {
    logger::Logger logger("127.0.0.1", 8080);
    logger.stop(); // Should not crash when not connected
    EXPECT_FALSE(logger.isConnected());
}

TEST_F(LoggerTest, LocalDisplayDefaultsToTrue) {
    logger::Logger logger("127.0.0.1", 8080);
    EXPECT_TRUE(logger.getLocalDisplay());
}

TEST_F(LoggerTest, LocalDisplayCanBeToggled) {
    logger::Logger logger("127.0.0.1", 8080);
    
    logger.setLocalDisplay(false);
    EXPECT_FALSE(logger.getLocalDisplay());
    
    logger.setLocalDisplay(true);
    EXPECT_TRUE(logger.getLocalDisplay());
}

TEST_F(LoggerTest, InterceptStdStreamsCanBeSet) {
    logger::Logger logger("127.0.0.1", 8080);
    // This should not crash - actual functionality implementation pending
    logger.setInterceptStdStreams(true);
    logger.setInterceptStdStreams(false);
}

TEST_F(LoggerTest, LoggingMethodsWorkWithoutConnection) {
    logger::Logger logger("127.0.0.1", 8080);
    logger.setLocalDisplay(false); // Disable local display for testing
    
    // These should not crash when not connected
    logger.logInfo("Test info message");
    logger.logWarning("Test warning message");
    logger.logError("Test error message");
    logger.logDebug("Test debug message");
    logger.log("Test formatted message: %d", 42);
}

TEST_F(LoggerTest, LoggingWithLocalDisplayEnabled) {
    logger::Logger logger("127.0.0.1", 8080);
    logger.setLocalDisplay(true);
    
    // Redirect cout to capture output for testing
    std::stringstream buffer;
    std::streambuf* orig_cout = std::cout.rdbuf();
    std::streambuf* orig_cerr = std::cerr.rdbuf();
    
    std::cout.rdbuf(buffer.rdbuf());
    std::cerr.rdbuf(buffer.rdbuf());
    
    logger.logInfo("Test info");
    logger.logWarning("Test warning");
    logger.logError("Test error");
    logger.logDebug("Test debug");
    
    // Restore original streams
    std::cout.rdbuf(orig_cout);
    std::cerr.rdbuf(orig_cerr);
    
    std::string output = buffer.str();
    EXPECT_NE(output.find("[INFO] Test info"), std::string::npos);
    EXPECT_NE(output.find("[WARNING] Test warning"), std::string::npos);
    EXPECT_NE(output.find("[ERROR] Test error"), std::string::npos);
    EXPECT_NE(output.find("[DEBUG] Test debug"), std::string::npos);
}

TEST_F(LoggerTest, LoggingWithLocalDisplayDisabled) {
    logger::Logger logger("127.0.0.1", 8080);
    logger.setLocalDisplay(false);
    
    // Redirect cout to capture output for testing
    std::stringstream buffer;
    std::streambuf* orig_cout = std::cout.rdbuf();
    std::streambuf* orig_cerr = std::cerr.rdbuf();
    
    std::cout.rdbuf(buffer.rdbuf());
    std::cerr.rdbuf(buffer.rdbuf());
    
    logger.logInfo("Test info");
    logger.logWarning("Test warning");
    logger.logError("Test error");
    logger.logDebug("Test debug");
    
    // Restore original streams
    std::cout.rdbuf(orig_cout);
    std::cerr.rdbuf(orig_cerr);
    
    std::string output = buffer.str();
    EXPECT_TRUE(output.empty()); // No output should be generated
}
