#pragma once

#include <gmock/gmock.h>
#include <WebServerBackendMongoose/IWebServerBackendMongoose.h>

namespace webserverbackendmongoose {

/**
 * Mock implementation of IWebServerBackendMongoose for testing
 */
class MockWebServerBackendMongoose : public IWebServerBackendMongoose {
public:
    // IWebServerBackend methods (inherited from base interface)
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isRunning, (), (const, override));
    MOCK_METHOD(void, registerHandler, (const std::string& method, const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, registerHandler, (const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, serveStatic, (const webserver::StaticRoute& route), (override));
    MOCK_METHOD(void, serveStaticWithMime, (const std::string& urlPrefix, const std::string& localPath, const webserver::MimeConfig& mimeConfig), (override));
    MOCK_METHOD(void, serveFile, (const std::string& path, const std::string& filePath, const std::string& mimeType), (override));
    MOCK_METHOD(void, setGlobalMimeConfig, (const webserver::MimeConfig& config), (override));
    MOCK_METHOD(void, registerWebSocketHandler, (const std::string& path, webserver::WebSocketHandler handler), (override));
    MOCK_METHOD(bool, sendWebSocketMessage, (const std::string& connectionId, const std::string& message), (override));
    MOCK_METHOD(std::string, getBindAddress, (), (const, override));
    MOCK_METHOD(uint16_t, getPort, (), (const, override));
    MOCK_METHOD(size_t, getActiveConnections, (), (const, override));
    
    // IWebServerBackendMongoose-specific methods
    MOCK_METHOD(void, setDocumentRoot, (const std::string& documentRoot), (override));
    MOCK_METHOD(void, setMaxRequestSize, (size_t maxSize), (override));
    MOCK_METHOD(void, setConnectionTimeout, (int timeoutSeconds), (override));
    MOCK_METHOD(bool, enableSSL, (const std::string& certFile, const std::string& keyFile), (override));
};

} // namespace webserverbackendmongoose
