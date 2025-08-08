#include <gtest/gtest.h>
#include <WebServer/WebServer.h>


class WebServerCoreTest : public ::testing::TestWithParam<WebServer::Backend> {};

INSTANTIATE_TEST_SUITE_P(
    AllBackends,
    WebServerCoreTest,
    ::testing::Values(WebServer::Backend::Mongoose, WebServer::Backend::_)
);

TEST_P(WebServerCoreTest, CanConstructAndDestruct) {
    WebServer server("127.0.0.1", 8081, GetParam());
    EXPECT_FALSE(server.isRunning());
}

TEST_P(WebServerCoreTest, StartAndStop) {
    WebServer server("127.0.0.1", 8082, GetParam());
    server.start();
    EXPECT_TRUE(server.isRunning());
    server.stop();
    EXPECT_FALSE(server.isRunning());
}

TEST_P(WebServerCoreTest, RegisterHttpHandler) {
    WebServer server("127.0.0.1", 8083, GetParam());
    server.registerHttpHandler("/test", "GET", [](std::string_view, std::string_view, const std::string&, std::string&, int&){});
    SUCCEED();
}

TEST_P(WebServerCoreTest, ServeStatic) {
    WebServer server("127.0.0.1", 8084, GetParam());
    server.serveStatic("/static/", ".");
    SUCCEED();
}

TEST_P(WebServerCoreTest, RegisterWebSocketHandler) {
    WebServer server("127.0.0.1", 8085, GetParam());
    server.registerWebSocketHandler("/ws");
    SUCCEED();
}
