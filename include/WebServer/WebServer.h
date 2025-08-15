#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <cstdint>
#include <memory>

namespace webserver {

/**
 * HTTP request structure providing raw access to all request data
 */
struct HttpRequest {
    std::string method;                                              // GET, POST, PUT, DELETE, etc.
    std::string uri;                                                // Request URI/path
    std::string version;                                            // HTTP version (e.g., "1.1")
    std::unordered_map<std::string, std::string> headers;          // Request headers (case-insensitive keys)
    std::string body;                                               // Raw request body
    std::unordered_map<std::string, std::string> queryParams;      // Parsed query parameters
    std::string remoteIp;                                           // Client IP address
    uint16_t remotePort;                                            // Client port
};

/**
 * HTTP response structure for building responses
 */
struct HttpResponse {
    int statusCode = 200;                                           // HTTP status code
    std::unordered_map<std::string, std::string> headers;          // Response headers
    std::string body;                                               // Response body
    
    // Convenience methods for common operations
    void setHeader(const std::string& name, const std::string& value) {
        headers[name] = value;
    }
    
    void setContentType(const std::string& contentType) {
        setHeader("Content-Type", contentType);
    }
    
    void setJsonResponse(const std::string& json) {
        setContentType("application/json");
        body = json;
    }
    
    void setHtmlResponse(const std::string& html) {
        setContentType("text/html; charset=utf-8");
        body = html;
    }
    
    void setPlainTextResponse(const std::string& text) {
        setContentType("text/plain; charset=utf-8");
        body = text;
    }
};

/**
 * HTTP handler function type
 */
using HttpHandler = std::function<void(const HttpRequest& request, HttpResponse& response)>;

/**
 * WebSocket message handler type
 */
using WebSocketHandler = std::function<void(const std::string& connectionId, const std::string& message)>;

/**
 * Static file serving configuration
 */
struct StaticRoute {
    std::string urlPrefix;      // URL prefix (e.g., "/static/")
    std::string localPath;      // Local filesystem path
    std::string defaultFile;    // Default file for directories (e.g., "index.html")
    bool allowDirectoryListing = false;
    std::unordered_map<std::string, std::string> mimeTypeOverrides;  // Custom MIME types by extension
    std::string defaultMimeType = "application/octet-stream";        // Fallback MIME type
};

/**
 * MIME type configuration for file serving
 */
struct MimeConfig {
    std::unordered_map<std::string, std::string> extensionMap;  // .ext -> MIME type mapping
    std::string defaultType = "application/octet-stream";       // Default for unknown extensions
    bool enableAutoDetection = true;                            // Try to detect MIME from content
    
    static MimeConfig createDefault() {
        MimeConfig config;
        config.extensionMap = {
            {".html", "text/html; charset=utf-8"},
            {".htm", "text/html; charset=utf-8"},
            {".css", "text/css; charset=utf-8"},
            {".js", "application/javascript; charset=utf-8"},
            {".json", "application/json; charset=utf-8"},
            {".xml", "application/xml; charset=utf-8"},
            {".txt", "text/plain; charset=utf-8"},
            {".png", "image/png"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".gif", "image/gif"},
            {".svg", "image/svg+xml"},
            {".ico", "image/x-icon"},
            {".pdf", "application/pdf"},
            {".zip", "application/zip"},
            {".mp4", "video/mp4"},
            {".mp3", "audio/mpeg"}
        };
        return config;
    }
};

/**
 * WebServer - Concrete HTTP server implementation
 * 
 * CRITICAL: First principles infrastructure design
 * - No interface abstraction (YAGNI - we only use one HTTP backend)
 * - PIMPL pattern for mongoose.h compilation isolation only
 * - Direct, high-performance implementation
 * - ApiRouter-enforced architecture: only setGlobalRequestHandler for routing
 */
class WebServer {
public:
    /**
     * Constructor with bind configuration
     */
    WebServer(const std::string& bindAddress, uint16_t port);

    /**
     * Destructor - ensures proper cleanup
     */
    ~WebServer();

    // Server lifecycle - core functionality
    bool start();
    void stop();
    bool isRunning() const;
    
    // Global request handler - ONLY way to configure HTTP handling
    // This should delegate ALL requests to ApiRouter
    void setGlobalRequestHandler(HttpHandler handler);
    
    // Server information - read-only access
    std::string getBindAddress() const;
    uint16_t getPort() const;
    size_t getActiveConnections() const;
    
    // Static file serving - infrastructure feature
    void serveStatic(const StaticRoute& route);
    void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig);
    void setGlobalMimeConfig(const MimeConfig& config);
    
    // WebSocket support - infrastructure feature
    void registerWebSocketHandler(const std::string& path, WebSocketHandler handler);
    bool sendWebSocketMessage(const std::string& connectionId, const std::string& message);

    // Delete copy constructor and assignment operator (RAII best practice)
    WebServer(const WebServer&) = delete;
    WebServer& operator=(const WebServer&) = delete;
    
    // Allow move constructor and assignment
    WebServer(WebServer&&) noexcept;
    WebServer& operator=(WebServer&&) noexcept;

private:
    // PIMPL idiom to hide mongoose.h implementation details
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace webserver
