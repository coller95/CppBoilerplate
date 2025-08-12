#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <mutex>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <vector>
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

    /**
     * Register a service instance for a specific interface type
     * @tparam TInterface The interface type to register for
     * @param instance Shared pointer to the service instance
     */
    template<typename TInterface>
    void registerInstance(std::shared_ptr<TInterface> instance) {
        std::lock_guard<std::mutex> lock(_mutex);
        _services[std::type_index(typeid(TInterface))] = instance;
    }
    
    /**
     * Resolve a service by its interface type
     * @tparam TInterface The interface type to resolve
     * @return Shared pointer to the service instance
     * @throws ServiceNotRegisteredException if service is not registered
     */
    template<typename TInterface>
    std::shared_ptr<TInterface> resolve() {
        std::lock_guard<std::mutex> lock(_mutex);
        auto typeIndex = std::type_index(typeid(TInterface));
        auto it = _services.find(typeIndex);
        
        if (it == _services.end()) {
            throw ServiceNotRegisteredException(typeid(TInterface).name());
        }
        
        return std::static_pointer_cast<TInterface>(it->second);
    }
    
    /**
     * Check if a service type is registered (templated version)
     * @tparam TInterface The interface type to check
     * @return True if the service is registered
     */
    template<typename TInterface>
    bool isRegistered() const {
        return isRegistered(typeid(TInterface));
    }

    // IIocContainer interface implementation
    bool isRegistered(const std::type_info& type) const override;
    void clear() override;
    size_t getRegisteredCount() const override;

    // Registry listing and debugging methods
    /**
     * Get a list of all registered type names
     * @return Vector of type names for all registered services
     */
    std::vector<std::string> getRegisteredTypeNames() const;
    
    /**
     * Get formatted information about all registered services as a string
     * @return String containing formatted information about registered services
     */
    std::string getRegisteredServicesInfo() const;

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

    // Thread-safe access to services
    mutable std::mutex _mutex;
    
    // Service registry: maps type_index to service instances
    std::unordered_map<std::type_index, std::shared_ptr<void>> _services;
};

} // namespace ioccontainer
