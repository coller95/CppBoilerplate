#include <WebServer/WebServer.h>
#include <WebServer/IWebServerBackend.h>
#include <WebServer/WebServerBackendFactory.h>
#include <stdexcept>
#include <algorithm>

namespace webserver {

// PIMPL implementation
class WebServer::Impl {
public:
    std::unique_ptr<IWebServerBackend> backend;
    Backend currentBackend;
    
    Impl(const std::string& bindAddress, uint16_t port, Backend backendType) 
        : currentBackend(backendType) {
        // Map WebServer::Backend to WebServerBackendFactory::Backend
        WebServerBackendFactory::Backend factoryBackend;
        switch (backendType) {
            case Backend::Mongoose:
                factoryBackend = WebServerBackendFactory::Backend::Mongoose;
                break;
            case Backend::Placeholder:
                factoryBackend = WebServerBackendFactory::Backend::Placeholder;
                break;
            default:
                factoryBackend = WebServerBackendFactory::Backend::Mongoose; // Default fallback
                break;
        }
        
        backend = WebServerBackendFactory::createBackend(factoryBackend, bindAddress, port);
    }
};

WebServer::WebServer(const std::string& bindAddress, uint16_t port, Backend backend)
    : _impl(std::make_unique<Impl>(bindAddress, port, backend)) {
}

WebServer::~WebServer() = default;

WebServer::WebServer(WebServer&&) noexcept = default;
WebServer& WebServer::operator=(WebServer&& other) noexcept = default;

// Server lifecycle
bool WebServer::start() {
    return _impl->backend->start();
}

void WebServer::stop() {
    _impl->backend->stop();
}

bool WebServer::isRunning() const {
    return _impl->backend->isRunning();
}

// HTTP handler registration
void WebServer::registerHandler(const std::string& method, const std::string& path, HttpHandler handler) {
    _impl->backend->registerHandler(method, path, std::move(handler));
}

void WebServer::registerHandler(const std::string& path, HttpHandler handler) {
    _impl->backend->registerHandler(path, std::move(handler));
}

// Convenience methods for common HTTP methods
void WebServer::get(const std::string& path, HttpHandler handler) {
    registerHandler("GET", path, std::move(handler));
}

void WebServer::post(const std::string& path, HttpHandler handler) {
    registerHandler("POST", path, std::move(handler));
}

void WebServer::put(const std::string& path, HttpHandler handler) {
    registerHandler("PUT", path, std::move(handler));
}

void WebServer::del(const std::string& path, HttpHandler handler) {
    registerHandler("DELETE", path, std::move(handler));
}

// Static file serving
void WebServer::serveStatic(const StaticRoute& route) {
    _impl->backend->serveStatic(route);
}

void WebServer::serveStatic(const std::string& urlPrefix, const std::string& localPath) {
    StaticRoute route;
    route.urlPrefix = urlPrefix;
    route.localPath = localPath;
    route.defaultFile = "index.html";
    route.allowDirectoryListing = false;
    route.defaultMimeType = "application/octet-stream";
    serveStatic(route);
}

void WebServer::serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig) {
    _impl->backend->serveStaticWithMime(urlPrefix, localPath, mimeConfig);
}

void WebServer::serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) {
    _impl->backend->serveFile(path, filePath, mimeType);
}

void WebServer::setGlobalMimeConfig(const MimeConfig& config) {
    _impl->backend->setGlobalMimeConfig(config);
}

// WebSocket support
void WebServer::registerWebSocketHandler(const std::string& path, WebSocketHandler handler) {
    _impl->backend->registerWebSocketHandler(path, std::move(handler));
}

bool WebServer::sendWebSocketMessage(const std::string& connectionId, const std::string& message) {
    return _impl->backend->sendWebSocketMessage(connectionId, message);
}

// Server information
std::string WebServer::getBindAddress() const {
    return _impl->backend->getBindAddress();
}

uint16_t WebServer::getPort() const {
    return _impl->backend->getPort();
}

size_t WebServer::getActiveConnections() const {
    return _impl->backend->getActiveConnections();
}

// Backend selection methods
WebServer::Backend WebServer::getCurrentBackend() const {
    return _impl->currentBackend;
}

std::string WebServer::getBackendName() const {
    switch (_impl->currentBackend) {
        case Backend::Mongoose:
            return "Mongoose";
        case Backend::Placeholder:
            return "Placeholder";
        default:
            return "Unknown";
    }
}

} // namespace webserver
