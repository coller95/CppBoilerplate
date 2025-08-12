#pragma once

#include <WebServer/IWebServer.h>
#include <string>
#include <memory>

namespace webserver {

/**
 * Interface for WebServer backend implementations.
 * This allows different HTTP server libraries (Mongoose, etc.) to be used
 * interchangeably with the main WebServer class.
 */
class IWebServerBackend {
public:
    virtual ~IWebServerBackend() = default;
    
    // Server lifecycle
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // HTTP handler registration
    virtual void registerHandler(const std::string& method, const std::string& path, HttpHandler handler) = 0;
    virtual void registerHandler(const std::string& path, HttpHandler handler) = 0;
    
    // Static file serving
    virtual void serveStatic(const StaticRoute& route) = 0;
    virtual void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig) = 0;
    virtual void serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) = 0;
    virtual void setGlobalMimeConfig(const MimeConfig& config) = 0;
    
    // WebSocket support
    virtual void registerWebSocketHandler(const std::string& path, WebSocketHandler handler) = 0;
    virtual bool sendWebSocketMessage(const std::string& connectionId, const std::string& message) = 0;
    
    // Server information
    virtual std::string getBindAddress() const = 0;
    virtual uint16_t getPort() const = 0;
    virtual size_t getActiveConnections() const = 0;
};

} // namespace webserver
