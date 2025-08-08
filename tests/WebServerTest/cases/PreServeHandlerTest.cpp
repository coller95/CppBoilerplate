#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <WebServer/impl/Factory.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Update the namespace usage
using namespace WebServerLib;


class PreServeHandlerTest : public ::testing::TestWithParam<WebServer::Backend> {
protected:
    void SetUp() override {
        webServer = std::make_unique<WebServer>("127.0.0.1", 8080, GetParam());
    }
    void TearDown() override {
        webServer->stop();
    }
    std::unique_ptr<WebServer> webServer;
};

INSTANTIATE_TEST_SUITE_P(
    AllBackends,
    PreServeHandlerTest,
    ::testing::Values(WebServer::Backend::Mongoose, WebServer::Backend::_)
);

TEST_P(PreServeHandlerTest, PreServeHandlerIsCalled) {
    bool handlerCalled = false;
    std::string capturedFilePath;

    webServer->registerPreServeHandler([&](const std::string& filePath) {
        handlerCalled = true;
        capturedFilePath = filePath;
    });

    // Prepare test file
    const std::string dir = "./test_files";
    const std::string fname = "example.txt";
    const std::string testFilePath = dir + "/" + fname;
    std::filesystem::create_directories(dir);
    {
        std::ofstream testFile(testFilePath);
        testFile << "Test content";
    }

    // Map static and start server on a unique port
    const int port = 9094;
    webServer = std::make_unique<WebServer>("127.0.0.1", port, GetParam());
    webServer->registerPreServeHandler([&](const std::string& filePath) {
        handlerCalled = true;
        capturedFilePath = filePath;
    });
    webServer->serveStatic("/static/", dir);

    std::thread serverThread([&]() { webServer->start(); });

    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    // Make HTTP GET request to the served file
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(sock, -1);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    ASSERT_EQ(connect(sock, (sockaddr*)&addr, sizeof(addr)), 0);

    std::string req =
        std::string("GET /static/") + fname + " HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Connection: close\r\n"
        "\r\n";
    send(sock, req.c_str(), req.size(), 0);

    char buf[1024] = {};
    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    ASSERT_GT(n, 0);
    std::string response(buf, n);
    EXPECT_NE(response.find("200"), std::string::npos);

    close(sock);

    // Allow handler to run and capture path
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Verify the handler was called with resolved file path
    EXPECT_TRUE(handlerCalled);
    EXPECT_EQ(capturedFilePath, testFilePath);

    webServer->stop();
    serverThread.join();

    std::remove(testFilePath.c_str());
}
