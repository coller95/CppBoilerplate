#include <gtest/gtest.h>
#include <IocContainer/IocContainer.h>
#include <Logger/ILogger.h>
#include <Logger/Logger.h>

namespace {

class IocContainerGlobalRegistrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear container for clean test state
        auto& container = ioccontainer::IocContainer::getInstance();
        container.clear();
    }

    void TearDown() override {
        // Cleanup after each test
        auto& container = ioccontainer::IocContainer::getInstance();
        container.clear();
    }
};

// Mock service for testing
class MockTestService {
public:
    MockTestService() = default;
    std::string getTestValue() const { return "test_value"; }
};

} // anonymous namespace

TEST_F(IocContainerGlobalRegistrationTest, RegisterGlobalInstanceWorks) {
    // Test the new static registerGlobal method
    auto testService = std::make_shared<MockTestService>();
    
    // This should work without needing getInstance()
    EXPECT_NO_THROW({
        ioccontainer::IocContainer::registerGlobal<MockTestService>(testService);
    });
    
    // Verify it was registered in the global instance
    auto& container = ioccontainer::IocContainer::getInstance();
    EXPECT_TRUE(container.isRegistered<MockTestService>());
    
    // Verify we can resolve it
    auto resolved = container.resolve<MockTestService>();
    EXPECT_NE(resolved, nullptr);
    EXPECT_EQ(resolved->getTestValue(), "test_value");
}

TEST_F(IocContainerGlobalRegistrationTest, RegisterGlobalFactoryWorks) {
    // Test the new static registerGlobal method with factory function
    EXPECT_NO_THROW({
        ioccontainer::IocContainer::registerGlobal<MockTestService>(
            []() { return std::make_shared<MockTestService>(); }
        );
    });
    
    // Verify it was registered and works
    auto& container = ioccontainer::IocContainer::getInstance();
    EXPECT_TRUE(container.isRegistered<MockTestService>());
    
    auto resolved = container.resolve<MockTestService>();
    EXPECT_NE(resolved, nullptr);
    EXPECT_EQ(resolved->getTestValue(), "test_value");
}

TEST_F(IocContainerGlobalRegistrationTest, RegisterGlobalWithLoggerExample) {
    // Test with actual Logger service
    EXPECT_NO_THROW({
        ioccontainer::IocContainer::registerGlobal<logger::ILogger>(
            []() { return std::make_shared<logger::Logger>("127.0.0.1", 9000); }
        );
    });
    
    // Verify registration
    auto& container = ioccontainer::IocContainer::getInstance();
    EXPECT_TRUE(container.isRegistered<logger::ILogger>());
    
    // Verify resolution
    auto logger = container.resolve<logger::ILogger>();
    EXPECT_NE(logger, nullptr);
}

TEST_F(IocContainerGlobalRegistrationTest, RegisterGlobalOverwritesExisting) {
    // Register first service
    auto service1 = std::make_shared<MockTestService>();
    ioccontainer::IocContainer::registerGlobal<MockTestService>(service1);
    
    // Register second service (should overwrite)
    auto service2 = std::make_shared<MockTestService>();
    EXPECT_NO_THROW({
        ioccontainer::IocContainer::registerGlobal<MockTestService>(service2);
    });
    
    // Verify the second service is now registered
    auto& container = ioccontainer::IocContainer::getInstance();
    auto resolved = container.resolve<MockTestService>();
    EXPECT_EQ(resolved.get(), service2.get()); // Should be the second instance
}

TEST_F(IocContainerGlobalRegistrationTest, ResolveGlobalWorks) {
    // Register a service using global registration
    auto testService = std::make_shared<MockTestService>();
    ioccontainer::IocContainer::registerGlobal<MockTestService>(testService);
    
    // Test the new static resolveGlobal method
    EXPECT_NO_THROW({
        auto resolved = ioccontainer::IocContainer::resolveGlobal<MockTestService>();
        EXPECT_NE(resolved, nullptr);
        EXPECT_EQ(resolved->getTestValue(), "test_value");
        EXPECT_EQ(resolved.get(), testService.get()); // Should be the same instance
    });
}

TEST_F(IocContainerGlobalRegistrationTest, ResolveGlobalThrowsWhenNotRegistered) {
    // Don't register anything
    
    // Should throw when trying to resolve unregistered service
    EXPECT_THROW({
        auto resolved = ioccontainer::IocContainer::resolveGlobal<MockTestService>();
    }, std::exception);
}

TEST_F(IocContainerGlobalRegistrationTest, IsRegisteredGlobalWorks) {
    // Initially not registered
    EXPECT_FALSE(ioccontainer::IocContainer::isRegisteredGlobal<MockTestService>());
    
    // Register service
    auto testService = std::make_shared<MockTestService>();
    ioccontainer::IocContainer::registerGlobal<MockTestService>(testService);
    
    // Now should be registered
    EXPECT_TRUE(ioccontainer::IocContainer::isRegisteredGlobal<MockTestService>());
}

TEST_F(IocContainerGlobalRegistrationTest, CompleteGlobalWorkflow) {
    // Complete workflow test using only global methods
    
    // 1. Check not registered
    EXPECT_FALSE(ioccontainer::IocContainer::isRegisteredGlobal<MockTestService>());
    
    // 2. Register with factory
    ioccontainer::IocContainer::registerGlobal<MockTestService>(
        []() { return std::make_shared<MockTestService>(); }
    );
    
    // 3. Check registered
    EXPECT_TRUE(ioccontainer::IocContainer::isRegisteredGlobal<MockTestService>());
    
    // 4. Resolve and use
    auto service = ioccontainer::IocContainer::resolveGlobal<MockTestService>();
    EXPECT_NE(service, nullptr);
    EXPECT_EQ(service->getTestValue(), "test_value");
}
