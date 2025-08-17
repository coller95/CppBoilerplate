#pragma once
#include <ApiRouter/AutoRegisterEndpoint.h>
#include <string>
#include <string_view>

namespace endpointhello {

/**
 * EndpointHello - HTTP endpoint handler using metaprogramming auto-registration
 * 
 * METAPROGRAMMING FEATURES:
 * - Automatic path inference: EndpointHello -> /hello
 * - No manual lambda wrapping required
 * - Type-safe method registration
 * - One-line registration with AutoRegister
 * 
 * This endpoint handles HTTP requests for /hello resources
 * and automatically registers with ApiRouter using CRTP pattern.
 */
class EndpointHello : public apirouter::AutoRegisterEndpoint<EndpointHello> {
public:
	// Override to register available methods - much cleaner than manual registerEndpoints()
	void registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& basePath) override;

protected:
	// HTTP handler methods - cleaner naming convention (testable protected methods)
	void handleGet(std::string_view path, std::string_view method, 
	               const std::string& requestBody, std::string& responseBody, int& statusCode);
	
	// TODO: Add more HTTP handler methods as needed
	// void handlePost(std::string_view path, std::string_view method, 
	//                 const std::string& requestBody, std::string& responseBody, int& statusCode);
	// void handlePut(std::string_view path, std::string_view method, 
	//                const std::string& requestBody, std::string& responseBody, int& statusCode);
	// void handleDelete(std::string_view path, std::string_view method, 
	//                   const std::string& requestBody, std::string& responseBody, int& statusCode);
};

}
