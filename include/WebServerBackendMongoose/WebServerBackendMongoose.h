#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include "WebServer/IWebServer.h"

namespace webserverbackendmongoose {

/**
 * WebServerBackendMongoose - Mongoose HTTP server backend implementation
 * 
 * Provides HTTP server functionality using the Mongoose embedded web server library.
 * Implements the IWebServer interface directly for optimal performance.
 */
class WebServerBackendMongoose : public webserver::IWebServer {
public:
    /**
     * Constructor
     * @param bindAddress The IP address to bind (e.g., "127.0.0.1" or "0.0.0.0")
     * @param port The port to bind
     */
    WebServerBackendMongoose(const std::string& bindAddress, uint16_t port);

    /**
     * Destructor - ensures proper cleanup
     */
    ~WebServerBackendMongoose() override;

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

    // Mongoose-specific configuration methods
    void setDocumentRoot(const std::string& documentRoot);
    void setMaxRequestSize(size_t maxSize);
    void setConnectionTimeout(int timeoutSeconds);
    bool enableSSL(const std::string& certFile, const std::string& keyFile);

    // Delete copy constructor and assignment operator (RAII best practice)
    WebServerBackendMongoose(const WebServerBackendMongoose&) = delete;
    WebServerBackendMongoose& operator=(const WebServerBackendMongoose&) = delete;
    
    // Allow move constructor and assignment
    WebServerBackendMongoose(WebServerBackendMongoose&&) noexcept;
    WebServerBackendMongoose& operator=(WebServerBackendMongoose&&) noexcept;

private:
    // PIMPL idiom for hiding Mongoose-specific implementation details
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace webserverbackendmongoose
