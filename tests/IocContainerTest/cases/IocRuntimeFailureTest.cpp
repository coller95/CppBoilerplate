#include <gtest/gtest.h>
#include <IocContainer/IIocContainer.h>
#include <Logger/Logger.h>
#include <Logger/ILogger.h>
#include <memory>

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

// Test error handling during HTTP request processing
TEST_F(IocRuntimeFailureTest, HandlesUnregisteredServiceGracefully) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Setup: Register Logger but not WebServer (partial startup)
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    logger->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger);
    
    // During HTTP request processing, try to resolve unregistered service
    EXPECT_THROW(
        container.resolve<IMockWebServer>(),
        ioccontainer::ServiceNotRegisteredException
    );
    
    // But Logger should still work (other services unaffected)
    auto resolvedLogger = container.resolve<logger::ILogger>();
    EXPECT_NE(resolvedLogger, nullptr);
    EXPECT_NO_THROW(resolvedLogger->logError("WebServer not available"));
}

// Test service replacement during runtime (hot-swap scenario)
TEST_F(IocRuntimeFailureTest, CanReplaceServicesAtRuntime) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Initial setup: Register first logger
    auto logger1 = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    logger1->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger1);
    
    // Verify first logger works
    auto resolved1 = container.resolve<logger::ILogger>();
    EXPECT_EQ(resolved1.get(), logger1.get());
    
    // Runtime: Replace with second logger (configuration change, failover, etc.)
    auto logger2 = std::make_shared<logger::Logger>("127.0.0.1", 9001);
    logger2->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger2);
    
    // Should now get the second logger
    auto resolved2 = container.resolve<logger::ILogger>();
    EXPECT_EQ(resolved2.get(), logger2.get());
    EXPECT_NE(resolved2.get(), logger1.get());
    
    // Should be usable immediately
    EXPECT_NO_THROW(resolved2->logInfo("Service replaced successfully"));
}

// Test multiple service resolution in single request context
TEST_F(IocRuntimeFailureTest, MultipleServiceResolutionInRequest) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Setup: Register multiple services (typical web server state)
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    logger->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger);
    
    auto webserver = std::make_shared<MockWebServerImpl>("127.0.0.1", 8080);
    container.registerInstance<IMockWebServer>(webserver);
    
    // Simulate HTTP request processing: resolve multiple services
    auto requestLogger = container.resolve<logger::ILogger>();
    auto requestWebServer = container.resolve<IMockWebServer>();
    
    EXPECT_NE(requestLogger, nullptr);
    EXPECT_NE(requestWebServer, nullptr);
    
    // Should be able to use both services
    EXPECT_NO_THROW(requestLogger->logInfo("Processing HTTP request"));
    EXPECT_EQ(requestWebServer->getHost(), "127.0.0.1");
    EXPECT_EQ(requestWebServer->getPort(), 8080);
}

// Test error recovery during request processing
TEST_F(IocRuntimeFailureTest, ErrorRecoveryDuringRequests) {
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Setup partial services
    auto logger = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    logger->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger);
    
    // First request fails due to missing service
    EXPECT_THROW(
        container.resolve<IMockWebServer>(),
        ioccontainer::ServiceNotRegisteredException
    );
    
    // Log the error (this should work)
    auto errorLogger = container.resolve<logger::ILogger>();
    EXPECT_NO_THROW(errorLogger->logError("WebServer service not available"));
    
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
    auto logger1 = std::make_shared<logger::Logger>("127.0.0.1", 9000);
    logger1->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger1);
    
    // Resolve service (HTTP request gets reference)
    auto resolvedLogger = container.resolve<logger::ILogger>();
    EXPECT_EQ(resolvedLogger.get(), logger1.get());
    
    // Container registration changes (service replacement)
    auto logger2 = std::make_shared<logger::Logger>("127.0.0.1", 9001);
    logger2->setLocalDisplay(true);
    container.registerInstance<logger::ILogger>(logger2);
    
    // Previously resolved service should still work (shared_ptr keeps it alive)
    EXPECT_NO_THROW(resolvedLogger->logInfo("Using old service reference"));
    
    // New resolutions get new service
    auto newlyResolvedLogger = container.resolve<logger::ILogger>();
    EXPECT_EQ(newlyResolvedLogger.get(), logger2.get());
    EXPECT_NE(newlyResolvedLogger.get(), resolvedLogger.get());
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