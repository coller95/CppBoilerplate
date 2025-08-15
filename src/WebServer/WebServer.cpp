#include <WebServer/WebServer.h>
#include <mongoose.h>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <sstream>
#include <thread>
#include <atomic>
#include <arpa/inet.h>

namespace webserver {

// PIMPL implementation class
class WebServer::Impl {
    friend void eventHandler(struct mg_connection* c, int ev, void* ev_data);
    
public:
    std::string bindAddress;
    uint16_t port;
    std::atomic<bool> running{false};
    std::atomic<bool> shouldStop{false};
    
    // Mongoose-specific members
    struct mg_mgr manager;
    struct mg_connection* connection = nullptr;
    
    // Self-contained event processing
    std::thread eventThread;
    
    // Configuration
    std::string documentRoot = ".";
    size_t maxRequestSize = 1024 * 1024; // 1MB default
    int connectionTimeout = 30; // 30 seconds default
    std::string sslCert;
    std::string sslKey;
    
    // Global request handler - ApiRouter-enforced architecture
    HttpHandler globalHandler;
    std::unordered_map<std::string, WebSocketHandler> wsHandlers;
    MimeConfig globalMimeConfig;
    
    Impl(const std::string& bindAddr, uint16_t portNum)
        : bindAddress(bindAddr), port(portNum) {
        mg_mgr_init(&manager);
        globalMimeConfig = MimeConfig::createDefault();
    }
    
    ~Impl() {
        stop();
        mg_mgr_free(&manager);
    }
    
    bool start() {
        if (running) {
            return true; // Already running
        }
        
        std::string listenUrl = bindAddress + ":" + std::to_string(port);
        
        connection = mg_http_listen(&manager, listenUrl.c_str(), Impl::eventHandler, this);
        if (connection == nullptr) {
            return false;
        }
        
        // Start internal event processing thread
        shouldStop = false;
        eventThread = std::thread([this]() {
            while (!shouldStop) {
                mg_mgr_poll(&manager, 50); // 50ms polling
            }
        });
        
        running = true;
        return true;
    }
    
    void stop() {
        if (!running) {
            return; // Not running
        }
        
        shouldStop = true;
        
        // Join event thread
        if (eventThread.joinable()) {
            eventThread.join();
        }
        
        // Close connection
        if (connection) {
            connection->is_closing = 1;
            connection = nullptr;
        }
        
        running = false;
    }
    
    HttpRequest parseHttpRequest(struct mg_http_message* hm, struct mg_connection* c) {
        HttpRequest request;
        
        // Parse method
        request.method = std::string(hm->method.ptr, hm->method.len);
        
        // Parse URI
        request.uri = std::string(hm->uri.ptr, hm->uri.len);
        
        // Parse HTTP version
        request.version = "1.1"; // Default for now
        
        // Parse headers
        for (int i = 0; i < MG_MAX_HTTP_HEADERS && hm->headers[i].name.len > 0; i++) {
            std::string name(hm->headers[i].name.ptr, hm->headers[i].name.len);
            std::string value(hm->headers[i].value.ptr, hm->headers[i].value.len);
            request.headers[name] = value;
        }
        
        // Parse body
        request.body = std::string(hm->body.ptr, hm->body.len);
        
        // Parse query parameters
        if (hm->query.len > 0) {
            std::string query(hm->query.ptr, hm->query.len);
            // Simple query parsing
            std::istringstream iss(query);
            std::string pair;
            while (std::getline(iss, pair, '&')) {
                size_t pos = pair.find('=');
                if (pos != std::string::npos) {
                    request.queryParams[pair.substr(0, pos)] = pair.substr(pos + 1);
                }
            }
        }
        
        // Parse client info - simplified for now
        request.remoteIp = "unknown";
        request.remotePort = 0;
        
        return request;
    }
    
    void sendHttpResponse(struct mg_connection* c, const HttpResponse& response) {
        // Build headers
        std::ostringstream headers;
        for (const auto& [name, value] : response.headers) {
            headers << name << ": " << value << "\r\n";
        }
        
        // Send HTTP response
        mg_http_reply(c, response.statusCode, headers.str().c_str(), "%s", response.body.c_str());
    }
    
    // HTTP event handler
    void handleHttpEvent(struct mg_connection* c, struct mg_http_message* hm) {
        HttpRequest request = parseHttpRequest(hm, c);
        HttpResponse response;
        
        // Use the global handler - ApiRouter-enforced architecture
        // ALL HTTP requests go through this single handler, which should delegate to ApiRouter
        if (globalHandler) {
            globalHandler(request, response);
        } else {
            // No global handler set, return 500
            response.statusCode = 500;
            response.setPlainTextResponse("Internal Server Error: No request handler configured");
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

WebServer::WebServer(const std::string& bindAddress, uint16_t port)
    : _impl(std::make_unique<Impl>(bindAddress, port)) {
}

WebServer::~WebServer() = default;

WebServer::WebServer(WebServer&&) noexcept = default;
WebServer& WebServer::operator=(WebServer&& other) noexcept = default;

// Server lifecycle
bool WebServer::start() {
    return _impl->start();
}

void WebServer::stop() {
    _impl->stop();
}

bool WebServer::isRunning() const {
    return _impl->running;
}

// Global request handler - ApiRouter-enforced architecture
void WebServer::setGlobalRequestHandler(HttpHandler handler) {
    _impl->globalHandler = std::move(handler);
}

// Server information
std::string WebServer::getBindAddress() const {
    return _impl->bindAddress;
}

uint16_t WebServer::getPort() const {
    return _impl->port;
}

size_t WebServer::getActiveConnections() const {
    // TODO: Implement connection counting if needed
    return _impl->running ? 1 : 0;
}

// Static file serving - simplified for ApiRouter-enforced architecture
void WebServer::serveStatic(const StaticRoute& route) {
    // TODO: In ApiRouter-enforced architecture, static files should be handled
    // through ApiRouter or a dedicated static file service
    (void)route; // Suppress unused parameter warning
}

void WebServer::serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig) {
    // TODO: In ApiRouter-enforced architecture, static files should be handled
    // through ApiRouter or a dedicated static file service
    (void)urlPrefix; (void)localPath; (void)mimeConfig; // Suppress unused parameter warnings
}

void WebServer::setGlobalMimeConfig(const MimeConfig& config) {
    _impl->globalMimeConfig = config;
}

// WebSocket support
void WebServer::registerWebSocketHandler(const std::string& path, WebSocketHandler handler) {
    _impl->wsHandlers[path] = std::move(handler);
}

bool WebServer::sendWebSocketMessage(const std::string& connectionId, const std::string& message) {
    // TODO: Implement WebSocket message sending
    (void)connectionId; (void)message; // Suppress unused parameter warnings
    return false;
}

} // namespace webserver
