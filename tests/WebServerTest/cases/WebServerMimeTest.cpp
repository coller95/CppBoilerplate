#include <gtest/gtest.h>
#include <WebServer/WebServer.h>

namespace {

class WebServerMimeTest : public ::testing::Test {
protected:
    void SetUp() override {
        server = std::make_unique<webserver::WebServer>("127.0.0.1", 8200);
    }

    void TearDown() override {
        if (server && server->isRunning()) {
            server->stop();
        }
    }
    
    std::unique_ptr<webserver::WebServer> server;
};

} // anonymous namespace

TEST_F(WebServerMimeTest, StaticRouteSupportsCustomMimeTypes) {
    webserver::StaticRoute route;
    route.urlPrefix = "/files/";
    route.localPath = "./test_files";
    route.defaultFile = "index.html";
    route.allowDirectoryListing = false;
    route.defaultMimeType = "text/plain";
    
    // Add custom MIME type overrides
    route.mimeTypeOverrides[".dat"] = "application/x-custom-data";
    route.mimeTypeOverrides[".config"] = "application/x-configuration";
    route.mimeTypeOverrides[".log"] = "text/plain; charset=utf-8";
    
    EXPECT_NO_THROW(server->serveStatic(route));
    
    // Verify the route structure
    EXPECT_EQ("/files/", route.urlPrefix);
    EXPECT_EQ("./test_files", route.localPath);
    EXPECT_EQ("text/plain", route.defaultMimeType);
    EXPECT_EQ("application/x-custom-data", route.mimeTypeOverrides[".dat"]);
    EXPECT_EQ("application/x-configuration", route.mimeTypeOverrides[".config"]);
    EXPECT_EQ("text/plain; charset=utf-8", route.mimeTypeOverrides[".log"]);
}

TEST_F(WebServerMimeTest, MimeConfigDefaultPreset) {
    auto defaultConfig = webserver::MimeConfig::createDefault();
    
    // Test common web file types
    EXPECT_EQ("text/html; charset=utf-8", defaultConfig.extensionMap[".html"]);
    EXPECT_EQ("text/html; charset=utf-8", defaultConfig.extensionMap[".htm"]);
    EXPECT_EQ("text/css; charset=utf-8", defaultConfig.extensionMap[".css"]);
    EXPECT_EQ("application/javascript; charset=utf-8", defaultConfig.extensionMap[".js"]);
    EXPECT_EQ("application/json; charset=utf-8", defaultConfig.extensionMap[".json"]);
    
    // Test image types
    EXPECT_EQ("image/png", defaultConfig.extensionMap[".png"]);
    EXPECT_EQ("image/jpeg", defaultConfig.extensionMap[".jpg"]);
    EXPECT_EQ("image/jpeg", defaultConfig.extensionMap[".jpeg"]);
    EXPECT_EQ("image/gif", defaultConfig.extensionMap[".gif"]);
    EXPECT_EQ("image/svg+xml", defaultConfig.extensionMap[".svg"]);
    
    // Test other common types
    EXPECT_EQ("application/pdf", defaultConfig.extensionMap[".pdf"]);
    EXPECT_EQ("application/zip", defaultConfig.extensionMap[".zip"]);
    EXPECT_EQ("video/mp4", defaultConfig.extensionMap[".mp4"]);
    EXPECT_EQ("audio/mpeg", defaultConfig.extensionMap[".mp3"]);
    
    EXPECT_EQ("application/octet-stream", defaultConfig.defaultType);
    EXPECT_TRUE(defaultConfig.enableAutoDetection);
}

TEST_F(WebServerMimeTest, MimeConfigWebAssetsPreset) {
    auto webConfig = webserver::MimeConfig::createWebAssets();
    
    // Test web development file types
    EXPECT_EQ("text/html; charset=utf-8", webConfig.extensionMap[".html"]);
    EXPECT_EQ("text/css; charset=utf-8", webConfig.extensionMap[".css"]);
    EXPECT_EQ("application/javascript; charset=utf-8", webConfig.extensionMap[".js"]);
    EXPECT_EQ("application/typescript; charset=utf-8", webConfig.extensionMap[".ts"]);
    EXPECT_EQ("text/javascript; charset=utf-8", webConfig.extensionMap[".jsx"]);
    EXPECT_EQ("text/vue; charset=utf-8", webConfig.extensionMap[".vue"]);
    
    // Test CSS preprocessors
    EXPECT_EQ("text/scss; charset=utf-8", webConfig.extensionMap[".scss"]);
    EXPECT_EQ("text/sass; charset=utf-8", webConfig.extensionMap[".sass"]);
    EXPECT_EQ("text/less; charset=utf-8", webConfig.extensionMap[".less"]);
    
    // Test font types
    EXPECT_EQ("font/woff", webConfig.extensionMap[".woff"]);
    EXPECT_EQ("font/woff2", webConfig.extensionMap[".woff2"]);
    EXPECT_EQ("font/ttf", webConfig.extensionMap[".ttf"]);
    EXPECT_EQ("application/vnd.ms-fontobject", webConfig.extensionMap[".eot"]);
    
    // Test image types including modern formats
    EXPECT_EQ("image/webp", webConfig.extensionMap[".webp"]);
    EXPECT_EQ("image/svg+xml", webConfig.extensionMap[".svg"]);
    
    EXPECT_EQ("text/plain; charset=utf-8", webConfig.defaultType);
    EXPECT_TRUE(webConfig.enableAutoDetection);
}

TEST_F(WebServerMimeTest, MimeConfigBinaryPreset) {
    auto binaryConfig = webserver::MimeConfig::createBinary();
    
    // Test executable file types
    EXPECT_EQ("application/octet-stream", binaryConfig.extensionMap[".bin"]);
    EXPECT_EQ("application/octet-stream", binaryConfig.extensionMap[".exe"]);
    EXPECT_EQ("application/octet-stream", binaryConfig.extensionMap[".dll"]);
    EXPECT_EQ("application/octet-stream", binaryConfig.extensionMap[".so"]);
    EXPECT_EQ("application/octet-stream", binaryConfig.extensionMap[".dylib"]);
    
    // Test archive types
    EXPECT_EQ("application/zip", binaryConfig.extensionMap[".zip"]);
    EXPECT_EQ("application/x-tar", binaryConfig.extensionMap[".tar"]);
    EXPECT_EQ("application/gzip", binaryConfig.extensionMap[".gz"]);
    EXPECT_EQ("application/x-7z-compressed", binaryConfig.extensionMap[".7z"]);
    EXPECT_EQ("application/vnd.rar", binaryConfig.extensionMap[".rar"]);
    
    // Test package types
    EXPECT_EQ("application/vnd.debian.binary-package", binaryConfig.extensionMap[".deb"]);
    EXPECT_EQ("application/x-rpm", binaryConfig.extensionMap[".rpm"]);
    EXPECT_EQ("application/x-apple-diskimage", binaryConfig.extensionMap[".dmg"]);
    EXPECT_EQ("application/x-iso9660-image", binaryConfig.extensionMap[".iso"]);
    
    EXPECT_EQ("application/octet-stream", binaryConfig.defaultType);
    EXPECT_FALSE(binaryConfig.enableAutoDetection); // Binary files shouldn't be auto-detected
}

TEST_F(WebServerMimeTest, ServeStaticWithCustomMimeConfig) {
    auto customConfig = webserver::MimeConfig::createDefault();
    
    // Add custom MIME types
    customConfig.extensionMap[".myext"] = "application/x-my-custom-type";
    customConfig.extensionMap[".data"] = "application/x-binary-data";
    customConfig.defaultType = "text/plain";
    
    EXPECT_NO_THROW(server->serveStaticWithMime("/custom/", "./custom_files", customConfig));
}

TEST_F(WebServerMimeTest, ServeIndividualFileWithMimeType) {
    // Serve a specific file with explicit MIME type
    EXPECT_NO_THROW(server->serveFile("/download/report.pdf", "./reports/monthly.pdf", "application/pdf"));
    EXPECT_NO_THROW(server->serveFile("/api/schema", "./api/schema.json", "application/json"));
    EXPECT_NO_THROW(server->serveFile("/docs/readme", "./README.md", "text/markdown; charset=utf-8"));
    
    // Serve file without explicit MIME type (should auto-detect)
    EXPECT_NO_THROW(server->serveFile("/auto/detect", "./unknown.file"));
}

TEST_F(WebServerMimeTest, SetGlobalMimeConfiguration) {
    auto globalConfig = webserver::MimeConfig::createWebAssets();
    
    // Add some global overrides
    globalConfig.extensionMap[".component"] = "text/html; charset=utf-8";
    globalConfig.extensionMap[".module"] = "application/javascript; charset=utf-8";
    globalConfig.defaultType = "application/octet-stream";
    
    EXPECT_NO_THROW(server->setGlobalMimeConfig(globalConfig));
    
    // After setting global config, all static serving should use these settings
    EXPECT_NO_THROW(server->serveStatic("/assets/", "./assets"));
}

TEST_F(WebServerMimeTest, CustomMimeTypesForSpecialFiles) {
    webserver::StaticRoute route;
    route.urlPrefix = "/special/";
    route.localPath = "./special_files";
    route.defaultMimeType = "application/octet-stream";
    
    // Custom MIME types for special use cases
    route.mimeTypeOverrides[".dockerfile"] = "text/x-dockerfile; charset=utf-8";
    route.mimeTypeOverrides[".jenkinsfile"] = "text/x-groovy; charset=utf-8";
    route.mimeTypeOverrides[".makefile"] = "text/x-makefile; charset=utf-8";
    route.mimeTypeOverrides[".cmake"] = "text/x-cmake; charset=utf-8";
    route.mimeTypeOverrides[".yaml"] = "application/x-yaml; charset=utf-8";
    route.mimeTypeOverrides[".yml"] = "application/x-yaml; charset=utf-8";
    route.mimeTypeOverrides[".toml"] = "application/toml; charset=utf-8";
    route.mimeTypeOverrides[".ini"] = "text/plain; charset=utf-8";
    route.mimeTypeOverrides[".env"] = "text/plain; charset=utf-8";
    route.mimeTypeOverrides[".gitignore"] = "text/plain; charset=utf-8";
    
    EXPECT_NO_THROW(server->serveStatic(route));
    
    // Verify all custom MIME types are set
    EXPECT_EQ("text/x-dockerfile; charset=utf-8", route.mimeTypeOverrides[".dockerfile"]);
    EXPECT_EQ("text/x-groovy; charset=utf-8", route.mimeTypeOverrides[".jenkinsfile"]);
    EXPECT_EQ("application/x-yaml; charset=utf-8", route.mimeTypeOverrides[".yaml"]);
    EXPECT_EQ("application/toml; charset=utf-8", route.mimeTypeOverrides[".toml"]);
}

TEST_F(WebServerMimeTest, MimeConfigCanBeCustomized) {
    // Start with default config and customize it
    auto config = webserver::MimeConfig::createDefault();
    
    // Override some defaults
    config.extensionMap[".txt"] = "text/plain; charset=iso-8859-1";  // Different charset
    config.extensionMap[".log"] = "text/plain; charset=utf-8";
    config.extensionMap[".csv"] = "text/csv; charset=utf-8";
    config.extensionMap[".tsv"] = "text/tab-separated-values; charset=utf-8";
    
    // Add completely new types
    config.extensionMap[".proto"] = "application/x-protobuf";
    config.extensionMap[".avro"] = "application/avro";
    config.extensionMap[".parquet"] = "application/x-parquet";
    
    // Change defaults
    config.defaultType = "text/plain; charset=utf-8";
    config.enableAutoDetection = false;
    
    EXPECT_NO_THROW(server->serveStaticWithMime("/data/", "./data_files", config));
    
    // Verify customizations
    EXPECT_EQ("text/plain; charset=iso-8859-1", config.extensionMap[".txt"]);
    EXPECT_EQ("application/x-protobuf", config.extensionMap[".proto"]);
    EXPECT_EQ("text/plain; charset=utf-8", config.defaultType);
    EXPECT_FALSE(config.enableAutoDetection);
}
