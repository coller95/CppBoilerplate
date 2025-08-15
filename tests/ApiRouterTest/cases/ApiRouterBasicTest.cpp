#include <gtest/gtest.h>
#include <ApiRouter/ApiRouter.h>
#include <memory>

namespace {

class ApiRouterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
        // Note: ApiRouter is a singleton, so we test the singleton instance
    }

    void TearDown() override {
        // Cleanup code for each test
        // Note: Cannot cleanup singleton, but tests should be independent
    }
};

} // anonymous namespace

TEST_F(ApiRouterTest, SingletonInstanceIsValid) {
    auto& router = apirouter::ApiRouter::getInstance();
    // Verify we can get the singleton instance
    EXPECT_NO_THROW(router.getEndpointCount());
}

TEST_F(ApiRouterTest, SingletonReturnsSameInstance) {
    auto& router1 = apirouter::ApiRouter::getInstance();
    auto& router2 = apirouter::ApiRouter::getInstance();
    
    // Verify singleton returns the same instance
    EXPECT_EQ(&router1, &router2);
}

TEST_F(ApiRouterTest, InitializeReturnsTrueOnSuccess) {
    auto& router = apirouter::ApiRouter::getInstance();
    EXPECT_TRUE(router.initialize());
    
    // Second initialization should also return true
    EXPECT_TRUE(router.initialize());
}

TEST_F(ApiRouterTest, StaticGlobalMethodsWork) {
    // Test that static global methods can be called
    EXPECT_NO_THROW(apirouter::ApiRouter::initializeGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::getEndpointCountGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::getRegisteredEndpointsGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::getRegisteredModuleCountGlobal());
    EXPECT_NO_THROW(apirouter::ApiRouter::createAllModulesGlobal());
}

TEST_F(ApiRouterTest, EndpointRegistrationWorks) {
    auto& router = apirouter::ApiRouter::getInstance();
    
    // Get initial endpoint count
    size_t initialCount = router.getEndpointCount();
    
    // Register a test endpoint
    router.registerHttpHandler("/test", "GET", 
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            responseBody = "Test response";
            statusCode = 200;
        });
    
    // Verify endpoint was registered
    EXPECT_EQ(router.getEndpointCount(), initialCount + 1);
    
    auto endpoints = router.getRegisteredEndpoints();
    bool foundTestEndpoint = false;
    for (const auto& endpoint : endpoints) {
        if (endpoint == "/test:GET") {
            foundTestEndpoint = true;
            break;
        }
    }
    EXPECT_TRUE(foundTestEndpoint);
}

TEST_F(ApiRouterTest, RequestHandlingWorks) {
    auto& router = apirouter::ApiRouter::getInstance();
    router.initialize();
    
    // Register a test endpoint
    router.registerHttpHandler("/hello", "GET", 
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            responseBody = "Hello, World!";
            statusCode = 200;
        });
    
    // Test handling a request to our endpoint
    std::string responseBody;
    int statusCode = 0;
    bool handled = router.handleRequest("/hello", "GET", "", responseBody, statusCode);
    
    EXPECT_TRUE(handled);
    EXPECT_EQ(statusCode, 200);
    EXPECT_EQ(responseBody, "Hello, World!");
}

TEST_F(ApiRouterTest, NonExistentEndpointReturns404) {
    auto& router = apirouter::ApiRouter::getInstance();
    router.initialize();
    
    // Test handling a request to a non-existent endpoint
    std::string responseBody;
    int statusCode = 0;
    bool handled = router.handleRequest("/nonexistent", "GET", "", responseBody, statusCode);
    
    EXPECT_FALSE(handled);
    EXPECT_EQ(statusCode, 404);
    EXPECT_EQ(responseBody, "Not found: GET /nonexistent is not registered");
}

TEST_F(ApiRouterTest, ModuleFactoryRegistrationWorks) {
    auto& router = apirouter::ApiRouter::getInstance();
    
    // Get initial module count
    size_t initialCount = router.getRegisteredModuleCount();
    
    // Register a test module factory
    router.registerModuleFactory([]() -> std::unique_ptr<apirouter::IApiModule> {
        class TestModule : public apirouter::IApiModule {
        public:
            void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override {
                registrar.registerHttpHandler("/testmodule", "GET",
                    [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                        responseBody = "Test module response";
                        statusCode = 200;
                    });
            }
        };
        return std::make_unique<TestModule>();
    });
    
    // Verify module factory was registered
    EXPECT_EQ(router.getRegisteredModuleCount(), initialCount + 1);
}
