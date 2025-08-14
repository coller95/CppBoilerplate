#include <gtest/gtest.h>
#include <IocContainer/IIocContainer.h>
#include <Logger/Logger.h>
#include <Logger/ILogger.h>
#include <WebServer/IWebServer.h>

namespace {

class IocStartupFailureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean slate for each test - prevent test interference
        auto& container = ioccontainer::IIocContainer::getInstance();
        container.clear();
    }
};

} // anonymous namespace

// Test EXACT startup sequence from main.cpp
TEST_F(IocStartupFailureTest, ApplicationStartupWorkflow) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // 1. Register Logger (exactly like main.cpp does)
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    logger->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger);
    
    // 2. Resolve Logger (exactly like main.cpp does)
    auto globalLogger = container.resolve<logger::ILogger>();
    EXPECT_NE(globalLogger, nullptr);
    
    // 3. Verify it's the same instance we registered
    EXPECT_EQ(globalLogger.get(), logger.get());
    
    // 4. Verify we can actually use the service
    EXPECT_NO_THROW(globalLogger->logInfo("Application starting up..."));
}

// Test failure mode: missing service during startup
TEST_F(IocStartupFailureTest, StartupFailsWhenServiceMissing) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    // Don't register anything - simulate missing service
    
    // Should throw ServiceNotRegisteredException when trying to resolve
    EXPECT_THROW(
        container.resolve<logger::ILogger>(),
        ioccontainer::ServiceNotRegisteredException
    );
    
    // Verify container is still functional for other services
    EXPECT_EQ(container.getRegisteredCount(), 0);
}

// Test auto-registration workflow used by modules
TEST_F(IocStartupFailureTest, AutoRegistrationWorks) {
    // Test what endpoint modules do for auto-registration
    ioccontainer::IIocContainer::registerGlobal<logger::ILogger>(
        []() { 
            auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
            logger->setLocalDisplay(true);
            return logger;
        }
    );
    
    // Should be able to resolve the auto-registered service
    auto service = ioccontainer::IIocContainer::resolveGlobal<logger::ILogger>();
    EXPECT_NE(service, nullptr);
    
    // Should be usable
    EXPECT_NO_THROW(service->logInfo("Auto-registered service works"));
}

// Test service overwrite behavior during startup
TEST_F(IocStartupFailureTest, ServiceOverwriteWorks) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Register first logger
    auto logger1 = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger1);
    
    // Register second logger (should overwrite first)
    auto logger2 = std::make_shared<logger::Logger>("127.0.0.1", 9001);
    container.registerInstance<logger::ILogger>(logger2);
    
    // Should get the second logger
    auto resolved = container.resolve<logger::ILogger>();
    EXPECT_EQ(resolved.get(), logger2.get());
    EXPECT_NE(resolved.get(), logger1.get());
    
    // Container should still have only 1 service registered
    EXPECT_EQ(container.getRegisteredCount(), 1);
}

// Test mixed registration methods work together
TEST_F(IocStartupFailureTest, MixedRegistrationMethods) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Use instance method (like main.cpp)
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger);
    
    // Use global static method (like modules)  
    ioccontainer::IIocContainer::registerGlobal<logger::Logger>(
        std::make_shared<logger::Logger>("127.0.0.1", 9001)
    );
    
    // Both should be resolvable
    auto loggerInterface = container.resolve<logger::ILogger>();
    auto loggerConcrete = container.resolve<logger::Logger>();
    
    EXPECT_NE(loggerInterface, nullptr);
    EXPECT_NE(loggerConcrete, nullptr);
    
    // Should have 2 different service types registered
    EXPECT_EQ(container.getRegisteredCount(), 2);
}

// Test interface-first access - can't accidentally use concrete class
TEST_F(IocStartupFailureTest, InterfaceFirstAccessWorks) {
    // This is the ONLY way users should access the container
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Register service through interface
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    container.registerInstance<logger::ILogger>(logger);
    
    // Resolve service through interface
    auto resolved = container.resolve<logger::ILogger>();
    EXPECT_NE(resolved, nullptr);
    
    // Verify global methods also work through interface
    EXPECT_TRUE(ioccontainer::IIocContainer::isRegisteredGlobal<logger::ILogger>());
}