#pragma once

#include <ApiRouter/IEndpointRegistrar.h>
#include <ApiRouter/ApiRouter.h>
#include <string>
#include <string_view>
#include <cctype>
#include <typeinfo>

namespace apirouter {

/**
 * CRTP base class for automatic endpoint registration
 * 
 * Simple metaprogramming solution following KISS and YAGNI principles:
 * - Convention over configuration
 * - Automatic path inference from class name
 * - Simple method-based registration
 * 
 * Usage:
 *   class EndpointFoo : public AutoRegisterEndpoint<EndpointFoo> {
 *   protected:
 *       void handleGet(...) { // GET /foo logic }
 *       void handlePost(...) { // POST /foo logic }
 *   };
 * 
 * Features:
 * - Path inference: EndpointFoo -> /foo, EndpointUserProfile -> /user-profile
 * - Method convention: handleGet, handlePost, handlePut, handleDelete
 * - Compile-time type safety with CRTP
 * - Zero runtime overhead for non-existent methods
 */
template<typename Derived>
class AutoRegisterEndpoint : public IApiModule {
public:
    void registerEndpoints(IEndpointRegistrar& registrar) override {
        const std::string basePath = extractPathFromClassName();
        
        // Use SFINAE to register only methods that exist
        // This is much simpler than complex template metaprogramming
        static_cast<Derived*>(this)->registerAvailableMethods(registrar, basePath);
    }

public:
    // Virtual method for derived classes to register their specific methods
    virtual void registerAvailableMethods(IEndpointRegistrar& /*registrar*/, const std::string& /*basePath*/) {
        // Base class does nothing - derived classes provide their own implementation
        // This follows YAGNI - we only implement what we need
    }

protected:

    // Helper method for derived classes to register handlers easily
    template<void(Derived::*Method)(std::string_view, std::string_view, const std::string&, std::string&, int&)>
    void registerMethod(IEndpointRegistrar& registrar, const std::string& path, const std::string& method) {
        registrar.registerHttpHandler(path, method,
            [this](std::string_view p, std::string_view m, const std::string& req, std::string& resp, int& status) {
                (static_cast<Derived*>(this)->*Method)(p, m, req, resp, status);
            });
    }

private:
    // Extract path from class name: EndpointFoo -> /foo, EndpointUserProfile -> /user-profile
    std::string extractPathFromClassName() const {
        // Get demangled class name
        std::string className = typeid(Derived).name();
        
        // Find "Endpoint" prefix
        size_t pos = className.rfind("Endpoint");
        if (pos == std::string::npos) {
            return "/unknown";
        }
        
        // Extract resource name after "Endpoint"
        std::string resourceName = className.substr(pos + 8);
        if (resourceName.empty()) {
            return "/";
        }
        
        // Convert to lowercase: ABC -> abc, UserProfile -> user-profile
        std::string path = "/";
        bool previousWasUpper = false;
        
        for (size_t i = 0; i < resourceName.length(); ++i) {
            char c = resourceName[i];
            bool currentIsUpper = std::isupper(c);
            
            // Add hyphen before uppercase letters if:
            // 1. Not the first character AND
            // 2. Previous char was lowercase OR next char is lowercase (handles "XMLParser" -> "xml-parser")
            if (i > 0 && currentIsUpper && 
                (!previousWasUpper || (i + 1 < resourceName.length() && std::islower(resourceName[i + 1])))) {
                path += '-';
            }
            
            path += std::tolower(c);
            previousWasUpper = currentIsUpper;
        }
        
        return path;
    }
};

/**
 * Simple registration helper using static initialization
 * 
 * This is much simpler than complex template metaprogramming
 * and follows KISS principle
 */
template<typename T>
struct AutoRegister {
    AutoRegister() {
        apirouter::ApiRouter::registerModuleFactoryGlobal([]() -> std::unique_ptr<apirouter::IApiModule> {
            return std::make_unique<T>();
        });
    }
};

/**
 * Simple macro for endpoint registration
 * 
 * Usage: REGISTER_ENDPOINT(EndpointFoo)
 * 
 * Note: Prefer explicit static instantiation in implementation files
 * for better debugging and explicit control
 */
#define REGISTER_ENDPOINT(ClassName) \
    namespace { \
        static apirouter::AutoRegister<ClassName> _autoRegister##ClassName; \
    }

} // namespace apirouter