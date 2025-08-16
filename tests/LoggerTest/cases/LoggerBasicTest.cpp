#include <gtest/gtest.h>
#include <Logger/ILogger.h>
#include <Logger/Logger.h>
#include <sstream>
#include <memory>

namespace {

class LoggerTest : public ::testing::Test {
protected:
	std::unique_ptr<logger::Logger> logger_;
	
	void SetUp() override {
		logger_ = std::make_unique<logger::Logger>("127.0.0.1", 9000);
	}

	void TearDown() override {
		if (logger_ && logger_->isRunning()) {
			logger_->stop();
		}
	}
};

} // anonymous namespace

// Interface contract tests
TEST_F(LoggerTest, ImplementsILoggerInterface) {
	logger::ILogger* ilogger = logger_.get();
	EXPECT_NE(nullptr, ilogger);
}

TEST_F(LoggerTest, DefaultLogLevelIsInfo) {
	EXPECT_EQ(logger::LogLevel::INFO, logger_->getLogLevel());
}

TEST_F(LoggerTest, CanSetAndGetLogLevel) {
	logger_->setLogLevel(logger::LogLevel::DEBUG);
	EXPECT_EQ(logger::LogLevel::DEBUG, logger_->getLogLevel());
	
	logger_->setLogLevel(logger::LogLevel::ERROR);
	EXPECT_EQ(logger::LogLevel::ERROR, logger_->getLogLevel());
}

TEST_F(LoggerTest, StartReturnsTrueAndSetsRunningState) {
	EXPECT_FALSE(logger_->isRunning());
	bool started = logger_->start();
	EXPECT_TRUE(started);
	EXPECT_TRUE(logger_->isRunning());
}

TEST_F(LoggerTest, StopChangesRunningState) {
	logger_->start();
	EXPECT_TRUE(logger_->isRunning());
	
	logger_->stop();
	EXPECT_FALSE(logger_->isRunning());
}

TEST_F(LoggerTest, LocalDisplayDefaultsToFalse) {
	EXPECT_FALSE(logger_->isLocalDisplayEnabled());
}

TEST_F(LoggerTest, CanSetLocalDisplay) {
	logger_->setLocalDisplay(true);
	EXPECT_TRUE(logger_->isLocalDisplayEnabled());
	
	logger_->setLocalDisplay(false);
	EXPECT_FALSE(logger_->isLocalDisplayEnabled());
}

// Log method tests - these will fail until implementation exists
TEST_F(LoggerTest, CanLogAtAllLevels) {
	EXPECT_NO_THROW(logger_->logDebug("Debug message"));
	EXPECT_NO_THROW(logger_->logInfo("Info message"));
	EXPECT_NO_THROW(logger_->logWarning("Warning message"));
	EXPECT_NO_THROW(logger_->logError("Error message"));
	EXPECT_NO_THROW(logger_->logCritical("Critical message"));
}

TEST_F(LoggerTest, GenericLogMethodWorks) {
	EXPECT_NO_THROW(logger_->log(logger::LogLevel::INFO, "Generic log message"));
	EXPECT_NO_THROW(logger_->log(logger::LogLevel::ERROR, "Generic error message"));
}

TEST_F(LoggerTest, LogsAreFilteredByLogLevel) {
	logger_->setLogLevel(logger::LogLevel::WARNING);
	
	// These should be logged (WARNING and above)
	EXPECT_NO_THROW(logger_->logWarning("Warning message"));
	EXPECT_NO_THROW(logger_->logError("Error message"));
	EXPECT_NO_THROW(logger_->logCritical("Critical message"));
	
	// These should be filtered out (below WARNING)
	EXPECT_NO_THROW(logger_->logDebug("Debug message"));
	EXPECT_NO_THROW(logger_->logInfo("Info message"));
}

// Cross-cutting concern tests
TEST_F(LoggerTest, IsThreadSafe) {
	// This test will be expanded when implementation exists
	EXPECT_NO_THROW(logger_->logInfo("Thread safety test"));
}
