#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include "WebServerBackendPlaceholder/IWebServerBackendPlaceholder.h"

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
class WebServerBackendPlaceholder : public IWebServerBackendPlaceholder {
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

    // Server lifecycle (from IWebServerBackend)
    bool start() override;
    void stop() override;
    bool isRunning() const override;
    
    // HTTP handler registration (from IWebServerBackend)
    void registerHandler(const std::string& method, const std::string& path, webserver::HttpHandler handler) override;
    void registerHandler(const std::string& path, webserver::HttpHandler handler) override;
    
    // Static file serving (from IWebServerBackend)
    void serveStatic(const webserver::StaticRoute& route) override;
    void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const webserver::MimeConfig& mimeConfig) override;
    void serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) override;
    void setGlobalMimeConfig(const webserver::MimeConfig& config) override;
    
    // WebSocket support (from IWebServerBackend)
    void registerWebSocketHandler(const std::string& path, webserver::WebSocketHandler handler) override;
    bool sendWebSocketMessage(const std::string& connectionId, const std::string& message) override;
    
    // Server information (from IWebServerBackend)
    std::string getBindAddress() const override;
    uint16_t getPort() const override;
    size_t getActiveConnections() const override;

    // Placeholder-specific methods (from IWebServerBackendPlaceholder)
    void setSimulatedLatency(int milliseconds) override;
    size_t getHandlerCount() const override;
    void clearAllHandlers() override;

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
