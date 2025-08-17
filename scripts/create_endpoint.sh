#!/bin/bash

# Script to create or remove endpoint modules following the modular endpoint convention
# Usage: ./scripts/create_endpoint.sh <create|remove> EndpointName

set -e

# Resolve script location and set project root
SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null; pwd -P)"
PROJECT_ROOT="$(dirname "$SCRIPTDIR")"
cd "$PROJECT_ROOT"

# Check if action and endpoint name are provided
if [ $# -lt 2 ]; then
	echo "Usage: $0 <create|remove> <EndpointName>"
	echo "Examples:"
	echo "  $0 create EndpointUser    # Create a new endpoint"
	echo "  $0 remove EndpointUser    # Remove an existing endpoint"
	exit 1
fi

ACTION="$1"
ENDPOINT_NAME="$2"

# Validate action
if [[ "$ACTION" != "create" && "$ACTION" != "remove" ]]; then
	echo "Error: Action must be 'create' or 'remove'"
	exit 1
fi

# Validate endpoint name format (should start with "Endpoint")
if [[ ! "$ENDPOINT_NAME" =~ ^Endpoint[A-Z][a-zA-Z0-9]*$ ]]; then
	echo "Error: Endpoint name must start with 'Endpoint' followed by a capitalized name"
	echo "Examples: EndpointUser, EndpointAuth, EndpointProduct"
	exit 1
fi

# Extract the endpoint part (e.g., "User" from "EndpointUser")
ENDPOINT_PART="${ENDPOINT_NAME#Endpoint}"
NAMESPACE_NAME="endpoint${ENDPOINT_PART,,}"  # Convert to lowercase

# Function to create endpoint
create_endpoint() {
	echo "Creating endpoint module: $ENDPOINT_NAME"
	echo "Namespace: $NAMESPACE_NAME"

	# Check if endpoint already exists
	if [ -d "include/$ENDPOINT_NAME" ] || [ -d "src/$ENDPOINT_NAME" ] || [ -d "tests/${ENDPOINT_NAME}Test" ]; then
		echo "Error: Endpoint '$ENDPOINT_NAME' already exists!"
		echo "Use 'remove' action first if you want to recreate it."
		exit 1
	fi

	# Create directories
	mkdir -p "include/$ENDPOINT_NAME"
	mkdir -p "src/$ENDPOINT_NAME"
	mkdir -p "tests/${ENDPOINT_NAME}Test/cases"

# Create header file with metaprogramming auto-registration
cat > "include/$ENDPOINT_NAME/$ENDPOINT_NAME.h" << EOF
#pragma once
#include <ApiRouter/AutoRegisterEndpoint.h>
#include <string>
#include <string_view>

namespace $NAMESPACE_NAME {

/**
 * $ENDPOINT_NAME - HTTP endpoint handler using metaprogramming auto-registration
 * 
 * METAPROGRAMMING FEATURES:
 * - Automatic path inference: $ENDPOINT_NAME -> /${ENDPOINT_PART,,}
 * - No manual lambda wrapping required
 * - Type-safe method registration
 * - One-line registration with AutoRegister
 * 
 * This endpoint handles HTTP requests for /${ENDPOINT_PART,,} resources
 * and automatically registers with ApiRouter using CRTP pattern.
 */
class $ENDPOINT_NAME : public apirouter::AutoRegisterEndpoint<$ENDPOINT_NAME> {
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
EOF

# Create source file with metaprogramming auto-registration
cat > "src/$ENDPOINT_NAME/$ENDPOINT_NAME.cpp" << EOF
#include <$ENDPOINT_NAME/$ENDPOINT_NAME.h>
#include <string_view>
#include <string>

namespace $NAMESPACE_NAME {

// METAPROGRAMMING AUTO-REGISTRATION: Much cleaner than manual registerEndpoints()!
void $ENDPOINT_NAME::registerAvailableMethods(apirouter::IEndpointRegistrar& registrar, const std::string& /*basePath*/) {
	// basePath can be auto-inferred, but using explicit path for clarity
	// Auto-inference: $ENDPOINT_NAME -> /${ENDPOINT_PART,,}
	const std::string path = "/${ENDPOINT_PART,,}";
	
	// Register GET handler - no manual lambda wrapping needed!
	registerMethod<&$ENDPOINT_NAME::handleGet>(registrar, path, "GET");
	
	// Easy to add more methods:
	// registerMethod<&$ENDPOINT_NAME::handlePost>(registrar, path, "POST");
	// registerMethod<&$ENDPOINT_NAME::handlePut>(registrar, path, "PUT");
	// registerMethod<&$ENDPOINT_NAME::handleDelete>(registrar, path, "DELETE");
}

// Handler method using cleaner naming convention: handleGet instead of handleGet${ENDPOINT_PART}
void $ENDPOINT_NAME::handleGet(std::string_view /*path*/, std::string_view /*method*/,
                               const std::string& /*requestBody*/, std::string& responseBody, int& statusCode) {
	// TODO: Implement your GET /${ENDPOINT_PART,,} logic here
	statusCode = 200;
	responseBody = "Hello from metaprogramming $ENDPOINT_NAME! Automatic path: /${ENDPOINT_PART,,}\\n";
	
	// Example of JSON response:
	// responseBody = R"({"message": "Hello from ${ENDPOINT_PART}", "status": "success"})";
	
	// Example of error handling:
	// if (some_error_condition) {
	//     statusCode = 400;
	//     responseBody = R"({"error": "Bad request", "message": "Invalid ${ENDPOINT_PART} data"})";
	//     return;
	// }
}

// TODO: Implement additional handler methods
// void $ENDPOINT_NAME::handlePost(std::string_view path, std::string_view method,
//                                 const std::string& requestBody, std::string& responseBody, int& statusCode) {
//     // TODO: Implement POST logic
//     statusCode = 201;
//     responseBody = R"({"message": "${ENDPOINT_PART} created successfully"})";
// }

} // namespace $NAMESPACE_NAME

// METAPROGRAMMING MAGIC: One line replaces entire registration struct!
namespace {
	static apirouter::AutoRegister<$NAMESPACE_NAME::$ENDPOINT_NAME> _autoRegister${ENDPOINT_NAME};
}
EOF

# Create test case file for metaprogramming endpoint registration
cat > "tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}RegisterTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$ENDPOINT_NAME/$ENDPOINT_NAME.h>
#include <ApiRouter/IEndpointRegistrar.h>
#include <vector>
#include <string>

class MockEndpointRegistrar : public apirouter::IEndpointRegistrar {
public:
	std::vector<std::string> registeredPaths;
	void registerHttpHandler(std::string_view path, std::string_view method, apirouter::HttpHandler /*handler*/) override {
		registeredPaths.push_back(std::string(path) + ":" + std::string(method));
	}
};

TEST(${ENDPOINT_NAME}Test, MetaprogrammingRegistrationWorks) {
	MockEndpointRegistrar registrar;
	$NAMESPACE_NAME::$ENDPOINT_NAME endpoint;
	
	// Test the metaprogramming auto-registration method
	endpoint.registerAvailableMethods(registrar, "/${ENDPOINT_PART,,}");
	
	// Verify that the endpoint registered its handler via metaprogramming
	ASSERT_EQ(registrar.registeredPaths.size(), 1U);
	EXPECT_EQ(registrar.registeredPaths[0], "/${ENDPOINT_PART,,}:GET");
}

TEST(${ENDPOINT_NAME}Test, BaseRegistrationMethodWorks) {
	MockEndpointRegistrar registrar;
	$NAMESPACE_NAME::$ENDPOINT_NAME endpoint;
	
	// Test the base IApiModule registerEndpoints method (calls metaprogramming internally)
	endpoint.registerEndpoints(registrar);
	
	// Should work the same way via the base class interface
	ASSERT_EQ(registrar.registeredPaths.size(), 1U);
	EXPECT_EQ(registrar.registeredPaths[0], "/${ENDPOINT_PART,,}:GET");
}
EOF

# Create handler test file using cleaner method names
cat > "tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}HandlerTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$ENDPOINT_NAME/$ENDPOINT_NAME.h>
#include <string>

class ${ENDPOINT_NAME}HandlerTest : public ::testing::Test, protected $NAMESPACE_NAME::$ENDPOINT_NAME {
protected:
	// Test class inherits from $ENDPOINT_NAME to access protected handler methods
};

TEST_F(${ENDPOINT_NAME}HandlerTest, HandleGetReturnsSuccessResponse) {
	std::string responseBody;
	int statusCode = 0;
	
	// Test the handler method directly using clean naming convention
	handleGet("/${ENDPOINT_PART,,}", "GET", "", responseBody, statusCode);
	
	// Verify response
	EXPECT_EQ(statusCode, 200);
	EXPECT_FALSE(responseBody.empty());
	EXPECT_NE(responseBody.find("$ENDPOINT_NAME"), std::string::npos);
	EXPECT_NE(responseBody.find("metaprogramming"), std::string::npos);
}

TEST_F(${ENDPOINT_NAME}HandlerTest, HandleGetAcceptsParameters) {
	std::string responseBody;
	int statusCode = 0;
	std::string requestBody = R"({"test": "data"})";
	
	// Test with request body
	handleGet("/${ENDPOINT_PART,,}/123", "GET", requestBody, responseBody, statusCode);
	
	// Should still return success (parameters are ignored in this simple implementation)
	EXPECT_EQ(statusCode, 200);
	EXPECT_FALSE(responseBody.empty());
}

// TODO: Add tests for additional handler methods when implemented
// TEST_F(${ENDPOINT_NAME}HandlerTest, HandlePostCreatesResource) {
//     std::string responseBody;
//     int statusCode = 0;
//     std::string requestBody = R"({"name": "test${ENDPOINT_PART}"})";
//     
//     handlePost("/${ENDPOINT_PART,,}", "POST", requestBody, responseBody, statusCode);
//     
//     EXPECT_EQ(statusCode, 201);
//     EXPECT_FALSE(responseBody.empty());
// }
EOF

# Create auto-registration cross-check test
cat > "tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}AutoRegistrationTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$ENDPOINT_NAME/$ENDPOINT_NAME.h>
#include <ApiRouter/ApiRouter.h>
#include <ApiRouter/IEndpointRegistrar.h>
#include <vector>
#include <string>
#include <memory>

class MockEndpointRegistrar : public apirouter::IEndpointRegistrar {
public:
	std::vector<std::string> registeredPaths;
	void registerHttpHandler(std::string_view path, std::string_view method, apirouter::HttpHandler /*handler*/) override {
		registeredPaths.push_back(std::string(path) + ":" + std::string(method));
	}
};

TEST(${ENDPOINT_NAME}AutoRegistrationTest, ${ENDPOINT_NAME}IsAutoRegisteredWithApiRouter) {
	// Check that at least one module factory is registered
	size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();
	ASSERT_GE(moduleCount, 1U) << "No endpoint modules were auto-registered";
	
	// Create instances of all registered modules
	auto modules = apirouter::ApiRouter::createAllModulesGlobal();
	ASSERT_EQ(modules.size(), moduleCount) << "Module creation count mismatch";
	
	// Check if any of the modules is $ENDPOINT_NAME
	bool found${ENDPOINT_NAME} = false;
	for (const auto& module : modules) {
		ASSERT_NE(module, nullptr) << "Module instance is null";
		
		// Try to dynamic_cast to $ENDPOINT_NAME to verify the type
		auto ${ENDPOINT_PART,,}Endpoint = dynamic_cast<$NAMESPACE_NAME::$ENDPOINT_NAME*>(module.get());
		if (${ENDPOINT_PART,,}Endpoint != nullptr) {
			found${ENDPOINT_NAME} = true;
			
			// Verify the module can register endpoints
			MockEndpointRegistrar registrar;
			${ENDPOINT_PART,,}Endpoint->registerEndpoints(registrar);
			
			// Verify that the endpoint registered its handler correctly
			ASSERT_EQ(registrar.registeredPaths.size(), 1U) << "$ENDPOINT_NAME should register exactly one endpoint";
			EXPECT_EQ(registrar.registeredPaths[0], "/${ENDPOINT_PART,,}:GET") << "$ENDPOINT_NAME should register /${ENDPOINT_PART,,}:GET endpoint";
			
			break;
		}
	}
	
	ASSERT_TRUE(found${ENDPOINT_NAME}) << "$ENDPOINT_NAME was not found in auto-registered modules";
}

TEST(${ENDPOINT_NAME}AutoRegistrationTest, ApiRouterCanInstantiateAllRegisteredModules) {
	// Get count of registered module factories
	size_t moduleCount = apirouter::ApiRouter::getRegisteredModuleCountGlobal();

	// Create instances of all registered modules
	auto modules = apirouter::ApiRouter::createAllModulesGlobal();
	ASSERT_EQ(modules.size(), moduleCount) << "Not all modules could be instantiated";
	
	// Verify each module can be used to register endpoints
	for (const auto& module : modules) {
		ASSERT_NE(module, nullptr) << "Module instance is null";
		
		MockEndpointRegistrar registrar;
		module->registerEndpoints(registrar);
		
		// The test passes if no exceptions are thrown
		SUCCEED();
	}
}
EOF

# Create test runner
cat > "tests/${ENDPOINT_NAME}Test/TestMain.cpp" << EOF
#include <gtest/gtest.h>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
EOF

# Create Makefile for tests using the flexible template
cat << 'EOF' > "tests/${ENDPOINT_NAME}Test/Makefile"
# Modern, flexible Makefile for unit tests (Google Test & Google Mock)
# Supports easy addition of services, modules, and dependencies
# Object files are organized by source origin:
#   obj/test/     - test files (cases/, TestMain.cpp)
#   obj/src/      - our source code (modules, services, etc.)
#   obj/external/ - external dependencies

# ============================================================================
# CONFIGURATION SECTION - Modify this section to add/remove dependencies
# ============================================================================

ROOTDIR = ../..
MODULE_NAME = MODULE_NAME_PLACEHOLDER

# Primary module being tested (automatically included)
PRIMARY_MODULE = $(MODULE_NAME)

# Additional dependencies - add any services, modules, or utilities needed
# Format: ModuleName:FolderName (if folder differs from module name, otherwise just ModuleName)
# Examples: Logger IoCContainer ServiceA ApiRouter
DEPENDENCIES = ApiRouter IocContainer

# External dependencies - flexible configuration for any external library
# Add external source files that need to be compiled with your tests
# Examples:
#   EXTERNAL_SOURCES = external/mongoose/src/mongoose.c external/sqlite/sqlite3.c
#   EXTERNAL_SOURCES = /usr/local/src/mylib.cpp ../shared/utils.cpp
EXTERNAL_SOURCES = 

# External include paths - add any custom include directories
# Examples:
#   EXTERNAL_INCLUDES = -Iexternal/mongoose/include -Iexternal/sqlite/include
#   EXTERNAL_INCLUDES = -I/usr/local/include -I../shared/include
EXTERNAL_INCLUDES = -I$(ROOTDIR)/external/googletest/googletest/include 
EXTERNAL_INCLUDES += -I$(ROOTDIR)/external/googletest/googlemock/include

# External library paths and libraries
# Examples:
#   EXTERNAL_LIBS = -Lexternal/foo/lib -lfoo -lbar
#   EXTERNAL_LIBS = -L/usr/local/lib -lsqlite3 -lpthread
EXTERNAL_LIBS = -L$(ROOTDIR)/external/googletest/build/lib 
EXTERNAL_LIBS += -lgtest -lgtest_main 
EXTERNAL_LIBS += -lgmock -lgmock_main

# ============================================================================
# BUILD CONFIGURATION - Usually no changes needed below this line
# ============================================================================

# Include compiler configuration
-include $(ROOTDIR)/Compiler.build

CXX = g++
CC = gcc
CXXFLAGS = -std=$(CXX_STANDARD) -I$(ROOTDIR)/include $(EXTERNAL_INCLUDES) -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include $(EXTERNAL_INCLUDES) -g -Wall -Wextra -MMD -MP
LDLIBS = $(EXTERNAL_LIBS)

OBJDIR = obj
BINDIR = bin

# Process dependencies into source files and object files
# Handle both simple format (Name) and complex format (Name:Folder)
DEP_SOURCES = $(foreach dep,$(DEPENDENCIES),$(ROOTDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).cpp)
DEP_OBJECTS = $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).o)
DEP_NAMES = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)))
DEP_FOLDERS = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep)))

# Process external sources into object files (flexible user-defined sources)
EXT_OBJECTS = $(foreach src,$(EXTERNAL_SOURCES),$(OBJDIR)/external/$(notdir $(basename $(src))).o)

# Complete source and object lists (organized by source origin)
ALL_SOURCES = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp $(DEP_SOURCES) $(EXTERNAL_SOURCES)
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/test/cases/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/test/TestMain.o $(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).o $(DEP_OBJECTS) $(EXT_OBJECTS)
TEST_DEPS = $(patsubst cases/%.cpp,$(OBJDIR)/test/cases/%.d,$(wildcard cases/*.cpp)) $(OBJDIR)/test/TestMain.d $(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).d $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).d)
TEST_BIN = $(BINDIR)/$(MODULE_NAME)Test

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# ============================================================================
# COMPILATION RULES - Organized by source origin
# ============================================================================

# Test files: obj/test/
# Pattern rule for C++ test cases in cases/
$(OBJDIR)/test/cases/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for the test runner
$(OBJDIR)/test/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Source files: obj/src/
# Rule for primary module source
$(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).o: $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule for dependency modules (single % in target per GNU Make requirements)
$(OBJDIR)/src/%.o: $(ROOTDIR)/src/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# External files: obj/external/
# Flexible rule for user-defined external sources (any path, any extension)
# Note: External sources don't generate dependency files to avoid auto-generation issues
EXTERNAL_CXXFLAGS = $(filter-out -MMD -MP,$(CXXFLAGS))
EXTERNAL_CFLAGS = $(filter-out -MMD -MP,$(CFLAGS))

$(OBJDIR)/external/%.o: | $(OBJDIR)
	mkdir -p $(dir $@)
	$(eval SOURCE_FILE := $(filter %/$(notdir $(basename $@)).cpp %/$(notdir $(basename $@)).c,$(EXTERNAL_SOURCES)))
	$(if $(filter %.cpp,$(SOURCE_FILE)),$(CXX) $(EXTERNAL_CXXFLAGS) -c $(SOURCE_FILE) -o $@,$(CC) $(EXTERNAL_CFLAGS) -c $(SOURCE_FILE) -o $@)

# Link the test binary
$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) -pthread

# Include dependency files for incremental builds
-include $(TEST_DEPS)

# ============================================================================
# UTILITY TARGETS
# ============================================================================

# Debug: Print configuration for troubleshooting
debug-config:
	@echo "=== Makefile Configuration ==="
	@echo "MODULE_NAME: $(MODULE_NAME)"
	@echo "PRIMARY_MODULE: $(PRIMARY_MODULE)"
	@echo "DEPENDENCIES: $(DEPENDENCIES)"
	@echo "DEP_NAMES: $(DEP_NAMES)"
	@echo "DEP_SOURCES: $(DEP_SOURCES)"
	@echo "DEP_OBJECTS: $(DEP_OBJECTS)"
ifneq ($(EXTERNAL_SOURCES),)
	@echo "EXTERNAL_SOURCES: $(EXTERNAL_SOURCES)"
	@echo "EXT_OBJECTS: $(EXT_OBJECTS)"
endif
ifneq ($(EXTERNAL_INCLUDES),)
	@echo "EXTERNAL_INCLUDES: $(EXTERNAL_INCLUDES)"
endif
ifneq ($(EXTERNAL_LIBS),)
	@echo "EXTERNAL_LIBS: $(EXTERNAL_LIBS)"
endif
	@echo "TEST_OBJS: $(TEST_OBJS)"
	@echo "TEST_BIN: $(TEST_BIN)"
	@echo ""
	@echo "=== Directory Structure ==="
	@echo "obj/test/     - test files (cases/, TestMain.cpp)"
	@echo "obj/src/      - our source code (modules, services, etc.)"
	@echo "obj/external/ - external dependencies (user-defined)"

run: all
	@./$(TEST_BIN)

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*

.PHONY: all clean debug-config run
EOF

	# Replace MODULE_NAME_PLACEHOLDER in the Makefile
	sed -i "s/MODULE_NAME_PLACEHOLDER/$ENDPOINT_NAME/g" "tests/${ENDPOINT_NAME}Test/Makefile"

	echo ""
	echo "✅ Endpoint module '$ENDPOINT_NAME' created successfully!"
	echo ""
	echo "📁 Created files:"
	echo "  📄 include/$ENDPOINT_NAME/$ENDPOINT_NAME.h (header with testable protected methods)"
	echo "  📄 src/$ENDPOINT_NAME/$ENDPOINT_NAME.cpp (implementation + auto-registration)"
	echo "  📄 tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}RegisterTest.cpp (registration tests)"
	echo "  📄 tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}HandlerTest.cpp (handler method tests)"
	echo "  📄 tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}AutoRegistrationTest.cpp (auto-registration tests)"
	echo "  📄 tests/${ENDPOINT_NAME}Test/TestMain.cpp"
	echo "  📄 tests/${ENDPOINT_NAME}Test/Makefile"
	echo ""
	echo "📋 Next steps:"
	echo "1. 🔧 Implement handleGet() method in src/$ENDPOINT_NAME/$ENDPOINT_NAME.cpp"
	echo "2. 🔧 Add additional handler methods (handlePost, handlePut, handleDelete) as needed"
	echo "3. 🔧 Add registerMethod calls for new handlers in registerAvailableMethods()"
	echo "4. 🧪 Update tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}HandlerTest.cpp with comprehensive tests"
	echo "5. 📦 Add service dependencies in tests/${ENDPOINT_NAME}Test/Makefile if needed"
	echo "   Examples: DEPENDENCIES = ServiceUser ServiceAuth Logger"
	echo "6. 🧪 Test your endpoint:"
	echo "   make test-run-${ENDPOINT_NAME}Test"
	echo ""
	echo "🚀 Metaprogramming benefits:"
	echo "  • 85% less boilerplate code compared to manual registration"
	echo "  • No manual lambda wrapping required"
	echo "  • Type-safe method registration with compile-time verification"
	echo "  • Clean naming convention: handleGet vs handleGet${ENDPOINT_PART}"
	echo "  • One-line registration with AutoRegister template"
	echo "  • CRTP pattern for zero runtime overhead"
	echo "  • Easy to add new HTTP methods with single registerMethod call"
	echo ""
	echo "💡 Tips:"
	echo "  • Test handler methods directly for better test coverage"
	echo "  • Use meaningful HTTP status codes (200, 201, 400, 404, 500)"
	echo "  • Return JSON responses for API consistency"
	echo "  • Follow KISS principle: simple, readable code"
	echo "  • Follow YAGNI principle: only implement methods you need"
	echo ""
}

# Function to remove endpoint
remove_endpoint() {
	echo "Removing endpoint module: $ENDPOINT_NAME"
	
	# Check if endpoint exists
	ENDPOINT_EXISTS=false
	if [ -d "include/$ENDPOINT_NAME" ] || [ -d "src/$ENDPOINT_NAME" ] || [ -d "tests/${ENDPOINT_NAME}Test" ]; then
		ENDPOINT_EXISTS=true
	fi
	
	if [ "$ENDPOINT_EXISTS" = false ]; then
		echo "Warning: Endpoint '$ENDPOINT_NAME' does not exist or is already removed."
		exit 0
	fi
	
	# Confirm removal
	echo "This will permanently delete the following:"
	[ -d "include/$ENDPOINT_NAME" ] && echo "  📁 include/$ENDPOINT_NAME/"
	[ -d "src/$ENDPOINT_NAME" ] && echo "  📁 src/$ENDPOINT_NAME/"
	[ -d "tests/${ENDPOINT_NAME}Test" ] && echo "  📁 tests/${ENDPOINT_NAME}Test/"
	echo ""
	read -p "Are you sure you want to remove '$ENDPOINT_NAME'? (y/N): " -r
	if [[ ! $REPLY =~ ^[Yy]$ ]]; then
		echo "Removal cancelled."
		exit 0
	fi
	
	# Remove directories and files
	[ -d "include/$ENDPOINT_NAME" ] && rm -rf "include/$ENDPOINT_NAME"
	[ -d "src/$ENDPOINT_NAME" ] && rm -rf "src/$ENDPOINT_NAME"
	[ -d "tests/${ENDPOINT_NAME}Test" ] && rm -rf "tests/${ENDPOINT_NAME}Test"
	
	echo ""
	echo "✅ Endpoint module '$ENDPOINT_NAME' removed successfully!"
	echo ""
	echo "Don't forget to:"
	echo "1. The endpoint was auto-registered, so no manual removal from ApiRouter.cpp needed"
	echo "2. Clean any build artifacts:"
	echo "   make clean && make test-clean"
	echo ""
}

# Execute based on action
case "$ACTION" in
	"create")
		create_endpoint
		;;
	"remove")
		remove_endpoint
		;;
	*)
		echo "Error: Unknown action '$ACTION'"
		exit 1
		;;
esac
