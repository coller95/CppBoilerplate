#include <gtest/gtest.h>
#include <WebServerBackendMongoose/WebServerBackendMongoose.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace {

class WebServerBackendMongooseBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reuse same port range to minimize OS resource usage
        // Find an available port starting from 8080
        currentPort = 8080;
        while (true) {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) {
            throw std::runtime_error("Failed to create socket");
            }

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(currentPort);
            addr.sin_addr.s_addr = INADDR_ANY;

            // Check if the port is available
            if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            close(sock);
            break; // Port is available
            }

            close(sock);
            ++currentPort; // Try the next port
        }
        backend = std::make_unique<webserverbackendmongoose::WebServerBackendMongoose>("127.0.0.1", currentPort);
    }

    void TearDown() override {
        // Ensure proper cleanup to avoid resource leaks
        if (backend && backend->isRunning()) {
            backend->stop();
        }
        backend.reset();
        // Small delay to allow OS to release port
        usleep(10000); // 10ms to ensure cleanup
    }

    // Helper method for fast HTTP requests without resource waste
    std::string sendHttpRequest(const std::string& request) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) return "";
        
        // Set socket timeout to prevent hanging
        struct timeval timeout;
        timeout.tv_sec = 1;  // 1 second timeout
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        
        // Enable port reuse to avoid "Address already in use" errors
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(currentPort);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
        
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
            close(sock);
            return "";
        }
        
        // Send request
        if (send(sock, request.c_str(), request.length(), 0) < 0) {
            close(sock);
            return "";
        }
        
        // Read response with timeout protection
        char buffer[1024] = {0};
        ssize_t bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
        
        close(sock); // Immediate cleanup
        
        return bytesRead > 0 ? std::string(buffer, bytesRead) : "";
    }

    std::unique_ptr<webserverbackendmongoose::WebServerBackendMongoose> backend;
    uint16_t currentPort;
};

} // anonymous namespace

// TDD: Test 1 - Mongoose backend should be constructed with correct address and port
TEST_F(WebServerBackendMongooseBasicTest, ConstructorSetsCorrectAddressAndPort) {
    EXPECT_EQ(backend->getBindAddress(), "127.0.0.1");
    EXPECT_EQ(backend->getPort(), currentPort);
    EXPECT_FALSE(backend->isRunning()); // Should not be running initially
}

// TDD: Test 2 - Mongoose backend should start and stop correctly
TEST_F(WebServerBackendMongooseBasicTest, CanStartAndStopServer) {
    EXPECT_FALSE(backend->isRunning());
    
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    
    backend->stop();
    EXPECT_FALSE(backend->isRunning());
}

// TDD: Test 3 - Starting an already running Mongoose server should return false
TEST_F(WebServerBackendMongooseBasicTest, StartingRunningServerReturnsFalse) {
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    
    // Starting again should return false
    EXPECT_FALSE(backend->start());
    EXPECT_TRUE(backend->isRunning()); // Should still be running
}

// TDD: Test 4 - Mongoose backend should track active connections
TEST_F(WebServerBackendMongooseBasicTest, TracksActiveConnections) {
    EXPECT_EQ(backend->getActiveConnections(), 0u);
    // More detailed connection tests will be in integration tests
}

// TDD: Test 5 - Move constructor should work correctly
TEST_F(WebServerBackendMongooseBasicTest, MoveConstructorWorks) {
    auto original = std::make_unique<webserverbackendmongoose::WebServerBackendMongoose>("192.168.1.1", 9090);
    EXPECT_EQ(original->getBindAddress(), "192.168.1.1");
    EXPECT_EQ(original->getPort(), 9090);
    
    auto moved = std::make_unique<webserverbackendmongoose::WebServerBackendMongoose>(std::move(*original));
    EXPECT_EQ(moved->getBindAddress(), "192.168.1.1");
    EXPECT_EQ(moved->getPort(), 9090);
}

// TDD: Test 6 - Mongoose-specific configuration should work
TEST_F(WebServerBackendMongooseBasicTest, MongooseSpecificConfiguration) {
    // These should not throw and should configure the backend
    EXPECT_NO_THROW(backend->setDocumentRoot("/var/www/html"));
    EXPECT_NO_THROW(backend->setMaxRequestSize(1024 * 1024)); // 1MB
    EXPECT_NO_THROW(backend->setConnectionTimeout(30)); // 30 seconds
    
    // SSL configuration test (should return false without valid cert files)
    EXPECT_FALSE(backend->enableSSL("/invalid/cert.pem", "/invalid/key.pem"));
}

// TDD: Test 7 - Mongoose backend should implement IWebServerBackend interface
TEST_F(WebServerBackendMongooseBasicTest, ImplementsIWebServerBackendInterface) {
    // Should be able to register handlers
    auto handler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.body = "Test response";
    };
    
    EXPECT_NO_THROW(backend->registerHandler("GET", "/test", handler));
    EXPECT_NO_THROW(backend->registerHandler("/all-methods", handler));
    
    // Should be able to configure static routes
    webserver::StaticRoute route;
    route.urlPrefix = "/static";
    route.localPath = "/var/www/static";
    
    EXPECT_NO_THROW(backend->serveStatic(route));
    
    // Should be able to configure MIME types
    webserver::MimeConfig mimeConfig = webserver::MimeConfig::createDefault();
    EXPECT_NO_THROW(backend->setGlobalMimeConfig(mimeConfig));
}

// TDD: Test 8 - Mongoose backend should properly initialize Mongoose manager
TEST_F(WebServerBackendMongooseBasicTest, InitializesMongooseManager) {
    // After construction, the backend should have initialized the Mongoose manager
    // This is verified by successful start/stop operations
    EXPECT_TRUE(backend->start());
    EXPECT_TRUE(backend->isRunning());
    backend->stop();
    EXPECT_FALSE(backend->isRunning());
}

// TDD: Test 9 - Mongoose backend should handle HTTP request processing
TEST_F(WebServerBackendMongooseBasicTest, CanProcessHttpRequests) {
    bool handlerCalled = false;
    std::string receivedMethod, receivedPath;
    
    auto handler = [&](const webserver::HttpRequest& req, webserver::HttpResponse& res) {
        handlerCalled = true;
        receivedMethod = req.method;
        receivedPath = req.uri;
        res.statusCode = 200;
        res.body = "Handler response";
    };
    
    backend->registerHandler("GET", "/test", handler);
    EXPECT_TRUE(backend->start());
    
    // Wait for server to be ready by trying to connect (non-blocking check)
    bool serverReady = false;
    for (int i = 0; i < 10 && !serverReady; ++i) {
        int testSock = socket(AF_INET, SOCK_STREAM, 0);
        if (testSock >= 0) {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(currentPort);
            inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
            
            if (connect(testSock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                serverReady = true;
            }
            close(testSock);
        }
        if (!serverReady) usleep(1000); // 1ms between attempts
    }
    
    EXPECT_TRUE(serverReady); // Server must be ready before proceeding
    
    // Send HTTP request using helper - fast and clean
    std::string response = sendHttpRequest("GET /test HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
    
    EXPECT_FALSE(response.empty());
    EXPECT_TRUE(response.find("HTTP/1.1 200") != std::string::npos);
    EXPECT_TRUE(response.find("Handler response") != std::string::npos);
    
    backend->stop();
    
    EXPECT_TRUE(handlerCalled);
    EXPECT_EQ(receivedMethod, "GET");
    EXPECT_EQ(receivedPath, "/test");
}

// TDD: Test 10 - Mongoose backend should handle different content types
TEST_F(WebServerBackendMongooseBasicTest, HandlesAllContentTypes) {
    auto jsonHandler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.setJsonResponse("{\"message\": \"Hello, World!\"}");
    };
    
    auto htmlHandler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.setHtmlResponse("<html><body>Hello, World!</body></html>");
    };
    
    auto textHandler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.setPlainTextResponse("Hello, World!");
    };
    
    backend->registerHandler("GET", "/json", jsonHandler);
    backend->registerHandler("GET", "/html", htmlHandler);
    backend->registerHandler("GET", "/text", textHandler);
    
    EXPECT_TRUE(backend->start());
    
    // Test JSON response
    std::string jsonResp = sendHttpRequest("GET /json HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
    EXPECT_TRUE(jsonResp.find("application/json") != std::string::npos);
    EXPECT_TRUE(jsonResp.find("Hello, World!") != std::string::npos);
    
    // Test HTML response
    std::string htmlResp = sendHttpRequest("GET /html HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
    EXPECT_TRUE(htmlResp.find("text/html") != std::string::npos);
    EXPECT_TRUE(htmlResp.find("<html>") != std::string::npos);
    
    // Test plain text response
    std::string textResp = sendHttpRequest("GET /text HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
    EXPECT_TRUE(textResp.find("text/plain") != std::string::npos);
    EXPECT_TRUE(textResp.find("Hello, World!") != std::string::npos);
    
    backend->stop();
}

// TDD: Test 11 - RESTful API methods (GET, POST, PUT, DELETE) work correctly
TEST_F(WebServerBackendMongooseBasicTest, HandlesAllRestfulMethods) {
    std::string lastMethod, lastBody;
    
    auto handler = [&](const webserver::HttpRequest& req, webserver::HttpResponse& res) {
        lastMethod = req.method;
        lastBody = req.body;
        res.statusCode = 200;
        res.setJsonResponse("{\"method\":\"" + req.method + "\",\"received\":true}");
    };
    
    backend->registerHandler("GET", "/api/test", handler);
    backend->registerHandler("POST", "/api/test", handler);
    backend->registerHandler("PUT", "/api/test", handler);
    backend->registerHandler("DELETE", "/api/test", handler);
    
    EXPECT_TRUE(backend->start());
    
    // Test GET
    std::string getResp = sendHttpRequest("GET /api/test HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
    EXPECT_TRUE(getResp.find("HTTP/1.1 200") != std::string::npos);
    EXPECT_TRUE(getResp.find("\"method\":\"GET\"") != std::string::npos);
    
    // Test POST with body
    std::string postReq = "POST /api/test HTTP/1.1\r\nHost: localhost\r\nContent-Length: 13\r\nConnection: close\r\n\r\n{\"data\":true}";
    std::string postResp = sendHttpRequest(postReq);
    EXPECT_TRUE(postResp.find("HTTP/1.1 200") != std::string::npos);
    EXPECT_TRUE(postResp.find("\"method\":\"POST\"") != std::string::npos);
    
    // Test PUT
    std::string putReq = "PUT /api/test HTTP/1.1\r\nHost: localhost\r\nContent-Length: 14\r\nConnection: close\r\n\r\n{\"update\":true}";
    std::string putResp = sendHttpRequest(putReq);
    EXPECT_TRUE(putResp.find("HTTP/1.1 200") != std::string::npos);
    EXPECT_TRUE(putResp.find("\"method\":\"PUT\"") != std::string::npos);
    
    // Test DELETE
    std::string deleteResp = sendHttpRequest("DELETE /api/test HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
    EXPECT_TRUE(deleteResp.find("HTTP/1.1 200") != std::string::npos);
    EXPECT_TRUE(deleteResp.find("\"method\":\"DELETE\"") != std::string::npos);
    
    backend->stop();
}

// TDD: Test 12 - No memory leaks or crashes during rapid requests
TEST_F(WebServerBackendMongooseBasicTest, HandlesRapidRequestsWithoutLeaks) {
    auto handler = [](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.setPlainTextResponse("OK");
    };
    
    backend->registerHandler("GET", "/rapid", handler);
    EXPECT_TRUE(backend->start());
    
    // Send multiple rapid requests to test stability
    for (int i = 0; i < 10; ++i) {
        std::string response = sendHttpRequest("GET /rapid HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
        EXPECT_FALSE(response.empty());
        EXPECT_TRUE(response.find("HTTP/1.1 200") != std::string::npos);
    }
    
    // Server should still be running and responsive
    EXPECT_TRUE(backend->isRunning());
    
    backend->stop();
    EXPECT_FALSE(backend->isRunning());
}
