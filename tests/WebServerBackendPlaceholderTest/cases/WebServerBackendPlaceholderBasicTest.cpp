#include <gtest/gtest.h>
#include <WebServerBackendPlaceholder/WebServerBackendPlaceholder.h>

namespace {

class WebServerBackendPlaceholderBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use different ports for different tests to avoid conflicts
        static uint16_t port = 7000;
        currentPort = port++;
        backend = std::make_unique<webserverbackendplaceholder::WebServerBackendPlaceholder>("127.0.0.1", currentPort);
    }

    void TearDown() override {
        if (backend && backend->isRunning()) {
            backend->stop();
        }
        backend.reset();
    }

    std::unique_ptr<webserverbackendplaceholder::WebServerBackendPlaceholder> backend;
    uint16_t currentPort;
};

} // anonymous namespace

// TDD: Test 1 - Placeholder backend should implement IWebServerBackend interface
TEST_F(WebServerBackendPlaceholderBasicTest, ImplementsIWebServerBackendInterface) {
    // Should be castable to IWebServerBackend
    webserver::IWebServerBackend* basePtr = backend.get();
    EXPECT_NE(basePtr, nullptr);
    
    // Should have correct bind info
    EXPECT_EQ(backend->getBindAddress(), "127.0.0.1");
    EXPECT_EQ(backend->getPort(), currentPort);
}

// TDD: Test 2 - Placeholder backend should start and stop without actual networking
TEST_F(WebServerBackendPlaceholderBasicTest, StartsAndStopsWithoutNetworking) {
    EXPECT_FALSE(backend->isRunning());
    
    // Should start successfully (no actual network binding)
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    
    // Should stop successfully
    backend->stop();
    EXPECT_FALSE(backend->isRunning());
}

// TDD: Test 3 - Placeholder backend should register and track handlers
TEST_F(WebServerBackendPlaceholderBasicTest, RegistersAndTracksHandlers) {
    EXPECT_EQ(backend->getHandlerCount(), 0u);
    
    auto handler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.setPlainTextResponse("Test response");
    };
    
    backend->registerHandler("GET", "/test", handler);
    EXPECT_EQ(backend->getHandlerCount(), 1u);
    
    backend->registerHandler("/all-methods", handler);
    EXPECT_EQ(backend->getHandlerCount(), 2u);
    
    backend->clearAllHandlers();
    EXPECT_EQ(backend->getHandlerCount(), 0u);
}

// TDD: Test 4 - Placeholder backend should support simulated latency
TEST_F(WebServerBackendPlaceholderBasicTest, SupportsSimulatedLatency) {
    // Should accept latency settings
    EXPECT_NO_THROW(backend->setSimulatedLatency(10));
    
    // Operations should still work (latency is internal)
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    
    // Reset latency
    backend->setSimulatedLatency(0);
    backend->stop();
}
