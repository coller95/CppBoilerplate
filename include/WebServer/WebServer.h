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

    WebServer(uint16_t port = 8080);
    ~WebServer();

    /**
     * Registers a handler for a given path and HTTP method.
     * @param path The URL path (e.g., "/api/upload")
     * @param method The HTTP method (e.g., "GET", "POST")
     * @param handler The handler function
     */
    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler);

    /**
     * Serves static files from a directory.
     * @param urlPrefix The URL prefix (e.g., "/static/")
     * @param directory The local directory to serve
     */
    void serveStatic(std::string_view urlPrefix, std::string_view directory);

    /**
     * Serves static files from a directory with a specified MIME type.
     * @param urlPrefix The URL prefix (e.g., "/static/")
     * @param directory The local directory to serve
     * @param mimeType The MIME type to use for the files
     */
    void serveStaticWithMime(std::string_view urlPrefix, std::string_view directory, std::string_view mimeType);

    /**
     * Registers a WebSocket handler for a given path.
     * (Stub for future extension)
     */
    void registerWebSocketHandler(std::string_view path /*, handler type TBD */);

    /** Starts the server. */
    void start();
    /** Stops the server. */
    void stop();
    /** Returns true if the server is running. */
    bool isRunning() const;

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
