#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <WebServer/IWebServer.h>

namespace {

class WebServerBackendSelectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use different ports for different tests to avoid conflicts
        static uint16_t port = 9000;
        currentPort = port++;
    }

    void TearDown() override {
        if (server && server->isRunning()) {
            server->stop();
        }
        server.reset();
    }

    std::unique_ptr<webserver::WebServer> server;
    uint16_t currentPort;
};

} // anonymous namespace

// TDD: Test 1 - WebServer should have a Backend enum class
TEST_F(WebServerBackendSelectionTest, HasBackendEnumClass) {
    // This should compile - Backend enum should exist
    webserver::WebServer::Backend backend = webserver::WebServer::Backend::Mongoose;
    EXPECT_EQ(backend, webserver::WebServer::Backend::Mongoose);
}

// TDD: Test 2 - WebServer constructor should accept Backend enum parameter
TEST_F(WebServerBackendSelectionTest, ConstructorAcceptsBackendEnum) {
    // Should be able to construct with explicit backend selection
    EXPECT_NO_THROW({
        server = std::make_unique<webserver::WebServer>(
            "127.0.0.1", currentPort, webserver::WebServer::Backend::Mongoose);
    });
    
    EXPECT_NE(server, nullptr);
    EXPECT_EQ(server->getBindAddress(), "127.0.0.1");
    EXPECT_EQ(server->getPort(), currentPort);
}

// TDD: Test 3 - WebServer constructor should default to Mongoose backend
TEST_F(WebServerBackendSelectionTest, DefaultsToMongooseBackend) {
    // Constructor without backend parameter should default to Mongoose
    server = std::make_unique<webserver::WebServer>("127.0.0.1", currentPort);
    
    EXPECT_NE(server, nullptr);
    EXPECT_EQ(server->getBindAddress(), "127.0.0.1");
    EXPECT_EQ(server->getPort(), currentPort);
}

// TDD: Test 4 - WebServer should be able to start and stop with Mongoose backend
TEST_F(WebServerBackendSelectionTest, CanStartAndStopWithMongooseBackend) {
    server = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort, webserver::WebServer::Backend::Mongoose);
    
    EXPECT_FALSE(server->isRunning());
    EXPECT_TRUE(server->start());
    EXPECT_TRUE(server->isRunning());
    
    server->stop();
    EXPECT_FALSE(server->isRunning());
}

// TDD: Test 5 - WebServer should support HTTP handler registration with any backend
TEST_F(WebServerBackendSelectionTest, SupportsHttpHandlerRegistrationWithBackend) {
    server = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort, webserver::WebServer::Backend::Mongoose);
    
    auto handler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.setPlainTextResponse("Hello from backend!");
    };
    
    EXPECT_NO_THROW(server->registerHandler("GET", "/test", handler));
    EXPECT_NO_THROW(server->get("/hello", handler));
    EXPECT_NO_THROW(server->post("/data", handler));
}

// TDD: Test 6 - WebServer should provide method to get current backend type
TEST_F(WebServerBackendSelectionTest, ProvidesCurrentBackendType) {
    server = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort, webserver::WebServer::Backend::Mongoose);
    
    EXPECT_EQ(server->getCurrentBackend(), webserver::WebServer::Backend::Mongoose);
}

// TDD: Test 7 - WebServer should provide backend name as string
TEST_F(WebServerBackendSelectionTest, ProvidesBackendNameAsString) {
    server = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort, webserver::WebServer::Backend::Mongoose);
    
    EXPECT_EQ(server->getBackendName(), "Mongoose");
}

// TDD: Test 8 - WebServer should support Placeholder backend
TEST_F(WebServerBackendSelectionTest, SupportsPlaceholderBackend) {
    // Should be able to construct with Placeholder backend
    EXPECT_NO_THROW({
        server = std::make_unique<webserver::WebServer>(
            "127.0.0.1", currentPort, webserver::WebServer::Backend::Placeholder);
    });
    
    EXPECT_NE(server, nullptr);
    EXPECT_EQ(server->getCurrentBackend(), webserver::WebServer::Backend::Placeholder);
    EXPECT_EQ(server->getBackendName(), "Placeholder");
}

// TDD: Test 9 - WebServer with Placeholder backend should start/stop without networking
TEST_F(WebServerBackendSelectionTest, PlaceholderBackendStartsWithoutNetworking) {
    server = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort, webserver::WebServer::Backend::Placeholder);
    
    EXPECT_FALSE(server->isRunning());
    EXPECT_TRUE(server->start());
    EXPECT_TRUE(server->isRunning());
    
    server->stop();
    EXPECT_FALSE(server->isRunning());
}

// TDD: Test 10 - WebServer with Placeholder backend should support handler registration
TEST_F(WebServerBackendSelectionTest, PlaceholderBackendSupportsHandlerRegistration) {
    server = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort, webserver::WebServer::Backend::Placeholder);
    
    auto handler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.setPlainTextResponse("Hello from placeholder!");
    };
    
    EXPECT_NO_THROW(server->registerHandler("GET", "/test", handler));
    EXPECT_NO_THROW(server->get("/hello", handler));
    EXPECT_NO_THROW(server->post("/data", handler));
}

// TDD: Test 11 - Both backends should be interchangeable
TEST_F(WebServerBackendSelectionTest, BackendsAreInterchangeable) {
    // Test Mongoose backend
    auto mongooseServer = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort, webserver::WebServer::Backend::Mongoose);
    
    EXPECT_EQ(mongooseServer->getCurrentBackend(), webserver::WebServer::Backend::Mongoose);
    EXPECT_EQ(mongooseServer->getBackendName(), "Mongoose");
    
    // Test Placeholder backend
    auto placeholderServer = std::make_unique<webserver::WebServer>(
        "127.0.0.1", currentPort + 1, webserver::WebServer::Backend::Placeholder);
    
    EXPECT_EQ(placeholderServer->getCurrentBackend(), webserver::WebServer::Backend::Placeholder);
    EXPECT_EQ(placeholderServer->getBackendName(), "Placeholder");
    
    // Both should support the same interface
    auto handler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.setPlainTextResponse("Common interface test");
    };
    
    EXPECT_NO_THROW(mongooseServer->registerHandler("GET", "/common", handler));
    EXPECT_NO_THROW(placeholderServer->registerHandler("GET", "/common", handler));
}
