#include <gtest/gtest.h>
#include <IocContainer/IIocContainer.h>
#include <string>
#include <memory>

// Completely generic test services - no domain knowledge
class IServiceA {
public:
    virtual ~IServiceA() = default;
    virtual int getValue() = 0;
    virtual void setValue(int value) = 0;
    virtual void increment() = 0;
};

class ServiceAImpl : public IServiceA {
private:
    int _value;

public:
    ServiceAImpl(int initialValue = 42) : _value(initialValue) {}

    int getValue() override {
        return _value;
    }

    void setValue(int value) override {
        _value = value;
    }

    void increment() override {
        _value++;
    }
};

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

// Test basic startup workflow
TEST_F(IocStartupFailureTest, ApplicationStartupWorkflow) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // 1. Register service
    auto serviceA = std::make_shared<ServiceAImpl>(100);
    container.registerInstance<IServiceA>(serviceA);
    
    // 2. Resolve service
    auto globalService = container.resolve<IServiceA>();
    EXPECT_NE(globalService, nullptr);
    
    // 3. Verify it's the same instance we registered
    EXPECT_EQ(globalService.get(), serviceA.get());
    
    // 4. Verify we can actually use the service
    EXPECT_NO_THROW(globalService->increment());
    EXPECT_EQ(globalService->getValue(), 101);
}

// Test failure mode: missing service during startup
TEST_F(IocStartupFailureTest, StartupFailsWhenServiceMissing) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    // Don't register anything - simulate missing service
    
    // Should throw ServiceNotRegisteredException when trying to resolve
    EXPECT_THROW(
        container.resolve<IServiceA>(),
        ioccontainer::ServiceNotRegisteredException
    );
    
    // Verify container is still functional for other services
    EXPECT_EQ(container.getRegisteredCount(), static_cast<size_t>(0));
}

// Test auto-registration workflow
TEST_F(IocStartupFailureTest, AutoRegistrationWorks) {
    // Test factory-based auto-registration
    ioccontainer::IIocContainer::registerGlobal<IServiceA>(
        []() { 
            return std::make_shared<ServiceAImpl>(200);
        }
    );
    
    // Should be able to resolve the auto-registered service
    auto service = ioccontainer::IIocContainer::resolveGlobal<IServiceA>();
    EXPECT_NE(service, nullptr);
    
    // Should be usable
    EXPECT_NO_THROW(service->increment());
    EXPECT_EQ(service->getValue(), 201);
}

// Test service overwrite behavior during startup
TEST_F(IocStartupFailureTest, ServiceOverwriteWorks) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Register first service
    auto service1 = std::make_shared<ServiceAImpl>(300);
    container.registerInstance<IServiceA>(service1);
    
    // Register second service (should overwrite first)
    auto service2 = std::make_shared<ServiceAImpl>(400);
    container.registerInstance<IServiceA>(service2);
    
    // Should get the second service
    auto resolved = container.resolve<IServiceA>();
    EXPECT_EQ(resolved.get(), service2.get());
    EXPECT_NE(resolved.get(), service1.get());
    EXPECT_EQ(resolved->getValue(), 400);
    
    // Container should still have only 1 service registered
    EXPECT_EQ(container.getRegisteredCount(), static_cast<size_t>(1));
}

// Test mixed registration methods work together
TEST_F(IocStartupFailureTest, MixedRegistrationMethods) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Use instance method
    auto serviceA = std::make_shared<ServiceAImpl>(500);
    container.registerInstance<IServiceA>(serviceA);
    
    // Use global static method  
    ioccontainer::IIocContainer::registerGlobal<ServiceAImpl>(
        std::make_shared<ServiceAImpl>(600)
    );
    
    // Both should be resolvable
    auto serviceInterface = container.resolve<IServiceA>();
    auto serviceConcrete = container.resolve<ServiceAImpl>();
    
    EXPECT_NE(serviceInterface, nullptr);
    EXPECT_NE(serviceConcrete, nullptr);
    EXPECT_EQ(serviceInterface->getValue(), 500);
    EXPECT_EQ(serviceConcrete->getValue(), 600);
    
    // Should have 2 different service types registered
    EXPECT_EQ(container.getRegisteredCount(), static_cast<size_t>(2));
}

// Test interface-first access pattern
TEST_F(IocStartupFailureTest, InterfaceFirstAccessWorks) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Register service through interface
    auto serviceA = std::make_shared<ServiceAImpl>(700);
    container.registerInstance<IServiceA>(serviceA);
    
    // Resolve service through interface
    auto resolved = container.resolve<IServiceA>();
    EXPECT_NE(resolved, nullptr);
    EXPECT_EQ(resolved->getValue(), 700);
    
    // Verify global methods also work through interface
    EXPECT_TRUE(ioccontainer::IIocContainer::isRegisteredGlobal<IServiceA>());
}