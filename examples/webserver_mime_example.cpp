/**
 * WebServer MIME Type Configuration Example
 * 
 * This example demonstrates how to configure custom MIME types for different file types
 * when serving static files with the WebServer.
 */

#include <WebServer/WebServer.h>
#include <iostream>

void demonstrateMimePresets(webserver::WebServer& server) {
    std::cout << "=== MIME Type Presets Demo ===" << std::endl;
    
    // 1. Default preset - good for general web content
    auto defaultConfig = webserver::MimeConfig::createDefault();
    server.serveStaticWithMime("/content/", "./content", defaultConfig);
    std::cout << "✓ Default MIME config applied to /content/" << std::endl;
    
    // 2. Web assets preset - optimized for modern web development
    auto webConfig = webserver::MimeConfig::createWebAssets();
    server.serveStaticWithMime("/assets/", "./frontend/dist", webConfig);
    std::cout << "✓ Web assets MIME config applied to /assets/" << std::endl;
    
    // 3. Binary preset - for downloads and executables
    auto binaryConfig = webserver::MimeConfig::createBinary();
    server.serveStaticWithMime("/downloads/", "./downloads", binaryConfig);
    std::cout << "✓ Binary MIME config applied to /downloads/" << std::endl;
}

void demonstrateCustomMimeTypes(webserver::WebServer& server) {
    std::cout << "\n=== Custom MIME Types Demo ===" << std::endl;
    
    // Custom MIME configuration for API documentation
    auto apiDocsConfig = webserver::MimeConfig::createDefault();
    apiDocsConfig.extensionMap[".md"] = "text/markdown; charset=utf-8";
    apiDocsConfig.extensionMap[".swagger"] = "application/x-swagger+json";
    apiDocsConfig.extensionMap[".openapi"] = "application/vnd.oai.openapi+json";
    apiDocsConfig.extensionMap[".postman"] = "application/vnd.postman.collection+json";
    server.serveStaticWithMime("/docs/", "./api_docs", apiDocsConfig);
    std::cout << "✓ API documentation MIME types configured" << std::endl;
    
    // Custom configuration for data files
    auto dataConfig = webserver::MimeConfig::createDefault();
    dataConfig.extensionMap[".csv"] = "text/csv; charset=utf-8";
    dataConfig.extensionMap[".tsv"] = "text/tab-separated-values; charset=utf-8";
    dataConfig.extensionMap[".jsonl"] = "application/jsonlines";
    dataConfig.extensionMap[".ndjson"] = "application/x-ndjson";
    dataConfig.extensionMap[".parquet"] = "application/vnd.apache.parquet";
    dataConfig.extensionMap[".avro"] = "application/vnd.apache.avro+json";
    server.serveStaticWithMime("/data/", "./datasets", dataConfig);
    std::cout << "✓ Data file MIME types configured" << std::endl;
    
    // Custom configuration for development tools
    auto devConfig = webserver::MimeConfig::createDefault();
    devConfig.extensionMap[".dockerfile"] = "text/x-dockerfile; charset=utf-8";
    devConfig.extensionMap[".dockerignore"] = "text/plain; charset=utf-8";
    devConfig.extensionMap[".gitignore"] = "text/plain; charset=utf-8";
    devConfig.extensionMap[".gitattributes"] = "text/plain; charset=utf-8";
    devConfig.extensionMap[".editorconfig"] = "text/plain; charset=utf-8";
    devConfig.extensionMap[".eslintrc"] = "application/json; charset=utf-8";
    devConfig.extensionMap[".prettierrc"] = "application/json; charset=utf-8";
    devConfig.extensionMap[".babelrc"] = "application/json; charset=utf-8";
    server.serveStaticWithMime("/dev/", "./dev_files", devConfig);
    std::cout << "✓ Development tool MIME types configured" << std::endl;
}

void demonstrateStaticRouteOverrides(webserver::WebServer& server) {
    std::cout << "\n=== Static Route MIME Overrides Demo ===" << std::endl;
    
    // Create a static route with per-route MIME overrides
    webserver::StaticRoute mediaRoute;
    mediaRoute.urlPrefix = "/media/";
    mediaRoute.localPath = "./media_files";
    mediaRoute.defaultFile = "index.html";
    mediaRoute.allowDirectoryListing = true;
    mediaRoute.defaultMimeType = "application/octet-stream";
    
    // Override MIME types for specific extensions
    mediaRoute.mimeTypeOverrides[".m4v"] = "video/x-m4v";
    mediaRoute.mimeTypeOverrides[".webm"] = "video/webm";
    mediaRoute.mimeTypeOverrides[".ogv"] = "video/ogg";
    mediaRoute.mimeTypeOverrides[".flv"] = "video/x-flv";
    mediaRoute.mimeTypeOverrides[".avi"] = "video/x-msvideo";
    mediaRoute.mimeTypeOverrides[".mov"] = "video/quicktime";
    mediaRoute.mimeTypeOverrides[".wmv"] = "video/x-ms-wmv";
    
    // Audio formats
    mediaRoute.mimeTypeOverrides[".ogg"] = "audio/ogg";
    mediaRoute.mimeTypeOverrides[".wav"] = "audio/wav";
    mediaRoute.mimeTypeOverrides[".flac"] = "audio/flac";
    mediaRoute.mimeTypeOverrides[".aac"] = "audio/aac";
    mediaRoute.mimeTypeOverrides[".m4a"] = "audio/mp4";
    
    // Image formats
    mediaRoute.mimeTypeOverrides[".webp"] = "image/webp";
    mediaRoute.mimeTypeOverrides[".avif"] = "image/avif";
    mediaRoute.mimeTypeOverrides[".heic"] = "image/heic";
    mediaRoute.mimeTypeOverrides[".bmp"] = "image/bmp";
    mediaRoute.mimeTypeOverrides[".tiff"] = "image/tiff";
    
    server.serveStatic(mediaRoute);
    std::cout << "✓ Media files with custom MIME overrides configured" << std::endl;
}

void demonstrateIndividualFileServing(webserver::WebServer& server) {
    std::cout << "\n=== Individual File Serving Demo ===" << std::endl;
    
    // Serve specific files with explicit MIME types
    server.serveFile("/api/schema.json", "./api/openapi.json", "application/vnd.oai.openapi+json");
    std::cout << "✓ API schema served with OpenAPI MIME type" << std::endl;
    
    server.serveFile("/favicon.ico", "./assets/favicon.ico", "image/x-icon");
    std::cout << "✓ Favicon served with correct MIME type" << std::endl;
    
    server.serveFile("/robots.txt", "./seo/robots.txt", "text/plain; charset=utf-8");
    std::cout << "✓ Robots.txt served as plain text" << std::endl;
    
    server.serveFile("/sitemap.xml", "./seo/sitemap.xml", "application/xml; charset=utf-8");
    std::cout << "✓ Sitemap served as XML" << std::endl;
    
    server.serveFile("/manifest.json", "./pwa/manifest.json", "application/manifest+json");
    std::cout << "✓ PWA manifest served with correct MIME type" << std::endl;
    
    // Serve files without explicit MIME type (auto-detection)
    server.serveFile("/license", "./LICENSE");
    server.serveFile("/readme", "./README.md");
    std::cout << "✓ Files served with auto-detected MIME types" << std::endl;
}

void demonstrateGlobalMimeConfig(webserver::WebServer& server) {
    std::cout << "\n=== Global MIME Configuration Demo ===" << std::endl;
    
    // Set up a global MIME configuration that applies to all static serving
    auto globalConfig = webserver::MimeConfig::createWebAssets();
    
    // Add organization-specific extensions
    globalConfig.extensionMap[".company"] = "application/x-company-data";
    globalConfig.extensionMap[".internal"] = "text/plain; charset=utf-8";
    globalConfig.extensionMap[".template"] = "text/template; charset=utf-8";
    globalConfig.extensionMap[".snippet"] = "text/x-code-snippet; charset=utf-8";
    
    // Set a more specific default for unknown types
    globalConfig.defaultType = "text/plain; charset=utf-8";
    globalConfig.enableAutoDetection = true;
    
    server.setGlobalMimeConfig(globalConfig);
    std::cout << "✓ Global MIME configuration applied" << std::endl;
    
    // Now all subsequent static serving will use this global config
    server.serveStatic("/files/", "./various_files");
    std::cout << "✓ Static files will use global MIME configuration" << std::endl;
}

void demonstrateApiEndpointsWithMimeHandling(webserver::WebServer& server) {
    std::cout << "\n=== API Endpoints with MIME Handling Demo ===" << std::endl;
    
    // Endpoint that serves different content based on Accept header
    server.get("/api/data", [](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        auto acceptHeader = req.headers.find("Accept");
        
        if (acceptHeader != req.headers.end()) {
            const std::string& accept = acceptHeader->second;
            
            if (accept.find("application/json") != std::string::npos) {
                resp.setJsonResponse(R"({"data": "json format", "format": "application/json"})");
            } else if (accept.find("application/xml") != std::string::npos) {
                resp.setHeader("Content-Type", "application/xml; charset=utf-8");
                resp.body = "<?xml version=\"1.0\"?><data><format>application/xml</format></data>";
            } else if (accept.find("text/csv") != std::string::npos) {
                resp.setHeader("Content-Type", "text/csv; charset=utf-8");
                resp.body = "format,value\napplication/csv,csv format";
            } else if (accept.find("text/plain") != std::string::npos) {
                resp.setPlainTextResponse("data: plain text format");
            } else {
                // Default to JSON
                resp.setJsonResponse(R"({"data": "default json", "format": "application/json"})");
            }
        } else {
            // No Accept header, default to JSON
            resp.setJsonResponse(R"({"data": "no accept header", "format": "application/json"})");
        }
        
        resp.setHeader("X-Content-Negotiation", "enabled");
    });
    
    std::cout << "✓ Content negotiation endpoint configured" << std::endl;
    
    // File upload endpoint that validates MIME types
    server.post("/api/upload", [](const webserver::HttpRequest& req, webserver::HttpResponse& resp) {
        auto contentTypeHeader = req.headers.find("Content-Type");
        
        if (contentTypeHeader == req.headers.end()) {
            resp.statusCode = 400;
            resp.setJsonResponse(R"({"error": "Content-Type header required"})");
            return;
        }
        
        const std::string& contentType = contentTypeHeader->second;
        
        // Validate allowed MIME types
        if (contentType.find("image/") == 0) {
            // Image upload
            resp.setJsonResponse(R"({"message": "Image upload accepted", "type": "image"})");
        } else if (contentType.find("application/pdf") == 0) {
            // PDF upload
            resp.setJsonResponse(R"({"message": "PDF upload accepted", "type": "pdf"})");
        } else if (contentType.find("text/") == 0) {
            // Text file upload
            resp.setJsonResponse(R"({"message": "Text file upload accepted", "type": "text"})");
        } else if (contentType.find("application/json") == 0) {
            // JSON data upload
            resp.setJsonResponse(R"({"message": "JSON data upload accepted", "type": "json"})");
        } else {
            // Unsupported MIME type
            resp.statusCode = 415; // Unsupported Media Type
            resp.setJsonResponse(R"({"error": "Unsupported media type", "received": ")" + contentType + R"("})");
        }
    });
    
    std::cout << "✓ File upload validation endpoint configured" << std::endl;
}

int main() {
    std::cout << "WebServer MIME Type Configuration Example" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        webserver::WebServer server("127.0.0.1", 8080, webserver::WebServer::Backend::Mongoose);
        
        // Demonstrate different MIME configuration approaches
        demonstrateMimePresets(server);
        demonstrateCustomMimeTypes(server);
        demonstrateStaticRouteOverrides(server);
        demonstrateIndividualFileServing(server);
        demonstrateGlobalMimeConfig(server);
        demonstrateApiEndpointsWithMimeHandling(server);
        
        std::cout << "\n=== Server Ready ===" << std::endl;
        std::cout << "Server configured with comprehensive MIME type handling" << std::endl;
        std::cout << "\nTry these requests:" << std::endl;
        std::cout << "  curl -H 'Accept: application/json' http://localhost:8080/api/data" << std::endl;
        std::cout << "  curl -H 'Accept: application/xml' http://localhost:8080/api/data" << std::endl;
        std::cout << "  curl -H 'Accept: text/csv' http://localhost:8080/api/data" << std::endl;
        std::cout << "  curl -X POST -H 'Content-Type: image/png' http://localhost:8080/api/upload" << std::endl;
        std::cout << "  curl -X POST -H 'Content-Type: application/pdf' http://localhost:8080/api/upload" << std::endl;
        std::cout << "  curl -X POST -H 'Content-Type: video/mp4' http://localhost:8080/api/upload" << std::endl;
        
        // Note: In a real application, you would start the server here
        // server.start();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
