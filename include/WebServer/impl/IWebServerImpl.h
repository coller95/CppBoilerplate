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
};
} // namespace WebServerLib
