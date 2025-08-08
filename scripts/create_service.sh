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
#include <IoCContainer/IoCContainer.h>

namespace $NAMESPACE_NAME {
class $SERVICE_NAME {
public:
    $SERVICE_NAME();
    std::string doSomething${SERVICE_NAME}() const;
    // Static registration helper for TDD and decoupling
    static void registerWith(IoCContainer& container);
};
}
EOF

# Create service implementation
cat > "src/$SERVICE_NAME/$SERVICE_NAME.cpp" << EOF
#include <IoCContainer/IoCContainer.h>
#include <$SERVICE_NAME/$SERVICE_NAME.h>

namespace $NAMESPACE_NAME {
$SERVICE_NAME::$SERVICE_NAME() = default;
std::string $SERVICE_NAME::doSomething${SERVICE_NAME}() const {
    return "$SERVICE_NAME result";
}

namespace {
struct ${SERVICE_NAME}StaticRegistration {
    ${SERVICE_NAME}StaticRegistration() {
        IoCContainer::registerGlobal<$SERVICE_NAME>([]() { return std::make_shared<$SERVICE_NAME>(); });
    }
};
static ${SERVICE_NAME}StaticRegistration _${NAMESPACE_NAME}StaticRegistration;
}

void $SERVICE_NAME::registerWith(IoCContainer& container) {
    container.registerType<$SERVICE_NAME>([]() { return std::make_shared<$SERVICE_NAME>(); });
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

# Create basic test case
cat > "tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}BasicTest.cpp" << EOF
#include <gtest/gtest.h>
#include <$SERVICE_NAME/$SERVICE_NAME.h>

TEST(${SERVICE_NAME}Test, DoSomething${SERVICE_NAME}ReturnsExpected) {
    $NAMESPACE_NAME::$SERVICE_NAME service;
    EXPECT_EQ(service.doSomething${SERVICE_NAME}(), "$SERVICE_NAME result");
}
EOF

# Create registration test
cat > "tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}RegistrationTest.cpp" << EOF
#include <gtest/gtest.h>
#include <IoCContainer/IoCContainer.h>
#include <$SERVICE_NAME/$SERVICE_NAME.h>

TEST(${SERVICE_NAME}RegistrationTest, ${SERVICE_NAME}IsRegisteredInIoC) {
    IoCContainer container;
    container.importGlobals();
    EXPECT_NO_THROW({
        auto ptr = container.resolve<$NAMESPACE_NAME::$SERVICE_NAME>();
        EXPECT_NE(ptr, nullptr);
    });
}
EOF

# Create Makefile for tests
cat << 'EOF' > "tests/${SERVICE_NAME}Test/Makefile"
# Modern, robust Makefile for ServiceName unit tests (Google Test)
# All object and dependency files are placed flat in obj/ (not nested)

ROOTDIR = ../..
CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -I$(ROOTDIR)/include -I$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
CFLAGS = -I$(ROOTDIR)/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin
SERVICE = ServiceName

TEST_SRC = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(SERVICE)/$(SERVICE).cpp $(ROOTDIR)/src/IoCContainer/IoCContainer.cpp
TEST_OBJS = $(patsubst cases/%.cpp,$(OBJDIR)/%.o,$(wildcard cases/*.cpp)) $(OBJDIR)/TestMain.o $(OBJDIR)/$(SERVICE).o $(OBJDIR)/IoCContainer.o
TEST_DEPS = $(patsubst %.cpp,$(OBJDIR)/%.d,$(notdir $(basename $(wildcard cases/*.cpp)))) $(OBJDIR)/TestMain.d $(OBJDIR)/$(SERVICE).d $(OBJDIR)/IoCContainer.d
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

# Explicit rule for service source
$(OBJDIR)/$(SERVICE).o: $(ROOTDIR)/src/$(SERVICE)/$(SERVICE).cpp | $(OBJDIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rule for IoCContainer source
$(OBJDIR)/IoCContainer.o: $(ROOTDIR)/src/IoCContainer/IoCContainer.cpp | $(OBJDIR)
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

    # Replace ServiceName placeholders in the Makefile
    sed -i "s/ServiceName/$SERVICE_NAME/g" "tests/${SERVICE_NAME}Test/Makefile"

    echo ""
    echo "✅ Service module '$SERVICE_NAME' created successfully!"
    echo ""
    echo "Created files:"
    echo "  📁 include/$SERVICE_NAME/$SERVICE_NAME.h"
    echo "  📁 src/$SERVICE_NAME/$SERVICE_NAME.cpp"
    echo "  📁 tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}BasicTest.cpp"
    echo "  📁 tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}RegistrationTest.cpp"
    echo "  📁 tests/${SERVICE_NAME}Test/TestMain.cpp"
    echo "  📁 tests/${SERVICE_NAME}Test/Makefile"
    echo ""
    echo "Next steps:"
    echo "1. Edit src/$SERVICE_NAME/$SERVICE_NAME.cpp to implement your service logic"
    echo "2. Update tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}BasicTest.cpp with proper test assertions"
    echo "3. Update tests/${SERVICE_NAME}Test/cases/${SERVICE_NAME}RegistrationTest.cpp with IoC verification"
    echo "4. The service will auto-register with IoCContainer (no manual integration needed!)"
    echo "5. Test your service:"
    echo "   make test-run-${SERVICE_NAME}Test"
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
