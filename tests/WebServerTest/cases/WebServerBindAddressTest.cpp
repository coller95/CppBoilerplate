// WebServerBindAddressTest.cpp
// Test that WebServer binds to the correct IP address when constructed
// (TDD: Failing test first)

#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <memory>
#include <string>

using namespace ::testing;

namespace webserver_test {

class WebServerBindAddressTest : public ::testing::TestWithParam<WebServer::Backend> {};

INSTANTIATE_TEST_SUITE_P(
    AllBackends,
    WebServerBindAddressTest,
    ::testing::Values(WebServer::Backend::Mongoose, WebServer::Backend::_)
);

TEST_P(WebServerBindAddressTest, BindsToSpecifiedIpAddress)
{
    std::string ip = "0.0.0.0";
    int port = 12345;
    std::unique_ptr<WebServer> server = std::make_unique<WebServer>(ip, port, GetParam());
    std::string actualIp = server->getBindIp();
    int actualPort = server->getBindPort();
    EXPECT_EQ(actualIp, ip);
    EXPECT_EQ(actualPort, port);
}

} // namespace webserver_test
