#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include "IocContainer/IIocContainer.h"

namespace ioccontainer {

/**
 * IocContainer - Singleton IoC Container for dependency injection
 * 
 * This container implements the singleton pattern to provide a single,
 * globally accessible instance for service registration and resolution.
 * Thread-safe singleton implementation using Meyer's singleton pattern.
 */
class IocContainer : public IIocContainer {
public:
    /**
     * Get the singleton instance of the IoC container
     * Thread-safe implementation using Meyer's singleton pattern
     * @return Reference to the singleton instance
     */
    static IocContainer& getInstance();

    /**
     * Destructor - ensures proper cleanup
     */
    ~IocContainer() override;

    // TODO: Add your public interface methods here
    // Example:
    // bool initialize() override;
    // void cleanup() override;
    // std::string getStatus() const;

    // Delete copy constructor and assignment operator (singleton pattern)
    IocContainer(const IocContainer&) = delete;
    IocContainer& operator=(const IocContainer&) = delete;
    
    // Delete move constructor and assignment (singleton should not be moved)
    IocContainer(IocContainer&&) = delete;
    IocContainer& operator=(IocContainer&&) = delete;

private:
    /**
     * Private constructor for singleton pattern
     */
    IocContainer();

    // TODO: Add private members here
    // Consider using PIMPL idiom for complex implementations:
    // class Impl;
    // std::unique_ptr<Impl> _impl;
};

} // namespace ioccontainer
