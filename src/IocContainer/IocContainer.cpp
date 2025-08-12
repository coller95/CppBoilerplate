#include <IocContainer/IocContainer.h>
#include <sstream>
#include <iomanip>

namespace ioccontainer {

IocContainer& IocContainer::getInstance() {
    // Meyer's singleton: thread-safe in C++11 and later
    // The static local variable is initialized only once,
    // even in multi-threaded environments
    static IocContainer instance;
    return instance;
}

IocContainer::IocContainer() {
    // Private constructor for singleton pattern
    // Initialize empty service registry
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
