#pragma once

#include <memory>
#include <typeinfo>
#include <functional>
#include <vector>
#include <string>

namespace ioccontainer {

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

/**
 * Interface for IocContainer - Complete IoC Container Interface
 * This interface provides both template methods and virtual methods.
 * Template methods delegate to virtual implementation methods for mockability.
 */
class IIocContainer {
private:
    // Virtual implementation methods for delegation (mockable)
    virtual void registerService_impl(const std::type_info& type, std::shared_ptr<void> instance) = 0;
    virtual std::shared_ptr<void> resolveService_impl(const std::type_info& type) = 0;
    virtual bool isRegistered_impl(const std::type_info& type) const = 0;

public:
    virtual ~IIocContainer() = default;
    
    /**
     * Get the singleton instance of the IoC container
     * Thread-safe implementation using Meyer's singleton pattern
     * @return Reference to the singleton instance (as interface)
     */
    static IIocContainer& getInstance();
    
    // === TEMPLATE METHODS (delegate to virtual implementation) ===
    
    /**
     * Register a service instance for a specific interface type
     * @tparam TInterface The interface type to register for
     * @param instance Shared pointer to the service instance
     */
    template<typename TInterface>
    void registerInstance(std::shared_ptr<TInterface> instance) {
        registerService_impl(typeid(TInterface), std::static_pointer_cast<void>(instance));
    }
    
    /**
     * Resolve a service by its interface type
     * @tparam TInterface The interface type to resolve
     * @return Shared pointer to the service instance
     * @throws ServiceNotRegisteredException if service is not registered
     */
    template<typename TInterface>
    std::shared_ptr<TInterface> resolve() {
        auto result = resolveService_impl(typeid(TInterface));
        if (!result) {
            throw ServiceNotRegisteredException(typeid(TInterface).name());
        }
        return std::static_pointer_cast<TInterface>(result);
    }
    
    /**
     * Check if a service type is registered (templated version)
     * @tparam TInterface The interface type to check
     * @return True if the service is registered
     */
    template<typename TInterface>
    bool isRegistered() const {
        return isRegistered_impl(typeid(TInterface));
    }
    
    // === GLOBAL STATIC METHODS (convenience wrappers) ===
    
    /**
     * Register a service instance globally using the singleton container
     * @tparam TInterface The interface type to register for
     * @param instance Shared pointer to the service instance
     */
    template<typename TInterface>
    static void registerGlobal(std::shared_ptr<TInterface> instance) {
        getInstance().registerInstance<TInterface>(instance);
    }
    
    /**
     * Register a service factory globally using the singleton container
     * @tparam TInterface The interface type to register for
     * @param factory Factory function that creates the service instance
     */
    template<typename TInterface>
    static void registerGlobal(std::function<std::shared_ptr<TInterface>()> factory) {
        getInstance().registerInstance<TInterface>(factory());
    }
    
    /**
     * Resolve a service globally using the singleton container
     * @tparam TInterface The interface type to resolve
     * @return Shared pointer to the service instance
     * @throws ServiceNotRegisteredException if service is not registered
     */
    template<typename TInterface>
    static std::shared_ptr<TInterface> resolveGlobal() {
        return getInstance().resolve<TInterface>();
    }
    
    /**
     * Check if a service type is registered globally using the singleton container
     * @tparam TInterface The interface type to check
     * @return True if the service is registered
     */
    template<typename TInterface>
    static bool isRegisteredGlobal() {
        return getInstance().isRegistered<TInterface>();
    }
    
    // === VIRTUAL METHODS ===
    
    /**
     * Check if a service type is registered
     * @param type The type_info to check
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
    
    /**
     * Get a list of all registered type names
     * @return Vector of type names for all registered services
     */
    virtual std::vector<std::string> getRegisteredTypeNames() const = 0;
    
    /**
     * Get formatted information about all registered services as a string
     * @return String containing formatted information about registered services
     */
    virtual std::string getRegisteredServicesInfo() const = 0;
};

} // namespace ioccontainer
