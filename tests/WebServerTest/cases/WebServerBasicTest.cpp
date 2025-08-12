#include <gtest/gtest.h>
#include <WebServer/WebServer.h>
#include <thread>
#include <chrono>

namespace {

class WebServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use a different port for each test to avoid conflicts
        static uint16_t testPort = 8080;
        server = std::make_unique<webserver::WebServer>("127.0.0.1", testPort++);
    }

    void TearDown() override {
        if (server && server->isRunning()) {
            server->stop();
        }
    }
    
    std::unique_ptr<webserver::WebServer> server;
};

} // anonymous namespace

TEST_F(WebServerTest, ConstructorCreatesValidInstance) {
    webserver::WebServer webServer("127.0.0.1", 8090);
    EXPECT_FALSE(webServer.isRunning());
    EXPECT_EQ("127.0.0.1", webServer.getBindAddress());
    EXPECT_EQ(static_cast<uint16_t>(8090), webServer.getPort());
    EXPECT_EQ(static_cast<size_t>(0), webServer.getActiveConnections());
}

TEST_F(WebServerTest, ConstructorWithBackendSelection) {
    webserver::WebServer mongooseServer("127.0.0.1", 8091, webserver::WebServer::Backend::Mongoose);
    EXPECT_FALSE(mongooseServer.isRunning());
    EXPECT_EQ("127.0.0.1", mongooseServer.getBindAddress());
    EXPECT_EQ(static_cast<uint16_t>(8091), mongooseServer.getPort());
    
    webserver::WebServer defaultServer("127.0.0.1", 8092, webserver::WebServer::Backend::_);
    EXPECT_FALSE(defaultServer.isRunning());
    EXPECT_EQ("127.0.0.1", defaultServer.getBindAddress());
    EXPECT_EQ(static_cast<uint16_t>(8092), defaultServer.getPort());
}

TEST_F(WebServerTest, StartAndStopServer) {
    EXPECT_FALSE(server->isRunning());
    
    bool started = server->start();
    EXPECT_TRUE(started);
    EXPECT_TRUE(server->isRunning());
    
    server->stop();
    EXPECT_FALSE(server->isRunning());
}

TEST_F(WebServerTest, CannotStartAlreadyRunningServer) {
    EXPECT_TRUE(server->start());
    EXPECT_TRUE(server->isRunning());
    
    // Trying to start again should fail
    bool secondStart = server->start();
    EXPECT_FALSE(secondStart);
    EXPECT_TRUE(server->isRunning());
}

TEST_F(WebServerTest, RegisterHttpHandlerWithMethodAndPath) {
    bool handlerCalled = false;
    webserver::HttpHandler handler = [&handlerCalled](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        handlerCalled = true;
        EXPECT_EQ("GET", req.method);
        EXPECT_EQ("/test", req.uri);
        resp.setPlainTextResponse("Hello, World!");
    };
    
    // Should not throw when registering handler
    EXPECT_NO_THROW(server->registerHandler("GET", "/test", std::move(handler)));
}

TEST_F(WebServerTest, RegisterHttpHandlerForAllMethods) {
    bool handlerCalled = false;
    webserver::HttpHandler handler = [&handlerCalled](const webserver::HttpRequest& /*req*/, webserver::HttpResponse& resp) {
        handlerCalled = true;
        resp.setJsonResponse(R"({"message": "received"})");
    };
    
    // Should not throw when registering handler for all methods
    EXPECT_NO_THROW(server->registerHandler("/api/endpoint", std::move(handler)));
}

TEST_F(WebServerTest, ConvenienceMethodRegistration) {
    bool getCalled = false, postCalled = false, putCalled = false, deleteCalled = false;
    
    server->get("/users", [&getCalled](const webserver::HttpRequest&, webserver::HttpResponse& resp) {
        getCalled = true;
        resp.setJsonResponse(R"([{"id": 1, "name": "John"}])");
    });
    
    server->post("/users", [&postCalled](const webserver::HttpRequest&, webserver::HttpResponse& resp) {
        postCalled = true;
        resp.statusCode = 201;
        resp.setJsonResponse(R"({"id": 2, "name": "Jane"})");
    });
    
    server->put("/users/1", [&putCalled](const webserver::HttpRequest&, webserver::HttpResponse& resp) {
        putCalled = true;
        resp.setJsonResponse(R"({"id": 1, "name": "John Updated"})");
    });
    
    server->del("/users/1", [&deleteCalled](const webserver::HttpRequest&, webserver::HttpResponse& resp) {
        deleteCalled = true;
        resp.statusCode = 204;
        resp.body = "";
    });
    
    // All registrations should succeed without throwing
    SUCCEED();
}

TEST_F(WebServerTest, StaticFileServing) {
    webserver::StaticRoute route;
    route.urlPrefix = "/static/";
    route.localPath = "./test_files";
    route.defaultFile = "index.html";
    route.allowDirectoryListing = true;
    
    EXPECT_NO_THROW(server->serveStatic(route));
    
    // Test convenience method
    EXPECT_NO_THROW(server->serveStatic("/assets/", "./assets"));
}

TEST_F(WebServerTest, WebSocketHandlerRegistration) {
    bool wsHandlerCalled = false;
    webserver::WebSocketHandler wsHandler = [&wsHandlerCalled](const std::string& connectionId, const std::string& message) {
        wsHandlerCalled = true;
        EXPECT_FALSE(connectionId.empty());
        EXPECT_FALSE(message.empty());
    };
    
    EXPECT_NO_THROW(server->registerWebSocketHandler("/ws", std::move(wsHandler)));
}

TEST_F(WebServerTest, WebSocketMessageSending) {
    // Should return false for non-existent connection
    bool sent = server->sendWebSocketMessage("nonexistent", "test message");
    EXPECT_FALSE(sent);
}

TEST_F(WebServerTest, HttpRequestStructure) {
    webserver::HttpRequest request;
    request.method = "POST";
    request.uri = "/api/data";
    request.version = "1.1";
    request.headers["Content-Type"] = "application/json";
    request.headers["Authorization"] = "Bearer token123";
    request.body = R"({"key": "value"})";
    request.queryParams["param1"] = "value1";
    request.queryParams["param2"] = "value2";
    request.remoteIp = "192.168.1.100";
    request.remotePort = 54321;
    
    EXPECT_EQ("POST", request.method);
    EXPECT_EQ("/api/data", request.uri);
    EXPECT_EQ("1.1", request.version);
    EXPECT_EQ("application/json", request.headers["Content-Type"]);
    EXPECT_EQ("Bearer token123", request.headers["Authorization"]);
    EXPECT_EQ(R"({"key": "value"})", request.body);
    EXPECT_EQ("value1", request.queryParams["param1"]);
    EXPECT_EQ("value2", request.queryParams["param2"]);
    EXPECT_EQ("192.168.1.100", request.remoteIp);
    EXPECT_EQ(54321, request.remotePort);
}

TEST_F(WebServerTest, HttpResponseStructure) {
    webserver::HttpResponse response;
    
    // Test default values
    EXPECT_EQ(200, response.statusCode);
    EXPECT_TRUE(response.headers.empty());
    EXPECT_TRUE(response.body.empty());
    
    // Test setHeader
    response.setHeader("X-Custom-Header", "custom-value");
    EXPECT_EQ("custom-value", response.headers["X-Custom-Header"]);
    
    // Test setContentType
    response.setContentType("application/xml");
    EXPECT_EQ("application/xml", response.headers["Content-Type"]);
    
    // Test setJsonResponse
    response.setJsonResponse(R"({"status": "ok"})");
    EXPECT_EQ("application/json", response.headers["Content-Type"]);
    EXPECT_EQ(R"({"status": "ok"})", response.body);
    
    // Test setHtmlResponse
    response.setHtmlResponse("<html><body>Hello</body></html>");
    EXPECT_EQ("text/html; charset=utf-8", response.headers["Content-Type"]);
    EXPECT_EQ("<html><body>Hello</body></html>", response.body);
    
    // Test setPlainTextResponse
    response.setPlainTextResponse("Plain text content");
    EXPECT_EQ("text/plain; charset=utf-8", response.headers["Content-Type"]);
    EXPECT_EQ("Plain text content", response.body);
}

TEST_F(WebServerTest, MoveConstructorWorks) {
    webserver::WebServer original("127.0.0.1", 8095);
    std::string originalAddress = original.getBindAddress();
    uint16_t originalPort = original.getPort();
    
    webserver::WebServer moved(std::move(original));
    
    EXPECT_EQ(originalAddress, moved.getBindAddress());
    EXPECT_EQ(originalPort, moved.getPort());
    EXPECT_FALSE(moved.isRunning());
}
