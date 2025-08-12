#include <WebServer/WebServer.h>
#include <stdexcept>
#include <algorithm>

namespace webserver {

// Forward declaration for backend implementation
class IWebServerBackend {
public:
    virtual ~IWebServerBackend() = default;
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual void registerHandler(const std::string& method, const std::string& path, HttpHandler handler) = 0;
    virtual void registerHandler(const std::string& path, HttpHandler handler) = 0;
    virtual void serveStatic(const StaticRoute& route) = 0;
    virtual void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig) = 0;
    virtual void serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) = 0;
    virtual void setGlobalMimeConfig(const MimeConfig& config) = 0;
    virtual void registerWebSocketHandler(const std::string& path, WebSocketHandler handler) = 0;
    virtual bool sendWebSocketMessage(const std::string& connectionId, const std::string& message) = 0;
    virtual std::string getBindAddress() const = 0;
    virtual uint16_t getPort() const = 0;
    virtual size_t getActiveConnections() const = 0;
};

// Placeholder backend implementation for now
class PlaceholderBackend : public IWebServerBackend {
private:
    std::string _bindAddress;
    uint16_t _port;
    bool _running = false;
    
public:
    PlaceholderBackend(const std::string& bindAddress, uint16_t port)
        : _bindAddress(bindAddress), _port(port) {}
    
    bool start() override {
        if (_running) return false;
        _running = true;
        return true;
    }
    
    void stop() override {
        _running = false;
    }
    
    bool isRunning() const override {
        return _running;
    }
    
    void registerHandler(const std::string& method, const std::string& path, HttpHandler handler) override {
        // Placeholder implementation
        (void)method; (void)path; (void)handler;
    }
    
    void registerHandler(const std::string& path, HttpHandler handler) override {
        // Placeholder implementation
        (void)path; (void)handler;
    }
    
    void serveStatic(const StaticRoute& route) override {
        // Placeholder implementation
        (void)route;
    }
    
    void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig) override {
        // Placeholder implementation
        (void)urlPrefix; (void)localPath; (void)mimeConfig;
    }
    
    void serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) override {
        // Placeholder implementation
        (void)path; (void)filePath; (void)mimeType;
    }
    
    void setGlobalMimeConfig(const MimeConfig& config) override {
        // Placeholder implementation
        (void)config;
    }
    
    void registerWebSocketHandler(const std::string& path, WebSocketHandler handler) override {
        // Placeholder implementation
        (void)path; (void)handler;
    }
    
    bool sendWebSocketMessage(const std::string& connectionId, const std::string& message) override {
        // Placeholder implementation
        (void)connectionId; (void)message;
        return false;
    }
    
    std::string getBindAddress() const override {
        return _bindAddress;
    }
    
    uint16_t getPort() const override {
        return _port;
    }
    
    size_t getActiveConnections() const override {
        return 0;
    }
};

// PIMPL implementation
class WebServer::Impl {
public:
    std::unique_ptr<IWebServerBackend> backend;
    
    Impl(const std::string& bindAddress, uint16_t port, Backend backendType) {
        switch (backendType) {
            case Backend::Mongoose:
                // TODO: Implement Mongoose backend
                backend = std::make_unique<PlaceholderBackend>(bindAddress, port);
                break;
            case Backend::_:
            default:
                backend = std::make_unique<PlaceholderBackend>(bindAddress, port);
                break;
        }
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

} // namespace webserver
