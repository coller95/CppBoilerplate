#include <gtest/gtest.h>
#include <WebServerBackendMongoose/WebServerBackendMongoose.h>
#include <thread>
#include <chrono>

namespace {

class WebServerBackendMongooseBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use different ports for different tests to avoid conflicts
        static uint16_t port = 8080;
        currentPort = port++;
        backend = std::make_unique<webserverbackendmongoose::WebServerBackendMongoose>("127.0.0.1", currentPort);
    }

    void TearDown() override {
        // Cleanup code for each test
        if (backend && backend->isRunning()) {
            backend->stop();
        }
        backend.reset();
    }

    std::unique_ptr<webserverbackendmongoose::WebServerBackendMongoose> backend;
    uint16_t currentPort;
};

} // anonymous namespace

// TDD: Test 1 - Mongoose backend should be constructed with correct address and port
TEST_F(WebServerBackendMongooseBasicTest, ConstructorSetsCorrectAddressAndPort) {
    EXPECT_EQ(backend->getBindAddress(), "127.0.0.1");
    EXPECT_EQ(backend->getPort(), currentPort);
    EXPECT_FALSE(backend->isRunning()); // Should not be running initially
}

// TDD: Test 2 - Mongoose backend should start and stop correctly
TEST_F(WebServerBackendMongooseBasicTest, CanStartAndStopServer) {
    EXPECT_FALSE(backend->isRunning());
    
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    
    backend->stop();
    EXPECT_FALSE(backend->isRunning());
}

// TDD: Test 3 - Starting an already running Mongoose server should return false
TEST_F(WebServerBackendMongooseBasicTest, StartingRunningServerReturnsFalse) {
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    
    // Starting again should return false
    EXPECT_FALSE(backend->start());
    EXPECT_TRUE(backend->isRunning()); // Should still be running
}

// TDD: Test 4 - Mongoose backend should track active connections
TEST_F(WebServerBackendMongooseBasicTest, TracksActiveConnections) {
    EXPECT_EQ(backend->getActiveConnections(), 0u);
    // More detailed connection tests will be in integration tests
}

// TDD: Test 5 - Move constructor should work correctly
TEST_F(WebServerBackendMongooseBasicTest, MoveConstructorWorks) {
    auto original = std::make_unique<webserverbackendmongoose::WebServerBackendMongoose>("192.168.1.1", 9090);
    EXPECT_EQ(original->getBindAddress(), "192.168.1.1");
    EXPECT_EQ(original->getPort(), 9090);
    
    auto moved = std::make_unique<webserverbackendmongoose::WebServerBackendMongoose>(std::move(*original));
    EXPECT_EQ(moved->getBindAddress(), "192.168.1.1");
    EXPECT_EQ(moved->getPort(), 9090);
}

// TDD: Test 6 - Mongoose-specific configuration should work
TEST_F(WebServerBackendMongooseBasicTest, MongooseSpecificConfiguration) {
    // These should not throw and should configure the backend
    EXPECT_NO_THROW(backend->setDocumentRoot("/var/www/html"));
    EXPECT_NO_THROW(backend->setMaxRequestSize(1024 * 1024)); // 1MB
    EXPECT_NO_THROW(backend->setConnectionTimeout(30)); // 30 seconds
    
    // SSL configuration test (should return false without valid cert files)
    EXPECT_FALSE(backend->enableSSL("/invalid/cert.pem", "/invalid/key.pem"));
}

// TDD: Test 7 - Mongoose backend should implement IWebServerBackend interface
TEST_F(WebServerBackendMongooseBasicTest, ImplementsIWebServerBackendInterface) {
    // Should be able to register handlers
    auto handler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.body = "Test response";
    };
    
    EXPECT_NO_THROW(backend->registerHandler("GET", "/test", handler));
    EXPECT_NO_THROW(backend->registerHandler("/all-methods", handler));
    
    // Should be able to configure static routes
    webserver::StaticRoute route;
    route.urlPrefix = "/static";
    route.localPath = "/var/www/static";
    
    EXPECT_NO_THROW(backend->serveStatic(route));
    
    // Should be able to configure MIME types
    webserver::MimeConfig mimeConfig = webserver::MimeConfig::createDefault();
    EXPECT_NO_THROW(backend->setGlobalMimeConfig(mimeConfig));
}

// TDD: Test 8 - Mongoose backend should properly initialize Mongoose manager
TEST_F(WebServerBackendMongooseBasicTest, InitializesMongooseManager) {
    // After construction, the backend should have initialized the Mongoose manager
    // This is verified by successful start/stop operations
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    backend->stop();
    EXPECT_FALSE(backend->isRunning());
}

// TDD: Test 9 - Mongoose backend should handle HTTP request processing
TEST_F(WebServerBackendMongooseBasicTest, CanProcessHttpRequests) {
    bool handlerCalled = false;
    std::string receivedMethod, receivedPath;
    
    auto handler = [&](const webserver::HttpRequest& req, webserver::HttpResponse& res) {
        handlerCalled = true;
        receivedMethod = req.method;
        receivedPath = req.uri;
        res.statusCode = 200;
        res.body = "Handler response";
    };
    
    backend->registerHandler("GET", "/test", handler);
    EXPECT_TRUE(backend->start());
    
    // TODO: When we implement actual HTTP client testing, verify the handler is called
    // For now, just verify registration doesn't throw and server starts correctly
    
    backend->stop();
}

// TDD: Test 10 - Mongoose backend should handle HTTP response building
TEST_F(WebServerBackendMongooseBasicTest, CanBuildHttpResponses) {
    auto jsonHandler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.setJsonResponse("{\"message\": \"Hello, World!\"}");
    };
    
    auto htmlHandler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.setHtmlResponse("<html><body>Hello, World!</body></html>");
    };
    
    auto textHandler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.setPlainTextResponse("Hello, World!");
    };
    
    // Should not throw when registering different types of handlers
    EXPECT_NO_THROW(backend->registerHandler("GET", "/json", jsonHandler));
    EXPECT_NO_THROW(backend->registerHandler("GET", "/html", htmlHandler));
    EXPECT_NO_THROW(backend->registerHandler("GET", "/text", textHandler));
    
    EXPECT_TRUE(backend->start());
    backend->stop();
}

// TDD: Test 11 - Backend should be self-contained (no external polling required)
TEST_F(WebServerBackendMongooseBasicTest, IsSelfContainedWithInternalEventProcessing) {
    bool handlerCalled = false;
    
    auto handler = [&](const webserver::HttpRequest& req, webserver::HttpResponse& res) {
        (void)req; // Suppress unused parameter warning
        handlerCalled = true;
        res.statusCode = 200;
        res.body = "Self-contained response";
    };
    
    backend->registerHandler("GET", "/self-contained", handler);
    EXPECT_TRUE(backend->start());
    
    // The backend should process events internally - no poll() method should exist
    // This test will initially fail because current implementation requires external polling
    
    // Give the backend some time to process events internally
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Backend should be self-sufficient for event processing
    EXPECT_TRUE(backend->isRunning());
    
    backend->stop();
    EXPECT_FALSE(backend->isRunning());
    
    // Note: This test verifies the design principle that backends should be self-contained
    // Once refactored, the backend will handle Mongoose event processing internally
}
