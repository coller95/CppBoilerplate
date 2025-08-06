#include <iostream>
#include <printHello.h>
#include <foo.h>
#include <foo_c.h>

/**
 * Application class encapsulates the main application logic
 */
class Application
{
  public:
	/**
	 * Runs the application
	 * @return Exit code
	 */
	int run()
	{
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