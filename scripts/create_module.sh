#!/bin/bash

# Script to create or remove utility/infrastructure modules following the modular convention
# Usage: ./scripts/create_module.sh <create|remove> ModuleName

set -e

# Resolve script location and set project root
SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
PROJECT_ROOT="$(dirname "$SCRIPTDIR")"
cd "$PROJECT_ROOT"

# Check if action and module name are provided
if [ $# -lt 2 ]; then
    echo "Usage: $0 <create|remove> <ModuleName>"
    echo "Examples:"
    echo "  $0 create DatabaseManager    # Create a new utility module"
    echo "  $0 create ConfigReader        # Create a new infrastructure module"
    echo "  $0 remove DatabaseManager     # Remove an existing module"
    echo ""
    echo "Note: Module names should be in PascalCase (e.g., DatabaseManager, not databaseManager)"
    exit 1
fi

ACTION="$1"
MODULE_NAME="$2"

# Validate action
if [[ "$ACTION" != "create" && "$ACTION" != "remove" ]]; then
    echo "Error: Action must be 'create' or 'remove'"
    exit 1
fi

# Validate module name format (should be PascalCase, not start with Service/Endpoint)
if [[ ! "$MODULE_NAME" =~ ^[A-Z][a-zA-Z0-9]*$ ]]; then
    echo "Error: Module name must be in PascalCase (e.g., DatabaseManager, ConfigReader)"
    echo "Invalid examples: databaseManager, database_manager, DATABASE_MANAGER"
    exit 1
fi

# Prevent conflicts with reserved prefixes
if [[ "$MODULE_NAME" =~ ^(Service|Endpoint) ]]; then
    echo "Error: Module name cannot start with 'Service' or 'Endpoint'"
    echo "Use './scripts/create_service.sh' for services or './scripts/create_endpoint.sh' for endpoints"
    exit 1
fi

# Create namespace name (convert to lowercase)
NAMESPACE_NAME="${MODULE_NAME,,}"

# Function to create module
create_module() {
    echo "🔧 Creating utility/infrastructure module: $MODULE_NAME"
    echo "📁 Namespace: $NAMESPACE_NAME"

    # Check if module already exists
    if [ -d "include/$MODULE_NAME" ] || [ -d "src/$MODULE_NAME" ] || [ -d "tests/${MODULE_NAME}Test" ]; then
        echo "❌ Error: Module '$MODULE_NAME' already exists!"
        echo "Use 'remove' action first if you want to recreate it."
        exit 1
    fi

    # Create directories
    mkdir -p "include/$MODULE_NAME"
    mkdir -p "src/$MODULE_NAME"
    mkdir -p "tests/${MODULE_NAME}Test/cases"

# Create interface header (optional - many modules don't need interfaces)
cat > "include/$MODULE_NAME/I$MODULE_NAME.h" << EOF
#pragma once

namespace $NAMESPACE_NAME {

/**
 * Interface for $MODULE_NAME
 * Define the contract that implementations must follow
 */
class I$MODULE_NAME {
public:
    virtual ~I$MODULE_NAME() = default;
    
    // TODO: Define your interface methods here
    // Example:
    // virtual bool initialize() = 0;
    // virtual void cleanup() = 0;
};

} // namespace $NAMESPACE_NAME
EOF

# Create main header file
cat > "include/$MODULE_NAME/$MODULE_NAME.h" << EOF
#pragma once

#include <memory>
#include <string>
#include <string_view>
#include "$MODULE_NAME/I$MODULE_NAME.h"

namespace $NAMESPACE_NAME {

/**
 * $MODULE_NAME - Utility/Infrastructure module
 * 
 * TODO: Add detailed description of what this module does
 */
class $MODULE_NAME : public I$MODULE_NAME {
public:
    /**
     * Default constructor
     */
    $MODULE_NAME();

    /**
     * Destructor - ensures proper cleanup
     */
    ~$MODULE_NAME() override;

    // TODO: Add your public interface methods here
    // Example:
    // bool initialize() override;
    // void cleanup() override;
    // std::string getStatus() const;

    // Delete copy constructor and assignment operator (RAII best practice)
    $MODULE_NAME(const $MODULE_NAME&) = delete;
    $MODULE_NAME& operator=(const $MODULE_NAME&) = delete;
    
    // Allow move constructor and assignment
    $MODULE_NAME($MODULE_NAME&&) noexcept;
    $MODULE_NAME& operator=($MODULE_NAME&&) noexcept;

private:
    // TODO: Add private members here
    // Consider using PIMPL idiom for complex implementations:
    // class Impl;
    // std::unique_ptr<Impl> _impl;
};

} // namespace $NAMESPACE_NAME
EOF

# Create source file
cat > "src/$MODULE_NAME/$MODULE_NAME.cpp" << EOF
#include <$MODULE_NAME/$MODULE_NAME.h>

namespace $NAMESPACE_NAME {

$MODULE_NAME::$MODULE_NAME() {
    // TODO: Initialize your module here
}

$MODULE_NAME::~$MODULE_NAME() {
    // TODO: Cleanup resources here
    // Note: If using PIMPL, destructor must be defined in .cpp file
}

$MODULE_NAME::$MODULE_NAME($MODULE_NAME&&) noexcept {
    // TODO: Implement move constructor if needed
}

$MODULE_NAME& $MODULE_NAME::operator=($MODULE_NAME&& other) noexcept {
    if (this != &other) {
        // TODO: Implement move assignment if needed
    }
    return *this;
}

// TODO: Implement your public methods here
// Example:
// bool $MODULE_NAME::initialize() {
//     // Implementation here
//     return true;
// }

} // namespace $NAMESPACE_NAME
EOF

# Create test runner
cat > "tests/${MODULE_NAME}Test/TestMain.cpp" << EOF
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
EOF

# Create basic functionality test
cat > "tests/${MODULE_NAME}Test/cases/${MODULE_NAME}BasicTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$MODULE_NAME/$MODULE_NAME.h>

namespace {

class ${MODULE_NAME}Test : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code for each test
    }

    void TearDown() override {
        // Cleanup code for each test
    }
};

} // anonymous namespace

TEST_F(${MODULE_NAME}Test, ConstructorCreatesValidInstance) {
    $NAMESPACE_NAME::$MODULE_NAME module;
    // TODO: Add assertions to verify the module is properly constructed
    // Example:
    // EXPECT_TRUE(module.isInitialized());
    SUCCEED(); // Remove this when you add real tests
}

TEST_F(${MODULE_NAME}Test, MoveConstructorWorks) {
    $NAMESPACE_NAME::$MODULE_NAME original;
    $NAMESPACE_NAME::$MODULE_NAME moved(std::move(original));
    
    // TODO: Add assertions to verify move constructor works correctly
    // Example:
    // EXPECT_TRUE(moved.isValid());
    SUCCEED(); // Remove this when you add real tests
}

// TODO: Add more test cases for your module's functionality
// Example:
// TEST_F(${MODULE_NAME}Test, InitializeReturnsTrueOnSuccess) {
//     $NAMESPACE_NAME::$MODULE_NAME module;
//     EXPECT_TRUE(module.initialize());
// }
EOF

# Create mock header for testing (if needed)
cat > "tests/${MODULE_NAME}Test/Mock$MODULE_NAME.h" << EOF
#pragma once

#include <gmock/gmock.h>
#include <$MODULE_NAME/I$MODULE_NAME.h>

namespace $NAMESPACE_NAME {

/**
 * Mock implementation of I$MODULE_NAME for testing
 */
class Mock$MODULE_NAME : public I$MODULE_NAME {
public:
    // TODO: Add mock methods for your interface
    // Example:
    // MOCK_METHOD(bool, initialize, (), (override));
    // MOCK_METHOD(void, cleanup, (), (override));
};

} // namespace $NAMESPACE_NAME
EOF

# Create interface test (to ensure contract compliance)
cat > "tests/${MODULE_NAME}Test/cases/${MODULE_NAME}InterfaceTest.cpp" << EOF
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <$MODULE_NAME/$MODULE_NAME.h>
#include "../Mock$MODULE_NAME.h"

using ::testing::_;
using ::testing::Return;

namespace {

class ${MODULE_NAME}InterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockModule = std::make_unique<$NAMESPACE_NAME::Mock$MODULE_NAME>();
    }

    std::unique_ptr<$NAMESPACE_NAME::Mock$MODULE_NAME> mockModule;
};

} // anonymous namespace

TEST_F(${MODULE_NAME}InterfaceTest, ImplementsInterface) {
    // Verify that $MODULE_NAME implements I$MODULE_NAME
    $NAMESPACE_NAME::$MODULE_NAME module;
    $NAMESPACE_NAME::I$MODULE_NAME* interface = &module;
    EXPECT_NE(interface, nullptr);
}

TEST_F(${MODULE_NAME}InterfaceTest, MockCanBeUsedForTesting) {
    // Example of how to use the mock for testing other components
    // TODO: Add expectations and test interactions
    // Example:
    // EXPECT_CALL(*mockModule, initialize())
    //     .WillOnce(Return(true));
    // 
    // bool result = mockModule->initialize();
    // EXPECT_TRUE(result);
    
    SUCCEED(); // Remove this when you add real mock tests
}

// TODO: Add more interface compliance tests
EOF

# Create Makefile for tests using the flexible template
cat << 'EOF' > "tests/${MODULE_NAME}Test/Makefile"
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
DEPENDENCIES = 

# External dependencies (from external/ folder) - uncomment if needed
# EXTERNAL_DEPS = mongoose foo

# ============================================================================
# BUILD CONFIGURATION - Usually no changes needed below this line
# ============================================================================

CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -I$(ROOTDIR)/external/googletest/googlemock/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main -lgmock -lgmock_main

OBJDIR = obj
BINDIR = bin

# Process dependencies into source files and object files
# Handle both simple format (Name) and complex format (Name:Folder)
DEP_SOURCES = $(foreach dep,$(DEPENDENCIES),$(ROOTDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).cpp)
DEP_OBJECTS = $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).o)
DEP_NAMES = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)))
DEP_FOLDERS = $(foreach dep,$(DEPENDENCIES),$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep)))

# External dependencies (if any)
ifdef EXTERNAL_DEPS
EXT_SOURCES = $(foreach ext,$(EXTERNAL_DEPS),$(ROOTDIR)/external/$(ext)/src/$(ext).cpp)
EXT_OBJECTS = $(foreach ext,$(EXTERNAL_DEPS),$(OBJDIR)/external/$(ext)/$(ext).o)
EXT_NAMES = $(EXTERNAL_DEPS)
else
EXT_SOURCES =
EXT_OBJECTS =
EXT_NAMES =
endif

# Complete source and object lists (organized by source origin)
ALL_SOURCES = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).cpp $(DEP_SOURCES) $(EXT_SOURCES)
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/test/cases/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/test/TestMain.o $(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).o $(DEP_OBJECTS) $(EXT_OBJECTS)
TEST_DEPS = $(patsubst cases/%.cpp,$(OBJDIR)/test/cases/%.d,$(wildcard cases/*.cpp)) $(OBJDIR)/test/TestMain.d $(OBJDIR)/src/$(PRIMARY_MODULE)/$(PRIMARY_MODULE).d $(foreach dep,$(DEPENDENCIES),$(OBJDIR)/src/$(if $(findstring :,$(dep)),$(word 2,$(subst :, ,$(dep))),$(dep))/$(if $(findstring :,$(dep)),$(word 1,$(subst :, ,$(dep))),$(dep)).d) $(foreach ext,$(EXT_NAMES),$(OBJDIR)/external/$(ext)/$(ext).d)
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
# Generic rule for external C++ dependencies
$(OBJDIR)/external/%.o: $(ROOTDIR)/external/%/src/%.cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generic rule for external C dependencies
$(OBJDIR)/external/%.o: $(ROOTDIR)/external/%/src/%.c | $(OBJDIR)
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
	@echo ""
	@echo "=== Directory Structure ==="
	@echo "obj/test/     - test files (cases/, TestMain.cpp)"
	@echo "obj/src/      - our source code (modules, services, etc.)"
	@echo "obj/external/ - external dependencies"

run: all
	@./$(TEST_BIN)

clean:
	rm -rf $(OBJDIR)/* $(BINDIR)/*

.PHONY: all clean debug-config run
EOF

    # Replace MODULE_NAME_PLACEHOLDER in the Makefile
    sed -i "s/MODULE_NAME_PLACEHOLDER/$MODULE_NAME/g" "tests/${MODULE_NAME}Test/Makefile"

    echo ""
    echo "✅ Utility/Infrastructure module '$MODULE_NAME' created successfully!"
    echo ""
    echo "📁 Created files:"
    echo "  📄 include/$MODULE_NAME/I$MODULE_NAME.h (interface)"
    echo "  📄 include/$MODULE_NAME/$MODULE_NAME.h (main header)"
    echo "  📄 src/$MODULE_NAME/$MODULE_NAME.cpp (implementation)"
    echo "  📄 tests/${MODULE_NAME}Test/cases/${MODULE_NAME}BasicTest.cpp"
    echo "  📄 tests/${MODULE_NAME}Test/cases/${MODULE_NAME}InterfaceTest.cpp"
    echo "  📄 tests/${MODULE_NAME}Test/Mock$MODULE_NAME.h (mock for testing)"
    echo "  📄 tests/${MODULE_NAME}Test/TestMain.cpp"
    echo "  📄 tests/${MODULE_NAME}Test/Makefile"
    echo ""
    echo "📋 Next steps:"
    echo "1. 🔧 Edit include/$MODULE_NAME/I$MODULE_NAME.h to define your interface"
    echo "2. 🔧 Edit include/$MODULE_NAME/$MODULE_NAME.h to add your public API"
    echo "3. 🔧 Edit src/$MODULE_NAME/$MODULE_NAME.cpp to implement your functionality"
    echo "4. 🧪 Update tests/${MODULE_NAME}Test/cases/${MODULE_NAME}BasicTest.cpp with real test cases"
    echo "5. 🃏 Update tests/${MODULE_NAME}Test/Mock$MODULE_NAME.h with mock methods for your interface"
    echo "6. 📦 To add dependencies, edit the DEPENDENCIES line in tests/${MODULE_NAME}Test/Makefile"
    echo "   Examples: DEPENDENCIES = Logger IoCContainer ServiceA"
    echo "   Use 'make debug-config' in the test folder to verify dependency resolution"
    echo "7. 🧪 Test your module:"
    echo "   make test-run-${MODULE_NAME}Test"
    echo ""
    echo "💡 Tips:"
    echo "  • Use the interface (I$MODULE_NAME) to define contracts"
    echo "  • Consider PIMPL idiom for complex implementations"
    echo "  • The mock class helps test components that depend on your module"
    echo "  • Follow RAII principles for resource management"
    echo ""
}

# Function to remove module
remove_module() {
    echo "🗑️  Removing utility/infrastructure module: $MODULE_NAME"
    
    # Check if module exists
    MODULE_EXISTS=false
    if [ -d "include/$MODULE_NAME" ] || [ -d "src/$MODULE_NAME" ] || [ -d "tests/${MODULE_NAME}Test" ]; then
        MODULE_EXISTS=true
    fi
    
    if [ "$MODULE_EXISTS" = false ]; then
        echo "⚠️  Warning: Module '$MODULE_NAME' does not exist or is already removed."
        exit 0
    fi
    
    # Confirm removal
    echo "⚠️  This will permanently delete the following:"
    [ -d "include/$MODULE_NAME" ] && echo "  📁 include/$MODULE_NAME/"
    [ -d "src/$MODULE_NAME" ] && echo "  📁 src/$MODULE_NAME/"
    [ -d "tests/${MODULE_NAME}Test" ] && echo "  📁 tests/${MODULE_NAME}Test/"
    echo ""
    read -p "Are you sure you want to remove '$MODULE_NAME'? (y/N): " -r
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "❌ Removal cancelled."
        exit 0
    fi
    
    # Remove directories and files
    [ -d "include/$MODULE_NAME" ] && rm -rf "include/$MODULE_NAME"
    [ -d "src/$MODULE_NAME" ] && rm -rf "src/$MODULE_NAME"
    [ -d "tests/${MODULE_NAME}Test" ] && rm -rf "tests/${MODULE_NAME}Test"
    
    echo ""
    echo "✅ Utility/Infrastructure module '$MODULE_NAME' removed successfully!"
    echo ""
    echo "📋 Don't forget to:"
    echo "1. 🧹 Update any code that depends on this module"
    echo "2. 🧽 Clean any build artifacts:"
    echo "   make clean && make test-clean"
    echo "3. 📝 Remove any references from documentation"
    echo ""
}

# Execute based on action
case "$ACTION" in
    "create")
        create_module
        ;;
    "remove")
        remove_module
        ;;
    *)
        echo "❌ Error: Unknown action '$ACTION'"
        exit 1
        ;;
esac
