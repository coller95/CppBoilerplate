#include <iostream>
#include <memory>
#include <Logger/Logger.h>
#include <Logger/ILogger.h>
#include <IocContainer/IocContainer.h>
#include <WebServer/WebServer.h>
#include <WebServer/IWebServer.h>

// Forward declarations
void demonstrateGlobalLoggerAccess();
void demonstrateRegistryListing();

/**
 * Application configuration struct (local to this file for convenience)
 */
struct AppConfig
{
	std::string loggerIp = "127.0.0.1";
	int loggerPort = 9000;
	std::string webServerIp = "127.0.0.1";
	int webServerPort = 8080;
	webserver::WebServer::Backend webServerBackend = webserver::WebServer::Backend::Mongoose;
};

/**
 * Main application class that encapsulates the IoC container and dependencies
 */
class Application
{
  private:
	AppConfig _config;
	std::unique_ptr<webserver::WebServer> _webServer;
	
	/**
	 * Setup WebServer routes and handlers
	 */
	void setupWebServerRoutes();

  public:
	Application() 
	{
		// Initialize IoC container with Logger service
		auto& container = ioccontainer::IocContainer::getInstance();
		
		// Create logger instance
		auto logger = std::make_shared<logger::Logger>(_config.loggerIp, _config.loggerPort);
		
		// Configure logger
		logger->setLocalDisplay(true);
		
		// Register logger in IoC container for global access
		container.registerInstance<logger::ILogger>(logger);
		
		// Log application startup using the global logger
		auto globalLogger = container.resolve<logger::ILogger>();
		globalLogger->logInfo("Application starting up...");
		globalLogger->logInfo("Logger configured for " + _config.loggerIp + ":" + std::to_string(_config.loggerPort));
		
		// Create and configure WebServer
		_webServer = std::make_unique<webserver::WebServer>(
			_config.webServerIp, 
			_config.webServerPort, 
			_config.webServerBackend
		);
		
		// Create a shared_ptr wrapper for IoC registration (non-owning)
		// Note: The unique_ptr maintains ownership, this is just for IoC access
		std::shared_ptr<webserver::IWebServer> webServerPtr(_webServer.get(), [](webserver::IWebServer*){});
		container.registerInstance<webserver::IWebServer>(webServerPtr);
		
		globalLogger->logInfo("WebServer configured for " + _config.webServerIp + ":" + std::to_string(_config.webServerPort));
		globalLogger->logInfo("WebServer backend: " + _webServer->getBackendName());
		
		// Setup basic routes
		setupWebServerRoutes();
	}

	~Application()
	{
		// Access logger through IoC container for cleanup
		auto& container = ioccontainer::IocContainer::getInstance();
		
		try {
			auto logger = container.resolve<logger::ILogger>();
			logger->logInfo("Application shutting down...");
			
			// Stop WebServer if running
			if (_webServer && _webServer->isRunning()) {
				logger->logInfo("Stopping WebServer...");
				_webServer->stop();
				logger->logInfo("WebServer stopped");
			}
			
			logger->stop();
		} catch (const ioccontainer::ServiceNotRegisteredException&) {
			// Logger might not be registered in some edge cases
			std::cout << "Application shutting down..." << std::endl;
			
			// Still try to stop WebServer
			if (_webServer && _webServer->isRunning()) {
				std::cout << "Stopping WebServer..." << std::endl;
				_webServer->stop();
			}
		}
	}

	/**
	 * Runs the main application logic.
	 * @return Exit code (0 for success)
	 */
	int run()
	{
		// Access logger through IoC container for global logging
		auto& container = ioccontainer::IocContainer::getInstance();
		auto logger = container.resolve<logger::ILogger>();
		
		logger->logInfo("Application main loop started");
		
		// Try to connect to remote logging server
		if (logger->start()) {
			logger->logInfo("Connected to remote logging server");
		} else {
			logger->logWarning("Could not connect to remote logging server, using local display only");
		}
		
		// Start the WebServer
		logger->logInfo("Starting WebServer...");
		if (_webServer->start()) {
			logger->logInfo("WebServer started successfully");
			logger->logInfo("WebServer listening on " + _webServer->getBindAddress() + ":" + std::to_string(_webServer->getPort()));
			logger->logInfo("WebServer backend: " + _webServer->getBackendName());
		} else {
			logger->logError("Failed to start WebServer");
			return 1; // Exit with error code
		}
		
		// Demonstrate different log levels
		logger->logDebug("This is a debug message");
		logger->logInfo("Application is running normally");
		logger->logWarning("This is a sample warning message");
		
		// Demonstrate printf-style logging
		logger->log("Printf-style logging: User %s has %d points and %.2f%% completion", "Alice", 1250, 87.5);
		logger->log("System status: Memory usage %d%%, CPU %d%%, Disk %.1fGB free", 67, 23, 45.2);
		logger->log("Processing file #%d of %d: %s", 7, 10, "data_file.csv");
		
		// Log the IoC container registry information using the logger
		logger->logInfo("=== IoC Container Registry ===");
		auto servicesInfo = container.getRegisteredServicesInfo();
		logger->logInfo(servicesInfo);
		
		// Demonstrate global logger access from a standalone function
		demonstrateGlobalLoggerAccess();
		
		// Demonstrate programmatic registry access
		demonstrateRegistryListing();
		
		// Show WebServer status
		logger->logInfo("=== WebServer Status ===");
		logger->logInfo("WebServer is " + std::string(_webServer->isRunning() ? "running" : "stopped"));
		logger->logInfo("Active connections: " + std::to_string(_webServer->getActiveConnections()));
		
		// Keep running until user presses Enter
		std::cout << "WebServer is running. Visit http://" << _webServer->getBindAddress() << ":" << _webServer->getPort() << std::endl;
		std::cout << "Press Enter to exit..." << std::endl;
		std::cin.get();
		
		logger->logInfo("User requested application exit");
		return 0;
	}
};

/**
 * Setup WebServer routes and handlers
 */
void Application::setupWebServerRoutes() {
	auto& container = ioccontainer::IocContainer::getInstance();
	auto logger = container.resolve<logger::ILogger>();
	
	// Basic routes
	_webServer->get("/", [logger](const webserver::HttpRequest& request, webserver::HttpResponse& response) {
		logger->logInfo("GET / - Homepage requested from " + request.remoteIp);
		response.setHtmlResponse(
			"<!DOCTYPE html>"
			"<html><head><title>CppBoilerplate WebServer</title></head>"
			"<body>"
			"<h1>Welcome to CppBoilerplate WebServer!</h1>"
			"<p>This is a C++ web server with modular architecture.</p>"
			"<ul>"
			"<li><a href=\"/api/status\">Server Status</a></li>"
			"<li><a href=\"/api/info\">Server Info</a></li>"
			"<li><a href=\"/api/logs\">Recent Logs</a></li>"
			"</ul>"
			"</body></html>"
		);
	});
	
	_webServer->get("/api/status", [this, logger](const webserver::HttpRequest& request, webserver::HttpResponse& response) {
		logger->logInfo("GET /api/status - Status requested from " + request.remoteIp);
		response.setJsonResponse(
			"{"
			"\"status\": \"running\","
			"\"server\": \"CppBoilerplate WebServer\","
			"\"backend\": \"" + _webServer->getBackendName() + "\","
			"\"address\": \"" + _webServer->getBindAddress() + "\","
			"\"port\": " + std::to_string(_webServer->getPort()) + ","
			"\"activeConnections\": " + std::to_string(_webServer->getActiveConnections()) +
			"}"
		);
	});
	
	_webServer->get("/api/info", [this, logger](const webserver::HttpRequest& request, webserver::HttpResponse& response) {
		logger->logInfo("GET /api/info - Info requested from " + request.remoteIp);
		
		auto& container = ioccontainer::IocContainer::getInstance();
		auto servicesInfo = container.getRegisteredServicesInfo();
		
		response.setJsonResponse(
			"{"
			"\"application\": \"CppBoilerplate\","
			"\"webserver\": {"
			"\"backend\": \"" + _webServer->getBackendName() + "\","
			"\"address\": \"" + _webServer->getBindAddress() + "\","
			"\"port\": " + std::to_string(_webServer->getPort()) + ","
			"\"activeConnections\": " + std::to_string(_webServer->getActiveConnections()) +
			"},"
			"\"iocContainer\": {"
			"\"registeredServices\": " + std::to_string(container.getRegisteredCount()) +
			"}"
			"}"
		);
	});
	
	_webServer->get("/api/logs", [logger](const webserver::HttpRequest& request, webserver::HttpResponse& response) {
		logger->logInfo("GET /api/logs - Logs requested from " + request.remoteIp);
		response.setJsonResponse(
			"{"
			"\"message\": \"Log endpoint accessed\","
			"\"note\": \"Detailed logging implementation would go here\","
			"\"loggerConnected\": " + std::string(logger->isConnected() ? "true" : "false") + ","
			"\"localDisplay\": " + std::string(logger->getLocalDisplay() ? "true" : "false") +
			"}"
		);
	});
	
	// POST example for testing
	_webServer->post("/api/echo", [logger](const webserver::HttpRequest& request, webserver::HttpResponse& response) {
		logger->logInfo("POST /api/echo - Echo requested from " + request.remoteIp);
		response.setJsonResponse(
			"{"
			"\"method\": \"" + request.method + "\","
			"\"uri\": \"" + request.uri + "\","
			"\"body\": \"" + request.body + "\","
			"\"contentLength\": " + std::to_string(request.body.length()) +
			"}"
		);
	});
	
	logger->logInfo("WebServer routes configured successfully");
}

/**
 * Example function that demonstrates global logger access from anywhere in the application
 */
void demonstrateGlobalLoggerAccess() {
    // This function can be called from anywhere and will have access to the logger
    auto& container = ioccontainer::IocContainer::getInstance();
    
    try {
        auto logger = container.resolve<logger::ILogger>();
        logger->logInfo("This message was logged from a standalone function using global logger access!");
        logger->logDebug("Demonstrating that the logger is accessible throughout the entire application");
        
        // Demonstrate printf-style logging from a global function
        logger->log("Global function printf-style: timestamp %ld, thread_id %d", 1234567890L, 42);
        logger->log("From global context: %s operation completed in %.3f seconds", "Database", 2.567);
    } catch (const ioccontainer::ServiceNotRegisteredException& e) {
        std::cout << "Logger is not available globally: " << e.what() << std::endl;
    }
}

/**
 * Function that demonstrates programmatic access to IoC registry information
 */
void demonstrateRegistryListing() {
    auto& container = ioccontainer::IocContainer::getInstance();
    
    // Get logger for all output
    auto logger = container.resolve<logger::ILogger>();
    
    logger->logInfo("=== Programmatic IoC Registry Access ===");
    
    // Get list of registered type names
    auto typeNames = container.getRegisteredTypeNames();
    logger->logInfo("Number of registered services: " + std::to_string(container.getRegisteredCount()));
    
    logger->logInfo("Registered type names:");
    for (size_t i = 0; i < typeNames.size(); ++i) {
        logger->logInfo("  " + std::to_string(i + 1) + ". " + typeNames[i]);
    }
    
    // Log registry summary
    logger->logInfo("IoC Registry contains " + std::to_string(typeNames.size()) + " registered service(s)");
    logger->logInfo("=== End Registry Access ===");
}

/**
 * Main entry point for the application
 *
 * This function initializes and runs the application.
 */

int main()
{
	Application app;
	return app.run();
}