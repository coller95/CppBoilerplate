#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include "WebServer/IWebServer.h"

namespace webserver {

/**
 * WebServer - HTTP/WebSocket server implementation
 * 
 * Provides a high-level interface for HTTP server functionality with pluggable backends.
 * Supports raw access to HTTP headers and body, static file serving, and WebSocket connections.
 * 
 * Features:
 * - Multiple backend support (Mongoose, etc.)
 * - Raw HTTP request/response access
 * - Static file serving with MIME type detection
 * - WebSocket support
 * - Thread-safe operations
 * - RAII resource management
 */
class WebServer : public IWebServer {
public:
    /**
     * Supported backend implementations
     */
    enum class Backend {
        Mongoose,   // Mongoose embedded web server
        _           // Placeholder for future backends
    };

    /**
     * Construct a WebServer bound to the given IP address and port
     * @param bindAddress The IP address to bind (e.g., "127.0.0.1" or "0.0.0.0")
     * @param port The port to bind
     * @param backend The backend implementation to use (default: Mongoose)
     */
    WebServer(const std::string& bindAddress, uint16_t port, Backend backend = Backend::Mongoose);

    /**
     * Destructor - ensures proper cleanup
     */
    ~WebServer() override;

    // Server lifecycle (inherited from IWebServer)
    bool start() override;
    void stop() override;
    bool isRunning() const override;
    
    // HTTP handler registration (inherited from IWebServer)
    void registerHandler(const std::string& method, const std::string& path, HttpHandler handler) override;
    void registerHandler(const std::string& path, HttpHandler handler) override;
    
    // Convenience methods for common HTTP methods (inherited from IWebServer)
    void get(const std::string& path, HttpHandler handler) override;
    void post(const std::string& path, HttpHandler handler) override;
    void put(const std::string& path, HttpHandler handler) override;
    void del(const std::string& path, HttpHandler handler) override;
    
    // Static file serving (inherited from IWebServer)
    void serveStatic(const StaticRoute& route) override;
    void serveStatic(const std::string& urlPrefix, const std::string& localPath) override;
    void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig) override;
    void serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType = "") override;
    void setGlobalMimeConfig(const MimeConfig& config) override;
    
    // WebSocket support (inherited from IWebServer)
    void registerWebSocketHandler(const std::string& path, WebSocketHandler handler) override;
    bool sendWebSocketMessage(const std::string& connectionId, const std::string& message) override;
    
    // Server information (inherited from IWebServer)
    std::string getBindAddress() const override;
    uint16_t getPort() const override;
    size_t getActiveConnections() const override;

    // Delete copy constructor and assignment operator (RAII best practice)
    WebServer(const WebServer&) = delete;
    WebServer& operator=(const WebServer&) = delete;
    
    // Allow move constructor and assignment
    WebServer(WebServer&&) noexcept;
    WebServer& operator=(WebServer&&) noexcept;

private:
    // PIMPL idiom for hiding implementation details and backend-specific dependencies
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace webserver
