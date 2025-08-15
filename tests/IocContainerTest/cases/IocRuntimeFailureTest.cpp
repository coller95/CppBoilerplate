#include <gtest/gtest.h>
#include <IocContainer/IIocContainer.h>
#include <memory>
#include <string>

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

class IocRuntimeFailureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clean slate for each test
        auto& container = ioccontainer::IIocContainer::getInstance();
        container.clear();
    }
};

// Mock service for testing runtime scenarios
class MockWebServer {
public:
    MockWebServer(const std::string& host, int port) : _host(host), _port(port) {}
    std::string getHost() const { return _host; }
    int getPort() const { return _port; }
    
private:
    std::string _host;
    int _port;
};

class IMockWebServer {
public:
    virtual ~IMockWebServer() = default;
    virtual std::string getHost() const = 0;
    virtual int getPort() const = 0;
};

class MockWebServerImpl : public IMockWebServer {
private:
    std::shared_ptr<MockWebServer> _impl;
    
public:
    MockWebServerImpl(const std::string& host, int port) 
        : _impl(std::make_shared<MockWebServer>(host, port)) {}
    
    std::string getHost() const override { return _impl->getHost(); }
    int getPort() const override { return _impl->getPort(); }
};

} // anonymous namespace

// Test error handling when service is missing
TEST_F(IocRuntimeFailureTest, HandlesUnregisteredServiceGracefully) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Setup: Register ServiceA but not MockWebServer (partial registration)
    auto serviceA = std::make_shared<ServiceAImpl>(100);
    container.registerInstance<IServiceA>(serviceA);
    
    // Try to resolve unregistered service
    EXPECT_THROW(
        container.resolve<IMockWebServer>(),
        ioccontainer::ServiceNotRegisteredException
    );
    
    // But ServiceA should still work (other services unaffected)
    auto resolvedServiceA = container.resolve<IServiceA>();
    EXPECT_NE(resolvedServiceA, nullptr);
    EXPECT_NO_THROW(resolvedServiceA->increment());
}

// Test service replacement during runtime (hot-swap scenario)
TEST_F(IocRuntimeFailureTest, CanReplaceServicesAtRuntime) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Initial setup: Register first service
    auto service1 = std::make_shared<ServiceAImpl>(100);
    container.registerInstance<IServiceA>(service1);
    
    // Verify first service works
    auto resolved1 = container.resolve<IServiceA>();
    EXPECT_EQ(resolved1.get(), service1.get());
    
    // Runtime: Replace with second service (hot-swap scenario)
    auto service2 = std::make_shared<ServiceAImpl>(200);
    container.registerInstance<IServiceA>(service2);
    
    // Should now get the second service
    auto resolved2 = container.resolve<IServiceA>();
    EXPECT_EQ(resolved2.get(), service2.get());
    EXPECT_NE(resolved2.get(), service1.get());
    
    // Should be usable immediately
    EXPECT_NO_THROW(resolved2->increment());
    EXPECT_EQ(resolved2->getValue(), 201);
}

// Test multiple service resolution in single request context
TEST_F(IocRuntimeFailureTest, MultipleServiceResolutionInRequest) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Setup: Register multiple services
    auto serviceA = std::make_shared<ServiceAImpl>(300);
    container.registerInstance<IServiceA>(serviceA);
    
    auto webserver = std::make_shared<MockWebServerImpl>("127.0.0.1", 8080);
    container.registerInstance<IMockWebServer>(webserver);
    
    // Simulate resolving multiple services in single context
    auto requestServiceA = container.resolve<IServiceA>();
    auto requestWebServer = container.resolve<IMockWebServer>();
    
    EXPECT_NE(requestServiceA, nullptr);
    EXPECT_NE(requestWebServer, nullptr);
    
    // Should be able to use both services
    EXPECT_NO_THROW(requestServiceA->increment());
    EXPECT_EQ(requestServiceA->getValue(), 301);
    EXPECT_EQ(requestWebServer->getHost(), "127.0.0.1");
    EXPECT_EQ(requestWebServer->getPort(), 8080);
}

// Test error recovery during request processing
TEST_F(IocRuntimeFailureTest, ErrorRecoveryDuringRequests) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Setup partial services
    auto serviceA = std::make_shared<ServiceAImpl>(400);
    container.registerInstance<IServiceA>(serviceA);
    
    // First request fails due to missing service
    EXPECT_THROW(
        container.resolve<IMockWebServer>(),
        ioccontainer::ServiceNotRegisteredException
    );
    
    // Available service should still work
    auto availableService = container.resolve<IServiceA>();
    EXPECT_NO_THROW(availableService->increment());
    EXPECT_EQ(availableService->getValue(), 401);
    
    // Admin registers missing service
    auto webserver = std::make_shared<MockWebServerImpl>("127.0.0.1", 8080);
    container.registerInstance<IMockWebServer>(webserver);
    
    // Subsequent requests should now work
    auto recoveredWebServer = container.resolve<IMockWebServer>();
    EXPECT_NE(recoveredWebServer, nullptr);
    EXPECT_EQ(recoveredWebServer->getHost(), "127.0.0.1");
}

// Test service lifecycle - what happens to resolved services when container changes
TEST_F(IocRuntimeFailureTest, ResolvedServiceLifecycle) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Register service
    auto service1 = std::make_shared<ServiceAImpl>(500);
    container.registerInstance<IServiceA>(service1);
    
    // Resolve service (get reference)
    auto resolvedService = container.resolve<IServiceA>();
    EXPECT_EQ(resolvedService.get(), service1.get());
    
    // Container registration changes (service replacement)
    auto service2 = std::make_shared<ServiceAImpl>(600);
    container.registerInstance<IServiceA>(service2);
    
    // Previously resolved service should still work (shared_ptr keeps it alive)
    EXPECT_NO_THROW(resolvedService->increment());
    EXPECT_EQ(resolvedService->getValue(), 501);
    
    // New resolutions get new service
    auto newlyResolvedService = container.resolve<IServiceA>();
    EXPECT_EQ(newlyResolvedService.get(), service2.get());
    EXPECT_NE(newlyResolvedService.get(), resolvedService.get());
    EXPECT_EQ(newlyResolvedService->getValue(), 600);
}

// Test interface substitutability - the core value of IoC
TEST_F(IocRuntimeFailureTest, InterfaceSubstitutability) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Register first implementation
    auto webserver1 = std::make_shared<MockWebServerImpl>("localhost", 3000);
    container.registerInstance<IMockWebServer>(webserver1);
    
    // Client code depends on interface, not implementation
    auto service1 = container.resolve<IMockWebServer>();
    EXPECT_EQ(service1->getHost(), "localhost");
    EXPECT_EQ(service1->getPort(), 3000);
    
    // Replace with different implementation
    auto webserver2 = std::make_shared<MockWebServerImpl>("0.0.0.0", 8080);
    container.registerInstance<IMockWebServer>(webserver2);
    
    // Same client code, different behavior
    auto service2 = container.resolve<IMockWebServer>();
    EXPECT_EQ(service2->getHost(), "0.0.0.0");
    EXPECT_EQ(service2->getPort(), 8080);
    
    // This is why IoC exists - implementation substitutability
}