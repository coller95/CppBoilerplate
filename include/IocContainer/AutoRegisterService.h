#pragma once

#include <IocContainer/IocContainer.h>
#include <memory>
#include <type_traits>

namespace ioccontainer {

/**
 * Metaprogramming auto-registration for IoC services using CRTP
 * 
 * DESIGN PRINCIPLES:
 * - KISS: Simple template patterns, no complex SFINAE
 * - YAGNI: Only implement what's needed
 * - Type safety: Compile-time interface verification
 * - Zero runtime overhead: Static initialization only
 * 
 * Usage patterns:
 * 
 * 1. Simple service (concrete class only):
 *    namespace { static AutoRegister<ServiceUser> _autoRegister; }
 * 
 * 2. Interface + Implementation:
 *    namespace { static AutoRegister<ServiceUser, IServiceUser> _autoRegister; }
 * 
 * 3. Multiple interfaces:
 *    namespace { static AutoRegister<ServiceUser, IServiceUser, IAnotherInterface> _autoRegister; }
 */

/**
 * AutoRegister template for IoC service registration
 * 
 * Template parameters:
 * - ConcreteService: The concrete service implementation class
 * - Interfaces...: Optional interface types to register
 * 
 * Features:
 * - Single shared instance for all registrations
 * - Automatic interface detection and registration
 * - Compile-time type safety
 * - Zero runtime overhead
 */
template<typename ConcreteService, typename... Interfaces>
class AutoRegister {
public:
    AutoRegister() {
        // Create single shared instance
        auto instance = std::make_shared<ConcreteService>();
        
        // Always register the concrete service
        IIocContainer::registerGlobal<ConcreteService>(instance);
        
        // Register for each interface (if any)
        if constexpr (sizeof...(Interfaces) > 0) {
            registerInterfaces(instance);
        }
    }

private:
    // Helper to register all interfaces
    void registerInterfaces(std::shared_ptr<ConcreteService> instance) {
        // Use fold expression (C++17) to register each interface
        (IIocContainer::registerGlobal<Interfaces>(std::static_pointer_cast<Interfaces>(instance)), ...);
    }
};

/**
 * CRTP base class for services (optional - for future extensibility)
 * 
 * This provides a foundation for additional metaprogramming features
 * if needed in the future, while maintaining YAGNI principle.
 * 
 * Usage:
 *   class ServiceUser : public AutoRegisterService<ServiceUser> {
 *       // Service implementation
 *   };
 */
template<typename Derived>
class AutoRegisterService {
public:
    // Virtual destructor for proper inheritance
    virtual ~AutoRegisterService() = default;

protected:
    // Protected constructor - only derived classes can instantiate
    AutoRegisterService() = default;

    // Helper method for derived classes to access their type information
    static constexpr const char* getServiceName() {
        return typeid(Derived).name();
    }
};

} // namespace ioccontainer