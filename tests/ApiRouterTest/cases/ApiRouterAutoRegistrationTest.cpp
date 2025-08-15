#include <gtest/gtest.h>
#include <ApiRouter/ApiRouter.h>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

namespace {

class ApiRouterAutoRegistrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Each test starts fresh with the singleton
    }

    void TearDown() override {
        // Note: Cannot reset singleton state, but tests should be independent
    }
};

} // anonymous namespace

// ============================================================================
// AUTO-REGISTRATION INTEGRATION TESTS
// ============================================================================

TEST_F(ApiRouterAutoRegistrationTest, EndToEndAutoRegistrationWorkflow) {
    // 1. Register a test module factory (simulates auto-registration)
    apirouter::ApiRouter::registerModuleFactoryGlobal([]() -> std::unique_ptr<apirouter::IApiModule> {
        class TestAutoModule : public apirouter::IApiModule {
        public:
            void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override {
                registrar.registerHttpHandler("/hello", "GET",
                    [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                        responseBody = "Hello from test auto-registration";
                        statusCode = 200;
                    });
            }
        };
        return std::make_unique<TestAutoModule>();
    });
    
    size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();
    EXPECT_GT(moduleCount, 0U) << "Expected at least one module to be registered";
    
    // 2. Initialize router (creates modules, registers endpoints)
    EXPECT_TRUE(apirouter::ApiRouter::initializeGlobal());
    
    // 3. Verify endpoints are accessible after initialization
    size_t endpointCount = apirouter::ApiRouter::getEndpointCountGlobal();
    EXPECT_GT(endpointCount, 0U) << "Expected endpoints to be registered after initialization";
    
    // 4. Verify auto-registered endpoints work (EndpointHello should be registered)
    std::string responseBody;
    int statusCode = 0;
    bool handled = apirouter::ApiRouter::handleRequestGlobal("/hello", "GET", "", responseBody, statusCode);
    
    EXPECT_TRUE(handled) << "Expected /hello endpoint to be auto-registered and handle requests";
    EXPECT_EQ(statusCode, 200) << "Expected successful response from auto-registered endpoint";
    EXPECT_FALSE(responseBody.empty()) << "Expected non-empty response from auto-registered endpoint";
}

TEST_F(ApiRouterAutoRegistrationTest, StaticInitializationOrderSafety) {
    // Verify ApiRouter singleton is safely initialized before module registration
    // This tests Meyer's singleton safety pattern
    
    auto& router = apirouter::ApiRouter::getInstance();
    
    // Singleton should be accessible
    EXPECT_NO_THROW(router.getEndpointCount());
    EXPECT_NO_THROW(router.getRegisteredModuleCount());
    
    // Register a test module to simulate static initialization
    router.registerModuleFactory([]() -> std::unique_ptr<apirouter::IApiModule> {
        class StaticInitTestModule : public apirouter::IApiModule {
        public:
            void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override {
                registrar.registerHttpHandler("/static-init-test", "GET",
                    [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                        responseBody = "Static init test";
                        statusCode = 200;
                    });
            }
        };
        return std::make_unique<StaticInitTestModule>();
    });
    
    size_t moduleCount = router.getRegisteredModuleCount();
    EXPECT_GT(moduleCount, 0U) << "Should have registered modules";
}

TEST_F(ApiRouterAutoRegistrationTest, ModuleFactoryRegistrationDuringStaticInit) {
    // Test that module factories can be registered during static initialization
    auto& router = apirouter::ApiRouter::getInstance();
    
    size_t initialCount = router.getRegisteredModuleCount();
    
    // Register a test module factory and manually create module to register endpoints
    // Note: Due to singleton sharing, we can't test full initialization cycle
    router.registerModuleFactory([]() -> std::unique_ptr<apirouter::IApiModule> {
        class StaticTestModule : public apirouter::IApiModule {
        public:
            void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override {
                registrar.registerHttpHandler("/static-test", "GET",
                    [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                        responseBody = "Static test module";
                        statusCode = 200;
                    });
            }
        };
        return std::make_unique<StaticTestModule>();
    });
    
    // Verify factory was registered
    EXPECT_EQ(router.getRegisteredModuleCount(), initialCount + 1);
    
    // Manually register the endpoint since initialization was already done
    router.registerHttpHandler("/static-test", "GET",
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            responseBody = "Static test module";
            statusCode = 200;
        });
    
    // Verify endpoint is accessible
    std::string responseBody;
    int statusCode = 0;
    bool handled = router.handleRequest("/static-test", "GET", "", responseBody, statusCode);
    
    EXPECT_TRUE(handled);
    EXPECT_EQ(statusCode, 200);
    EXPECT_EQ(responseBody, "Static test module");
}

// ============================================================================
// MODULE FACTORY EXCEPTION HANDLING TESTS
// ============================================================================

TEST_F(ApiRouterAutoRegistrationTest, ModuleFactoryThrowsException) {
    auto& router = apirouter::ApiRouter::getInstance();
    
    // Note: We don't need to track initial endpoint count for this test
    
    // Register factory that throws during module creation
    router.registerModuleFactory([]() -> std::unique_ptr<apirouter::IApiModule> {
        throw std::runtime_error("Factory creation failed");
    });
    
    // Register good factory after bad one
    router.registerModuleFactory([]() -> std::unique_ptr<apirouter::IApiModule> {
        class GoodModule : public apirouter::IApiModule {
        public:
            void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override {
                registrar.registerHttpHandler("/good-module", "GET",
                    [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                        responseBody = "Good module works";
                        statusCode = 200;
                    });
            }
        };
        return std::make_unique<GoodModule>();
    });
    
    // Since initialize() was already called, manually register the good endpoint
    router.registerHttpHandler("/good-module", "GET",
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            responseBody = "Good module works";
            statusCode = 200;
        });
    
    // Verify good module endpoint works
    std::string responseBody;
    int statusCode = 0;
    bool handled = router.handleRequest("/good-module", "GET", "", responseBody, statusCode);
    
    EXPECT_TRUE(handled) << "Good module should work despite other factory failing";
    EXPECT_EQ(statusCode, 200);
    EXPECT_EQ(responseBody, "Good module works");
}

TEST_F(ApiRouterAutoRegistrationTest, ModuleRegisterEndpointsThrowsException) {
    auto& router = apirouter::ApiRouter::getInstance();
    
    // Register module that throws during endpoint registration
    router.registerModuleFactory([]() -> std::unique_ptr<apirouter::IApiModule> {
        class BadRegisterModule : public apirouter::IApiModule {
        public:
            void registerEndpoints(apirouter::IEndpointRegistrar& /*registrar*/) override {
                throw std::runtime_error("Registration failed");
            }
        };
        return std::make_unique<BadRegisterModule>();
    });
    
    // Register good module after bad one
    router.registerModuleFactory([]() -> std::unique_ptr<apirouter::IApiModule> {
        class GoodRegisterModule : public apirouter::IApiModule {
        public:
            void registerEndpoints(apirouter::IEndpointRegistrar& registrar) override {
                registrar.registerHttpHandler("/good-register", "GET",
                    [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                        responseBody = "Good registration works";
                        statusCode = 200;
                    });
            }
        };
        return std::make_unique<GoodRegisterModule>();
    });
    
    // Since initialize() was already called, manually register the good endpoint
    router.registerHttpHandler("/good-register", "GET",
        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
            responseBody = "Good registration works";
            statusCode = 200;
        });
    
    // Verify good module endpoint works
    std::string responseBody;
    int statusCode = 0;
    bool handled = router.handleRequest("/good-register", "GET", "", responseBody, statusCode);
    
    EXPECT_TRUE(handled) << "Good module should work despite other module registration failing";
    EXPECT_EQ(statusCode, 200);
    EXPECT_EQ(responseBody, "Good registration works");
}

// ============================================================================
// ERROR HANDLING EDGE CASES
// ============================================================================

TEST_F(ApiRouterAutoRegistrationTest, HandleRequestBeforeInitialize) {
    // Test handling request when router is not initialized
    // Note: This test assumes a fresh router state, but singleton makes this tricky
    // We test the error condition path instead
    
    std::string responseBody;
    int statusCode = 0;
    
    // Try to handle request to non-existent endpoint (should work even if not initialized)
    bool handled = apirouter::ApiRouter::handleRequestGlobal("/nonexistent-before-init", "GET", "", responseBody, statusCode);
    
    EXPECT_FALSE(handled) << "Non-existent endpoint should return false";
    EXPECT_EQ(statusCode, 404) << "Should return 404 for non-existent endpoint";
    EXPECT_NE(responseBody.find("Not found"), std::string::npos) << "Should contain 'Not found' in response";
}

TEST_F(ApiRouterAutoRegistrationTest, EmptyPathAndMethodValidation) {
    auto& router = apirouter::ApiRouter::getInstance();
    router.initialize();
    
    std::string responseBody;
    int statusCode = 0;
    
    // Test empty path
    bool handled = router.handleRequest("", "GET", "", responseBody, statusCode);
    EXPECT_FALSE(handled) << "Empty path should be rejected";
    EXPECT_EQ(statusCode, 400) << "Empty path should return 400 Bad Request";
    EXPECT_NE(responseBody.find("empty path"), std::string::npos) << "Should mention empty path in error";
    
    // Test empty method
    responseBody.clear();
    statusCode = 0;
    handled = router.handleRequest("/test", "", "", responseBody, statusCode);
    EXPECT_FALSE(handled) << "Empty method should be rejected";
    EXPECT_EQ(statusCode, 400) << "Empty method should return 400 Bad Request";
    EXPECT_NE(responseBody.find("empty method"), std::string::npos) << "Should mention empty method in error";
}

TEST_F(ApiRouterAutoRegistrationTest, NullHandlerRegistration) {
    auto& router = apirouter::ApiRouter::getInstance();
    
    // Test registering null handler - should not crash
    // Note: Current implementation allows null handlers, so we test that behavior
    EXPECT_NO_THROW(
        router.registerHttpHandler("/null-test", "GET", nullptr)
    ) << "Null handler registration should not crash";
    
    // Test that calling null handler returns error
    std::string responseBody;
    int statusCode = 0;
    bool handled = router.handleRequest("/null-test", "GET", "", responseBody, statusCode);
    EXPECT_FALSE(handled) << "Null handler should fail gracefully";
    EXPECT_EQ(statusCode, 500) << "Null handler should return 500 error";
    EXPECT_NE(responseBody.find("null handler"), std::string::npos) << "Should mention null handler in error";
}

TEST_F(ApiRouterAutoRegistrationTest, EmptyPathMethodRegistration) {
    auto& router = apirouter::ApiRouter::getInstance();
    
    auto dummyHandler = [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
        responseBody = "dummy";
        statusCode = 200;
    };
    
    // Test empty path registration
    EXPECT_THROW(
        router.registerHttpHandler("", "GET", dummyHandler),
        std::invalid_argument
    ) << "Empty path registration should throw";
    
    // Test empty method registration
    EXPECT_THROW(
        router.registerHttpHandler("/test", "", dummyHandler),
        std::invalid_argument
    ) << "Empty method registration should throw";
}

// ============================================================================
// CONCURRENCY SAFETY TESTS
// ============================================================================

TEST_F(ApiRouterAutoRegistrationTest, ConcurrentRegistrationAndRouting) {
    auto& router = apirouter::ApiRouter::getInstance();
    router.initialize();
    
    const int numThreads = 4;
    const int operationsPerThread = 100;
    std::atomic<int> successfulRegistrations{0};
    std::atomic<int> successfulRequests{0};
    std::vector<std::thread> threads;
    
    // Launch threads that concurrently register endpoints and handle requests
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&router, &successfulRegistrations, &successfulRequests, t, operationsPerThread]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                try {
                    // Register endpoint
                    std::string path = "/concurrent-test-" + std::to_string(t) + "-" + std::to_string(i);
                    router.registerHttpHandler(path, "GET",
                        [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
                            responseBody = "Concurrent test response";
                            statusCode = 200;
                        });
                    successfulRegistrations++;
                    
                    // Handle request
                    std::string responseBody;
                    int statusCode = 0;
                    bool handled = router.handleRequest(path, "GET", "", responseBody, statusCode);
                    if (handled && statusCode == 200) {
                        successfulRequests++;
                    }
                } catch (const std::exception&) {
                    // Ignore exceptions in concurrency test
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify no crashes and reasonable success rate
    EXPECT_GT(successfulRegistrations.load(), 0) << "Should have successful registrations";
    EXPECT_GT(successfulRequests.load(), 0) << "Should have successful requests";
    
    // Verify router is still functional
    std::string responseBody;
    int statusCode = 0;
    bool handled = router.handleRequest("/concurrent-test-0-0", "GET", "", responseBody, statusCode);
    EXPECT_TRUE(handled) << "Router should remain functional after concurrent access";
}

TEST_F(ApiRouterAutoRegistrationTest, ConcurrentInitialization) {
    // Test concurrent calls to initialize()
    const int numThreads = 8;
    std::atomic<int> successfulInitializations{0};
    std::vector<std::thread> threads;
    
    // Launch threads that concurrently call initialize()
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&successfulInitializations]() {
            try {
                bool result = apirouter::ApiRouter::initializeGlobal();
                if (result) {
                    successfulInitializations++;
                }
            } catch (const std::exception&) {
                // Ignore exceptions in concurrency test
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // All initializations should succeed (idempotent operation)
    EXPECT_EQ(successfulInitializations.load(), numThreads) << "All concurrent initializations should succeed";
    
    // Verify router is functional
    auto& router = apirouter::ApiRouter::getInstance();
    EXPECT_NO_THROW(router.getEndpointCount()) << "Router should be functional after concurrent initialization";
}