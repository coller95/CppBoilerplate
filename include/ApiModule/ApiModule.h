#pragma once
#include <ApiModule/IEndpointRegistrar.h>
#include <ApiModule/IApiModule.h>
#include <vector>
#include <memory>
#include <functional>

namespace apimodule {
    /**
     * Static class to register all API modules with the registrar.
     */
    class ApiModule {
    private:
        using ModuleFactory = std::function<std::unique_ptr<IApiModule>()>;
        static std::vector<ModuleFactory>& getModuleFactories();
        
    public:
        static void registerAll(IEndpointRegistrar& registrar);
        
        /**
         * Register a module factory function that will be called during registerAll
         * This is typically called from module constructors or static initializers
         */
        static void registerModuleFactory(ModuleFactory factory);
        
        /**
         * Get the number of registered module factories (for testing)
         */
        static size_t getRegisteredModuleCount();
        
        /**
         * Create instances of all registered modules (for testing)
         */
        static std::vector<std::unique_ptr<IApiModule>> createAllModules();
    };
}
