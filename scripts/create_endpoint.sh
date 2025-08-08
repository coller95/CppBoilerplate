#!/bin/bash

# Script to create or remove endpoint modules following the modular endpoint convention
# Usage: ./scripts/create_endpoint.sh <create|remove> EndpointName

set -e

# Resolve script location and set project root
SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
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

# Create header file
cat > "include/$ENDPOINT_NAME/$ENDPOINT_NAME.h" << EOF
#pragma once
#include <ApiModule/IApiModule.h>

namespace $NAMESPACE_NAME {
    class $ENDPOINT_NAME : public apimodule::IApiModule {
    public:
        void registerEndpoints(IEndpointRegistrar& registrar) override;
    };
}
EOF

# Create source file
cat > "src/$ENDPOINT_NAME/$ENDPOINT_NAME.cpp" << EOF
#include <$ENDPOINT_NAME/$ENDPOINT_NAME.h>
#include <ApiModule/IEndpointRegistrar.h>
#include <ApiModule/ApiModules.h>
#include <string_view>
#include <string>
#include <memory>

namespace $NAMESPACE_NAME {

void $ENDPOINT_NAME::registerEndpoints(IEndpointRegistrar& registrar) {
    registrar.registerHttpHandler("/${ENDPOINT_PART,,}", "GET",
       [](std::string_view, std::string_view, const std::string&, std::string& responseBody, int& statusCode) {
           statusCode = 200;
           responseBody = "Hello from $ENDPOINT_NAME!\\n";
       });
}

// Auto-registration: Register this endpoint module with ApiModules
namespace {
    static bool registered = []() {
        apimodule::ApiModules::registerModuleFactory([]() -> std::unique_ptr<apimodule::IApiModule> {
            return std::make_unique<$ENDPOINT_NAME>();
        });
        return true;
    }();
}

} // namespace $NAMESPACE_NAME
EOF

# Create test case file
cat > "tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}RegisterTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$ENDPOINT_NAME/$ENDPOINT_NAME.h>
#include <ApiModule/IEndpointRegistrar.h>
#include <vector>
#include <string>

class MockEndpointRegistrar : public IEndpointRegistrar {
public:
    std::vector<std::string> registeredPaths;
    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler /*handler*/) override {
        registeredPaths.push_back(std::string(path) + ":" + std::string(method));
    }
};

TEST(${ENDPOINT_NAME}Test, RegisterEndpointsCanBeRegistered) {
    MockEndpointRegistrar registrar;
    $NAMESPACE_NAME::$ENDPOINT_NAME endpoint;
    endpoint.registerEndpoints(registrar);
    
    // TODO: Update this test based on your actual endpoints
    // Example:
    // ASSERT_EQ(registrar.registeredPaths.size(), 1U);
    // EXPECT_EQ(registrar.registeredPaths[0], "/${ENDPOINT_PART,,}:GET");
    
    // For now, just verify the method can be called without errors
    SUCCEED();
}
EOF

# Create auto-registration cross-check test
cat > "tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}AutoRegistrationTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$ENDPOINT_NAME/$ENDPOINT_NAME.h>
#include <ApiModule/ApiModules.h>
#include <ApiModule/IEndpointRegistrar.h>
#include <vector>
#include <string>
#include <memory>

class MockEndpointRegistrar : public IEndpointRegistrar {
public:
    std::vector<std::string> registeredPaths;
    void registerHttpHandler(std::string_view path, std::string_view method, HttpHandler /*handler*/) override {
        registeredPaths.push_back(std::string(path) + ":" + std::string(method));
    }
};

TEST(${ENDPOINT_NAME}AutoRegistrationTest, ${ENDPOINT_NAME}IsAutoRegisteredWithApiModules) {
    // Check that at least one module factory is registered
    size_t moduleCount = apimodule::ApiModules::getRegisteredModuleCount();
    ASSERT_GE(moduleCount, 1U) << "No endpoint modules were auto-registered";
    
    // Create instances of all registered modules
    auto modules = apimodule::ApiModules::createAllModules();
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
            
            // TODO: Update this based on your actual endpoints
            // Example:
            // ASSERT_EQ(registrar.registeredPaths.size(), 1U) << "$ENDPOINT_NAME should register exactly one endpoint";
            // EXPECT_EQ(registrar.registeredPaths[0], "/${ENDPOINT_PART,,}:GET") << "$ENDPOINT_NAME should register /${ENDPOINT_PART,,}:GET endpoint";
            
            // For now, just verify the method executes without error
            SUCCEED();
            break;
        }
    }
    
    ASSERT_TRUE(found${ENDPOINT_NAME}) << "$ENDPOINT_NAME was not found in auto-registered modules";
}

TEST(${ENDPOINT_NAME}AutoRegistrationTest, ApiModulesCanInstantiateAllRegisteredModules) {
    // Get count of registered module factories
    size_t moduleCount = apimodule::ApiModules::getRegisteredModuleCount();
    
    // Create instances of all registered modules
    auto modules = apimodule::ApiModules::createAllModules();
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

# Create Makefile for tests
cat << 'EOF' > "tests/${ENDPOINT_NAME}Test/Makefile"
# Modern, robust Makefile for EndpointName unit tests (Google Test)
# All object and dependency files are placed flat in obj/ (not nested)

ROOTDIR = ../..
CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin
SERVICE = EndpointName

TEST_SRC = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(SERVICE)/$(SERVICE).cpp $(ROOTDIR)/src/ApiModule/ApiModules.cpp
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/$(SERVICE).o $(OBJDIR)/ApiModules.o
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/$(SERVICE).d $(OBJDIR)/ApiModules.d
TEST_BIN = $(BINDIR)/$(SERVICE)Test

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Pattern rule for C++ test cases in cases/
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for the test runner
$(OBJDIR)/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for sources outside test dir
$(OBJDIR)/$(SERVICE).o: $(ROOTDIR)/src/$(SERVICE)/$(SERVICE).cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for ApiModules dependency
$(OBJDIR)/ApiModules.o: $(ROOTDIR)/src/ApiModule/ApiModules.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for C sources (if any)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

-include $(TEST_DEPS)

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*
EOF

    # Replace EndpointName placeholders in the Makefile
    sed -i "s/EndpointName/$ENDPOINT_NAME/g" "tests/${ENDPOINT_NAME}Test/Makefile"

    echo ""
    echo "✅ Endpoint module '$ENDPOINT_NAME' created successfully!"
    echo ""
    echo "Created files:"
    echo "  📁 include/$ENDPOINT_NAME/$ENDPOINT_NAME.h"
    echo "  📁 src/$ENDPOINT_NAME/$ENDPOINT_NAME.cpp"
    echo "  📁 tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}RegisterTest.cpp"
    echo "  📁 tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}AutoRegistrationTest.cpp"
    echo "  📁 tests/${ENDPOINT_NAME}Test/TestMain.cpp"
    echo "  📁 tests/${ENDPOINT_NAME}Test/Makefile"
    echo ""
    echo "Next steps:"
    echo "1. Edit src/$ENDPOINT_NAME/$ENDPOINT_NAME.cpp to implement your endpoints"
    echo "2. Update tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}RegisterTest.cpp with proper test assertions"
    echo "3. Update tests/${ENDPOINT_NAME}Test/cases/${ENDPOINT_NAME}AutoRegistrationTest.cpp with endpoint verification"
    echo "4. The endpoint will auto-register with ApiModules (no manual integration needed!)"
    echo "5. Test your endpoint:"
    echo "   make test-run-${ENDPOINT_NAME}Test"
    echo "6. The auto-registration cross-check will verify your endpoint is properly registered"
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
    echo "1. The endpoint was auto-registered, so no manual removal from ApiModules.cpp needed"
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
