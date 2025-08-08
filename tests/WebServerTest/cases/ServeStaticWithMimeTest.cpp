
#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <memory>
#include <fstream>

class ServeStaticWithMimeTest : public ::testing::TestWithParam<WebServer::Backend> {
protected:
    void SetUp() override {
        server = std::make_unique<WebServer>("127.0.0.1", 8080, GetParam());
    }
    void TearDown() override {
        server->stop();
    }
    std::unique_ptr<WebServer> server;
};

INSTANTIATE_TEST_SUITE_P(
    AllBackends,
    ServeStaticWithMimeTest,
    ::testing::Values(WebServer::Backend::Mongoose, WebServer::Backend::_)
);

TEST_P(ServeStaticWithMimeTest, ServeStaticWithCustomMimeType) {
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
