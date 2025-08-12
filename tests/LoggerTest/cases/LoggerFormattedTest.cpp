#include <gtest/gtest.h>
#include <Logger/Logger.h>
#include <sstream>
#include <iostream>

namespace {

class LoggerFormattedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }

    void TearDown() override {
        // Cleanup code for each test
    }
};

} // anonymous namespace

TEST_F(LoggerFormattedTest, FormattedLogWorks) {
    logger::Logger logger("127.0.0.1", 8080);
    
    // Redirect cout to capture output for testing
    std::stringstream buffer;
    std::streambuf* orig_cout = std::cout.rdbuf();
    
    std::cout.rdbuf(buffer.rdbuf());
    
    // Test formatted logging
    logger.log("Test number: %d, string: %s", 42, "hello");
    
    // Restore original streams
    std::cout.rdbuf(orig_cout);
    
    std::string output = buffer.str();
    EXPECT_NE(output.find("[INFO] Test number: 42, string: hello"), std::string::npos);
}

TEST_F(LoggerFormattedTest, FormattedLogWithVariousTypes) {
    logger::Logger logger("127.0.0.1", 8080);
    
    // Redirect cout to capture output for testing
    std::stringstream buffer;
    std::streambuf* orig_cout = std::cout.rdbuf();
    
    std::cout.rdbuf(buffer.rdbuf());
    
    // Test various format specifiers
    logger.log("Integer: %d, Float: %.2f, Char: %c", 123, 3.14159, 'A');
    
    // Restore original streams
    std::cout.rdbuf(orig_cout);
    
    std::string output = buffer.str();
    EXPECT_NE(output.find("[INFO] Integer: 123, Float: 3.14, Char: A"), std::string::npos);
}

TEST_F(LoggerFormattedTest, FormattedLogWithLocalDisplayDisabled) {
    logger::Logger logger("127.0.0.1", 8080);
    logger.setLocalDisplay(false);
    
    // Redirect cout to capture output for testing
    std::stringstream buffer;
    std::streambuf* orig_cout = std::cout.rdbuf();
    
    std::cout.rdbuf(buffer.rdbuf());
    
    // Test formatted logging with disabled display
    logger.log("This should not appear: %d", 999);
    
    // Restore original streams
    std::cout.rdbuf(orig_cout);
    
    std::string output = buffer.str();
    EXPECT_TRUE(output.empty()); // No output should be generated
}
