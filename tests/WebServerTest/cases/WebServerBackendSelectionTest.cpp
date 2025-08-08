// WebServerBackendSelectionTest.cpp
// Test that WebServer can be constructed with Mongoose backend (TDD: Failing test first)

#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <memory>
#include <string>

using namespace ::testing;

namespace webserver_test {

class WebServerBackendSelectionTest : public ::testing::TestWithParam<WebServer::Backend> {};

INSTANTIATE_TEST_SUITE_P(
    AllBackends,
    WebServerBackendSelectionTest,
    ::testing::Values(WebServer::Backend::Mongoose, WebServer::Backend::_)
);

TEST_P(WebServerBackendSelectionTest, CanSelectBackend)
{
    WebServer::Backend backend = GetParam();
    std::string ip = "127.0.0.1";
    int port = 12346;
    std::unique_ptr<WebServer> server = std::make_unique<WebServer>(ip, port, backend);
    std::string actualIp = server->getBindIp();
    int actualPort = server->getBindPort();
    EXPECT_EQ(actualIp, ip);
    EXPECT_EQ(actualPort, port);
    // Optionally: test that the backend is actually the selected backend (API TBD)
}

} // namespace webserver_test
