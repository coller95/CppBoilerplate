#include <iostream>

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
	}

	~Application()
	{
	}

	/**
	 * Runs the main application logic.
	 * @return Exit code (0 for success)
	 */
	int run()
	{
		// Keep running until user presses Enter
		std::cout << "Press Enter to exit..." << std::endl;
		std::cin.get();
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