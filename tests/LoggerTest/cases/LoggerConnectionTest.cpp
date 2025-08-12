#include <gtest/gtest.h>
#include <Logger/Logger.h>
#include <thread>
#include <chrono>

namespace {

class LoggerConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }

    void TearDown() override {
        // Cleanup code for each test
    }
};

} // anonymous namespace

TEST_F(LoggerConnectionTest, InvalidIPAddressReturnsFalse) {
    logger::Logger logger("invalid.ip.address", 8080);
    EXPECT_FALSE(logger.start());
    EXPECT_FALSE(logger.isConnected());
}

TEST_F(LoggerConnectionTest, InvalidPortReturnsFalse) {
    logger::Logger logger("127.0.0.1", -1); // Invalid port
    EXPECT_FALSE(logger.start());
    EXPECT_FALSE(logger.isConnected());
}

TEST_F(LoggerConnectionTest, ConnectionStatePersistsThroughMultipleCalls) {
    logger::Logger logger("127.0.0.1", 9999); // Likely unavailable port
    
    // Multiple start attempts should be consistent
    bool firstAttempt = logger.start();
    bool secondAttempt = logger.start();
    
    EXPECT_EQ(firstAttempt, secondAttempt);
    EXPECT_EQ(logger.isConnected(), firstAttempt);
}

TEST_F(LoggerConnectionTest, StopResetsConnectionState) {
    logger::Logger logger("127.0.0.1", 8080);
    
    // Try to start (may fail, that's okay)
    logger.start();
    
    // Stop should reset the connection state
    logger.stop();
    EXPECT_FALSE(logger.isConnected());
}

TEST_F(LoggerConnectionTest, StartAfterStopWorksCorrectly) {
    logger::Logger logger("127.0.0.1", 8080);
    
    // Try to start, then stop, then start again
    bool firstStart = logger.start();
    logger.stop();
    EXPECT_FALSE(logger.isConnected());
    
    bool secondStart = logger.start();
    
    // Both attempts should be consistent (both fail or both succeed)
    EXPECT_EQ(firstStart, secondStart);
    EXPECT_EQ(logger.isConnected(), secondStart);
}

TEST_F(LoggerConnectionTest, MultipleStopsDoNotCrash) {
    logger::Logger logger("127.0.0.1", 8080);
    
    // Multiple stops should not crash
    logger.stop();
    logger.stop();
    logger.stop();
    
    EXPECT_FALSE(logger.isConnected());
}
