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

class UploadFileTest : public ::testing::TestWithParam<WebServer::Backend> {};

INSTANTIATE_TEST_SUITE_P(
    AllBackends,
    UploadFileTest,
    ::testing::Values(WebServer::Backend::Mongoose, WebServer::Backend::_)
);

TEST_P(UploadFileTest, UploadFile) {
    const std::string uploadFileName = "upload_test_file.bin"; // Changed to .bin
    const std::string uploadFilePath = std::string("../") + uploadFileName;
    std::remove(uploadFilePath.c_str());
    const std::string fileContent = "Hello, this is an upload test!\n\x00\x01\x02"; // Binary content remains the same
    WebServer server("127.0.0.1", 9093, GetParam());
    server.registerHttpHandler("/api/upload", "POST",
        [uploadFilePath](std::string_view, std::string_view, const std::string& body, std::string& responseBody, int& statusCode) {
            const std::string boundary = "----boundary42";
            auto start = body.find("\r\n\r\n");
            if (start == std::string::npos) {
                statusCode = 400;
                responseBody = "Malformed multipart";
                return;
            }
            start += 4;
            auto end = body.rfind("--" + boundary);
            if (end == std::string::npos || end <= start) {
                end = body.size();
            }
            std::ofstream ofs(uploadFilePath, std::ios::binary); // Ensure binary mode
            ofs.write(body.data() + start, end - start);
            ofs.close();
            statusCode = 200;
            responseBody = "OK";
        });
    server.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    const std::string boundary = "----boundary42";
    std::string multipartBody;
    multipartBody += "--" + boundary + "\r\n";
    multipartBody += "Content-Disposition: form-data; name=\"file\"; filename=\"" + uploadFileName + "\"\r\n";
    multipartBody += "Content-Type: application/octet-stream\r\n\r\n";
    multipartBody += fileContent;
    multipartBody += "\r\n--" + boundary + "--\r\n";
    std::string req =
        "POST /api/upload HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
        "Content-Length: " + std::to_string(multipartBody.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + multipartBody;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(sock, -1);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9093);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    ASSERT_EQ(connect(sock, (sockaddr*)&addr, sizeof(addr)), 0);
    send(sock, req.c_str(), req.size(), 0);
    char buf[2048] = {};
    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    ASSERT_GT(n, 0);
    std::string response(buf, n);
    std::cout << "[UploadFile] HTTP response (size=" << response.size() << "):\n" << response << std::endl;
    EXPECT_NE(response.find("200"), std::string::npos);
    EXPECT_NE(response.find("OK"), std::string::npos);
    close(sock);
    server.stop();
    std::ifstream ifs(uploadFilePath, std::ios::binary); // Ensure binary mode
    ASSERT_TRUE(ifs.good()) << "File was not uploaded: " << uploadFilePath;
    std::string uploadedContent((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    // Ensure the file is deleted even if the test fails
    struct FileGuard {
        std::string filePath;
        ~FileGuard() { std::remove(filePath.c_str()); }
    } fileGuard{std::string("../") + uploadFileName};
    // Trim trailing CRLF (\r\n) from the uploaded content
    if (!uploadedContent.empty() && uploadedContent.back() == '\n') {
        uploadedContent.pop_back();
    }
    if (!uploadedContent.empty() && uploadedContent.back() == '\r') {
        uploadedContent.pop_back();
    }
    ASSERT_EQ(uploadedContent, fileContent);
    // File will be deleted automatically by FileGuard
}
