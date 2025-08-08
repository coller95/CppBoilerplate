
#include <ApiModule/ApiModules.h>
#include <ApiModule/IApiModule.h>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>

namespace apimodule {

/**
 * Wrapper class to count and track registered endpoints
 */
class EndpointCountingRegistrar : public IEndpointRegistrar {
private:
    IEndpointRegistrar& _actualRegistrar;
    int _count;
    std::vector<std::string> _endpoints;

public:
    explicit EndpointCountingRegistrar(IEndpointRegistrar& actualRegistrar) 
        : _actualRegistrar(actualRegistrar), _count(0) {}

    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler handler) override {
        _actualRegistrar.registerHttpHandler(path, method, handler);
        _count++;
        _endpoints.push_back(std::string(method) + " " + std::string(path));
    }

    int getCount() const { return _count; }
    
    void printEndpointDetails() const {
        std::cout << "ApiModules: Registered " << _count << " endpoint(s):" << std::endl;
        for (const auto& endpoint : _endpoints) {
            std::cout << "  - " << endpoint << std::endl;
        }
    }
};

void ApiModules::registerAll(IEndpointRegistrar& registrar) {
    EndpointCountingRegistrar countingRegistrar(registrar);
    
    // Automatically register all endpoint modules that have been registered
    const auto& factories = getModuleFactories();
    for (const auto& factory : factories) {
        auto module = factory();
        if (module) {
            module->registerEndpoints(countingRegistrar);
        }
    }
    
    // Print detailed information about registered endpoints
    countingRegistrar.printEndpointDetails();
}

std::vector<ApiModules::ModuleFactory>& ApiModules::getModuleFactories() {
    static std::vector<ModuleFactory> factories;
    return factories;
}

void ApiModules::registerModuleFactory(ModuleFactory factory) {
    getModuleFactories().push_back(factory);
}

size_t ApiModules::getRegisteredModuleCount() {
    return getModuleFactories().size();
}

std::vector<std::unique_ptr<IApiModule>> ApiModules::createAllModules() {
    std::vector<std::unique_ptr<IApiModule>> modules;
    const auto& factories = getModuleFactories();
    for (const auto& factory : factories) {
        auto module = factory();
        if (module) {
            modules.push_back(std::move(module));
        }
    }
    return modules;
}

} // namespace apimodule
