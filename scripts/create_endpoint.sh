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
# Examples: Logger IoCContainer ServiceA ApiModule:ApiModule
DEPENDENCIES = ApiModule IocContainer

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

CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -I$(ROOTDIR)/include $(EXTERNAL_INCLUDES) -g -Wall -Wextra -MMD -MP
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
