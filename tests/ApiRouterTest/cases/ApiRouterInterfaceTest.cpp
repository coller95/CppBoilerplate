#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ApiRouter/ApiRouter.h>
#include "../MockApiRouter.h"

using ::testing::_;
using ::testing::Return;

namespace {

class ApiRouterInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockModule = std::make_unique<apirouter::MockApiRouter>();
    }

    std::unique_ptr<apirouter::MockApiRouter> mockModule;
};

} // anonymous namespace

TEST_F(ApiRouterInterfaceTest, ImplementsIApiRouterInterface) {
    // Verify that ApiRouter implements IApiRouter
    auto& router = apirouter::ApiRouter::getInstance();
    apirouter::IApiRouter* interface = &router;
    EXPECT_NE(interface, nullptr);
}

TEST_F(ApiRouterInterfaceTest, ImplementsIEndpointRegistrarInterface) {
    // Verify that ApiRouter implements IEndpointRegistrar
    auto& router = apirouter::ApiRouter::getInstance();
    apirouter::IEndpointRegistrar* registrar = &router;
    EXPECT_NE(registrar, nullptr);
}

TEST_F(ApiRouterInterfaceTest, ApiRouterGlobalMethodsWork) {
    // Test that the global static methods work correctly
    
    // Test static methods work
    EXPECT_NO_THROW(apirouter::ApiRouter::initializeGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::getEndpointCountGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::getRegisteredEndpointsGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::getRegisteredModuleCountGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::createAllModulesGlobal());
    
    // Test that the global methods access the same singleton
    size_t globalCount = apirouter::ApiRouter::getEndpointCountGlobal();
    size_t instanceCount = apirouter::ApiRouter::getInstance().getEndpointCount();
    EXPECT_EQ(globalCount, instanceCount);
}

TEST_F(ApiRouterInterfaceTest, ModuleInterfaceWorks) {
    // Test that the module interface works correctly
    
    // Create a test module using the ApiRouter interfaces
    class TestModule : public apirouter::IApiModule {
    public:
        void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override {
            registrar.registerHttpHandler("/module-test", "GET",
                [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                    responseBody = "Module test";
                    statusCode = 200;
                });
        }
    };
    
    // Should compile and work correctly
    auto module = std::make_unique<TestModule>();
    EXPECT_NE(module, nullptr);
    
    // Test that it can register with the router
    auto& router = apirouter::ApiRouter::getInstance();
    EXPECT_NO_THROW(module->registerEndpoints(router));
}

TEST_F(ApiRouterInterfaceTest, MockCanBeUsedForTesting) {
    // Example of how to use the mock for testing other components
    EXPECT_CALL(*mockModule, initialize())
        .WillOnce(Return(true));
    
    EXPECT_CALL(*mockModule, getEndpointCount())
        .WillOnce(Return(5));
    
    bool result = mockModule->initialize();
    EXPECT_TRUE(result);
    
    size_t count = mockModule->getEndpointCount();
    EXPECT_EQ(count, 5U);
}

TEST_F(ApiRouterInterfaceTest, MockEndpointRegistrarWorks) {
    // Test that the mock can handle endpoint registration
    class MockEndpointRegistrar : public apirouter::IEndpointRegistrar {
    public:
        MOCK_METHOD(void, registerHttpHandler, 
                   (std::string_view path, std::string_view method, apirouter::HttpHandler handler), 
                   (override));
    };
    
    MockEndpointRegistrar mockRegistrar;
    EXPECT_CALL(mockRegistrar, registerHttpHandler(_, _, _))
        .Times(1);
    
    // Register an endpoint using the mock
    mockRegistrar.registerHttpHandler("/test", "GET", 
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            responseBody = "Mock test";
            statusCode = 200;
        });
}

TEST_F(ApiRouterInterfaceTest, AllRequiredMethodsAreImplemented) {
    // Verify all required interface methods are implemented
    auto& router = apirouter::ApiRouter::getInstance();
    
    // IApiRouter methods
    EXPECT_NO_THROW(router.initialize());
    EXPECT_NO_THROW(router.getEndpointCount());
    EXPECT_NO_THROW(router.getRegisteredEndpoints());
    EXPECT_NO_THROW(router.getRegisteredModuleCount());
    EXPECT_NO_THROW(router.createAllModules());
    
    // IEndpointRegistrar methods
    EXPECT_NO_THROW(router.registerHttpHandler("/test", "GET", 
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            responseBody = "Interface test";
            statusCode = 200;
        }));
    
    // Test request handling
    std::string responseBody;
    int statusCode = 0;
    EXPECT_NO_THROW(router.handleRequest("/test", "GET", "", responseBody, statusCode));
}
