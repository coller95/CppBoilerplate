#!/bin/bash

# Script to create or remove service modules following the modular service convention
# Usage: ./scripts/create_service.sh <create|remove> ServiceName

set -e

# Resolve script location and set project root
SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
PROJECT_ROOT="$(dirname "$SCRIPTDIR")"
cd "$PROJECT_ROOT"

# Check if action and service name are provided
if [ $# -lt 2 ]; then
    echo "Usage: $0 <create|remove> <ServiceName>"
    echo "Examples:"
    echo "  $0 create ServiceUser    # Create a new service"
    echo "  $0 remove ServiceUser    # Remove an existing service"
    exit 1
fi

ACTION="$1"
SERVICE_NAME="$2"

# Validate action
if [[ "$ACTION" != "create" && "$ACTION" != "remove" ]]; then
    echo "Error: Action must be 'create' or 'remove'"
    exit 1
fi

# Validate service name format (should start with "Service")
if [[ ! "$SERVICE_NAME" =~ ^Service[A-Z][a-zA-Z0-9]*$ ]]; then
    echo "Error: Service name must start with 'Service' followed by a capitalized name"
    echo "Examples: ServiceUser, ServiceAuth, ServiceProduct"
    exit 1
fi

# Extract the service part (e.g., "User" from "ServiceUser")
SERVICE_PART="${SERVICE_NAME#Service}"
NAMESPACE_NAME="service${SERVICE_PART,,}"  # Convert to lowercase

# Function to create service
create_service() {
    echo "Creating service module: $SERVICE_NAME"
    echo "Namespace: $NAMESPACE_NAME"

    # Check if service already exists
    if [ -d "include/$SERVICE_NAME" ] || [ -d "src/$SERVICE_NAME" ] || [ -d "tests/${SERVICE_NAME}Test" ]; then
        echo "Error: Service '$SERVICE_NAME' already exists!"
        echo "Use 'remove' action first if you want to recreate it."
        exit 1
    fi

    # Create directories
    mkdir -p "include/$SERVICE_NAME"
    mkdir -p "src/$SERVICE_NAME"
    mkdir -p "tests/${SERVICE_NAME}Test/cases"

# Create service header
cat > "include/$SERVICE_NAME/$SERVICE_NAME.h" << EOF
#pragma once
#include <string>
#include <memory>
#include <IocContainer/IocContainer.h>

namespace $NAMESPACE_NAME {

/**
 * Interface for $SERVICE_NAME
 * Define the service contract for dependency injection and testing
 */
class I$SERVICE_NAME {
public:
    virtual ~I$SERVICE_NAME() = default;
    
    // TODO: Define your service interface methods here
    virtual std::string process${SERVICE_PART}Data() const = 0;
};

/**
 * $SERVICE_NAME - Business logic service
 * Automatically registers with IoC container for dependency injection
 */
class $SERVICE_NAME : public I$SERVICE_NAME {
public:
    $SERVICE_NAME();
    
    // Service interface implementation
    std::string process${SERVICE_PART}Data() const override;
    
    // TODO: Add additional service methods here
    // Example: void update${SERVICE_PART}(const ${SERVICE_PART}& data);
};

}
EOF

# Create service implementation
cat > "src/$SERVICE_NAME/$SERVICE_NAME.cpp" << EOF
#include <$SERVICE_NAME/$SERVICE_NAME.h>

namespace $NAMESPACE_NAME {

$SERVICE_NAME::$SERVICE_NAME() = default;

std::string $SERVICE_NAME::process${SERVICE_PART}Data() const {
    // TODO: Implement your business logic here
    return "$SERVICE_PART data processed successfully";
}

// Auto-registration: Register both interface and concrete class with IoC container
namespace {
    struct ${SERVICE_NAME}Registration {
        ${SERVICE_NAME}Registration() {
            // Register both interface and concrete implementation
            ioccontainer::IIocContainer::registerGlobal<I$SERVICE_NAME>(
                []() { return std::make_shared<$SERVICE_NAME>(); }
            );
            ioccontainer::IIocContainer::registerGlobal<$SERVICE_NAME>(
                []() { return std::make_shared<$SERVICE_NAME>(); }
            );
        }
    };
    static ${SERVICE_NAME}Registration _registration;
}

} // namespace $NAMESPACE_NAME
EOF

# Create test main
cat > "tests/${SERVICE_NAME}Test/TestMain.cpp" << EOF
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
EOF

# Create mock header for testing
cat > "tests/${SERVICE_NAME}Test/Mock$SERVICE_NAME.h" << EOF
#pragma once

#include <gmock/gmock.h>
#include <$SERVICE_NAME/$SERVICE_NAME.h>

namespace $NAMESPACE_NAME {

/**
 * Mock implementation of I$SERVICE_NAME for testing
 */
class Mock$SERVICE_NAME : public I$SERVICE_NAME {
public:
    MOCK_METHOD(std::string, process${SERVICE_PART}Data, (), (const, override));
    
    // TODO: Add more mock methods as you expand your interface
    // Example:
    // MOCK_METHOD(void, update${SERVICE_PART}, (const ${SERVICE_PART}&), (override));
};

} // namespace $NAMESPACE_NAME
EOF

# Create basic test case
cat > "tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}BasicTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$SERVICE_NAME/$SERVICE_NAME.h>

TEST(${SERVICE_NAME}Test, Process${SERVICE_PART}DataReturnsExpected) {
    $NAMESPACE_NAME::$SERVICE_NAME service;
    std::string result = service.process${SERVICE_PART}Data();
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("$SERVICE_PART"), std::string::npos);
}

TEST(${SERVICE_NAME}Test, ImplementsInterface) {
    // Verify that $SERVICE_NAME implements I$SERVICE_NAME
    $NAMESPACE_NAME::$SERVICE_NAME service;
    $NAMESPACE_NAME::I$SERVICE_NAME* interface = &service;
    EXPECT_NE(interface, nullptr);
}
EOF

# Create registration test
cat > "tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}RegistrationTest.cpp" << EOF
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <IocContainer/IocContainer.h>
#include <$SERVICE_NAME/$SERVICE_NAME.h>
#include "../Mock$SERVICE_NAME.h"

using ::testing::Return;

TEST(${SERVICE_NAME}RegistrationTest, ConcreteServiceIsAutoRegistered) {
    // Verify the concrete service is automatically registered in the global container
    EXPECT_TRUE(ioccontainer::IIocContainer::isRegisteredGlobal<$NAMESPACE_NAME::$SERVICE_NAME>());
    
    // Verify we can resolve the concrete service globally
    EXPECT_NO_THROW({
        auto ptr = ioccontainer::IIocContainer::resolveGlobal<$NAMESPACE_NAME::$SERVICE_NAME>();
        EXPECT_NE(ptr, nullptr);
        std::string result = ptr->process${SERVICE_PART}Data();
        EXPECT_FALSE(result.empty());
    });
}

TEST(${SERVICE_NAME}RegistrationTest, InterfaceIsAutoRegistered) {
    // Verify the interface is automatically registered in the global container
    EXPECT_TRUE(ioccontainer::IIocContainer::isRegisteredGlobal<$NAMESPACE_NAME::I$SERVICE_NAME>());
    
    // Verify we can resolve via interface globally (returns concrete implementation)
    EXPECT_NO_THROW({
        auto ptr = ioccontainer::IIocContainer::resolveGlobal<$NAMESPACE_NAME::I$SERVICE_NAME>();
        EXPECT_NE(ptr, nullptr);
        std::string result = ptr->process${SERVICE_PART}Data();
        EXPECT_FALSE(result.empty());
    });
}

TEST(${SERVICE_NAME}RegistrationTest, MockCanBeUsedForTesting) {
    // Test that our mock can be used to replace the service in tests
    auto mockService = std::make_shared<$NAMESPACE_NAME::Mock$SERVICE_NAME>();
    
    // Set up expectations
    EXPECT_CALL(*mockService, process${SERVICE_PART}Data())
        .WillOnce(Return("Mock $SERVICE_PART data"));
    
    // Test the mock
    std::string result = mockService->process${SERVICE_PART}Data();
    EXPECT_EQ(result, "Mock $SERVICE_PART data");
    
    // Verify mock can be treated as interface
    $NAMESPACE_NAME::I$SERVICE_NAME* interface = mockService.get();
    EXPECT_NE(interface, nullptr);
}

TEST(${SERVICE_NAME}RegistrationTest, DependencyInjectionWorkflow) {
    // Test typical DI workflow: register mock, resolve interface
    auto& container = ioccontainer::IIocContainer::getInstance();
    
    // Register a mock instance for testing
    auto mockService = std::make_shared<$NAMESPACE_NAME::Mock$SERVICE_NAME>();
    container.registerInstance<$NAMESPACE_NAME::I$SERVICE_NAME>(mockService);
    
    // Verify we can resolve the mock via interface
    auto resolved = container.resolve<$NAMESPACE_NAME::I$SERVICE_NAME>();
    EXPECT_EQ(resolved.get(), mockService.get());
}
EOF

# Create Makefile for tests using the flexible template
cat << 'EOF' > "tests/${SERVICE_NAME}Test/Makefile"
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
DEPENDENCIES = IocContainer

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
-include \$(ROOTDIR)/Compiler.build

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
    sed -i "s/MODULE_NAME_PLACEHOLDER/$SERVICE_NAME/g" "tests/${SERVICE_NAME}Test/Makefile"

    echo ""
    echo "✅ Service module '$SERVICE_NAME' created successfully!"
    echo ""
    echo "📁 Created files:"
    echo "  📄 include/$SERVICE_NAME/$SERVICE_NAME.h (interface + implementation)"
    echo "  📄 src/$SERVICE_NAME/$SERVICE_NAME.cpp (implementation + auto-registration)"
    echo "  📄 tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}BasicTest.cpp"
    echo "  📄 tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}RegistrationTest.cpp"
    echo "  📄 tests/${SERVICE_NAME}Test/Mock$SERVICE_NAME.h (mock for testing)"
    echo "  📄 tests/${SERVICE_NAME}Test/TestMain.cpp"
    echo "  📄 tests/${SERVICE_NAME}Test/Makefile"
    echo ""
    echo "📋 Next steps:"
    echo "1. 🔧 Edit the I$SERVICE_NAME interface in include/$SERVICE_NAME/$SERVICE_NAME.h"
    echo "2. 🔧 Implement the service logic in src/$SERVICE_NAME/$SERVICE_NAME.cpp"
    echo "3. 🧪 Update tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}BasicTest.cpp with real test cases"
    echo "4. 🃏 Add mock expectations in tests/${SERVICE_NAME}Test/Mock$SERVICE_NAME.h as needed"
    echo "5. 📦 Add dependencies in tests/${SERVICE_NAME}Test/Makefile if needed"
    echo "   Examples: DEPENDENCIES = ServiceA ServiceB Logger"
    echo "   Use 'make debug-config' in the test folder to verify dependency resolution"
    echo "6. 🧪 Test your service:"
    echo "   make test-run-${SERVICE_NAME}Test"
    echo ""
    echo "🚀 Auto-registration features:"
    echo "  • Both I$SERVICE_NAME and $SERVICE_NAME are auto-registered with IoC container"
    echo "  • Resolve via interface: ioccontainer::IIocContainer::resolveGlobal<I$SERVICE_NAME>()"
    echo "  • Resolve concrete: ioccontainer::IIocContainer::resolveGlobal<$SERVICE_NAME>()"
    echo "  • Perfect for dependency injection and testing with mocks"
    echo ""
    echo "💡 Tips:"
    echo "  • Use the interface (I$SERVICE_NAME) for dependency injection"
    echo "  • The mock class enables easy unit testing of dependent components"
    echo "  • Method names are domain-specific (process${SERVICE_PART}Data) instead of generic"
    echo ""
}

# Function to remove service
remove_service() {
    echo "Removing service module: $SERVICE_NAME"
    
    # Check if service exists
    SERVICE_EXISTS=false
    if [ -d "include/$SERVICE_NAME" ] || [ -d "src/$SERVICE_NAME" ] || [ -d "tests/${SERVICE_NAME}Test" ]; then
        SERVICE_EXISTS=true
    fi
    
    if [ "$SERVICE_EXISTS" = false ]; then
        echo "Warning: Service '$SERVICE_NAME' does not exist or is already removed."
        exit 0
    fi
    
    # Confirm removal
    echo "This will permanently delete the following:"
    [ -d "include/$SERVICE_NAME" ] && echo "  📁 include/$SERVICE_NAME/"
    [ -d "src/$SERVICE_NAME" ] && echo "  📁 src/$SERVICE_NAME/"
    [ -d "tests/${SERVICE_NAME}Test" ] && echo "  📁 tests/${SERVICE_NAME}Test/"
    echo ""
    read -p "Are you sure you want to remove '$SERVICE_NAME'? (y/N): " -r
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Removal cancelled."
        exit 0
    fi
    
    # Remove directories and files
    [ -d "include/$SERVICE_NAME" ] && rm -rf "include/$SERVICE_NAME"
    [ -d "src/$SERVICE_NAME" ] && rm -rf "src/$SERVICE_NAME"
    [ -d "tests/${SERVICE_NAME}Test" ] && rm -rf "tests/${SERVICE_NAME}Test"
    
    echo ""
    echo "✅ Service module '$SERVICE_NAME' removed successfully!"
    echo ""
    echo "Don't forget to:"
    echo "1. The service was auto-registered, so no manual removal from IoCContainer needed"
    echo "2. Clean any build artifacts:"
    echo "   make clean && make test-clean"
    echo ""
}

# Execute based on action
case "$ACTION" in
    "create")
        create_service
        ;;
    "remove")
        remove_service
        ;;
    *)
        echo "Error: Unknown action '$ACTION'"
        exit 1
        ;;
esac
