#include <WebServerBackendPlaceholder/WebServerBackendPlaceholder.h>
#include <unordered_map>
#include <chrono>
#include <thread>

namespace webserverbackendplaceholder {

// PIMPL implementation class
class WebServerBackendPlaceholder::Impl {
public:
    std::string bindAddress;
    uint16_t port;
    bool running = false;
    int simulatedLatencyMs = 0;
    
    // Handler storage
    std::unordered_map<std::string, webserver::HttpHandler> httpHandlers;
    std::unordered_map<std::string, webserver::WebSocketHandler> wsHandlers;
    webserver::MimeConfig globalMimeConfig;
    
    Impl(const std::string& bindAddr, uint16_t portNum)
        : bindAddress(bindAddr), port(portNum) {
        globalMimeConfig = webserver::MimeConfig::createDefault();
    }
    
    void simulateLatency() {
        if (simulatedLatencyMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(simulatedLatencyMs));
        }
    }
};

WebServerBackendPlaceholder::WebServerBackendPlaceholder(const std::string& bindAddress, uint16_t port)
    : _impl(std::make_unique<Impl>(bindAddress, port)) {
}

WebServerBackendPlaceholder::~WebServerBackendPlaceholder() = default;

WebServerBackendPlaceholder::WebServerBackendPlaceholder(WebServerBackendPlaceholder&&) noexcept = default;
WebServerBackendPlaceholder& WebServerBackendPlaceholder::operator=(WebServerBackendPlaceholder&&) noexcept = default;

// Server lifecycle
bool WebServerBackendPlaceholder::start() {
    _impl->simulateLatency();
    if (_impl->running) return false;
    _impl->running = true;
    return true;
}

void WebServerBackendPlaceholder::stop() {
    _impl->simulateLatency();
    _impl->running = false;
}

bool WebServerBackendPlaceholder::isRunning() const {
    return _impl->running;
}

// HTTP handler registration
void WebServerBackendPlaceholder::registerHandler(const std::string& method, const std::string& path, webserver::HttpHandler handler) {
    std::string key = method + " " + path;
    _impl->httpHandlers[key] = std::move(handler);
}

void WebServerBackendPlaceholder::registerHandler(const std::string& path, webserver::HttpHandler handler) {
    _impl->httpHandlers[path] = std::move(handler);
}

// Static file serving (simulated)
void WebServerBackendPlaceholder::serveStatic(const webserver::StaticRoute& route) {
    // Simulate static file serving by registering a placeholder handler
    auto handler = [route](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        res.setPlainTextResponse("Simulated static content from: " + route.localPath);
    };
    registerHandler(route.urlPrefix + "*", handler);
}

void WebServerBackendPlaceholder::serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const webserver::MimeConfig& mimeConfig) {
    webserver::StaticRoute route;
    route.urlPrefix = urlPrefix;
    route.localPath = localPath;
    (void)mimeConfig; // Store for future use if needed
    serveStatic(route);
}

void WebServerBackendPlaceholder::serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) {
    auto handler = [filePath, mimeType](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        res.statusCode = 200;
        if (!mimeType.empty()) {
            res.setContentType(mimeType);
        }
        res.body = "Simulated file content from: " + filePath;
    };
    registerHandler(path, handler);
}

void WebServerBackendPlaceholder::setGlobalMimeConfig(const webserver::MimeConfig& config) {
    _impl->globalMimeConfig = config;
}

// WebSocket support (simulated)
void WebServerBackendPlaceholder::registerWebSocketHandler(const std::string& path, webserver::WebSocketHandler handler) {
    _impl->wsHandlers[path] = std::move(handler);
}

bool WebServerBackendPlaceholder::sendWebSocketMessage(const std::string& connectionId, const std::string& message) {
    // Simulate message sending
    (void)connectionId; (void)message;
    _impl->simulateLatency();
    return _impl->running; // Can only send if server is running
}

// Server information
std::string WebServerBackendPlaceholder::getBindAddress() const {
    return _impl->bindAddress;
}

uint16_t WebServerBackendPlaceholder::getPort() const {
    return _impl->port;
}

size_t WebServerBackendPlaceholder::getActiveConnections() const {
    // Simulate active connections
    return _impl->running ? 1 : 0;
}

// Placeholder-specific methods
void WebServerBackendPlaceholder::setSimulatedLatency(int milliseconds) {
    _impl->simulatedLatencyMs = milliseconds;
}

size_t WebServerBackendPlaceholder::getHandlerCount() const {
    return _impl->httpHandlers.size() + _impl->wsHandlers.size();
}

void WebServerBackendPlaceholder::clearAllHandlers() {
    _impl->httpHandlers.clear();
    _impl->wsHandlers.clear();
}

} // namespace webserverbackendplaceholder
