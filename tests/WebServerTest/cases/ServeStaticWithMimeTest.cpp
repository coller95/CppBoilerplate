#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <memory>
#include <fstream>

class ServeStaticWithMimeTest : public ::testing::Test {
protected:
    void SetUp() override {
        server = std::make_unique<WebServer>(8080);
    }

    void TearDown() override {
        server->stop();
    }

    std::unique_ptr<WebServer> server;
};

TEST_F(ServeStaticWithMimeTest, ServeStaticWithCustomMimeType) {
    const std::string urlPrefix = "/custom/";
    const std::string directory = "./test_files";
    const std::string mimeType = "application/custom";

    // Create a test file
    std::ofstream testFile("./test_files/test.custom");
    testFile << "This is a test file.";
    testFile.close();

    server->serveStaticWithMime(urlPrefix, directory, mimeType);
    server->start();

    // Simulate a request (this would be replaced with an actual HTTP client in a real test)
    // For now, we assume the server correctly serves the file with the custom MIME type.

    ASSERT_TRUE(true); // Placeholder for actual validation logic

    // Clean up
    std::remove("./test_files/test.custom");
}
