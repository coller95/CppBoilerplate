// All includes must be at the top for proper test macro expansion
#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <IoCContainer/IoCContainer.h>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// TDD: REST API echo endpoint integration test
TEST(WebServerTest, EchoRestApi) {
    WebServer server(9091);
    server.registerHttpHandler("/api/echo", "POST",
        [](std::string_view, std::string_view, const std::string& body, std::string& responseBody, int& statusCode) {
            statusCode = 200;
            responseBody = body;
        });
    server.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(sock, -1);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9091);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    ASSERT_EQ(connect(sock, (sockaddr*)&addr, sizeof(addr)), 0);

    std::string body = "hello world";
    std::string req =
        "POST /api/echo HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n" + body;

    send(sock, req.c_str(), req.size(), 0);

    char buf[1024] = {};
    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    ASSERT_GT(n, 0);
    std::string response(buf, n);

    EXPECT_NE(response.find("200"), std::string::npos);
    EXPECT_NE(response.find("hello world"), std::string::npos);

    close(sock);
    server.stop();
}


#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <IoCContainer/IoCContainer.h>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Test IoCContainer integration with WebServer (mirrors main.cpp pattern)
TEST(WebServerTest, IoCContainerCanRegisterAndResolveWebServer) {
    IoCContainer container;
    // Use a unique port to avoid conflicts with other tests
    container.registerType<WebServer>([]() { return std::make_shared<WebServer>(9199); });
    auto webServer = container.resolve<WebServer>();
    ASSERT_NE(webServer, nullptr);
    EXPECT_FALSE(webServer->isRunning());
    webServer->start();
    EXPECT_TRUE(webServer->isRunning());
    webServer->stop();
    EXPECT_FALSE(webServer->isRunning());
}

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
