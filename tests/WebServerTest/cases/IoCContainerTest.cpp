#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <IoCContainer/IoCContainer.h>

TEST(WebServerTest, IoCContainerCanRegisterAndResolveWebServer) {
    IoCContainer container;
    container.registerType<WebServer>([]() { return std::make_shared<WebServer>("127.0.0.1", 9199); });
    auto webServer = container.resolve<WebServer>();
    ASSERT_NE(webServer, nullptr);
    EXPECT_FALSE(webServer->isRunning());
    webServer->start();
    EXPECT_TRUE(webServer->isRunning());
    webServer->stop();
    EXPECT_FALSE(webServer->isRunning());
}
