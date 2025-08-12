#pragma once

#include <gmock/gmock.h>
#include <WebServerBackendPlaceholder/IWebServerBackendPlaceholder.h>

namespace webserverbackendplaceholder {

/**
 * Mock implementation of IWebServerBackendPlaceholder for testing
 */
class MockWebServerBackendPlaceholder : public IWebServerBackendPlaceholder {
public:
    // Server lifecycle methods (from IWebServerBackend)
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(bool, isRunning, (), (const, override));
    
    // HTTP handler registration methods (from IWebServerBackend)
    MOCK_METHOD(void, registerHandler, (const std::string& method, const std::string& path, webserver::HttpHandler handler), (override));
    MOCK_METHOD(void, registerHandler, (const std::string& path, webserver::HttpHandler handler), (override));
    
    // Static file serving methods (from IWebServerBackend)
    MOCK_METHOD(void, serveStatic, (const webserver::StaticRoute& route), (override));
    MOCK_METHOD(void, serveStaticWithMime, (const std::string& urlPrefix, const std::string& localPath, const webserver::MimeConfig& mimeConfig), (override));
    MOCK_METHOD(void, serveFile, (const std::string& path, const std::string& filePath, const std::string& mimeType), (override));
    MOCK_METHOD(void, setGlobalMimeConfig, (const webserver::MimeConfig& config), (override));
    
    // WebSocket support methods (from IWebServerBackend)
    MOCK_METHOD(void, registerWebSocketHandler, (const std::string& path, webserver::WebSocketHandler handler), (override));
    MOCK_METHOD(bool, sendWebSocketMessage, (const std::string& connectionId, const std::string& message), (override));
    
    // Server information methods (from IWebServerBackend)
    MOCK_METHOD(std::string, getBindAddress, (), (const, override));
    MOCK_METHOD(uint16_t, getPort, (), (const, override));
    MOCK_METHOD(size_t, getActiveConnections, (), (const, override));

    // Placeholder-specific methods (from IWebServerBackendPlaceholder)
    MOCK_METHOD(void, setSimulatedLatency, (int milliseconds), (override));
    MOCK_METHOD(size_t, getHandlerCount, (), (const, override));
    MOCK_METHOD(void, clearAllHandlers, (), (override));
};

} // namespace webserverbackendplaceholder
