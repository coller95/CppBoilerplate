#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <memory>

namespace {

class WebServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use unique ports for each test to avoid conflicts
        static uint16_t portCounter = 9000;
        testPort = ++portCounter;
    }

    void TearDown() override {
        // Cleanup code for each test
    }
    
    uint16_t testPort;
};

} // anonymous namespace

// ============================================================================
// CONFIGURATION TESTS - Test WebServer's configuration management
// ============================================================================

TEST_F(WebServerTest, ConstructorSetsConfigurationCorrectly) {
    webserver::WebServer webServer("192.168.1.100", testPort);
    
    EXPECT_EQ(webServer.getBindAddress(), "192.168.1.100");
    EXPECT_EQ(webServer.getPort(), testPort);
}

TEST_F(WebServerTest, ConstructorAcceptsValidPortRanges) {
    // Test valid port ranges
    webserver::WebServer webServer1("127.0.0.1", 1024);    // Low port
    webserver::WebServer webServer2("127.0.0.1", 8080);    // Common port
    webserver::WebServer webServer3("127.0.0.1", 65535);   // Max port
    
    EXPECT_EQ(webServer1.getPort(), 1024);
    EXPECT_EQ(webServer2.getPort(), 8080);
    EXPECT_EQ(webServer3.getPort(), 65535);
}

TEST_F(WebServerTest, HandlesVariousBindAddresses) {
    webserver::WebServer localhost("127.0.0.1", testPort);
    webserver::WebServer allInterfaces("0.0.0.0", testPort + 1);
    webserver::WebServer ipv6("::1", testPort + 2);
    
    EXPECT_EQ(localhost.getBindAddress(), "127.0.0.1");
    EXPECT_EQ(allInterfaces.getBindAddress(), "0.0.0.0");
    EXPECT_EQ(ipv6.getBindAddress(), "::1");
}

// ============================================================================
// LIFECYCLE TESTS - Test WebServer's state management without network
// ============================================================================

TEST_F(WebServerTest, InitialStateIsNotRunning) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    EXPECT_FALSE(webServer.isRunning());
    EXPECT_EQ(webServer.getActiveConnections(), 0u);
}

TEST_F(WebServerTest, LifecycleOperationsDoNotCrash) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    // Should not crash, regardless of success/failure
    EXPECT_NO_THROW(webServer.start());
    EXPECT_NO_THROW(webServer.stop());
    EXPECT_NO_THROW(webServer.isRunning());
    EXPECT_NO_THROW(webServer.getActiveConnections());
}

TEST_F(WebServerTest, CanStopWithoutStarting) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    // Should handle stop() gracefully even if never started
    EXPECT_NO_THROW(webServer.stop());
    EXPECT_FALSE(webServer.isRunning());
}

TEST_F(WebServerTest, MultipleStartCallsAreSafe) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    // Multiple start() calls should be safe
    EXPECT_NO_THROW(webServer.start());
    EXPECT_NO_THROW(webServer.start());  // Second call should be safe
    EXPECT_NO_THROW(webServer.stop());
}

TEST_F(WebServerTest, MultipleStopCallsAreSafe) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    EXPECT_NO_THROW(webServer.start());
    EXPECT_NO_THROW(webServer.stop());
    EXPECT_NO_THROW(webServer.stop());   // Second call should be safe
}

// ============================================================================
// HANDLER DELEGATION TESTS - Test WebServer's core responsibility
// ============================================================================

TEST_F(WebServerTest, CanSetGlobalRequestHandler) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    auto handler = [](const webserver::HttpRequest&, webserver::HttpResponse&) {
        // Empty handler for testing
    };
    
    // Should not throw when setting global handler
    EXPECT_NO_THROW(webServer.setGlobalRequestHandler(handler));
}

TEST_F(WebServerTest, CanReplaceGlobalRequestHandler) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    auto handler1 = [](const webserver::HttpRequest&, webserver::HttpResponse&) {};
    auto handler2 = [](const webserver::HttpRequest&, webserver::HttpResponse&) {};
    
    // Should be able to set and replace handlers
    EXPECT_NO_THROW(webServer.setGlobalRequestHandler(handler1));
    EXPECT_NO_THROW(webServer.setGlobalRequestHandler(handler2));
}

TEST_F(WebServerTest, CanSetNullHandler) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    // Should handle null handler gracefully
    EXPECT_NO_THROW(webServer.setGlobalRequestHandler(nullptr));
}

// ============================================================================
// STATIC FILE SERVING TESTS - Test configuration interface only
// ============================================================================

TEST_F(WebServerTest, StaticFileConfigurationDoesNotCrash) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    webserver::StaticRoute route;
    route.urlPrefix = "/static/";
    route.localPath = "/var/www/static";
    route.defaultFile = "index.html";
    
    // Should not crash (implementation may be no-op)
    EXPECT_NO_THROW(webServer.serveStatic(route));
}

TEST_F(WebServerTest, MimeConfigurationDoesNotCrash) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    auto mimeConfig = webserver::MimeConfig::createDefault();
    
    // Should not crash
    EXPECT_NO_THROW(webServer.setGlobalMimeConfig(mimeConfig));
    EXPECT_NO_THROW(webServer.serveStaticWithMime("/css/", "/var/www/css", mimeConfig));
}

// ============================================================================
// WEBSOCKET INTERFACE TESTS - Test interface only
// ============================================================================

TEST_F(WebServerTest, WebSocketConfigurationDoesNotCrash) {
    webserver::WebServer webServer("127.0.0.1", testPort);
    
    auto wsHandler = [](const std::string&, const std::string&) {
        // Empty WebSocket handler
    };
    
    // Should not crash (implementation may be no-op)
    EXPECT_NO_THROW(webServer.registerWebSocketHandler("/ws", wsHandler));
    EXPECT_NO_THROW(webServer.sendWebSocketMessage("conn123", "test message"));
}

// ============================================================================
// MOVE SEMANTICS TESTS - Test RAII and resource management
// ============================================================================

TEST_F(WebServerTest, MoveConstructorWorks) {
    auto webServer1 = std::make_unique<webserver::WebServer>("127.0.0.1", testPort);
    
    // Move constructor should work
    webserver::WebServer webServer2 = std::move(*webServer1);
    
    EXPECT_EQ(webServer2.getBindAddress(), "127.0.0.1");
    EXPECT_EQ(webServer2.getPort(), testPort);
}

TEST_F(WebServerTest, MoveAssignmentWorks) {
    webserver::WebServer webServer1("127.0.0.1", testPort);
    webserver::WebServer webServer2("192.168.1.1", testPort + 1);
    
    // Move assignment should work
    webServer2 = std::move(webServer1);
    
    EXPECT_EQ(webServer2.getBindAddress(), "127.0.0.1");
    EXPECT_EQ(webServer2.getPort(), testPort);
}

// ============================================================================
// DESTRUCTOR SAFETY TESTS - Test cleanup in various states
// ============================================================================

TEST_F(WebServerTest, DestructorSafeWithoutStarting) {
    // Should destruct safely without ever calling start()
    EXPECT_NO_THROW({
        webserver::WebServer webServer("127.0.0.1", testPort);
        // Destructor called automatically
    });
}

TEST_F(WebServerTest, DestructorSafeAfterStarting) {
    // Should destruct safely after start() (with or without stop())
    EXPECT_NO_THROW({
        webserver::WebServer webServer("127.0.0.1", testPort);
        webServer.start();
        // Destructor should handle cleanup
    });
}

TEST_F(WebServerTest, DestructorSafeAfterStartStop) {
    // Should destruct safely after proper start/stop cycle
    EXPECT_NO_THROW({
        webserver::WebServer webServer("127.0.0.1", testPort);
        webServer.start();
        webServer.stop();
        // Destructor called automatically
    });
}
