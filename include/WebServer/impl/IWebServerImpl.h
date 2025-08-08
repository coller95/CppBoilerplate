#pragma once
#include <WebServer/WebServer.h>
#include <string>
#include <string_view>
#include <cstdint>
#include <memory>

namespace WebServerLib {
/**
 * Abstract interface for HTTP/WebSocket server implementation.
 * Allows plugging in different backend libraries (e.g., Mongoose).
 */
class IWebServerImpl {
public:
    virtual ~IWebServerImpl() = default;
    virtual void registerHttpHandler(std::string_view path, std::string_view method, WebServer::HttpHandler handler) = 0;
    virtual void serveStatic(std::string_view urlPrefix, std::string_view directory) = 0;
    virtual void registerWebSocketHandler(std::string_view path) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    /**
     * Serves static files from a directory with a specified MIME type.
     * @param urlPrefix The URL prefix (e.g., "/static/")
     * @param directory The local directory to serve
     * @param mimeType The MIME type to use for the files
     */
    virtual void serveStaticWithMime(std::string_view urlPrefix, std::string_view directory, std::string_view mimeType) = 0;
    /**
     * Registers a handler to be executed before serving a file.
     * @param handler The handler function
     */
    virtual void registerPreServeHandler(WebServer::PreServeHandler handler) = 0;
};
} // namespace WebServerLib
