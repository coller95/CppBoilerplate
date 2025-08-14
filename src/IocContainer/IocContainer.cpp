#include <IocContainer/IIocContainer.h>
#include <sstream>
#include <iomanip>
#include <typeindex>
#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <functional>

namespace ioccontainer {

/**
 * IocContainer - Private implementation of IIocContainer interface
 * 
 * This container implements the IoC container interface with singleton pattern.
 * The singleton is managed by the interface, and this class provides the
 * concrete implementation of all virtual methods.
 * 
 * NOTE: This class is intentionally private - users can only access it 
 * through the IIocContainer interface.
 */
class IocContainer : public IIocContainer {
    // Friend the interface so it can create the singleton instance
    friend IIocContainer& IIocContainer::getInstance();

protected:
    // === VIRTUAL IMPLEMENTATION METHODS (for template delegation) ===
    void registerService_impl(const std::type_info& type, std::shared_ptr<void> instance) override;
    std::shared_ptr<void> resolveService_impl(const std::type_info& type) override;
    bool isRegistered_impl(const std::type_info& type) const override;

public:
    /**
     * Destructor - ensures proper cleanup
     */
    ~IocContainer() override;

    // === VIRTUAL INTERFACE METHODS ===
    bool isRegistered(const std::type_info& type) const override;
    void clear() override;
    size_t getRegisteredCount() const override;
    std::vector<std::string> getRegisteredTypeNames() const override;
    std::string getRegisteredServicesInfo() const override;

    // Delete copy constructor and assignment operator (singleton pattern)
    IocContainer(const IocContainer&) = delete;
    IocContainer& operator=(const IocContainer&) = delete;
    
    // Delete move constructor and assignment (singleton should not be moved)
    IocContainer(IocContainer&&) = delete;
    IocContainer& operator=(IocContainer&&) = delete;

private:
    /**
     * Private constructor for singleton pattern (only interface can create)
     */
    IocContainer();

    // Thread-safe access to services
    mutable std::mutex _mutex;
    
    // Service registry: maps type_index to service instances
    std::unordered_map<std::type_index, std::shared_ptr<void>> _services;
};

// Implementation of the singleton getInstance in the interface
IIocContainer& IIocContainer::getInstance() {
    // Meyer's singleton: thread-safe in C++11 and later
    // The static local variable is initialized only once,
    // even in multi-threaded environments
    static IocContainer instance;
    return instance;  // Returns as interface reference
}

IocContainer::IocContainer() {
    // Private constructor for singleton pattern
    // Initialize empty service registry
}

// === VIRTUAL IMPLEMENTATION METHODS ===

void IocContainer::registerService_impl(const std::type_info& type, std::shared_ptr<void> instance) {
    std::lock_guard<std::mutex> lock(_mutex);
    _services[std::type_index(type)] = instance;
}

std::shared_ptr<void> IocContainer::resolveService_impl(const std::type_info& type) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _services.find(std::type_index(type));
    return (it != _services.end()) ? it->second : nullptr;
}

bool IocContainer::isRegistered_impl(const std::type_info& type) const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _services.find(std::type_index(type)) != _services.end();
}

IocContainer::~IocContainer() {
    // Clear all services on destruction
    clear();
}

bool IocContainer::isRegistered(const std::type_info& type) const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _services.find(std::type_index(type)) != _services.end();
}

void IocContainer::clear() {
    std::lock_guard<std::mutex> lock(_mutex);
    _services.clear();
}

size_t IocContainer::getRegisteredCount() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _services.size();
}

std::vector<std::string> IocContainer::getRegisteredTypeNames() const {
    std::lock_guard<std::mutex> lock(_mutex);
    std::vector<std::string> typeNames;
    
    for (const auto& pair : _services) {
        typeNames.push_back(pair.first.name());
    }
    
    return typeNames;
}

std::string IocContainer::getRegisteredServicesInfo() const {
    std::lock_guard<std::mutex> lock(_mutex);
    std::ostringstream oss;
    
    oss << "IoC Container Registry: ";
    
    if (_services.empty()) {
        oss << "No services registered.";
    } else {
        oss << _services.size() << " service(s) registered - ";
        
        bool first = true;
        for (const auto& pair : _services) {
            if (!first) oss << ", ";
            
            std::string typeName = pair.first.name();
            
            // Try to make the type name more readable
            if (typeName.find("N6logger7ILoggerE") != std::string::npos || 
                typeName.find("ILogger") != std::string::npos) {
                typeName = "Logger Interface";
            } else if (typeName.find("Logger") != std::string::npos) {
                typeName = "Logger Service";
            }
            
            oss << typeName;
            first = false;
        }
    }
    
    return oss.str();
}

} // namespace ioccontainer
