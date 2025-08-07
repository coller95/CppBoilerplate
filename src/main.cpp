
#include <foo.h>
#include <foo_c.h>
#include <iostream>
#include <PrintHello.h>
#include <Logger.h>
#include <chrono>
#include <thread>



/**
 * Demonstrates Logger usage and logging scenarios
 * @param logger Reference to Logger instance
 * @return 0 on completion
 */
int loggerDemo(Logger& logger, const PrintHello& printer)
{
	std::cout << "=== Logger Class Demo ===" << std::endl;

	bool connected = logger.isConnected();

	if (connected) {
		std::cout << "Connected to log server successfully!" << std::endl;
		logger.logInfo("Application started successfully");
		logger.logDebug("This is a debug message");
		logger.logWarning("This is a warning message");
		logger.logError("This is an error message");
		logger.log("Custom formatted message: %d + %d = %d", 5, 3, 8);
		std::cout << "This stdout message should be intercepted by logger" << std::endl;
		printer.print();
		std::this_thread::sleep_for(std::chrono::seconds(2));
		logger.logInfo("Application finishing");
	} else {
		std::cout << "Could not connect to log server, running without remote logging" << std::endl;
		logger.logInfo("This won't be sent anywhere");
		logger.logError("Local error message");
		printer.print();
	}
	std::cout << "Demo completed." << std::endl;
	return 0;
}



/**
 * Encapsulates the main application logic
 *
 * TDD-friendly: All dependencies are injected, no side effects in constructor.
 */
class Application {
private:
	Logger& _logger;
	const PrintHello& _printer;

public:
	/**
	 * Constructs the Application with injected Logger and PrintHello dependencies
	 * @param logger Reference to Logger instance
	 * @param printer Reference to PrintHello instance
	 */
	Application(Logger& logger, const PrintHello& printer)
		: _logger(logger), _printer(printer)
	{
		// No side effects here; logger should be started by caller if needed
	}

	/**
	 * Runs the application
	 * @return Exit code
	 */
	int run()
	{
		if (_logger.isConnected()) {
			_logger.logInfo("Application started with remote logging");
		} else {
			std::cout << "Running without remote logging (no server at 127.0.0.1:9000)" << std::endl;
		}

		loggerDemo(_logger, _printer);
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
	Logger logger{"127.0.0.1", 9000};
	logger.start();
	PrintHello printer;
	Application app{logger, printer};
	int result = app.run();
	logger.stop();
	return result;
}