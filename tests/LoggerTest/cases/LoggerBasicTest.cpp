#include <gtest/gtest.h>
#include <Logger/ILogger.h>
#include <Logger/ConsoleLogger.h>
#include <Logger/NetworkLogger.h>
#include <Logger/CompositeLogger.h>
#include <sstream>
#include <memory>

namespace {

class LoggerTest : public ::testing::Test {
protected:
	std::shared_ptr<logger::ILogger> consoleLogger_;
	std::shared_ptr<logger::ILogger> networkLogger_;
	std::shared_ptr<logger::ILogger> compositeLogger_;
	
	void SetUp() override {
		consoleLogger_ = std::make_shared<logger::ConsoleLogger>();
		networkLogger_ = std::make_shared<logger::NetworkLogger>("127.0.0.1", 9000);
		
		auto composite = std::make_shared<logger::CompositeLogger>();
		composite->addLogger(std::make_shared<logger::ConsoleLogger>());
		composite->addLogger(std::make_shared<logger::NetworkLogger>("127.0.0.1", 9000));
		compositeLogger_ = composite;
	}

	void TearDown() override {
		if (consoleLogger_ && consoleLogger_->isRunning()) {
			consoleLogger_->stop();
		}
		if (networkLogger_ && networkLogger_->isRunning()) {
			networkLogger_->stop();
		}
		if (compositeLogger_ && compositeLogger_->isRunning()) {
			compositeLogger_->stop();
		}
	}
};

} // anonymous namespace

// Console Logger Tests
TEST_F(LoggerTest, ConsoleLoggerBasics) {
	EXPECT_NE(nullptr, consoleLogger_);
	EXPECT_EQ(logger::LogLevel::Info, consoleLogger_->getLogLevel());
	EXPECT_TRUE(consoleLogger_->isLocalDisplayEnabled()); // Console defaults to enabled
}

TEST_F(LoggerTest, ConsoleLoggerCanLog) {
	EXPECT_NO_THROW(consoleLogger_->logInfo("Console test message"));
	EXPECT_NO_THROW(consoleLogger_->start());
	EXPECT_TRUE(consoleLogger_->isRunning());
}

// Network Logger Tests  
TEST_F(LoggerTest, NetworkLoggerBasics) {
	EXPECT_NE(nullptr, networkLogger_);
	EXPECT_EQ(logger::LogLevel::Info, networkLogger_->getLogLevel());
	EXPECT_FALSE(networkLogger_->isLocalDisplayEnabled()); // Network defaults to disabled
}

TEST_F(LoggerTest, NetworkLoggerCanLog) {
	EXPECT_NO_THROW(networkLogger_->logInfo("Network test message"));
	EXPECT_NO_THROW(networkLogger_->start());
	EXPECT_TRUE(networkLogger_->isRunning());
}

// Composite Logger Tests
TEST_F(LoggerTest, CompositeLoggerBasics) {
	EXPECT_NE(nullptr, compositeLogger_);
	EXPECT_EQ(logger::LogLevel::Info, compositeLogger_->getLogLevel());
}

TEST_F(LoggerTest, CompositeLoggerDualLogging) {
	// Should log to both console and network
	EXPECT_NO_THROW(compositeLogger_->start());
	EXPECT_TRUE(compositeLogger_->isRunning());
	
	EXPECT_NO_THROW(compositeLogger_->logInfo("Dual logging test"));
	EXPECT_NO_THROW(compositeLogger_->logError("Error test"));
}

TEST_F(LoggerTest, CompositeLoggerSyncSettings) {
	// Settings should sync to all child loggers
	compositeLogger_->setLogLevel(logger::LogLevel::Error);
	EXPECT_EQ(logger::LogLevel::Error, compositeLogger_->getLogLevel());
	
	compositeLogger_->setLocalDisplay(false);
	EXPECT_FALSE(compositeLogger_->isLocalDisplayEnabled());
}

// Direct Construction Tests
TEST_F(LoggerTest, DirectConstructionWorks) {
	auto console = std::make_shared<logger::ConsoleLogger>();
	auto network = std::make_shared<logger::NetworkLogger>("localhost", 8080);
	
	auto composite = std::make_shared<logger::CompositeLogger>();
	composite->addLogger(console);
	composite->addLogger(network);
	
	EXPECT_NE(nullptr, console);
	EXPECT_NE(nullptr, network);
	EXPECT_NE(nullptr, composite);
	EXPECT_EQ(2u, composite->getLoggerCount());
}
