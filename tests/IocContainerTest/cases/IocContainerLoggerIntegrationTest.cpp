#include <gtest/gtest.h>
#include <IocContainer/IocContainer.h>
#include <Logger/ILogger.h>
#include <Logger/Logger.h>

namespace {

class IocContainerLoggerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup - each test will clear the container as needed
    }

    void TearDown() override {
        // Cleanup after each test
    }
};

} // anonymous namespace

TEST_F(IocContainerLoggerIntegrationTest, CanRegisterLoggerService) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Clear any existing registrations
    container.clear();
    
    // Create a logger instance
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    
    // Register the logger service
    container.registerInstance<logger::ILogger>(logger);
    
    // Verify registration
    EXPECT_TRUE(container.isRegistered<logger::ILogger>());
    EXPECT_EQ(1U, container.getRegisteredCount());
}

TEST_F(IocContainerLoggerIntegrationTest, CanResolveLoggerService) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Clear any existing registrations
    container.clear();
    
    // Create and register a logger instance
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger);
    
    // Resolve the logger service
    auto resolved = container.resolve<logger::ILogger>();
    
    // Verify same instance is returned
    EXPECT_EQ(logger.get(), resolved.get());
}

TEST_F(IocContainerLoggerIntegrationTest, GlobalLoggerAccessFromMultipleLocations) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Clear any existing registrations
    container.clear();
    
    // Create and register a logger instance
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger);
    
    // Resolve from multiple locations
    auto logger1 = container.resolve<logger::ILogger>();
    auto logger2 = container.resolve<logger::ILogger>();
    auto logger3 = container.resolve<logger::ILogger>();
    
    // All should be the same instance
    EXPECT_EQ(logger1.get(), logger2.get());
    EXPECT_EQ(logger2.get(), logger3.get());
    EXPECT_EQ(logger1.get(), logger.get());
}

TEST_F(IocContainerLoggerIntegrationTest, ThrowsExceptionWhenServiceNotRegistered) {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Clear any existing registrations
    container.clear();
    
    // Try to resolve unregistered service
    EXPECT_THROW(container.resolve<logger::ILogger>(), ioccontainer::ServiceNotRegisteredException);
}
