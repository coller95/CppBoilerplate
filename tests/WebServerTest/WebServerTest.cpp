#include <gtest/gtest.h>
#include <WebServer/WebServer.h>

// Basic construction/destruction test
TEST(WebServerTest, CanConstructAndDestruct) {
    WebServer server(8081);
    EXPECT_FALSE(server.isRunning());
}

// Start/stop test
TEST(WebServerTest, StartAndStop) {
    WebServer server(8082);
    server.start();
    EXPECT_TRUE(server.isRunning());
    server.stop();
    EXPECT_FALSE(server.isRunning());
}

// Register handler (stub)
TEST(WebServerTest, RegisterHttpHandler) {
    WebServer server(8083);
    server.registerHttpHandler("/test", "GET", [](std::string_view, std::string_view, const std::string&, std::string&, int&){});
    SUCCEED();
}

// Serve static (stub)
TEST(WebServerTest, ServeStatic) {
    WebServer server(8084);
    server.serveStatic("/static/", ".");
    SUCCEED();
}

// Register WebSocket handler (stub)
TEST(WebServerTest, RegisterWebSocketHandler) {
    WebServer server(8085);
    server.registerWebSocketHandler("/ws");
    SUCCEED();
}
