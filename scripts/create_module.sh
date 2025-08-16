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
	echo "Usage: $0 <create|remove> <ModuleName> [--interface] [--pimpl]"
	echo "Examples:"
	echo "  $0 create DatabaseManager              # Simple concrete class (default)"
	echo "  $0 create Logger --interface           # Add interface for polymorphism"
	echo "  $0 create WebServer --interface --pimpl # Both interface and PIMPL"
	echo "  $0 remove DatabaseManager              # Remove an existing module"
	echo ""
	echo "Flags:"
	echo "  --interface   Generate interface class for polymorphism/DI"
	echo "  --pimpl       Use PIMPL idiom to hide implementation details"
	echo ""
	echo "Note: Module names should be in PascalCase (e.g., DatabaseManager, not databaseManager)"
	exit 1
fi

ACTION="$1"
MODULE_NAME="$2"

# Parse optional flags
GENERATE_INTERFACE=false
GENERATE_PIMPL=false

shift 2  # Remove action and module name from arguments
while [[ $# -gt 0 ]]; do
	case $1 in
		--interface)
			GENERATE_INTERFACE=true
			shift
			;;
		--pimpl)
			GENERATE_PIMPL=true
			shift
			;;
		*)
			echo "Error: Unknown flag '$1'"
			echo "Valid flags: --interface, --pimpl"
			exit 1
			;;
	esac
done

# Reserved module names that cannot be manipulated
RESERVED_MODULES=("ApiRouter" "IocContainer" "Logger" "WebServer")

# Check for reserved module names
for reserved in "${RESERVED_MODULES[@]}"; do
	if [[ "$MODULE_NAME" == "$reserved" ]]; then
		echo "Error: Module name '$MODULE_NAME' is reserved and cannot be created or removed."
		echo "These core modules are managed by the project and cannot be manipulated via this script."
		exit 1
	fi
done

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
	echo "🏗️  Configuration: Interface=$GENERATE_INTERFACE, PIMPL=$GENERATE_PIMPL"

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

	# Create interface header (only if requested)
	if [ "$GENERATE_INTERFACE" = true ]; then
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
	fi

	# Create main header file with conditional content
	cat > "include/$MODULE_NAME/$MODULE_NAME.h" << EOF
#pragma once

#include <memory>
#include <string>
#include <string_view>
$(if [ "$GENERATE_INTERFACE" = true ]; then echo "#include \"$MODULE_NAME/I$MODULE_NAME.h\""; fi)

namespace $NAMESPACE_NAME {

/**
 * $MODULE_NAME - Utility/Infrastructure module
 * 
 * TODO: Add detailed description of what this module does
 */
class $MODULE_NAME$(if [ "$GENERATE_INTERFACE" = true ]; then echo " : public I$MODULE_NAME"; fi) {
public:
	/**
	 * Default constructor
	 */
	$MODULE_NAME();

	/**
	 * Destructor - ensures proper cleanup
	 */
	$(if [ "$GENERATE_INTERFACE" = true ]; then echo "~$MODULE_NAME() override;"; else echo "~$MODULE_NAME();"; fi)

	// TODO: Add your public interface methods here
	// Example:
	$(if [ "$GENERATE_INTERFACE" = true ]; then echo "    // bool initialize() override;"; else echo "    // bool initialize();"; fi)
	$(if [ "$GENERATE_INTERFACE" = true ]; then echo "    // void cleanup() override;"; else echo "    // void cleanup();"; fi)
	// std::string getStatus() const;

	// Delete copy constructor and assignment operator (RAII best practice)
	$MODULE_NAME(const $MODULE_NAME&) = delete;
	$MODULE_NAME& operator=(const $MODULE_NAME&) = delete;
	
	// Allow move constructor and assignment
	$MODULE_NAME($MODULE_NAME&&) noexcept;
	$MODULE_NAME& operator=($MODULE_NAME&&) noexcept;

private:
$(if [ "$GENERATE_PIMPL" = true ]; then 
	echo "    // PIMPL idiom to hide implementation details"
	echo "    class Impl;"
	echo "    std::unique_ptr<Impl> _impl;"
else
	echo "    // TODO: Add private members here"
fi)
};

} // namespace $NAMESPACE_NAME
EOF

	# Create source file with conditional PIMPL content
	cat > "src/$MODULE_NAME/$MODULE_NAME.cpp" << EOF
#include <$MODULE_NAME/$MODULE_NAME.h>
$(if [ "$GENERATE_PIMPL" = true ]; then 
	echo ""
	echo "namespace $NAMESPACE_NAME {"
	echo ""
	echo "// PIMPL implementation class"
	echo "class $MODULE_NAME::Impl {"
	echo "public:"
	echo "    Impl() {"
	echo "        // TODO: Initialize implementation here"
	echo "    }"
	echo ""
	echo "    // TODO: Add implementation methods here"
	echo "};"
	echo ""
fi)

namespace $NAMESPACE_NAME {

$MODULE_NAME::$MODULE_NAME()$(if [ "$GENERATE_PIMPL" = true ]; then echo " : _impl(std::make_unique<Impl>())"; fi) {
	$(if [ "$GENERATE_PIMPL" = false ]; then echo "    // TODO: Initialize your module here"; fi)
}

$MODULE_NAME::~$MODULE_NAME()$(if [ "$GENERATE_PIMPL" = true ]; then echo " = default;"; else echo " {"; echo "    // TODO: Cleanup resources here"; echo "}"; fi)

$MODULE_NAME::$MODULE_NAME($MODULE_NAME&&) noexcept$(if [ "$GENERATE_PIMPL" = true ]; then echo " = default;"; else echo " {"; echo "    // TODO: Implement move constructor if needed"; echo "}"; fi)

$MODULE_NAME& $MODULE_NAME::operator=($MODULE_NAME&& other) noexcept {
$(if [ "$GENERATE_PIMPL" = true ]; then 
	echo "    _impl = std::move(other._impl);"
	echo "    return *this;"
else
	echo "    if (this != &other) {"
	echo "        // TODO: Implement move assignment if needed"
	echo "    }"
	echo "    return *this;"
fi)
}

// TODO: Implement your public methods here
// Example:
// bool $MODULE_NAME::initialize() {
$(if [ "$GENERATE_PIMPL" = true ]; then 
	echo "//     return _impl->initialize();"
else
	echo "//     // Implementation here"
	echo "//     return true;"
fi)
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

	# Create mock header for testing (only if interface is generated)
	if [ "$GENERATE_INTERFACE" = true ]; then
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
	fi

	# Create interface test (only if interface is generated)
	if [ "$GENERATE_INTERFACE" = true ]; then
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
	fi

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
	sed -i "s/MODULE_NAME_PLACEHOLDER/$MODULE_NAME/g" "tests/${MODULE_NAME}Test/Makefile"

	echo ""
	echo "✅ Utility/Infrastructure module '$MODULE_NAME' created successfully!"
	echo "🏗️  Configuration: Interface=$GENERATE_INTERFACE, PIMPL=$GENERATE_PIMPL"
	echo ""
	echo "📁 Created files:"
	if [ "$GENERATE_INTERFACE" = true ]; then
		echo "  📄 include/$MODULE_NAME/I$MODULE_NAME.h (interface)"
	fi
	echo "  📄 include/$MODULE_NAME/$MODULE_NAME.h (main header)"
	echo "  📄 src/$MODULE_NAME/$MODULE_NAME.cpp (implementation)"
	echo "  📄 tests/${MODULE_NAME}Test/cases/${MODULE_NAME}BasicTest.cpp"
	if [ "$GENERATE_INTERFACE" = true ]; then
		echo "  📄 tests/${MODULE_NAME}Test/cases/${MODULE_NAME}InterfaceTest.cpp"
		echo "  📄 tests/${MODULE_NAME}Test/Mock$MODULE_NAME.h (mock for testing)"
	fi
	echo "  📄 tests/${MODULE_NAME}Test/TestMain.cpp"
	echo "  📄 tests/${MODULE_NAME}Test/Makefile"
	echo ""
	echo "📋 Next steps:"
	local step=1
	if [ "$GENERATE_INTERFACE" = true ]; then
		echo "$step. 🔧 Edit include/$MODULE_NAME/I$MODULE_NAME.h to define your interface"
		step=$((step + 1))
	fi
	echo "$step. 🔧 Edit include/$MODULE_NAME/$MODULE_NAME.h to add your public API"
	step=$((step + 1))
	echo "$step. 🔧 Edit src/$MODULE_NAME/$MODULE_NAME.cpp to implement your functionality"
	step=$((step + 1))
	if [ "$GENERATE_PIMPL" = true ]; then
		echo "$step. 🏗️  Implement the PIMPL Impl class in the .cpp file"
		step=$((step + 1))
	fi
	echo "$step. 🧪 Update tests/${MODULE_NAME}Test/cases/${MODULE_NAME}BasicTest.cpp with real test cases"
	step=$((step + 1))
	if [ "$GENERATE_INTERFACE" = true ]; then
		echo "$step. 🃏 Update tests/${MODULE_NAME}Test/Mock$MODULE_NAME.h with mock methods for your interface"
		step=$((step + 1))
	fi
	echo "$step. 📦 To add dependencies, edit the DEPENDENCIES line in tests/${MODULE_NAME}Test/Makefile"
	echo "   Examples: DEPENDENCIES = Logger IoCContainer ServiceA"
	echo "   Use 'make debug-config' in the test folder to verify dependency resolution"
	step=$((step + 1))
	echo "$step. 🧪 Test your module:"
	echo "   make test-run-${MODULE_NAME}Test"
	echo ""
	echo "💡 Tips:"
	if [ "$GENERATE_INTERFACE" = false ]; then
		echo "  • Simple concrete class - add --interface flag if you later need polymorphism"
	fi
	if [ "$GENERATE_INTERFACE" = true ]; then
		echo "  • Use the interface (I$MODULE_NAME) to define contracts"
		echo "  • The mock class helps test components that depend on your module"
	fi
	if [ "$GENERATE_PIMPL" = true ]; then
		echo "  • PIMPL hides implementation details and speeds up compilation"
		echo "  • Destructor is defined in .cpp file for proper PIMPL cleanup"
	fi
	if [ "$GENERATE_PIMPL" = false ]; then
		echo "  • Add --pimpl flag if you later need to hide complex dependencies"
	fi
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
