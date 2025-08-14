#pragma once

#include <gmock/gmock.h>
#include <WebServer/IWebServer.h>

namespace webserverbackendmongoose {

/**
 * Mock implementation of IWebServer with Mongoose-specific methods for testing
 */
class MockWebServerBackendMongoose : public webserver::IWebServer {
public:
    // IWebServer methods
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isRunning, (), (const, override));
    MOCK_METHOD(void, registerHandler, (const std::string& method, const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, registerHandler, (const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, get, (const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, post, (const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, put, (const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, del, (const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, serveStatic, (const webserver::StaticRoute& route), (override));
    MOCK_METHOD(void, serveStatic, (const std::string& urlPrefix, const std::string& localPath), (override));
    MOCK_METHOD(void, serveStaticWithMime, (const std::string& urlPrefix, const std::string& localPath, const webserver::MimeConfig& mimeConfig), (override));
    MOCK_METHOD(void, serveFile, (const std::string& path, const std::string& filePath, const std::string& mimeType), (override));
    MOCK_METHOD(void, setGlobalMimeConfig, (const webserver::MimeConfig& config), (override));
    MOCK_METHOD(void, registerWebSocketHandler, (const std::string& path, webserver::WebSocketHandler handler), (override));
    MOCK_METHOD(bool, sendWebSocketMessage, (const std::string& connectionId, const std::string& message), (override));
    MOCK_METHOD(std::string, getBindAddress, (), (const, override));
    MOCK_METHOD(uint16_t, getPort, (), (const, override));
    MOCK_METHOD(size_t, getActiveConnections, (), (const, override));
    
    // Mongoose-specific methods (no longer from interface, but can still be mocked)
    MOCK_METHOD(void, setDocumentRoot, (const std::string& documentRoot));
    MOCK_METHOD(void, setMaxRequestSize, (size_t maxSize));
    MOCK_METHOD(void, setConnectionTimeout, (int timeoutSeconds));
    MOCK_METHOD(bool, enableSSL, (const std::string& certFile, const std::string& keyFile));
};

} // namespace webserverbackendmongoose
