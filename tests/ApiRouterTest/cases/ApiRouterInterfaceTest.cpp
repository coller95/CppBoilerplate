#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ApiRouter/ApiRouter.h>
#include <ApiModule/ApiModule.h>
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

TEST_F(ApiRouterInterfaceTest, CompatibilityLayerWorks) {
    // Test that the compatibility layer (ApiModule namespace) works correctly
    
    // Test static methods work
    EXPECT_NO_THROW(apimodule::ApiModule::initialize());
    EXPECT_NO_THROW(apimodule::ApiModule::getEndpointCount());
    EXPECT_NO_THROW(apimodule::ApiModule::getRegisteredEndpoints());
    EXPECT_NO_THROW(apimodule::ApiModule::getRegisteredModuleCount());
    EXPECT_NO_THROW(apimodule::ApiModule::createAllModules());
    
    // Test that the compatibility layer delegates to the same singleton
    size_t directCount = apirouter::ApiRouter::getEndpointCountGlobal();
    size_t compatCount = apimodule::ApiModule::getEndpointCount();
    EXPECT_EQ(directCount, compatCount);
}

TEST_F(ApiRouterInterfaceTest, TypeAliasesWork) {
    // Test that the type aliases in the compatibility layer work
    
    // Create a test module using the compatibility aliases
    class TestCompatModule : public apimodule::IApiModule {
    public:
        void registerEndpoints(apimodule::IEndpointRegistrar& registrar) override {
            registrar.registerHttpHandler("/compat", "GET",
                [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                    responseBody = "Compatibility test";
                    statusCode = 200;
                });
        }
    };
    
    // Should compile and work correctly
    auto module = std::make_unique<TestCompatModule>();
    EXPECT_NE(module, nullptr);
    
    // Test that it can register with the real router through compatibility layer
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
