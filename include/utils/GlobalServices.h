#pragma once

#include <IocContainer/IocContainer.h>
#include <Logger/ILogger.h>
#include <memory>

namespace utils {

/**
 * Global utility functions for accessing IoC container services
 */
class GlobalServices {
public:
    /**
     * Get the global logger instance from the IoC container
     * @return Shared pointer to the logger interface
     * @throws ServiceNotRegisteredException if logger is not registered
     */
    static std::shared_ptr<logger::ILogger> getLogger() {
        auto& container = ioccontainer::IocContainer::getInstance();
        return container.resolve<logger::ILogger>();
    }
    
    /**
     * Check if the logger is available globally
     * @return True if logger is registered in the IoC container
     */
    static bool isLoggerAvailable() {
        auto& container = ioccontainer::IocContainer::getInstance();
        return container.isRegistered<logger::ILogger>();
    }
};

} // namespace utils
