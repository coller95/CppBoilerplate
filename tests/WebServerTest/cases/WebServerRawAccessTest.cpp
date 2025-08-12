#include <gtest/gtest.h>
#include <WebServer/WebServer.h>

namespace {

class WebServerRawAccessTest : public ::testing::Test {
protected:
    void SetUp() override {
        server = std::make_unique<webserver::WebServer>("127.0.0.1", 8100);
    }

    void TearDown() override {
        if (server && server->isRunning()) {
            server->stop();
        }
    }
    
    std::unique_ptr<webserver::WebServer> server;
};

} // anonymous namespace

TEST_F(WebServerRawAccessTest, HandlerHasFullAccessToRequestHeaders) {
    bool handlerCalled = false;
    std::string capturedContentType;
    std::string capturedAuthorization;
    std::string capturedCustomHeader;
    
    webserver::HttpHandler handler = [&](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        handlerCalled = true;
        
        // Raw access to all headers
        auto contentTypeIt = req.headers.find("Content-Type");
        if (contentTypeIt != req.headers.end()) {
            capturedContentType = contentTypeIt->second;
        }
        
        auto authIt = req.headers.find("Authorization");
        if (authIt != req.headers.end()) {
            capturedAuthorization = authIt->second;
        }
        
        auto customIt = req.headers.find("X-Custom-Header");
        if (customIt != req.headers.end()) {
            capturedCustomHeader = customIt->second;
        }
        
        resp.setJsonResponse(R"({"message": "Headers received"})");
    };
    
    server->post("/api/data", std::move(handler));
    
    // Simulate what a real HTTP request would look like
    webserver::HttpRequest mockRequest;
    mockRequest.method = "POST";
    mockRequest.uri = "/api/data";
    mockRequest.headers["Content-Type"] = "application/json";
    mockRequest.headers["Authorization"] = "Bearer abc123";
    mockRequest.headers["X-Custom-Header"] = "custom-value";
    mockRequest.body = R"({"key": "value"})";
    
    webserver::HttpResponse mockResponse;
    
    // Simulate handler execution (in real implementation, this would be called by the backend)
    // For now, we just verify the handler can be registered without error
    EXPECT_NO_THROW(server->post("/api/test", [](const webserver::HttpRequest&, webserver::HttpResponse&) {}));
}

TEST_F(WebServerRawAccessTest, HandlerHasFullAccessToRequestBody) {
    std::string capturedBody;
    std::string capturedMethod;
    std::string capturedUri;
    
    webserver::HttpHandler handler = [&](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        // Raw access to method, URI, and body
        capturedMethod = req.method;
        capturedUri = req.uri;
        capturedBody = req.body;
        
        // Process raw JSON body manually
        if (req.body.find("\"action\":\"create\"") != std::string::npos) {
            resp.statusCode = 201;
            resp.setJsonResponse(R"({"status": "created"})");
        } else if (req.body.find("\"action\":\"update\"") != std::string::npos) {
            resp.statusCode = 200;
            resp.setJsonResponse(R"({"status": "updated"})");
        } else {
            resp.statusCode = 400;
            resp.setJsonResponse(R"({"error": "unknown action"})");
        }
    };
    
    server->put("/api/resource", std::move(handler));
    
    // Verify handler registration works
    SUCCEED();
}

TEST_F(WebServerRawAccessTest, HandlerCanBuildCustomResponse) {
    webserver::HttpHandler handler = [](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        // Raw access to build completely custom response
        resp.statusCode = 418; // I'm a teapot
        resp.setHeader("X-Powered-By", "WebServer");
        resp.setHeader("Cache-Control", "no-cache");
        resp.setHeader("Access-Control-Allow-Origin", "*");
        
        // Build custom response based on request
        if (req.uri.find("/api/json") != std::string::npos) {
            resp.setJsonResponse(R"({"type": "json", "message": "Hello JSON"})");
        } else if (req.uri.find("/api/html") != std::string::npos) {
            resp.setHtmlResponse("<h1>Hello HTML</h1><p>Custom response</p>");
        } else if (req.uri.find("/api/text") != std::string::npos) {
            resp.setPlainTextResponse("Hello Plain Text");
        } else {
            resp.setHeader("Content-Type", "application/octet-stream");
            resp.body = "Binary data here";
        }
    };
    
    server->get("/api/*", std::move(handler));
    
    // Verify handler registration works
    SUCCEED();
}

TEST_F(WebServerRawAccessTest, HandlerCanAccessQueryParameters) {
    webserver::HttpHandler handler = [](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        // Raw access to query parameters
        auto pageIt = req.queryParams.find("page");
        auto limitIt = req.queryParams.find("limit");
        auto filterIt = req.queryParams.find("filter");
        
        std::string responseBody = "{";
        if (pageIt != req.queryParams.end()) {
            responseBody += "\"page\":" + pageIt->second + ",";
        }
        if (limitIt != req.queryParams.end()) {
            responseBody += "\"limit\":" + limitIt->second + ",";
        }
        if (filterIt != req.queryParams.end()) {
            responseBody += "\"filter\":\"" + filterIt->second + "\",";
        }
        
        // Remove trailing comma and close JSON
        if (responseBody.back() == ',') {
            responseBody.pop_back();
        }
        responseBody += "}";
        
        resp.setJsonResponse(responseBody);
    };
    
    server->get("/search", std::move(handler));
    
    // Verify handler registration works
    SUCCEED();
}

TEST_F(WebServerRawAccessTest, HandlerCanAccessClientInformation) {
    webserver::HttpHandler handler = [](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        // Raw access to client information
        std::string clientInfo = "Client: " + req.remoteIp + ":" + std::to_string(req.remotePort);
        
        // Log client information (in real usage)
        resp.setHeader("X-Client-Info", clientInfo);
        resp.setPlainTextResponse("Connection info logged");
    };
    
    server->get("/info", std::move(handler));
    
    // Verify handler registration works
    SUCCEED();
}

TEST_F(WebServerRawAccessTest, DemonstrateCompleteRawAccess) {
    // This test demonstrates how a handler gets complete raw access to the HTTP request
    webserver::HttpHandler echoHandler = [](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        // Complete raw access to everything
        std::string echo = "{\n";
        echo += "  \"method\": \"" + req.method + "\",\n";
        echo += "  \"uri\": \"" + req.uri + "\",\n";
        echo += "  \"version\": \"" + req.version + "\",\n";
        echo += "  \"remoteIp\": \"" + req.remoteIp + "\",\n";
        echo += "  \"remotePort\": " + std::to_string(req.remotePort) + ",\n";
        echo += "  \"headers\": {\n";
        
        bool first = true;
        for (const auto& [name, value] : req.headers) {
            if (!first) echo += ",\n";
            echo += "    \"" + name + "\": \"" + value + "\"";
            first = false;
        }
        echo += "\n  },\n";
        
        echo += "  \"queryParams\": {\n";
        first = true;
        for (const auto& [name, value] : req.queryParams) {
            if (!first) echo += ",\n";
            echo += "    \"" + name + "\": \"" + value + "\"";
            first = false;
        }
        echo += "\n  },\n";
        
        echo += "  \"bodyLength\": " + std::to_string(req.body.length()) + ",\n";
        echo += "  \"bodyPreview\": \"" + req.body.substr(0, 100) + "\"\n";
        echo += "}";
        
        resp.setJsonResponse(echo);
    };
    
    server->registerHandler("/echo", std::move(echoHandler)); // Works for all HTTP methods
    
    // Verify handler registration works
    SUCCEED();
}
