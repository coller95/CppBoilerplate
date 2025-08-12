#pragma once

#include <gmock/gmock.h>
#include <WebServer/IWebServer.h>

namespace webserver {

/**
 * Mock implementation of IWebServer for testing
 */
class MockWebServer : public IWebServer {
public:
    // Server lifecycle
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isRunning, (), (const, override));
    
    // HTTP handler registration
    MOCK_METHOD(void, registerHandler, (const std::string& method, const std::string& path, HttpHandler handler), (override));
    MOCK_METHOD(void, registerHandler, (const std::string& path, HttpHandler handler), (override));
    
    // Convenience methods for common HTTP methods
    MOCK_METHOD(void, get, (const std::string& path, HttpHandler handler), (override));
    MOCK_METHOD(void, post, (const std::string& path, HttpHandler handler), (override));
    MOCK_METHOD(void, put, (const std::string& path, HttpHandler handler), (override));
    MOCK_METHOD(void, del, (const std::string& path, HttpHandler handler), (override));
    
    // Static file serving
    MOCK_METHOD(void, serveStatic, (const StaticRoute& route), (override));
    MOCK_METHOD(void, serveStatic, (const std::string& urlPrefix, const std::string& localPath), (override));
    MOCK_METHOD(void, serveStaticWithMime, (const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig), (override));
    MOCK_METHOD(void, serveFile, (const std::string& path, const std::string& filePath, const std::string& mimeType), (override));
    MOCK_METHOD(void, setGlobalMimeConfig, (const MimeConfig& config), (override));
    
    // WebSocket support
    MOCK_METHOD(void, registerWebSocketHandler, (const std::string& path, WebSocketHandler handler), (override));
    MOCK_METHOD(bool, sendWebSocketMessage, (const std::string& connectionId, const std::string& message), (override));
    
    // Server information
    MOCK_METHOD(std::string, getBindAddress, (), (const, override));
    MOCK_METHOD(uint16_t, getPort, (), (const, override));
    MOCK_METHOD(size_t, getActiveConnections, (), (const, override));
};

} // namespace webserver
