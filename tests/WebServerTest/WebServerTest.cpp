// All includes must be at the top for proper test macro expansion
#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <IoCContainer/IoCContainer.h>
#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
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


// TDD: Serve static file integration test
TEST(WebServerTest, ServeStaticFile) {
    // Create a temporary file with known content
    const std::string tempFilePath = "test_static_file.bin";
    // Use binary content without leading null bytes to avoid string truncation
    const std::string fileContent = "This is a static file test.\x00\x01\x02\x03\x04\x05";

    // Print working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        std::cout << "[ServeStaticFile] CWD: " << cwd << std::endl;
    }

    // Only create the file in the project root (where the server will look)
    std::string rootFile = std::string("../") + tempFilePath;
    std::cout << "[ServeStaticFile] Creating file at: " << rootFile << std::endl;
    std::cout << "[ServeStaticFile] File content size: " << fileContent.size() << std::endl;
    std::cout << "[ServeStaticFile] File content hex: ";
    for (size_t i = 0; i < fileContent.size(); ++i) {
        printf("%02X ", static_cast<unsigned char>(fileContent[i]));
    }
    std::cout << std::endl;
    
    std::ofstream ofs(rootFile, std::ios::binary);
    ASSERT_TRUE(ofs.is_open()) << "Failed to open file for writing: " << rootFile;
    ofs.write(fileContent.data(), fileContent.size());
    ofs.flush();
    ASSERT_TRUE(ofs.good()) << "File stream error after writing";
    ofs.close();
    
    // Verify file was written correctly
    std::ifstream ifs(rootFile, std::ios::binary);
    ASSERT_TRUE(ifs.good()) << "File does not exist at " << rootFile;
    ifs.seekg(0, std::ios::end);
    std::streamsize fileSize = ifs.tellg();
    std::cout << "[ServeStaticFile] File size at " << rootFile << ": " << fileSize << std::endl;
    ASSERT_EQ(static_cast<size_t>(fileSize), fileContent.size());
    
    // Also verify content
    ifs.seekg(0, std::ios::beg);
    std::string readContent(fileSize, '\0');
    ifs.read(&readContent[0], fileSize);
    ifs.close();
    ASSERT_EQ(readContent, fileContent);

    WebServer server(9092);
    // Serve files from project root at /static/
    server.serveStatic("/static/", "..");
    server.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(sock, -1);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9092);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    ASSERT_EQ(connect(sock, (sockaddr*)&addr, sizeof(addr)), 0);

    std::string req =
        "GET /static/" + tempFilePath + " HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(sock, req.c_str(), req.size(), 0);

    char buf[2048] = {};
    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    ASSERT_GT(n, 0);
    std::string response(buf, n);



    std::cout << "[ServeStaticFile] HTTP response (size=" << response.size() << "):\n" << response << std::endl;
    // Print hex dump of response body (after headers)
    auto bodyPos = response.find("\r\n\r\n");
    if (bodyPos != std::string::npos) {
        size_t bodyStart = bodyPos + 4;
        std::cout << "[ServeStaticFile] Response body hex:";
        for (size_t i = bodyStart; i < response.size(); ++i) {
            if ((i - bodyStart) % 16 == 0) std::cout << "\n  ";
            printf("%02X ", static_cast<unsigned char>(response[i]));
        }
        std::cout << std::endl;
    }
    EXPECT_NE(response.find("200"), std::string::npos);
    // For binary, check for a unique substring in the response
    EXPECT_NE(response.find("This is a static file test."), std::string::npos);

    close(sock);
    server.stop();
    // Remove the file from the project root after the test
    std::remove((std::string("../") + tempFilePath).c_str());
}
