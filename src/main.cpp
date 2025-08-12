#include <iostream>
#include <memory>
#include <Logger/Logger.h>
#include <Logger/ILogger.h>
#include <IocContainer/IocContainer.h>

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
};

/**
 * Main application class that encapsulates the IoC container and dependencies
 */
class Application
{
  private:
	AppConfig _config;

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
	}

	~Application()
	{
		// Access logger through IoC container for cleanup
		auto& container = ioccontainer::IocContainer::getInstance();
		
		try {
			auto logger = container.resolve<logger::ILogger>();
			logger->logInfo("Application shutting down...");
			logger->stop();
		} catch (const ioccontainer::ServiceNotRegisteredException&) {
			// Logger might not be registered in some edge cases
			std::cout << "Application shutting down..." << std::endl;
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
		
		// Demonstrate different log levels
		logger->logDebug("This is a debug message");
		logger->logInfo("Application is running normally");
		logger->logWarning("This is a sample warning message");
		
		// Log the IoC container registry information using the logger
		logger->logInfo("=== IoC Container Registry ===");
		auto servicesInfo = container.getRegisteredServicesInfo();
		logger->logInfo(servicesInfo);
		
		// Demonstrate global logger access from a standalone function
		demonstrateGlobalLoggerAccess();
		
		// Demonstrate programmatic registry access
		demonstrateRegistryListing();
		
		// Keep running until user presses Enter
		std::cout << "Press Enter to exit..." << std::endl;
		std::cin.get();
		
		logger->logInfo("User requested application exit");
		return 0;
	}
};

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