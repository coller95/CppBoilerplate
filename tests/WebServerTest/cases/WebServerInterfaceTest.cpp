#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <WebServer/WebServer.h>
#include "../MockWebServer.h"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

namespace {

class WebServerInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockWebServer = std::make_unique<StrictMock<webserver::MockWebServer>>();
    }

    std::unique_ptr<StrictMock<webserver::MockWebServer>> mockWebServer;
};

} // anonymous namespace

TEST_F(WebServerInterfaceTest, ImplementsIWebServerInterface) {
    // Verify that WebServer implements IWebServer
    webserver::WebServer webServer("127.0.0.1", 8080);
    webserver::IWebServer* interface = &webServer;
    EXPECT_NE(interface, nullptr);
}

TEST_F(WebServerInterfaceTest, MockImplementsInterface) {
    // Verify that MockWebServer implements IWebServer
    webserver::IWebServer* interface = mockWebServer.get();
    EXPECT_NE(interface, nullptr);
}

TEST_F(WebServerInterfaceTest, MockStartStopBehavior) {
    EXPECT_CALL(*mockWebServer, isRunning())
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .WillOnce(Return(false));
    
    EXPECT_CALL(*mockWebServer, start())
        .WillOnce(Return(true));
    
    EXPECT_CALL(*mockWebServer, stop())
        .Times(1);
    
    // Test mock behavior
    EXPECT_FALSE(mockWebServer->isRunning());
    bool started = mockWebServer->start();
    EXPECT_TRUE(started);
    EXPECT_TRUE(mockWebServer->isRunning());
    mockWebServer->stop();
    EXPECT_FALSE(mockWebServer->isRunning());
}

TEST_F(WebServerInterfaceTest, MockHandlerRegistration) {
    EXPECT_CALL(*mockWebServer, registerHandler("GET", "/test", _))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, registerHandler("/api", _))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, get("/users", _))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, post("/users", _))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, put("/users/1", _))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, del("/users/1", _))
        .Times(1);
    
    // Test handler registration
    webserver::HttpHandler handler = [](const webserver::HttpRequest&, webserver::HttpResponse&) {};
    
    mockWebServer->registerHandler("GET", "/test", handler);
    mockWebServer->registerHandler("/api", handler);
    mockWebServer->get("/users", handler);
    mockWebServer->post("/users", handler);
    mockWebServer->put("/users/1", handler);
    mockWebServer->del("/users/1", handler);
}

TEST_F(WebServerInterfaceTest, MockStaticFileServing) {
    EXPECT_CALL(*mockWebServer, serveStatic(_))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, serveStatic("/static/", "./public"))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, serveStaticWithMime("/assets/", "./assets", _))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, serveFile("/file", "./path/file.txt", "text/plain"))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, setGlobalMimeConfig(_))
        .Times(1);
    
    webserver::StaticRoute route;
    route.urlPrefix = "/assets/";
    route.localPath = "./assets";
    route.defaultFile = "index.html";
    route.allowDirectoryListing = false;
    
    webserver::MimeConfig mimeConfig = webserver::MimeConfig::createDefault();
    
    mockWebServer->serveStatic(route);
    mockWebServer->serveStatic("/static/", "./public");
    mockWebServer->serveStaticWithMime("/assets/", "./assets", mimeConfig);
    mockWebServer->serveFile("/file", "./path/file.txt", "text/plain");
    mockWebServer->setGlobalMimeConfig(mimeConfig);
}

TEST_F(WebServerInterfaceTest, MockWebSocketSupport) {
    EXPECT_CALL(*mockWebServer, registerWebSocketHandler("/ws", _))
        .Times(1);
    
    EXPECT_CALL(*mockWebServer, sendWebSocketMessage("conn123", "Hello"))
        .WillOnce(Return(true));
    
    webserver::WebSocketHandler wsHandler = [](const std::string&, const std::string&) {};
    
    mockWebServer->registerWebSocketHandler("/ws", wsHandler);
    bool sent = mockWebServer->sendWebSocketMessage("conn123", "Hello");
    EXPECT_TRUE(sent);
}

TEST_F(WebServerInterfaceTest, MockServerInformation) {
    EXPECT_CALL(*mockWebServer, getBindAddress())
        .WillOnce(Return("0.0.0.0"));
    
    EXPECT_CALL(*mockWebServer, getPort())
        .WillOnce(Return(8080));
    
    EXPECT_CALL(*mockWebServer, getActiveConnections())
        .WillOnce(Return(5));
    
    std::string address = mockWebServer->getBindAddress();
    uint16_t port = mockWebServer->getPort();
    size_t connections = mockWebServer->getActiveConnections();
    
    EXPECT_EQ("0.0.0.0", address);
    EXPECT_EQ(static_cast<uint16_t>(8080), port);
    EXPECT_EQ(static_cast<size_t>(5), connections);
}

TEST_F(WebServerInterfaceTest, HttpRequestStructureIsComplete) {
    webserver::HttpRequest request;
    
    // Verify all fields exist and can be set
    request.method = "POST";
    request.uri = "/api/test";
    request.version = "1.1";
    request.headers["Content-Type"] = "application/json";
    request.body = R"({"test": true})";
    request.queryParams["debug"] = "true";
    request.remoteIp = "127.0.0.1";
    request.remotePort = 12345;
    
    EXPECT_EQ("POST", request.method);
    EXPECT_EQ("/api/test", request.uri);
    EXPECT_EQ("1.1", request.version);
    EXPECT_EQ("application/json", request.headers["Content-Type"]);
    EXPECT_EQ(R"({"test": true})", request.body);
    EXPECT_EQ("true", request.queryParams["debug"]);
    EXPECT_EQ("127.0.0.1", request.remoteIp);
    EXPECT_EQ(static_cast<uint16_t>(12345), request.remotePort);
}

TEST_F(WebServerInterfaceTest, HttpResponseStructureHasConvenienceMethods) {
    webserver::HttpResponse response;
    
    // Test convenience methods
    response.setHeader("X-Test", "value");
    EXPECT_EQ("value", response.headers["X-Test"]);
    
    response.setContentType("text/xml");
    EXPECT_EQ("text/xml", response.headers["Content-Type"]);
    
    response.setJsonResponse(R"({"success": true})");
    EXPECT_EQ("application/json", response.headers["Content-Type"]);
    EXPECT_EQ(R"({"success": true})", response.body);
    
    response.setHtmlResponse("<h1>Test</h1>");
    EXPECT_EQ("text/html; charset=utf-8", response.headers["Content-Type"]);
    EXPECT_EQ("<h1>Test</h1>", response.body);
    
    response.setPlainTextResponse("Hello World");
    EXPECT_EQ("text/plain; charset=utf-8", response.headers["Content-Type"]);
    EXPECT_EQ("Hello World", response.body);
}

TEST_F(WebServerInterfaceTest, StaticRouteStructureIsComplete) {
    webserver::StaticRoute route;
    
    route.urlPrefix = "/files/";
    route.localPath = "/var/www/files";
    route.defaultFile = "default.html";
    route.allowDirectoryListing = true;
    route.defaultMimeType = "text/plain";
    
    // Test MIME type overrides
    route.mimeTypeOverrides[".txt"] = "text/plain; charset=utf-8";
    route.mimeTypeOverrides[".log"] = "text/plain; charset=utf-8";
    route.mimeTypeOverrides[".data"] = "application/octet-stream";
    
    EXPECT_EQ("/files/", route.urlPrefix);
    EXPECT_EQ("/var/www/files", route.localPath);
    EXPECT_EQ("default.html", route.defaultFile);
    EXPECT_TRUE(route.allowDirectoryListing);
    EXPECT_EQ("text/plain", route.defaultMimeType);
    EXPECT_EQ("text/plain; charset=utf-8", route.mimeTypeOverrides[".txt"]);
    EXPECT_EQ("text/plain; charset=utf-8", route.mimeTypeOverrides[".log"]);
    EXPECT_EQ("application/octet-stream", route.mimeTypeOverrides[".data"]);
}
