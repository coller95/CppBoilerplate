#!/bin/bash
# Usage: ./scripts/create_service.sh ServiceName
# Creates a new service module (src/, include/, test/), registers in IoC, and sets up test skeletons.

set -euo pipefail

# Resolve script directory and project root, then run from project root
SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
PROJECT_ROOT="$(dirname "$SCRIPTDIR")"
cd "$PROJECT_ROOT"
echo "[INFO] Script dir: $SCRIPTDIR"
echo "[INFO] Changed to project root: $(pwd)"

# Enhanced argument parser: -f/--force, -n/--name, -r/--remove
FORCE=0
NAME=""
REMOVE=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    -f|--force)
      FORCE=1
      shift
      ;;
    -n|--name)
      NAME="$2"
      shift 2
      ;;
    -r|--remove)
      REMOVE=1
      shift
      ;;
    -h|--help)
      echo "Usage: $0 [-f|--force] -n|--name <ServiceName>"
      echo "       $0 -r|--remove -n|--name <ServiceName>   # Remove service files"
      echo "Example: $0 -n FooService"
      echo "         $0 --force --name FooService   # Overwrite if exists"
      echo "         $0 --remove --name FooService  # Remove service files"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "Usage: $0 [-f|--force] -n|--name <ServiceName>"
      echo "       $0 -r|--remove -n|--name <ServiceName>"
      exit 1
      ;;
  esac
done

if [ -z "$NAME" ]; then
  echo "[ERROR] -n|--name <ServiceName> is required."
  exit 1
fi

# Remove mode
if [ "$REMOVE" -eq 1 ]; then
  SERVICE_RAW="$NAME"
  if [[ "$SERVICE_RAW" != Service* ]]; then
    SERVICE="Service${SERVICE_RAW}"
  else
    SERVICE="$SERVICE_RAW"
  fi
  echo "[CONFIRM] This will permanently delete src/$SERVICE, include/$SERVICE, and tests/${SERVICE}Test."
  read -p "Are you sure you want to remove $SERVICE? [y/N]: " CONFIRM
  if [[ ! "$CONFIRM" =~ ^[Yy]$ ]]; then
    echo "Aborted. No files were removed."
    exit 0
  fi
  echo "Removing service: $SERVICE"
  rm -rf "src/$SERVICE" "include/$SERVICE" "tests/${SERVICE}Test"
  echo "Removed $SERVICE module, header, source, and test skeleton."
  exit 0
fi


# Ensure service name starts with 'Service'
SERVICE_RAW="$NAME"
if [[ "$SERVICE_RAW" != Service* ]]; then
  SERVICE="Service${SERVICE_RAW}"
else
  SERVICE="$SERVICE_RAW"
fi
SERVICE_LC=$(echo "$SERVICE" | awk '{print tolower($0)}')

# Check if service already exists
if [ -d "src/$SERVICE" ] || [ -d "include/$SERVICE" ] || [ -d "tests/${SERVICE}Test" ]; then
  if [ "$FORCE" -eq 1 ]; then
    echo "[WARNING] $SERVICE already exists. Files may be overwritten."
  else
    echo "Error: $SERVICE already exists. Aborting to prevent overwrite."
    exit 1
  fi
fi

# Create source and include directories
mkdir -p "src/$SERVICE/"
mkdir -p "include/$SERVICE/"

# Create service header
cat > "include/$SERVICE/$SERVICE.h" <<EOF
#pragma once
#include <string>

namespace ${SERVICE_LC} {
class $SERVICE {
public:
	std::string doSomething$SERVICE() const;
};
}
EOF

# Create service implementation
cat > "src/$SERVICE/$SERVICE.cpp" <<EOF
#include <$SERVICE/$SERVICE.h>

namespace ${SERVICE_LC} {
std::string $SERVICE::doSomething$SERVICE() const {
	return "$SERVICE result";
}
}
EOF

# Create test folder structure
mkdir -p "tests/${SERVICE}Test/cases"

# Create test main
cat > "tests/${SERVICE}Test/TestMain.cpp" <<EOF
#include <gtest/gtest.h>

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
EOF

# Create basic test case
cat > "tests/${SERVICE}Test/cases/${SERVICE}BasicTest.cpp" <<EOF
#include <gtest/gtest.h>
#include <$SERVICE/$SERVICE.h>

TEST(${SERVICE}Test, DoSomething${SERVICE}ReturnsExpected) {
	${SERVICE_LC}::$SERVICE service;
	EXPECT_EQ(service.doSomething$SERVICE(), "$SERVICE result");
}
EOF

# Create Makefile for the test
cat > "tests/${SERVICE}Test/Makefile" <<EOF
# Modern, robust Makefile for unit tests (Google Test)
# All object and dependency files are placed flat in obj/ (not nested)

ROOTDIR = ../..
SERVICE = $SERVICE
CXX = g++
CXXFLAGS = -std=c++20 -I\$(ROOTDIR)/include -I\$(ROOTDIR)/external/googletest/googletest/include -g -Wall -Wextra -MMD -MP
GTEST_LIBS = -L\$(ROOTDIR)/external/googletest/build/lib -lgtest -lgtest_main

OBJDIR = obj
BINDIR = bin

TEST_SRC = cases/*.cpp TestMain.cpp \$(ROOTDIR)/src/\$(SERVICE)/\$(SERVICE).cpp
TEST_OBJS = \$(patsubst cases/%.cpp,\$(OBJDIR)/%.o,\$(wildcard cases/*.cpp)) \$(OBJDIR)/TestMain.o \$(OBJDIR)/\$(SERVICE).o
TEST_DEPS = \$(patsubst %.cpp,\$(OBJDIR)/%.d,\$(notdir \$(basename \$(wildcard cases/*.cpp)))) \$(OBJDIR)/TestMain.d \$(OBJDIR)/\$(SERVICE).d
TEST_BIN = \$(BINDIR)/\$(SERVICE)Test

all: \$(OBJDIR) \$(BINDIR) \$(TEST_BIN)

\$(OBJDIR):
	mkdir -p \$(OBJDIR)

\$(BINDIR):
	mkdir -p \$(BINDIR)

# Pattern rule for C++ test cases in cases/
\$(OBJDIR)/%.o: cases/%.cpp | \$(OBJDIR)
	mkdir -p \$(dir \$@)
	\$(CXX) \$(CXXFLAGS) -c \$< -o \$@

# Pattern rule for the test runner
\$(OBJDIR)/TestMain.o: TestMain.cpp | \$(OBJDIR)
	mkdir -p \$(dir \$@)
	\$(CXX) \$(CXXFLAGS) -c \$< -o \$@

# Explicit rule for service source
\$(OBJDIR)/\$(SERVICE).o: \$(ROOTDIR)/src/\$(SERVICE)/\$(SERVICE).cpp | \$(OBJDIR)
	mkdir -p \$(dir \$@)
	\$(CXX) \$(CXXFLAGS) -c \$< -o \$@

\$(TEST_BIN): \$(TEST_OBJS) | \$(BINDIR)
	\$(CXX) \$(CXXFLAGS) -o \$@ \$^ \$(GTEST_LIBS) -pthread

-include \$(TEST_DEPS)

clean:
	rm -rf \$(OBJDIR)/* \$(BINDIR)/*
EOF

# Create obj and bin directories for test
mkdir -p "tests/${SERVICE}Test/obj"
mkdir -p "tests/${SERVICE}Test/bin"

echo "$SERVICE module, header, source, and test skeleton created."
