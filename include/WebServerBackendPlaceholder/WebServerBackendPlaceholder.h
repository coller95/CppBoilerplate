#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include "WebServer/IWebServer.h"

namespace webserverbackendplaceholder {

/**
 * WebServerBackendPlaceholder - Reference implementation for testing
 * 
 * A minimal, in-memory implementation of IWebServerBackend that doesn't
 * actually bind to network ports. Perfect for unit testing, development,
 * and as a reference implementation for other backends.
 * 
 * Features:
 * - No actual network binding (safe for testing)
 * - Handler registration and tracking
 * - Simulated server lifecycle
 * - Optional simulated latency for testing
 * - RAII resource management
 */
class WebServerBackendPlaceholder : public webserver::IWebServer {
public:
    /**
     * Constructor
     * @param bindAddress The IP address to simulate binding to
     * @param port The port to simulate binding to
     */
    WebServerBackendPlaceholder(const std::string& bindAddress, uint16_t port);

    /**
     * Destructor - ensures proper cleanup
     */
    ~WebServerBackendPlaceholder() override;

    // Server lifecycle (from IWebServer)
    bool start() override;
    void stop() override;
    bool isRunning() const override;
    
    // HTTP handler registration (from IWebServer)
    void registerHandler(const std::string& method, const std::string& path, webserver::HttpHandler handler) override;
    void registerHandler(const std::string& path, webserver::HttpHandler handler) override;
    
    // Convenience methods for common HTTP methods (from IWebServer)
    void get(const std::string& path, webserver::HttpHandler handler) override;
    void post(const std::string& path, webserver::HttpHandler handler) override;
    void put(const std::string& path, webserver::HttpHandler handler) override;
    void del(const std::string& path, webserver::HttpHandler handler) override;
    
    // Static file serving (from IWebServer)
    void serveStatic(const webserver::StaticRoute& route) override;
    void serveStatic(const std::string& urlPrefix, const std::string& localPath) override;
    void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const webserver::MimeConfig& mimeConfig) override;
    void serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) override;
    void setGlobalMimeConfig(const webserver::MimeConfig& config) override;
    
    // WebSocket support (from IWebServer)
    void registerWebSocketHandler(const std::string& path, webserver::WebSocketHandler handler) override;
    bool sendWebSocketMessage(const std::string& connectionId, const std::string& message) override;
    
    // Server information (from IWebServer)
    std::string getBindAddress() const override;
    uint16_t getPort() const override;
    size_t getActiveConnections() const override;

    // Placeholder-specific testing methods
    void setSimulatedLatency(int milliseconds);
    size_t getHandlerCount() const;
    void clearAllHandlers();

    // Delete copy constructor and assignment operator (RAII best practice)
    WebServerBackendPlaceholder(const WebServerBackendPlaceholder&) = delete;
    WebServerBackendPlaceholder& operator=(const WebServerBackendPlaceholder&) = delete;
    
    // Allow move constructor and assignment
    WebServerBackendPlaceholder(WebServerBackendPlaceholder&&) noexcept;
    WebServerBackendPlaceholder& operator=(WebServerBackendPlaceholder&&) noexcept;

private:
    // PIMPL idiom for hiding implementation details
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace webserverbackendplaceholder
