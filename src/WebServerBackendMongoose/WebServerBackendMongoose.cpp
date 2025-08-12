#include <WebServerBackendMongoose/WebServerBackendMongoose.h>
#include <mongoose.h>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <sstream>

namespace webserverbackendmongoose {

// PIMPL implementation class
class WebServerBackendMongoose::Impl {
    // Friend declaration for static event handler
    friend void eventHandler(struct mg_connection* c, int ev, void* ev_data);
    
public:
    std::string bindAddress;
    uint16_t port;
    bool running = false;
    
    // Mongoose-specific members
    struct mg_mgr manager;
    struct mg_connection* connection = nullptr;
    
    // Configuration
    std::string documentRoot = ".";
    size_t maxRequestSize = 1024 * 1024; // 1MB default
    int connectionTimeout = 30; // 30 seconds default
    std::string sslCert;
    std::string sslKey;
    
    // Handlers storage
    std::unordered_map<std::string, webserver::HttpHandler> httpHandlers;
    std::unordered_map<std::string, webserver::WebSocketHandler> wsHandlers;
    webserver::MimeConfig globalMimeConfig;
    
    Impl(const std::string& bindAddr, uint16_t portNum)
        : bindAddress(bindAddr), port(portNum) {
        mg_mgr_init(&manager);
        globalMimeConfig = webserver::MimeConfig::createDefault();
    }
    
    ~Impl() {
        if (running) {
            stop();
        }
        mg_mgr_free(&manager);
    }
    
    bool start() {
        if (running) return false;
        
        // Create listen URL for Mongoose
        std::string listenUrl = bindAddress + ":" + std::to_string(port);
        connection = mg_http_listen(&manager, listenUrl.c_str(), Impl::eventHandler, this);
        if (connection == nullptr) {
            return false;
        }
        
        running = true;
        return true;
    }
    
    void stop() {
        if (!running) return;
        
        if (connection) {
            mg_close_conn(connection);
            connection = nullptr;
        }
        running = false;
    }
    
    // Process events - this should be called periodically
    void poll(int timeoutMs = 1) {
        if (running) {
            mg_mgr_poll(&manager, timeoutMs);
        }
    }
    
    // Helper method to parse HTTP request from Mongoose
    webserver::HttpRequest parseHttpRequest(struct mg_http_message* hm, struct mg_connection* c) {
        webserver::HttpRequest req;
        
        // Extract method, URI, and version
        req.method = std::string(hm->method.ptr, hm->method.len);
        req.uri = std::string(hm->uri.ptr, hm->uri.len);
        req.version = "1.1"; // Mongoose typically uses HTTP/1.1
        
        // Extract headers
        for (int i = 0; i < MG_MAX_HTTP_HEADERS && hm->headers[i].name.len > 0; i++) {
            std::string name(hm->headers[i].name.ptr, hm->headers[i].name.len);
            std::string value(hm->headers[i].value.ptr, hm->headers[i].value.len);
            req.headers[name] = value;
        }
        
        // Extract body
        if (hm->body.len > 0) {
            req.body = std::string(hm->body.ptr, hm->body.len);
        }
        
        // Extract query parameters
        char queryBuf[256];
        if (mg_http_get_var(&hm->query, "", queryBuf, sizeof(queryBuf)) > 0) {
            // TODO: Parse query parameters properly
            // For now, store raw query string
        }
        
        // Extract remote IP and port
        char addr[64];
        mg_snprintf(addr, sizeof(addr), "%M", mg_print_ip, &c->rem);
        req.remoteIp = addr;
        req.remotePort = mg_ntohs(c->rem.port);
        
        return req;
    }
    
    // Helper method to send HTTP response via Mongoose
    void sendHttpResponse(struct mg_connection* c, const webserver::HttpResponse& response) {
        std::ostringstream headers;
        
        // Build headers string
        for (const auto& header : response.headers) {
            headers << header.first << ": " << header.second << "\r\n";
        }
        
        // Send response
        mg_http_reply(c, response.statusCode, headers.str().c_str(), "%s", response.body.c_str());
    }
    
    // HTTP event handler
    void handleHttpEvent(struct mg_connection* c, struct mg_http_message* hm) {
        webserver::HttpRequest request = parseHttpRequest(hm, c);
        webserver::HttpResponse response;
        
        // Try to find specific method+path handler first
        std::string methodPath = request.method + " " + request.uri;
        auto handlerIt = httpHandlers.find(methodPath);
        
        if (handlerIt == httpHandlers.end()) {
            // Try to find path-only handler (matches all methods)
            handlerIt = httpHandlers.find(request.uri);
        }
        
        if (handlerIt != httpHandlers.end()) {
            // Call the registered handler
            handlerIt->second(request, response);
        } else {
            // No handler found, return 404
            response.statusCode = 404;
            response.setPlainTextResponse("Not Found");
        }
        
        sendHttpResponse(c, response);
    }
    
    // Static event handler for Mongoose callbacks
    static void eventHandler(struct mg_connection* c, int ev, void* ev_data) {
        auto* impl = static_cast<Impl*>(c->fn_data);
        
        switch (ev) {
            case MG_EV_HTTP_MSG: {
                auto* hm = static_cast<struct mg_http_message*>(ev_data);
                impl->handleHttpEvent(c, hm);
                break;
            }
            
            case MG_EV_WS_MSG: {
                // TODO: Implement WebSocket message handling
                break;
            }
            
            case MG_EV_ACCEPT: {
                // New connection accepted
                break;
            }
            
            case MG_EV_CLOSE: {
                // Connection closed
                break;
            }
            
            default:
                break;
        }
    }
};

WebServerBackendMongoose::WebServerBackendMongoose(const std::string& bindAddress, uint16_t port)
    : _impl(std::make_unique<Impl>(bindAddress, port)) {
}

WebServerBackendMongoose::~WebServerBackendMongoose() = default;

WebServerBackendMongoose::WebServerBackendMongoose(WebServerBackendMongoose&&) noexcept = default;
WebServerBackendMongoose& WebServerBackendMongoose::operator=(WebServerBackendMongoose&& other) noexcept = default;

// Server lifecycle
bool WebServerBackendMongoose::start() {
    return _impl->start();
}

void WebServerBackendMongoose::stop() {
    _impl->stop();
}

bool WebServerBackendMongoose::isRunning() const {
    return _impl->running;
}

// HTTP handler registration
void WebServerBackendMongoose::registerHandler(const std::string& method, const std::string& path, webserver::HttpHandler handler) {
    std::string key = method + " " + path;
    _impl->httpHandlers[key] = std::move(handler);
}

void WebServerBackendMongoose::registerHandler(const std::string& path, webserver::HttpHandler handler) {
    _impl->httpHandlers[path] = std::move(handler);
}

// Static file serving
void WebServerBackendMongoose::serveStatic(const webserver::StaticRoute& route) {
    // Register a catch-all handler for the URL prefix
    auto handler = [this, route](const webserver::HttpRequest& req, webserver::HttpResponse& res) {
        // Extract the file path from the URI
        std::string requestPath = req.uri;
        if (requestPath.find(route.urlPrefix) == 0) {
            std::string relativePath = requestPath.substr(route.urlPrefix.length());
            std::string fullPath = route.localPath + "/" + relativePath;
            
            // TODO: Implement actual file serving logic
            // For now, just return a placeholder response
            res.statusCode = 200;
            res.setPlainTextResponse("Static file: " + fullPath);
        } else {
            res.statusCode = 404;
            res.setPlainTextResponse("File not found");
        }
    };
    
    // Register handler for this static route
    registerHandler(route.urlPrefix + "*", handler);
}

void WebServerBackendMongoose::serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const webserver::MimeConfig& mimeConfig) {
    webserver::StaticRoute route;
    route.urlPrefix = urlPrefix;
    route.localPath = localPath;
    // TODO: Use mimeConfig for MIME type detection
    (void)mimeConfig; // Suppress unused parameter warning for now
    serveStatic(route);
}

void WebServerBackendMongoose::serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType) {
    auto handler = [filePath, mimeType](const webserver::HttpRequest& /* req */, webserver::HttpResponse& res) {
        // TODO: Read and serve the actual file
        // For now, just return a placeholder response
        res.statusCode = 200;
        if (!mimeType.empty()) {
            res.setContentType(mimeType);
        }
        res.body = "File content from: " + filePath;
    };
    
    registerHandler(path, handler);
}

void WebServerBackendMongoose::setGlobalMimeConfig(const webserver::MimeConfig& config) {
    _impl->globalMimeConfig = config;
}

// WebSocket support
void WebServerBackendMongoose::registerWebSocketHandler(const std::string& path, webserver::WebSocketHandler handler) {
    _impl->wsHandlers[path] = std::move(handler);
    // TODO: Integrate with Mongoose WebSocket handling
}

bool WebServerBackendMongoose::sendWebSocketMessage(const std::string& connectionId, const std::string& message) {
    // TODO: Implement WebSocket message sending
    // For now, return false as WebSocket is not fully implemented
    (void)connectionId; (void)message;
    return false;
}

// Server information
std::string WebServerBackendMongoose::getBindAddress() const {
    return _impl->bindAddress;
}

uint16_t WebServerBackendMongoose::getPort() const {
    return _impl->port;
}

size_t WebServerBackendMongoose::getActiveConnections() const {
    if (!_impl->running) return 0;
    
    // Count active connections in the manager
    size_t count = 0;
    for (struct mg_connection* c = _impl->manager.conns; c != nullptr; c = c->next) {
        if (c->is_listening == 0) { // Don't count the listening socket
            count++;
        }
    }
    return count;
}

// Mongoose-specific configuration
void WebServerBackendMongoose::setDocumentRoot(const std::string& documentRoot) {
    _impl->documentRoot = documentRoot;
}

void WebServerBackendMongoose::setMaxRequestSize(size_t maxSize) {
    _impl->maxRequestSize = maxSize;
}

void WebServerBackendMongoose::setConnectionTimeout(int timeoutSeconds) {
    _impl->connectionTimeout = timeoutSeconds;
}

bool WebServerBackendMongoose::enableSSL(const std::string& certFile, const std::string& keyFile) {
    _impl->sslCert = certFile;
    _impl->sslKey = keyFile;
    
    // TODO: Validate certificate and key files exist
    // For now, just store the paths - actual SSL would require file validation
    // and Mongoose SSL configuration during server startup
    
    // Return false for now as SSL implementation is not complete
    return false;
}

} // namespace webserverbackendmongoose
