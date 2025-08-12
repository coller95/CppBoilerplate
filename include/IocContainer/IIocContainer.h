#pragma once

#include <memory>
#include <typeinfo>

namespace ioccontainer {

/**
 * Interface for IocContainer
 * Define the contract that implementations must follow
 */
class IIocContainer {
public:
    virtual ~IIocContainer() = default;
    
    /**
     * Check if a service type is registered
     * @return True if the service is registered
     */
    virtual bool isRegistered(const std::type_info& type) const = 0;
    
    /**
     * Clear all registered services
     */
    virtual void clear() = 0;
    
    /**
     * Get the number of registered services
     * @return Number of registered services
     */
    virtual size_t getRegisteredCount() const = 0;
};

/**
 * Exception thrown when a service is not registered
 */
class ServiceNotRegisteredException : public std::exception {
private:
    std::string _message;
    
public:
    explicit ServiceNotRegisteredException(const std::string& typeName) 
        : _message("Service not registered: " + typeName) {}
    
    const char* what() const noexcept override {
        return _message.c_str();
    }
};

} // namespace ioccontainer
