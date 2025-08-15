#!/bin/bash

# ============================================================================
# CppBoilerplate Test Script - Multi-Mode Build, Test, and Run Automation
# ============================================================================
# 
# Usage:
#   ./scripts/test.sh           # Default: minimal mode (agent-friendly)
#   ./scripts/test.sh minimal   # Ultra-concise: TEST=OK BUILD=OK RUN=OK TIME=42s
#   ./scripts/test.sh human     # Human-friendly (emojis, colors, detailed)
#   ./scripts/test.sh debug     # Show everything (full compiler output)
#
# Author: Generated with Claude Code
# ============================================================================

set -e  # Exit immediately if a command exits with a non-zero status

# ============================================================================
# CONFIGURATION & SETUP
# ============================================================================

# Mode selection and configuration
MODE="minimal"
case "$1" in
    "minimal") MODE="minimal"; export VERBOSE=minimal ;;
    "human")   MODE="human";   export VERBOSE=human ;;
    "debug")   MODE="debug";   export VERBOSE=debug ;;
    *)         MODE="minimal"; export VERBOSE=minimal ;;
esac

# Color definitions for human mode
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color

# Timing for minimal mode
if [ "$MODE" = "minimal" ]; then
    readonly START_TIME=$(date +%s)
fi

# ============================================================================
# UTILITY FUNCTIONS
# ============================================================================

# Cleanup function for temporary files
cleanup_temp_files() {
    # Clean up any remaining temp files that might be left behind
    find /tmp -name "test_*.log" -user $(whoami) -delete 2>/dev/null || true
    find /tmp -name "human_test.log" -user $(whoami) -delete 2>/dev/null || true
}

# Set up cleanup trap
trap cleanup_temp_files EXIT

# Clean build artifacts
cleanup() {
    local verbose_flag="$1"
    make clean ${verbose_flag:+VERBOSE=$verbose_flag} >/dev/null 2>&1
    make test-clean ${verbose_flag:+VERBOSE=$verbose_flag} >/dev/null 2>&1
}

# Run tests with specified verbosity
run_tests() {
    local verbose_flag="$1"
    make test ${verbose_flag:+VERBOSE=$verbose_flag} ${verbose_flag:+-j$(nproc)}
}

# Build both debug and release
build_all() {
    local verbose_flag="$1"
    make debug ${verbose_flag:+VERBOSE=$verbose_flag} ${verbose_flag:+-j$(nproc)}
    make release ${verbose_flag:+VERBOSE=$verbose_flag} ${verbose_flag:+-j$(nproc)}
}

# Test both debug and release applications
test_applications() {
    local verbose_flag="$1"
    make test_debug ${verbose_flag:+VERBOSE=$verbose_flag}
    make test_release ${verbose_flag:+VERBOSE=$verbose_flag}
}

# Discover available test modules
discover_test_modules() {
    local modules=()
    
    if [ ! -d "tests" ]; then
        echo "ERROR: Tests directory not found" >&2
        exit 1
    fi
    
    for dir in tests/*/; do
        if [ -d "$dir" ]; then
            local module_name=$(basename "$dir")
            if make -n test-run-$module_name >/dev/null 2>&1; then
                modules+=("$module_name")
            fi
        fi
    done
    
    if [ ${#modules[@]} -eq 0 ]; then
        echo "ERROR: No valid test modules found" >&2
        exit 1
    fi
    
    # Sort for consistent ordering
    IFS=$'\n' modules=($(sort <<<"${modules[*]}")); unset IFS
    
    # Return via global variable (bash limitation workaround)
    TEST_MODULES=("${modules[@]}")
}

# ============================================================================
# MODE-SPECIFIC HANDLERS
# ============================================================================

# Minimal mode: Ultra-concise output for agents/LLMs
run_minimal_mode() {
    # Clean (silent)
    cleanup
    
    # Test (capture result)
    if run_tests >/dev/null 2>&1; then
        echo -n "TEST=OK "
    else
        echo -n "TEST=FAIL "
        exit 1
    fi
    
    # Build (capture result)
    if build_all >/dev/null 2>&1; then
        echo -n "BUILD=OK "
    else
        echo -n "BUILD=FAIL "
        exit 1
    fi
    
    # Run (capture result)
    if test_applications >/dev/null 2>&1; then
        echo -n "RUN=OK "
    else
        echo -n "RUN=FAIL "
        exit 1
    fi
    
    # Final cleanup (silent)
    cleanup
    
    # Output timing
    local end_time=$(date +%s)
    local duration=$((end_time - START_TIME))
    echo "TIME=${duration}s"
}

# Human mode: Rich interactive experience
run_human_mode() {
    echo -e "${BLUE}🧹 Cleaning main project and test artifacts...${NC}"
    cleanup human
    echo -e "${GREEN}✅ Clean completed${NC}"

    echo -e "${BLUE}🧪 Running comprehensive test suite with detailed progress...${NC}"
    echo ""
    
    # Discover test modules with rich feedback
    echo -e "${BLUE}🔍 Discovering test modules...${NC}"
    discover_test_modules
    
    for module in "${TEST_MODULES[@]}"; do
        echo -e "  ${GREEN}📦${NC} Found: $module"
    done
    echo -e "${GREEN}✅ Discovered ${#TEST_MODULES[@]} test modules${NC}"
    
    # Show execution plan
    local total=${#TEST_MODULES[@]}
    echo ""
    echo -e "${BLUE}📋 Test Execution Plan:${NC}"
    for module in "${TEST_MODULES[@]}"; do
        echo -e "  ${YELLOW}▶${NC} $module"
    done
    echo ""
    echo -e "${BLUE}🚀 Starting test execution...${NC}"
    echo ""
    
    # Execute tests with progress tracking
    local current=0
    local passed=0
    local start_time=$(date +%s)
    
    # Just run the standard test command instead of individual modules to avoid hanging
    echo -e "${YELLOW}▶️  Running all test modules...${NC}"
    local temp_log=$(mktemp)
    if run_tests human >"$temp_log" 2>&1; then
        echo -e "${GREEN}✅ All test modules passed${NC}"
        passed=${#TEST_MODULES[@]}
        rm -f "$temp_log"
    else
        echo -e "${RED}❌ Some test modules failed${NC}"
        echo -e "${RED}💥 Test execution stopped${NC}"
        if [ -f "$temp_log" ]; then
            echo -e "${RED}Last few lines of output:${NC}"
            tail -10 "$temp_log"
            rm -f "$temp_log"
        fi
        exit 1
    fi
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    echo ""
    echo -e "${GREEN}🎉 All test modules completed successfully!${NC}"
    echo -e "${BLUE}📊 Test Summary:${NC}"
    echo -e "  ${GREEN}✅ Passed: $passed${NC}"
    echo -e "  ${BLUE}⏱️  Duration: ${duration}s${NC}"
    echo -e "  ${BLUE}📁 Modules: $total${NC}"
    echo ""

    echo -e "${BLUE}📊 Test Statistics:${NC}"
    make test-stats VERBOSE=human

    echo -e "${BLUE}🔨 Building main application (debug + release)...${NC}"
    local build_start=$(date +%s)
    
    if build_all; then
        local build_end=$(date +%s)
        local build_duration=$((build_end - build_start))
        echo -e "${GREEN}✅ Both builds completed successfully in ${build_duration}s${NC}"
    else
        echo -e "${RED}❌ Build failed. Check the output above.${NC}"
        exit 1
    fi

    echo ""
    echo -e "${BLUE}🚀 Testing both debug and release builds...${NC}"
    echo -e "${YELLOW}⚠️  Each build will run for 5 seconds then automatically stop${NC}"
    echo ""
    
    local app_test_start=$(date +%s)
    
    echo -e "${BLUE}📝 Testing DEBUG build...${NC}"
    if ! make test_debug; then
        echo -e "${RED}❌ Debug build failed to run. Check the output above.${NC}"
        exit 1
    fi
    echo -e "${GREEN}✅ Debug build test completed${NC}"
    echo ""
    
    echo -e "${BLUE}🚀 Testing RELEASE build...${NC}"
    if ! make test_release; then
        echo -e "${RED}❌ Release build failed to run. Check the output above.${NC}"
        exit 1
    fi
    echo -e "${GREEN}✅ Release build test completed${NC}"
    
    local app_test_end=$(date +%s)
    local app_test_duration=$((app_test_end - app_test_start))
    echo ""
    echo -e "${GREEN}🎉 Both builds tested successfully in ${app_test_duration}s${NC}"

    echo ""
    echo -e "${BLUE}🧹 Final cleanup of build artifacts...${NC}"
    if cleanup; then
        echo -e "${GREEN}✅ Cleanup completed successfully${NC}"
    else
        echo -e "${RED}❌ Cleanup failed${NC}"
        exit 1
    fi

    echo ""
    echo -e "${GREEN}🎉 All checks complete! Your code is working perfectly!${NC}"
    echo ""
    echo -e "${BLUE}📋 Execution Summary:${NC}"
    echo -e "  ${GREEN}✅ Tests: All unit tests passed${NC}"
    echo -e "  ${GREEN}✅ Build: Debug + Release builds successful${NC}"
    echo -e "  ${GREEN}✅ Debug Test: Application started and ran correctly${NC}"
    echo -e "  ${GREEN}✅ Release Test: Application started and ran correctly${NC}"
    echo -e "  ${GREEN}✅ Cleanup: Build artifacts removed${NC}"
    echo ""
    echo -e "${BLUE}🚀 Both debug and release builds are ready for deployment!${NC}"
    echo ""
}

# Debug mode: Show everything with full output
run_debug_mode() {
    echo -e "${BLUE}[DEBUG]${NC} Cleaning build artifacts..."
    cleanup debug
    
    echo -e "${BLUE}[DEBUG]${NC} Running all tests with full verbose output..."
    run_tests debug
    
    echo -e "${BLUE}[DEBUG]${NC} Showing test statistics..."
    make test-stats VERBOSE=debug
    
    echo -e "${BLUE}[DEBUG]${NC} Building debug version with full compiler output..."
    make debug VERBOSE=debug -j$(nproc)
    
    echo -e "${BLUE}[DEBUG]${NC} Building release version with full compiler output..."
    make release VERBOSE=debug -j$(nproc)
    
    echo -e "${BLUE}[DEBUG]${NC} Testing debug build with full application output..."
    make test_debug VERBOSE=debug
    
    echo -e "${BLUE}[DEBUG]${NC} Testing release build with full application output..."
    make test_release VERBOSE=debug
    
    echo -e "${BLUE}[DEBUG]${NC} Final cleanup with full verbose output..."
    cleanup debug
    
    echo -e "${BLUE}[DEBUG]${NC} All operations completed successfully"
}

# ============================================================================
# MAIN EXECUTION FLOW
# ============================================================================

main() {
    case "$MODE" in
        "minimal") run_minimal_mode ;;
        "human")   run_human_mode ;;
        "debug")   run_debug_mode ;;
        *)         
            echo "ERROR: Unknown mode '$MODE'" >&2
            exit 1
            ;;
    esac
}

# Execute main function
main "$@"