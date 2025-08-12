#include <ApiRouter/ApiRouter.h>
#include <algorithm>
#include <stdexcept>

namespace apirouter {

// Static singleton implementation using Meyer's singleton pattern
ApiRouter& ApiRouter::getInstance() {
    static ApiRouter instance;
    return instance;
}

// Static global methods that use the singleton instance
void ApiRouter::registerModuleFactoryGlobal(std::function<std::unique_ptr<IApiModule>()> factory) {
    getInstance().registerModuleFactory(std::move(factory));
}

size_t ApiRouter::getRegisteredModuleCountGlobal() {
    return getInstance().getRegisteredModuleCount();
}

std::vector<std::unique_ptr<IApiModule>> ApiRouter::createAllModulesGlobal() {
    return getInstance().createAllModules();
}

bool ApiRouter::handleRequestGlobal(std::string_view path, std::string_view method, const std::string& requestBody, std::string& responseBody, int& statusCode) {
    return getInstance().handleRequest(path, method, requestBody, responseBody, statusCode);
}

bool ApiRouter::initializeGlobal() {
    return getInstance().initialize();
}

size_t ApiRouter::getEndpointCountGlobal() {
    return getInstance().getEndpointCount();
}

std::vector<std::string> ApiRouter::getRegisteredEndpointsGlobal() {
    return getInstance().getRegisteredEndpoints();
}

// Constructor (private for singleton)
ApiRouter::ApiRouter() : _initialized(false) {
    // Empty constructor - initialization happens in initialize()
}

ApiRouter::~ApiRouter() {
    // Cleanup is handled automatically by containers
}

// IApiRouter interface implementation
bool ApiRouter::initialize() {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    
    if (_initialized) {
        return true; // Already initialized
    }
    
    try {
        // Create instances of all registered modules and have them register their endpoints
        // With recursive mutex, we can safely call other methods that lock the same mutex
        std::vector<std::function<std::unique_ptr<IApiModule>()>> factoriesCopy = _moduleFactories;
        
        for (const auto& factory : factoriesCopy) {
            try {
                auto module = factory();
                if (module) {
                    module->registerEndpoints(*this); // This will call registerHttpHandler
                }
            } catch (const std::exception&) {
                // Skip modules that fail to create or register
                continue;
            }
        }
        
        _initialized = true;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ApiRouter::handleRequest(std::string_view path, std::string_view method, const std::string& requestBody, std::string& responseBody, int& statusCode) {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    
    if (!_initialized) {
        statusCode = 500;
        responseBody = "Router not initialized";
        return false;
    }
    
    std::string key = createEndpointKey(path, method);
    auto it = _endpoints.find(key);
    
    if (it != _endpoints.end()) {
        try {
            it->second(path, method, requestBody, responseBody, statusCode);
            return true;
        } catch (const std::exception&) {
            statusCode = 500;
            responseBody = "Internal server error";
            return false;
        }
    }
    
    // Endpoint not found
    statusCode = 404;
    responseBody = "Endpoint not found";
    return false;
}

size_t ApiRouter::getEndpointCount() const {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    return _endpoints.size();
}

std::vector<std::string> ApiRouter::getRegisteredEndpoints() const {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    std::vector<std::string> endpoints;
    endpoints.reserve(_endpoints.size());
    
    for (const auto& [key, handler] : _endpoints) {
        endpoints.push_back(key);
    }
    
    std::sort(endpoints.begin(), endpoints.end());
    return endpoints;
}

void ApiRouter::registerModuleFactory(std::function<std::unique_ptr<IApiModule>()> factory) {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    _moduleFactories.push_back(std::move(factory));
}

size_t ApiRouter::getRegisteredModuleCount() const {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    return _moduleFactories.size();
}

std::vector<std::unique_ptr<IApiModule>> ApiRouter::createAllModules() const {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    std::vector<std::unique_ptr<IApiModule>> modules;
    modules.reserve(_moduleFactories.size());
    
    for (const auto& factory : _moduleFactories) {
        try {
            auto module = factory();
            if (module) {
                modules.push_back(std::move(module));
            }
        } catch (const std::exception&) {
            // Skip modules that fail to create
            continue;
        }
    }
    
    return modules;
}

// IEndpointRegistrar interface implementation
void ApiRouter::registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    std::string key = createEndpointKey(path, method);
    _endpoints[key] = std::move(handler);
}

// Private helper methods
std::string ApiRouter::createEndpointKey(std::string_view path, std::string_view method) const {
    return std::string(path) + ":" + std::string(method);
}

} // namespace apirouter
