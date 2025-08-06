
#include <foo.h>
#include <foo_c.h>
#include <iostream>
#include <printHello.h>
#include <Logger.h>

#include <chrono>
#include <thread>

int loggerDemo(Logger &logger) {
    std::cout << "=== Logger Class Demo ===" << std::endl;

    // Try to start logger (will fail gracefully if no server)
    bool connected = logger.start();
    
    if (connected) {
        std::cout << "Connected to log server successfully!" << std::endl;
        
        // Test various logging methods
        logger.logInfo("Application started successfully");
        logger.logDebug("This is a debug message");
        logger.logWarning("This is a warning message");
        logger.logError("This is an error message");
        logger.log("Custom formatted message: %d + %d = %d", 5, 3, 8);
        
        // Test with regular stdout (should be intercepted)
        std::cout << "This stdout message should be intercepted by logger" << std::endl;
        
        // Call the original function
        printHello();
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        logger.logInfo("Application finishing");
    } else {
        std::cout << "Could not connect to log server, running without remote logging" << std::endl;
        
        // Still test the logger methods (they should handle disconnected state gracefully)
        logger.logInfo("This won't be sent anywhere");
        logger.logError("Local error message");
        
        // Call the original function
        printHello();
    }
    
    std::cout << "Demo completed." << std::endl;
    return 0;
}


/**
 * Encapsulates the main application logic
 */
class Application {
private:
	Logger _logger;
public:
	/**
	 * Constructs the Application and initializes the logger
	 */
	Application()
		: _logger("127.0.0.1", 9000) // Example: send logs to TCP server at 127.0.0.1:9000
	{
		_logger.start();
	}

	~Application() {
		_logger.stop();
	}

	/**
	 * Runs the application
	 * @return Exit code
	 */
	int run() {
		if (_logger.isConnected()) {
			_logger.logInfo("Application started with remote logging");
		} else {
			std::cout << "Running without remote logging (no server at 127.0.0.1:9000)" << std::endl;
		}

		loggerDemo(_logger);
		
		printHello();
		external_print("Hello from the external C++ library!");
		external_print_c("Hello from the external C library!");
		
		return 0;
	}
};

int main()
{
	Application app;
	return app.run();
}