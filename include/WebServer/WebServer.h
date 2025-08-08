#pragma once

#include <string>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>
#include <cstdint>

class WebServer;

/**
 * WebServer: Extensible HTTP/WebSocket server interface
 * Supports REST, static files, uploads, downloads, and WebSockets.
 */

class WebServer {
public:
    using HttpHandler = std::function<void(std::string_view path, std::string_view method, const std::string& body, std::string& responseBody, int& statusCode)>;
    using PreServeHandler = std::function<void(const std::string& filePath)>;

    enum class Backend {
        Mongoose,
        _
    };

    /**
     * Construct a WebServer bound to the given IP address and port.
     * @param ipAddr The IP address to bind (e.g., "127.0.0.1" or "0.0.0.0")
     * @param port The port to bind
     * @param backend The backend implementation to use (default: Mongoose)
     */
    WebServer(const std::string& ipAddr, uint16_t port, Backend backend = Backend::Mongoose);
    ~WebServer();

    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler);
    void serveStatic(std::string_view urlPrefix, std::string_view directory);
    void serveStaticWithMime(std::string_view urlPrefix, std::string_view directory, std::string_view mimeType);
    void registerWebSocketHandler(std::string_view path /*, handler type TBD */);
    void registerPreServeHandler(PreServeHandler handler);
    void start();
    void stop();
    bool isRunning() const;

    /** Accessors for test and diagnostics */
    std::string getBindIp() const;
    uint16_t getBindPort() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
