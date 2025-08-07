
#include <Logger/Logger.h>
#include <PrintHello/PrintHello.h>
#include <chrono>
#include <foo.h>
#include <foo_c.h>
#include <iostream>
#include <thread>

/**
 * Application configuration struct (local to this file for convenience)
 */
struct AppConfig
{
	std::string loggerIp = "127.0.0.1";
	int loggerPort = 9000;
};

/**
 * Encapsulates the main application logic and owns all dependencies
 *
 * TDD-friendly: All dependencies are constructed and managed here.
 */
class Application {
private:
	AppConfig _config;
	logger::Logger _logger;
	print_hello::PrintHello _printer;
public:
	Application()
		: _config(),
		  _logger(_config.loggerIp, _config.loggerPort),
		  _printer() {
		_logger.start();
	}

	~Application() {
		_logger.stop();
	}

private:
	void runLoggerDemo() {
		std::cout << "=== Logger Class Demo ===" << std::endl;
		bool connected = _logger.isConnected();
		if (connected) {
			std::cout << "Connected to log server successfully!" << std::endl;
			_logger.logInfo("Application started successfully");
			_logger.logDebug("This is a debug message");
			_logger.logWarning("This is a warning message");
			_logger.logError("This is an error message");
			_logger.log("Custom formatted message: %d + %d = %d", 5, 3, 8);
			std::cout << "This stdout message should be intercepted by logger" << std::endl;
			_printer.print();
			std::this_thread::sleep_for(std::chrono::seconds(2));
			_logger.logInfo("Application finishing");
		} else {
			std::cout << "Could not connect to log server, running without remote logging" << std::endl;
			_logger.logInfo("This won't be sent anywhere");
			_logger.logError("Local error message");
			_printer.print();
		}
		std::cout << "Demo completed." << std::endl;
	}

public:
	int run() {
		if (_logger.isConnected()) {
			_logger.logInfo("Application started with remote logging");
		} else {
			std::cout << "Running without remote logging (no server at 127.0.0.1:9000)" << std::endl;
		}
		runLoggerDemo();
		_printer.print();
		external_print("Hello from the external C++ library!");
		external_print_c("Hello from the external C library!");
		return 0;
	}
};

/**
 * Main entry point for the application
 *
 * TDD-friendly: All dependencies are constructed and injected here.
 */

int main()
{
	Application app;
	return app.run();
}