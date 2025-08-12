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
#include <ApiModule/ApiModule.h>
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

// Auto-registration: Register this endpoint module with ApiModule
namespace {
    static bool registered = []() {
        apimodule::ApiModule::registerModuleFactory([]() -> std::unique_ptr<apimodule::IApiModule> {
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
#include <ApiModule/ApiModule.h>
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

TEST(${ENDPOINT_NAME}AutoRegistrationTest, ${ENDPOINT_NAME}IsAutoRegisteredWithApiModule) {
    // Check that at least one module factory is registered
    size_t moduleCount = apimodule::ApiModule::getRegisteredModuleCount();
    ASSERT_GE(moduleCount, 1U) << "No endpoint modules were auto-registered";
    
    // Create instances of all registered modules
    auto modules = apimodule::ApiModule::createAllModules();
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

TEST(${ENDPOINT_NAME}AutoRegistrationTest, ApiModuleCanInstantiateAllRegisteredModules) {
    // Get count of registered module factories
    size_t moduleCount = apimodule::ApiModule::getRegisteredModuleCount();

    // Create instances of all registered modules
    auto modules = apimodule::ApiModule::createAllModules();
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
# Modern, flexible Makefile for unit tests (Google Test)
# Supports easy addition of services, modules, and dependencies
# All object and dependency files are placed flat in obj/ (not nested)

# ============================================================================
# CONFIGURATION SECTION - Modify this section to add/remove dependencies
# ============================================================================

ROOTDIR = ../..
MODULE_NAME = MODULE_NAME_PLACEHOLDER

# Primary module being tested (automatically included)
PRIMARY_MODULE = $(MODULE_NAME)

# Additional dependencies - add any services, modules, or utilities needed
# Format: ModuleName:FolderName (if folder differs from module name, otherwise just ModuleName)
DEPENDENCIES = ApiModule IoCContainer

# External dependencies (from external/ folder) - uncomment if needed
# EXTERNAL_DEPS = mongoose foo

# ============================================================================
# BUILD CONFIGURATION - Usually no changes needed below this line
# ============================================================================

CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin

# Process dependencies into source files and object files
# Handle both simple format (Name) and complex format (Name:Folder)
DEP_SOURCES = $(foreach dep,$(DEPENDENCIES),$(ROOTDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).cpp)
DEP_OBJECTS = $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).o)
DEP_NAMES = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)))
DEP_FOLDERS = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep)))

# External dependencies (if any)
ifdef EXTERNAL_DEPS
EXT_SOURCES = $(foreach ext,$(EXTERNAL_DEPS),$(ROOTDIR)/external/$(ext)/src/$(ext).cpp)
EXT_OBJECTS = $(foreach ext,$(EXTERNAL_DEPS),$(OBJDIR)/$(ext).o)
EXT_NAMES = $(EXTERNAL_DEPS)
else
EXT_SOURCES =
EXT_OBJECTS =
EXT_NAMES =
endif

# Complete source and object lists
ALL_SOURCES = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp $(DEP_SOURCES) $(EXT_SOURCES)
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/$(PRIMARY_MODULE).o $(DEP_OBJECTS) $(EXT_OBJECTS)
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/$(PRIMARY_MODULE).d $(foreach dep,$(DEP_NAMES),$(OBJDIR)/$(dep).d) $(foreach ext,$(EXT_NAMES),$(OBJDIR)/$(ext).d)
TEST_BIN = $(BINDIR)/$(MODULE_NAME)Test

all: $(OBJDIR) $(BINDIR) $(TEST_BIN)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# ============================================================================
# COMPILATION RULES - Generic patterns that work for any module
# ============================================================================

# Pattern rule for C++ test cases in cases/
$(OBJDIR)/%.o: cases/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for the test runner
$(OBJDIR)/TestMain.o: TestMain.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for primary module source
$(OBJDIR)/$(PRIMARY_MODULE).o: $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule for dependency modules
$(OBJDIR)/%.o: $(ROOTDIR)/src/%/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule for external dependencies
$(OBJDIR)/%.o: $(ROOTDIR)/external/%/src/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for C sources (if any)
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link the test binary
$(TEST_BIN): $(TEST_OBJS) | $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) -pthread

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
ifdef EXTERNAL_DEPS
	@echo "EXTERNAL_DEPS: $(EXTERNAL_DEPS)"
	@echo "EXT_SOURCES: $(EXT_SOURCES)"
	@echo "EXT_OBJECTS: $(EXT_OBJECTS)"
endif
	@echo "TEST_OBJS: $(TEST_OBJS)"
	@echo "TEST_BIN: $(TEST_BIN)"

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
    echo "4. The endpoint will auto-register with ApiModule (no manual integration needed!)"
    echo "5. To add dependencies, edit the DEPENDENCIES line in tests/${ENDPOINT_NAME}Test/Makefile"
    echo "   Examples: DEPENDENCIES = ServiceA ServiceB Logger"
    echo "   Use 'make debug-config' in the test folder to verify dependency resolution"
    echo "6. Test your endpoint:"
    echo "   make test-run-${ENDPOINT_NAME}Test"
    echo "7. The auto-registration cross-check will verify your endpoint is properly registered"
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
    echo "1. The endpoint was auto-registered, so no manual removal from ApiModule.cpp needed"
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
