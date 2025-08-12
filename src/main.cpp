#include <iostream>
#include <memory>
#include <ctime>
#include <Logger/Logger.h>
#include <Logger/ILogger.h>
#include <IocContainer/IocContainer.h>
#include <WebServer/WebServer.h>
#include <WebServer/IWebServer.h>
#include <ApiRouter/ApiRouter.h>

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
		// Initialize IoC container and register core services
		auto& container = ioccontainer::IocContainer::getInstance();
		
		// Create and register logger service
		auto logger = std::make_shared<logger::Logger>(_config.loggerIp, _config.loggerPort);
		logger->setLocalDisplay(true);
		container.registerInstance<logger::ILogger>(logger);
		
		// Log application startup
		auto globalLogger = container.resolve<logger::ILogger>();
		globalLogger->logInfo("Application starting up...");
		
		// Create and register WebServer
		_webServer = std::make_unique<webserver::WebServer>(
			_config.webServerIp, 
			_config.webServerPort, 
			_config.webServerBackend
		);
		
		// Register WebServer in IoC container (non-owning shared_ptr)
		std::shared_ptr<webserver::IWebServer> webServerPtr(_webServer.get(), [](webserver::IWebServer*){});
		container.registerInstance<webserver::IWebServer>(webServerPtr);
		
		globalLogger->logInfo("Services registered in IoC container");
		globalLogger->logInfo("WebServer configured for " + _config.webServerIp + ":" + std::to_string(_config.webServerPort));
		
		// Initialize ApiRouter and setup routes
		apirouter::ApiRouter::initializeGlobal();
		setupWebServerRoutes();
		
		globalLogger->logInfo("Application initialization complete");
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
		auto& container = ioccontainer::IocContainer::getInstance();
		auto logger = container.resolve<logger::ILogger>();
		
		// Start logger connection
		if (!logger->start()) {
			logger->logWarning("Remote logging unavailable, using local display only");
		}
		
		// Start the WebServer
		logger->logInfo("Starting WebServer...");
		if (!_webServer->start()) {
			logger->logError("Failed to start WebServer");
			return 1;
		}
		
		// Log system status
		logger->logInfo("WebServer started successfully on " + _webServer->getBindAddress() + ":" + std::to_string(_webServer->getPort()));
		logger->logInfo("Backend: " + _webServer->getBackendName());
		
		// Log registered services and endpoints
		logger->logInfo("IoC Container: " + std::to_string(container.getRegisteredCount()) + " services registered");
		
		// Log detailed service information
		auto typeNames = container.getRegisteredTypeNames();
		logger->logInfo("Registered services:");
		for (size_t i = 0; i < typeNames.size(); ++i) {
			std::string typeName = typeNames[i];
			std::string serviceName = typeName;
			std::string category = "unknown";
			
			// Parse and categorize service types
			if (typeName.find("Logger") != std::string::npos || typeName.find("ILogger") != std::string::npos) {
				serviceName = "Logger";
				category = "logging";
			} else if (typeName.find("WebServer") != std::string::npos || typeName.find("IWebServer") != std::string::npos) {
				serviceName = "WebServer";
				category = "infrastructure";
			} else if (typeName.find("ApiRouter") != std::string::npos || typeName.find("IApiRouter") != std::string::npos) {
				serviceName = "ApiRouter";
				category = "routing";
			} else if (typeName.find("Service") != std::string::npos) {
				category = "business";
			} else if (typeName.find("Manager") != std::string::npos || typeName.find("Handler") != std::string::npos) {
				category = "management";
			}
			
			logger->logInfo("  " + std::to_string(i + 1) + ". " + serviceName + " (" + category + ")");
		}
		
		size_t endpointCount = apirouter::ApiRouter::getEndpointCountGlobal();
		size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();
		logger->logInfo("ApiRouter: " + std::to_string(endpointCount) + " endpoints from " + std::to_string(moduleCount) + " modules");
		
		// Log detailed endpoint information
		auto endpoints = apirouter::ApiRouter::getRegisteredEndpointsGlobal();
		logger->logInfo("Registered endpoints:");
		for (size_t i = 0; i < endpoints.size(); ++i) {
			std::string endpoint = endpoints[i];
			size_t colonPos = endpoint.find(':');
			std::string path = (colonPos != std::string::npos) ? endpoint.substr(0, colonPos) : endpoint;
			std::string method = (colonPos != std::string::npos) ? endpoint.substr(colonPos + 1) : "ALL";
			
			std::string type = "custom";
			if (path == "/") type = "homepage";
			else if (path == "/api/status" || path == "/api/endpoints") type = "system";
			else if (path == "/hello") type = "module";
			else if (path.find("/api/") == 0) type = "api";
			
			logger->logInfo("  " + std::to_string(i + 1) + ". " + method + " " + path + " (" + type + ")");
		}
		
		// Wait for shutdown signal
		std::cout << "Server running at http://" << _webServer->getBindAddress() << ":" << _webServer->getPort() << std::endl;
		std::cout << "Press Enter to stop..." << std::endl;
		std::cin.get();
		
		logger->logInfo("Shutdown requested");
		return 0;
	}
};

/**
 * Setup WebServer routes and handlers
 */
void Application::setupWebServerRoutes() {
	auto& container = ioccontainer::IocContainer::getInstance();
	auto logger = container.resolve<logger::ILogger>();
	auto& apiRouter = apirouter::ApiRouter::getInstance();
	
	// Register core API endpoints through ApiRouter
	apiRouter.registerHttpHandler("/", "GET", 
		[logger](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
			responseBody = 
				"<!DOCTYPE html>"
				"<html><head><title>CppBoilerplate Server</title></head>"
				"<body>"
				"<h1>CppBoilerplate WebServer</h1>"
				"<p>Modular C++ web server with dependency injection.</p>"
				"<ul>"
				"<li><a href=\"/api/status\">Server Status</a></li>"
				"<li><a href=\"/api/endpoints\">API Endpoints</a></li>"
				"<li><a href=\"/api/services\">Registered Services</a></li>"
				"<li><a href=\"/hello\">Hello Endpoint</a></li>"
				"</ul>"
				"</body></html>";
			statusCode = 200;
		});
	
	apiRouter.registerHttpHandler("/api/status", "GET",
		[this, &container](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
			size_t endpointCount = apirouter::ApiRouter::getEndpointCountGlobal();
			size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();
			
			responseBody = 
				"{"
				"\"status\": \"running\","
				"\"server\": \"CppBoilerplate\","
				"\"backend\": \"" + _webServer->getBackendName() + "\","
				"\"address\": \"" + _webServer->getBindAddress() + "\","
				"\"port\": " + std::to_string(_webServer->getPort()) + ","
				"\"services\": " + std::to_string(container.getRegisteredCount()) + ","
				"\"endpoints\": " + std::to_string(endpointCount) + ","
				"\"modules\": " + std::to_string(moduleCount) +
				"}";
			statusCode = 200;
		});
	
	apiRouter.registerHttpHandler("/api/services", "GET",
		[&container](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
			auto typeNames = container.getRegisteredTypeNames();
			size_t serviceCount = container.getRegisteredCount();
			
			std::string serviceDetails = "[";
			for (size_t i = 0; i < typeNames.size(); ++i) {
				if (i > 0) serviceDetails += ",";
				
				std::string typeName = typeNames[i];
				std::string serviceName = typeName;
				std::string category = "unknown";
				std::string description = "Registered service";
				
				// Parse and categorize service types
				if (typeName.find("Logger") != std::string::npos || typeName.find("ILogger") != std::string::npos) {
					serviceName = "Logger";
					category = "logging";
					description = "Application logging service with remote and local display capabilities";
				} else if (typeName.find("WebServer") != std::string::npos || typeName.find("IWebServer") != std::string::npos) {
					serviceName = "WebServer";
					category = "infrastructure";
					description = "HTTP web server with Mongoose backend";
				} else if (typeName.find("ApiRouter") != std::string::npos || typeName.find("IApiRouter") != std::string::npos) {
					serviceName = "ApiRouter";
					category = "routing";
					description = "API routing and endpoint management service";
				} else if (typeName.find("Service") != std::string::npos) {
					category = "business";
					description = "Business logic service";
				} else if (typeName.find("Manager") != std::string::npos || typeName.find("Handler") != std::string::npos) {
					category = "management";
					description = "System management service";
				}
				
				serviceDetails += 
					"{"
					"\"id\": " + std::to_string(i + 1) + ","
					"\"name\": \"" + serviceName + "\","
					"\"typeName\": \"" + typeName + "\","
					"\"category\": \"" + category + "\","
					"\"description\": \"" + description + "\""
					"}";
			}
			serviceDetails += "]";
			
			responseBody = 
				"{"
				"\"summary\": {"
				"\"totalServices\": " + std::to_string(serviceCount) + ","
				"\"timestamp\": \"" + std::to_string(std::time(nullptr)) + "\""
				"},"
				"\"services\": " + serviceDetails + ","
				"\"categories\": {"
				"\"logging\": \"Logging and monitoring services\","
				"\"infrastructure\": \"Core infrastructure services\","
				"\"routing\": \"Request routing and API management\","
				"\"business\": \"Business logic and domain services\","
				"\"management\": \"System and resource management\","
				"\"unknown\": \"Unclassified services\""
				"}"
				"}";
			statusCode = 200;
		});

	apiRouter.registerHttpHandler("/api/endpoints", "GET",
		[](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
			auto endpoints = apirouter::ApiRouter::getRegisteredEndpointsGlobal();
			size_t endpointCount = apirouter::ApiRouter::getEndpointCountGlobal();
			size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();
			
			std::string endpointDetails = "[";
			for (size_t i = 0; i < endpoints.size(); ++i) {
				if (i > 0) endpointDetails += ",";
				
				// Parse endpoint format "path:method"
				std::string endpoint = endpoints[i];
				size_t colonPos = endpoint.find(':');
				std::string path = (colonPos != std::string::npos) ? endpoint.substr(0, colonPos) : endpoint;
				std::string method = (colonPos != std::string::npos) ? endpoint.substr(colonPos + 1) : "ALL";
				
				// Determine endpoint type based on path
				std::string type = "custom";
				std::string description = "Custom endpoint";
				if (path == "/") {
					type = "homepage";
					description = "Application homepage with navigation";
				} else if (path == "/api/status") {
					type = "system";
					description = "Server status and statistics";
				} else if (path == "/api/endpoints") {
					type = "system";
					description = "API endpoint listing and details";
				} else if (path == "/hello") {
					type = "module";
					description = "Auto-registered endpoint from EndpointHello module";
				} else if (path.find("/api/") == 0) {
					type = "api";
					description = "API endpoint";
				}
				
				endpointDetails += 
					"{"
					"\"id\": " + std::to_string(i + 1) + ","
					"\"path\": \"" + path + "\","
					"\"method\": \"" + method + "\","
					"\"type\": \"" + type + "\","
					"\"description\": \"" + description + "\","
					"\"fullEndpoint\": \"" + endpoint + "\""
					"}";
			}
			endpointDetails += "]";
			
			responseBody = 
				"{"
				"\"summary\": {"
				"\"totalEndpoints\": " + std::to_string(endpointCount) + ","
				"\"totalModules\": " + std::to_string(moduleCount) + ","
				"\"timestamp\": \"" + std::to_string(std::time(nullptr)) + "\""
				"},"
				"\"endpoints\": " + endpointDetails + ","
				"\"categories\": {"
				"\"homepage\": \"Application entry point\","
				"\"system\": \"Built-in system endpoints\","
				"\"api\": \"Custom API endpoints\","
				"\"module\": \"Auto-registered module endpoints\","
				"\"custom\": \"Other custom endpoints\""
				"}"
				"}";
			statusCode = 200;
		});
	
	// Register a catch-all handler with WebServer that delegates to ApiRouter
	_webServer->registerHandler("*", [&apiRouter, logger](const webserver::HttpRequest& request, webserver::HttpResponse& response) {
		std::string responseBody;
		int statusCode = 404;
		
		// Try to handle the request through ApiRouter
		bool handled = apiRouter.handleRequest(
			request.uri, request.method, request.body, responseBody, statusCode
		);
		
		if (handled) {
			// Set the response based on content type detection
			if (responseBody.find("<!DOCTYPE html>") != std::string::npos || 
				responseBody.find("<html>") != std::string::npos) {
				response.setHtmlResponse(responseBody);
			} else if (responseBody.find("{") != std::string::npos && 
					   responseBody.find("}") != std::string::npos) {
				response.setJsonResponse(responseBody);
			} else {
				response.setPlainTextResponse(responseBody);
			}
			response.statusCode = statusCode;
		} else {
			// Default 404 response
			response.statusCode = 404;
			response.setJsonResponse("{\"error\": \"Not Found\", \"path\": \"" + request.uri + "\"}");
		}
		
		logger->logInfo("Request: " + request.method + " " + request.uri + " -> " + std::to_string(response.statusCode));
	});
	
	logger->logInfo("WebServer integrated with ApiRouter");
}

/**
 * Main entry point for the application
 */
int main()
{
	Application app;
	return app.run();
}