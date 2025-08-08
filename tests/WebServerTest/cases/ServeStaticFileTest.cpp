// All includes must be at the top for proper test macro expansion
#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

TEST(WebServerTest, ServeStaticFile) {
    // Create a temporary file with known content
    const std::string tempFilePath = "test_static_file.bin";
    const std::string fileContent = "This is a static file test.\x00\x01\x02\x03\x04\x05";
    std::string rootFile = std::string("../") + tempFilePath;
    std::ofstream ofs(rootFile, std::ios::binary);
    ASSERT_TRUE(ofs.is_open()) << "Failed to open file for writing: " << rootFile;
    ofs.write(fileContent.data(), fileContent.size());
    ofs.flush();
    ASSERT_TRUE(ofs.good()) << "File stream error after writing";
    ofs.close();

    std::ifstream ifs(rootFile, std::ios::binary);
    ASSERT_TRUE(ifs.good()) << "File does not exist at " << rootFile;
    ifs.seekg(0, std::ios::end);
    std::streamsize fileSize = ifs.tellg();
    ASSERT_EQ(static_cast<size_t>(fileSize), fileContent.size());
    ifs.seekg(0, std::ios::beg);
    std::string readContent(fileSize, '\0');
    ifs.read(&readContent[0], fileSize);
    ifs.close();
    ASSERT_EQ(readContent, fileContent);

    WebServer server("127.0.0.1", 9092);
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
    EXPECT_NE(response.find("200"), std::string::npos);
    EXPECT_NE(response.find("This is a static file test."), std::string::npos);

    close(sock);
    server.stop();
    std::remove((std::string("../") + tempFilePath).c_str());
}
