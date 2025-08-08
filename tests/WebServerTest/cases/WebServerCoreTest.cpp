#include <gtest/gtest.h>
#include <WebServer/WebServer.h>

TEST(WebServerTest, CanConstructAndDestruct) {
    WebServer server("127.0.0.1", 8081);
    EXPECT_FALSE(server.isRunning());
}

TEST(WebServerTest, StartAndStop) {
    WebServer server("127.0.0.1", 8082);
    server.start();
    EXPECT_TRUE(server.isRunning());
    server.stop();
    EXPECT_FALSE(server.isRunning());
}

TEST(WebServerTest, RegisterHttpHandler) {
    WebServer server("127.0.0.1", 8083);
    server.registerHttpHandler("/test", "GET", [](std::string_view, std::string_view, const std::string&, std::string&, int&){});
    SUCCEED();
}

TEST(WebServerTest, ServeStatic) {
    WebServer server("127.0.0.1", 8084);
    server.serveStatic("/static/", ".");
    SUCCEED();
}

TEST(WebServerTest, RegisterWebSocketHandler) {
    WebServer server("127.0.0.1", 8085);
    server.registerWebSocketHandler("/ws");
    SUCCEED();
}
