#!/bin/bash

# CppBoilerplate Bootstrap Script
# Initializes the project after fresh clone from repository
# Usage: ./bootstrap.sh [--verbose] [--skip-build] [--help]

set -e  # Exit on any error

# Script configuration
SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
PROJECT_ROOT="$SCRIPTDIR"
VERBOSE=0
SKIP_BUILD=0
FORCE_REBUILD=0

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Emoji support detection
if command -v locale >/dev/null 2>&1 && [[ "$(locale charmap 2>/dev/null)" == "UTF-8" ]]; then
    EMOJI_SUPPORT=1
else
    EMOJI_SUPPORT=0
fi

# Output functions
print_header() {
    if [[ $EMOJI_SUPPORT -eq 1 ]]; then
        echo -e "${BLUE}🚀 $1${NC}"
    else
        echo -e "${BLUE}[BOOTSTRAP] $1${NC}"
    fi
}

print_success() {
    if [[ $EMOJI_SUPPORT -eq 1 ]]; then
        echo -e "${GREEN}✅ $1${NC}"
    else
        echo -e "${GREEN}[SUCCESS] $1${NC}"
    fi
}

print_warning() {
    if [[ $EMOJI_SUPPORT -eq 1 ]]; then
        echo -e "${YELLOW}⚠️  $1${NC}"
    else
        echo -e "${YELLOW}[WARNING] $1${NC}"
    fi
}

print_error() {
    if [[ $EMOJI_SUPPORT -eq 1 ]]; then
        echo -e "${RED}❌ $1${NC}"
    else
        echo -e "${RED}[ERROR] $1${NC}"
    fi
}

print_info() {
    if [[ $EMOJI_SUPPORT -eq 1 ]]; then
        echo -e "${CYAN}ℹ️  $1${NC}"
    else
        echo -e "${CYAN}[INFO] $1${NC}"
    fi
}

print_step() {
    if [[ $EMOJI_SUPPORT -eq 1 ]]; then
        echo -e "${PURPLE}🔧 $1${NC}"
    else
        echo -e "${PURPLE}[STEP] $1${NC}"
    fi
}

# Verbose output function
verbose_run() {
    if [[ $VERBOSE -eq 1 ]]; then
        echo -e "${YELLOW}Running: $*${NC}"
        "$@"
    else
        "$@" >/dev/null 2>&1
    fi
}

# Help function
show_help() {
    cat << EOF
CppBoilerplate Bootstrap Script

USAGE:
    ./bootstrap.sh [OPTIONS]

DESCRIPTION:
    Initializes the project after fresh clone from repository.
    Downloads and builds all dependencies including Google Test.

OPTIONS:
    --verbose, -v       Enable verbose output showing all commands
    --skip-build, -s    Skip building dependencies (download only)
    --force, -f         Force rebuild even if dependencies exist
    --help, -h          Show this help message

EXAMPLES:
    ./bootstrap.sh                    # Standard bootstrap
    ./bootstrap.sh --verbose          # With detailed output
    ./bootstrap.sh --skip-build       # Download only, no build
    ./bootstrap.sh --force --verbose  # Force rebuild with output

WHAT IT DOES:
    1. Checks system requirements (git, make, gcc/g++, cmake)
    2. Downloads Google Test framework
    3. Builds Google Test libraries
    4. Sets up project structure
    5. Validates the setup with a test build
    6. Provides next steps for development

EOF
}

# Parse command line arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --verbose|-v)
                VERBOSE=1
                shift
                ;;
            --skip-build|-s)
                SKIP_BUILD=1
                shift
                ;;
            --force|-f)
                FORCE_REBUILD=1
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
}

# Check system requirements
check_requirements() {
    print_step "Checking system requirements..."
    
    local missing_deps=()
    
    # Check for git
    if ! command -v git >/dev/null 2>&1; then
        missing_deps+=("git")
    fi
    
    # Check for make
    if ! command -v make >/dev/null 2>&1; then
        missing_deps+=("make")
    fi
    
    # Check for g++
    if ! command -v g++ >/dev/null 2>&1; then
        missing_deps+=("g++")
    fi
    
    # Check for gcc
    if ! command -v gcc >/dev/null 2>&1; then
        missing_deps+=("gcc")
    fi
    
    # Check for cmake
    if ! command -v cmake >/dev/null 2>&1; then
        missing_deps+=("cmake")
    fi
    
    # Check for curl or wget
    if ! command -v curl >/dev/null 2>&1 && ! command -v wget >/dev/null 2>&1; then
        missing_deps+=("curl or wget")
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        print_error "Missing required dependencies:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo
        print_info "Please install missing dependencies and try again."
        print_info "On Ubuntu/Debian: sudo apt-get install git make gcc g++ cmake curl"
        print_info "On RHEL/CentOS: sudo yum install git make gcc gcc-c++ cmake curl"
        print_info "On macOS: xcode-select --install && brew install cmake"
        exit 1
    fi
    
    print_success "All system requirements satisfied"
}

# Setup project directories
setup_directories() {
    print_step "Setting up project directories..."
    
    cd "$PROJECT_ROOT"
    
    # Create essential directories if they don't exist
    mkdir -p external
    mkdir -p bin
    mkdir -p lib
    
    print_success "Project directories ready"
}

# Download Google Test
download_googletest() {
    print_step "Setting up Google Test framework..."
    
    cd "$PROJECT_ROOT/external"
    
    # Check if Google Test already exists
    if [[ -d "googletest" && $FORCE_REBUILD -eq 0 ]]; then
        if [[ -f "googletest/build/lib/libgtest.a" && -f "googletest/build/lib/libgtest_main.a" ]]; then
            print_info "Google Test already exists and is built. Use --force to rebuild."
            return 0
        elif [[ $SKIP_BUILD -eq 1 ]]; then
            print_info "Google Test directory exists. Skipping download."
            return 0
        fi
    fi
    
    # Remove existing directory if force rebuild
    if [[ $FORCE_REBUILD -eq 1 && -d "googletest" ]]; then
        print_info "Force rebuild: removing existing Google Test..."
        rm -rf googletest
    fi
    
    # Download Google Test if directory doesn't exist
    if [[ ! -d "googletest" ]]; then
        print_info "Downloading Google Test framework..."
        
        if command -v git >/dev/null 2>&1; then
            verbose_run git clone --depth 1 --branch v1.14.0 https://github.com/google/googletest.git
            cd googletest
            verbose_run git checkout v1.14.0
            cd ..
        else
            print_error "Git is required to download Google Test"
            exit 1
        fi
        
        print_success "Google Test downloaded successfully"
    fi
}

# Build Google Test
build_googletest() {
    if [[ $SKIP_BUILD -eq 1 ]]; then
        print_info "Skipping Google Test build (--skip-build specified)"
        return 0
    fi
    
    print_step "Building Google Test libraries..."
    
    cd "$PROJECT_ROOT/external/googletest"
    
    # Check if already built
    if [[ -f "build/lib/libgtest.a" && -f "build/lib/libgtest_main.a" && $FORCE_REBUILD -eq 0 ]]; then
        print_info "Google Test libraries already built"
        return 0
    fi
    
    # Create build directory
    mkdir -p build
    cd build
    
    # Configure with CMake
    print_info "Configuring Google Test build..."
    verbose_run cmake -DCMAKE_BUILD_TYPE=Release ..
    
    # Build libraries
    print_info "Compiling Google Test libraries..."
    verbose_run make -j$(nproc 2>/dev/null || echo 4)
    
    # Verify build success
    if [[ -f "lib/libgtest.a" && -f "lib/libgtest_main.a" ]]; then
        print_success "Google Test libraries built successfully"
    else
        print_error "Failed to build Google Test libraries"
        exit 1
    fi
}

# Setup external library structure
setup_external_libs() {
    print_step "Setting up external library structure..."
    
    cd "$PROJECT_ROOT/external"
    
    # Create mongoose directory structure if it doesn't exist
    if [[ ! -d "mongoose" ]]; then
        print_info "Creating mongoose library structure..."
        mkdir -p mongoose/include
        mkdir -p mongoose/src
        
        # Create placeholder files to demonstrate structure
        cat > mongoose/include/mongoose.h << 'EOF'
#ifndef MONGOOSE_H
#define MONGOOSE_H

// Mongoose HTTP server library
// This is a placeholder - replace with actual mongoose.h when needed
// Download from: https://github.com/cesanta/mongoose

#ifdef __cplusplus
extern "C" {
#endif

// Placeholder function declarations
void mg_mgr_init(void* mgr);
void mg_mgr_free(void* mgr);

#ifdef __cplusplus
}
#endif

#endif // MONGOOSE_H
EOF
        
        cat > mongoose/src/mongoose.c << 'EOF'
// Mongoose HTTP server library implementation
// This is a placeholder - replace with actual mongoose.c when needed
// Download from: https://github.com/cesanta/mongoose

#include "../include/mongoose.h"

// Placeholder implementations
void mg_mgr_init(void* mgr) {
    (void)mgr; // Unused parameter
    // TODO: Replace with actual mongoose implementation
}

void mg_mgr_free(void* mgr) {
    (void)mgr; // Unused parameter
    // TODO: Replace with actual mongoose implementation
}
EOF
        
        print_info "Mongoose placeholder created (replace with actual library when needed)"
    fi
    
    # Create foo example library if it doesn't exist
    if [[ ! -d "foo" ]]; then
        print_info "Creating example foo library..."
        mkdir -p foo/include
        mkdir -p foo/src
        
        cat > foo/include/foo.h << 'EOF'
#pragma once

// Example C++ external library
void external_print(const char* message);
EOF
        
        cat > foo/include/foo_c.h << 'EOF'
#ifndef FOO_C_H
#define FOO_C_H

#ifdef __cplusplus
extern "C" {
#endif

void external_print_c(const char* message);

#ifdef __cplusplus
}
#endif

#endif // FOO_C_H
EOF
        
        cat > foo/src/foo.cpp << 'EOF'
#include "../include/foo.h"
#include <iostream>

void external_print(const char* message) {
    std::cout << "External C++ library says: " << message << std::endl;
}
EOF
        
        cat > foo/src/foo_c.c << 'EOF'
#include "../include/foo_c.h"
#include <stdio.h>

void external_print_c(const char* message) {
    printf("External C library says: %s\n", message);
}
EOF
        
        print_success "Example foo library created"
    fi
    
    print_success "External library structure ready"
}

# Validate setup with test build
validate_setup() {
    print_step "Validating setup with test build..."
    
    cd "$PROJECT_ROOT"
    
    # Clean any existing build
    verbose_run make clean
    
    # Try building debug version
    print_info "Testing debug build..."
    if verbose_run make debug; then
        print_success "Debug build successful"
    else
        print_error "Debug build failed"
        exit 1
    fi
    
    # Try building and running tests
    print_info "Testing test framework..."
    if verbose_run make test-run-LoggerTest; then
        print_success "Test framework working correctly"
    else
        print_warning "Test framework may need attention"
    fi
    
    print_success "Setup validation completed"
}

# Show next steps
show_next_steps() {
    print_header "Bootstrap completed successfully!"
    echo
    print_info "Your CppBoilerplate project is ready for development!"
    echo
    echo -e "${YELLOW}NEXT STEPS:${NC}"
    echo "1. Build and run the project:"
    echo "   make debug && make run_debug"
    echo
    echo "2. Run all tests:"
    echo "   make test"
    echo
    echo "3. Try the automation script:"
    echo "   ./scripts/test.sh"
    echo
    echo "4. Create new modules:"
    echo "   ./scripts/create_endpoint.sh create EndpointExample"
    echo "   ./scripts/create_service.sh create ServiceExample"
    echo "   ./scripts/create_module.sh create ExampleModule"
    echo
    echo "5. Open in VS Code for full IDE experience:"
    echo "   code ."
    echo
    print_info "For more information, see README.md"
    echo
    
    if [[ $EMOJI_SUPPORT -eq 1 ]]; then
        echo -e "${GREEN}🎉 Happy coding! 🎉${NC}"
    else
        echo -e "${GREEN}Happy coding!${NC}"
    fi
}

# Main function
main() {
    print_header "CppBoilerplate Bootstrap Script"
    echo
    
    parse_args "$@"
    
    check_requirements
    setup_directories
    download_googletest
    build_googletest
    setup_external_libs
    validate_setup
    
    echo
    show_next_steps
}

# Run main function with all arguments
main "$@"
