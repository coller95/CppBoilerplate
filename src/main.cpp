#include <iostream>
#include <memory>
#include <Logger/Logger.h>

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
	std::unique_ptr<logger::Logger> _logger;

  public:
	Application() 
	{
		// Initialize Logger with dependency injection
		_logger = std::make_unique<logger::Logger>(_config.loggerIp, _config.loggerPort);
		
		// Configure logger
		_logger->setLocalDisplay(true);
		
		// Log application startup
		_logger->logInfo("Application starting up...");
		_logger->log("Logger configured for %s:%d", _config.loggerIp.c_str(), _config.loggerPort);
	}

	~Application()
	{
		if (_logger) {
			_logger->logInfo("Application shutting down...");
			_logger->stop();
		}
	}

	/**
	 * Runs the main application logic.
	 * @return Exit code (0 for success)
	 */
	int run()
	{
		_logger->logInfo("Application main loop started");
		
		// Try to connect to remote logging server
		if (_logger->start()) {
			_logger->logInfo("Connected to remote logging server");
		} else {
			_logger->logWarning("Could not connect to remote logging server, using local display only");
		}
		
		// Demonstrate different log levels
		_logger->logDebug("This is a debug message");
		_logger->logInfo("Application is running normally");
		_logger->logWarning("This is a sample warning message");
		

		
		_logger->logInfo("User requested application exit");
		return 0;
	}
};

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