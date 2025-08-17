#include <EndpointHello/EndpointHello.h>
#include <string_view>
#include <string>

namespace endpointhello {

// METAPROGRAMMING AUTO-REGISTRATION: Much cleaner than manual registerEndpoints()!
void EndpointHello::registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& /*basePath*/) {
	// basePath can be auto-inferred, but using explicit path for clarity
	// Auto-inference: EndpointHello -> /hello
	const std::string path = "/hello";
	
	// Register GET handler - no manual lambda wrapping needed!
	registerMethod<&EndpointHello::handleGet>(registrar, path, "GET");
	
	// Easy to add more methods:
	// registerMethod<&EndpointHello::handlePost>(registrar, path, "POST");
	// registerMethod<&EndpointHello::handlePut>(registrar, path, "PUT");
	// registerMethod<&EndpointHello::handleDelete>(registrar, path, "DELETE");
}

// Handler method using cleaner naming convention: handleGet instead of handleGetHello
void EndpointHello::handleGet(std::string_view /*path*/, std::string_view /*method*/,
                               const std::string& /*requestBody*/, std::string& responseBody, int& statusCode) {
	// TODO: Implement your GET /hello logic here
	statusCode = 200;
	responseBody = "Hello from metaprogramming EndpointHello! Automatic path: /hello\n";
	
	// Example of JSON response:
	// responseBody = R"({"message": "Hello from Hello", "status": "success"})";
	
	// Example of error handling:
	// if (some_error_condition) {
	//     statusCode = 400;
	//     responseBody = R"({"error": "Bad request", "message": "Invalid Hello data"})";
	//     return;
	// }
}

// TODO: Implement additional handler methods
// void EndpointHello::handlePost(std::string_view path, std::string_view method,
//                                 const std::string& requestBody, std::string& responseBody, int& statusCode) {
//     // TODO: Implement POST logic
//     statusCode = 201;
//     responseBody = R"({"message": "Hello created successfully"})";
// }

} // namespace endpointhello

// METAPROGRAMMING MAGIC: One line replaces entire registration struct!
namespace {
	static apirouter::AutoRegister<endpointhello::EndpointHello> _autoRegisterEndpointHello;
}
