#pragma once

#include <memory>
#include <string>
#include <string_view>


namespace logger {

/**
 * Logger - Utility/Infrastructure module
 * 
 * TODO: Add detailed description of what this module does
 */
class Logger {
public:
	/**
	 * Default constructor
	 */
	Logger();

	/**
	 * Destructor - ensures proper cleanup
	 */
	~Logger();

	// TODO: Add your public interface methods here
	// Example:
	    // bool initialize();
	    // void cleanup();
	// std::string getStatus() const;

	// Delete copy constructor and assignment operator (RAII best practice)
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	
	// Allow move constructor and assignment
	Logger(Logger&&) noexcept;
	Logger& operator=(Logger&&) noexcept;

private:
    // TODO: Add private members here
};

} // namespace logger
