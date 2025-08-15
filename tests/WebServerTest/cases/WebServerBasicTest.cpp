#include <gtest/gtest.h>
#include <WebServer/WebServer.h>

namespace {

class WebServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }

    void TearDown() override {
        // Cleanup code for each test
    }
};

} // anonymous namespace

TEST_F(WebServerTest, ConstructorCreatesValidInstance) {
    webserver::WebServer webServer("127.0.0.1", 8081);
    
    EXPECT_EQ(webServer.getBindAddress(), "127.0.0.1");
    EXPECT_EQ(webServer.getPort(), 8081);
    EXPECT_FALSE(webServer.isRunning());
}

TEST_F(WebServerTest, CanSetGlobalRequestHandler) {
    webserver::WebServer webServer("127.0.0.1", 8082);
    
    bool handlerCalled = false;
    auto handler = [&handlerCalled](const webserver::HttpRequest&, webserver::HttpResponse&) {
        handlerCalled = true;
    };
    
    // Should not throw when setting global handler
    EXPECT_NO_THROW(webServer.setGlobalRequestHandler(handler));
}

TEST_F(WebServerTest, ServerLifecycleOperations) {
    webserver::WebServer webServer("127.0.0.1", 8083);
    
    // Initially not running
    EXPECT_FALSE(webServer.isRunning());
    
    // Can start (might fail if port is in use, but should not crash)
    EXPECT_NO_THROW(webServer.start());
    
    // Can stop safely
    EXPECT_NO_THROW(webServer.stop());
    
    // After stop, should not be running
    EXPECT_FALSE(webServer.isRunning());
}
