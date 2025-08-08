#include <ApiModule/ApiModules.h>
#include <IoCContainer/IoCContainer.h>
#include <Logger/Logger.h>
#include <PrintHello/PrintHello.h>
#include <WebServer/WebServer.h>
#include <ServiceA/ServiceA.h>
#include <ServiceB/ServiceB.h>
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
	IoCContainer _container;
	std::shared_ptr<logger::Logger> _logger;
	std::shared_ptr<print_hello::PrintHello> _printer;
	std::shared_ptr<WebServer> _webServer;
	std::shared_ptr<servicea::ServiceA> _serviceA;
	std::shared_ptr<serviceb::ServiceB> _serviceB;

  public:
	Application() : _config(), _container()
	{
		// Register dependencies in IoCContainer
		_container.registerType<logger::Logger>(
			[this]() { return std::make_shared<logger::Logger>(_config.loggerIp, _config.loggerPort); });
		_container.registerType<print_hello::PrintHello>([]() { return std::make_shared<print_hello::PrintHello>(); });
		_container.registerType<WebServer>([]() { return std::make_shared<WebServer>("127.0.0.1", 8080); });
		_container.registerType<servicea::ServiceA>([]() { return std::make_shared<servicea::ServiceA>(); });
		_container.registerType<serviceb::ServiceB>([]() { return std::make_shared<serviceb::ServiceB>(); });
		_logger = _container.resolve<logger::Logger>();
		_printer = _container.resolve<print_hello::PrintHello>();
		_webServer = _container.resolve<WebServer>();
		_serviceA = _container.resolve<servicea::ServiceA>();
		_serviceB = _container.resolve<serviceb::ServiceB>();
		_logger->start();
	}

	~Application()
	{
		_logger->stop();
	}

	/**
	 * Runs the main application logic.
	 * @return Exit code (0 for success)
	 */
	int run()
	{
		if (_logger->isConnected())
		{
			_logger->logInfo("Application started with remote logging");
		}
		else
		{
			std::cout << "Running without remote logging (no server at 127.0.0.1:9000)" << std::endl;
		}

		_printer->print();
		// Example usage of ServiceA and ServiceB
		std::cout << "ServiceA: " << _serviceA->doSomethingServiceA() << std::endl;
		std::cout << "ServiceB: " << _serviceB->doSomethingServiceB() << std::endl;

		// Register all API modules (endpoints) via ApiModules
		apimodule::ApiModules::registerAll(*_webServer);
		_webServer->start();
		std::cout << "WebServer running on http://localhost:8080/hello" << std::endl;

		// Keep running until user presses Enter
		std::cout << "Press Enter to exit..." << std::endl;
		std::cin.get();
		_webServer->stop();
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