#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>
#include <cstdint>

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
 * Provides raw access to request and allows building response
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
    
    // Common MIME type presets
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
    
    static MimeConfig createWebAssets() {
        MimeConfig config;
        config.extensionMap = {
            {".html", "text/html; charset=utf-8"},
            {".css", "text/css; charset=utf-8"},
            {".js", "application/javascript; charset=utf-8"},
            {".ts", "application/typescript; charset=utf-8"},
            {".jsx", "text/javascript; charset=utf-8"},
            {".vue", "text/vue; charset=utf-8"},
            {".scss", "text/scss; charset=utf-8"},
            {".sass", "text/sass; charset=utf-8"},
            {".less", "text/less; charset=utf-8"},
            {".woff", "font/woff"},
            {".woff2", "font/woff2"},
            {".ttf", "font/ttf"},
            {".eot", "application/vnd.ms-fontobject"},
            {".png", "image/png"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".gif", "image/gif"},
            {".svg", "image/svg+xml"},
            {".webp", "image/webp"},
            {".ico", "image/x-icon"}
        };
        config.defaultType = "text/plain; charset=utf-8";
        return config;
    }
    
    static MimeConfig createBinary() {
        MimeConfig config;
        config.extensionMap = {
            {".bin", "application/octet-stream"},
            {".exe", "application/octet-stream"},
            {".dll", "application/octet-stream"},
            {".so", "application/octet-stream"},
            {".dylib", "application/octet-stream"},
            {".zip", "application/zip"},
            {".tar", "application/x-tar"},
            {".gz", "application/gzip"},
            {".7z", "application/x-7z-compressed"},
            {".rar", "application/vnd.rar"},
            {".deb", "application/vnd.debian.binary-package"},
            {".rpm", "application/x-rpm"},
            {".dmg", "application/x-apple-diskimage"},
            {".iso", "application/x-iso9660-image"}
        };
        config.defaultType = "application/octet-stream";
        config.enableAutoDetection = false;  // Binary files shouldn't be auto-detected
        return config;
    }
};

/**
 * Interface for WebServer
 * Provides HTTP and WebSocket server capabilities with raw access to requests/responses
 */
class IWebServer {
public:
    virtual ~IWebServer() = default;
    
    // Server lifecycle
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // HTTP handler registration
    virtual void registerHandler(const std::string& method, const std::string& path, HttpHandler handler) = 0;
    virtual void registerHandler(const std::string& path, HttpHandler handler) = 0;  // Matches all methods
    
    // Convenience methods for common HTTP methods
    virtual void get(const std::string& path, HttpHandler handler) = 0;
    virtual void post(const std::string& path, HttpHandler handler) = 0;
    virtual void put(const std::string& path, HttpHandler handler) = 0;
    virtual void del(const std::string& path, HttpHandler handler) = 0;  // 'delete' is a keyword
    
    // Static file serving
    virtual void serveStatic(const StaticRoute& route) = 0;
    virtual void serveStatic(const std::string& urlPrefix, const std::string& localPath) = 0;
    virtual void serveStaticWithMime(const std::string& urlPrefix, const std::string& localPath, const MimeConfig& mimeConfig) = 0;
    virtual void serveFile(const std::string& path, const std::string& filePath, const std::string& mimeType = "") = 0;  // Serve single file
    virtual void setGlobalMimeConfig(const MimeConfig& config) = 0;  // Set MIME config for all static serving
    
    // WebSocket support
    virtual void registerWebSocketHandler(const std::string& path, WebSocketHandler handler) = 0;
    virtual bool sendWebSocketMessage(const std::string& connectionId, const std::string& message) = 0;
    
    // Server information
    virtual std::string getBindAddress() const = 0;
    virtual uint16_t getPort() const = 0;
    virtual size_t getActiveConnections() const = 0;
};

} // namespace webserver
