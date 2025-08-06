#include <gtest/gtest.h>
#include <Logger.h>
#include <sstream>
#include <iostream>

/**
 * Unit tests for Logger class
 */

TEST(LoggerTest, LogInfoPrintsMessage) {
    Logger logger("127.0.0.1", 9000);
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
    logger.logInfo("Test info message");
    std::cout.rdbuf(oldCout);
    std::string output = buffer.str();
    EXPECT_NE(output.find("Test info message"), std::string::npos);
}

TEST(LoggerTest, LogErrorPrintsMessage) {
    Logger logger("127.0.0.1", 9000);
    std::stringstream buffer;
    std::streambuf* oldCerr = std::cerr.rdbuf(buffer.rdbuf());
    logger.logError("Test error message");
    std::cerr.rdbuf(oldCerr);
    std::string output = buffer.str();
    EXPECT_NE(output.find("Test error message"), std::string::npos);
}
