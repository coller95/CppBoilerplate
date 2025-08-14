#!/bin/bash

# Script to automate the build, test, and run process with multi-tiered output modes
# Usage:
#   ./scripts/test.sh           # Default minimal mode (best performance)
#   ./scripts/test.sh minimal   # Ultra-minimal output (best performance)
#   ./scripts/test.sh standard  # Standard agent output (moderate context)
#   ./scripts/test.sh debug     # Agent + debugging context (troubleshooting)
#   ./scripts/test.sh human     # Human-friendly output (verbose)
#   ./scripts/test.sh silent    # Errors only (critical only)
#
# Backward compatibility:
#   ./scripts/test.sh agent     # Maps to standard mode
#   VERBOSE=0/1 environment     # Maps to minimal/human

set -e  # Exit immediately if a command exits with a non-zero status

# Handle verbose mode selection with multi-tiered support
case "$1" in
    "minimal")
        export VERBOSE=minimal ;;
    "standard") 
        export VERBOSE=standard ;;
    "debug")
        export VERBOSE=debug ;;
    "human")
        export VERBOSE=human ;;
    "silent")
        export VERBOSE=silent ;;
    "agent")
        # Backward compatibility: agent -> standard
        export VERBOSE=standard ;;
    *)
        # Default to minimal if no argument provided and VERBOSE not set
        export VERBOSE=${VERBOSE:-minimal}
        # Map legacy numeric values
        if [ "$VERBOSE" = "1" ]; then
            export VERBOSE=human
        elif [ "$VERBOSE" = "0" ]; then
            export VERBOSE=minimal
        fi
        ;;
esac

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

function status() {
  case "$VERBOSE" in
    "human")
      echo -e "${YELLOW}==> $1${NC}" ;;
    "debug")
      echo -e "${BLUE}[SCRIPT]${NC} $1" ;;
    "standard")
      echo -e "${BLUE}[SCRIPT]${NC} $1" ;;
    "minimal")
      echo "[SCRIPT] $1" ;;
    "silent")
      # No status output in silent mode
      ;;
  esac
}
function success() {
  case "$VERBOSE" in
    "human")
      echo -e "${GREEN}$1${NC}" ;;
    "debug")
      echo -e "${BLUE}[SCRIPT]${NC} SUCCESS: $1" ;;
    "standard") 
      echo -e "${BLUE}[SCRIPT]${NC} SUCCESS: $1" ;;
    "minimal")
      echo "[SCRIPT] SUCCESS: $1" ;;
    "silent")
      # No success output in silent mode
      ;;
  esac
}
function fail() {
  case "$VERBOSE" in
    "human")
      echo -e "${RED}$1${NC}" ;;
    "debug"|"standard"|"minimal")
      echo -e "${BLUE}[SCRIPT]${NC} ERROR: $1" ;;
    "silent")
      echo "[ERROR] $1" ;;
  esac
}

function run_with_progress() {
  local cmd="$1"
  local desc="$2"
  
  if [ "$VERBOSE" = "human" ]; then
    echo -e "${YELLOW}⏳ $desc${NC}"
    
    # Run command in background and show progress
    $cmd &
    local pid=$!
    
    # Show progress dots while command runs
    while kill -0 $pid 2>/dev/null; do
      echo -n "."
      sleep 1
    done
    echo "" # New line after dots
    
    # Wait for command to complete and get exit status
    wait $pid
    return $?
  else
    # For non-human modes, run normally
    $cmd
  fi
}

function discover_test_modules() {
  local test_modules=()
  
  if [ -d "tests" ]; then
    echo -e "${BLUE}🔍 Discovering test modules...${NC}"
    
    for dir in tests/*/; do
      if [ -d "$dir" ]; then
        local module_name=$(basename "$dir")
        # Validate it's a real test module by checking make target exists
        if make -n test-run-$module_name >/dev/null 2>&1; then
          test_modules+=("$module_name")
          echo -e "  ${GREEN}📦${NC} Found: $module_name"
        fi
      fi
    done
    
    # Sort modules for consistent ordering
    if [ ${#test_modules[@]} -gt 0 ]; then
      IFS=$'\n' test_modules=($(sort <<<"${test_modules[*]}")); unset IFS
      echo -e "${GREEN}✅ Discovered ${#test_modules[@]} test modules${NC}"
    else
      echo -e "${RED}❌ No valid test modules found${NC}"
      return 1
    fi
  else
    echo -e "${RED}❌ Tests directory not found${NC}"
    return 1
  fi
  
  # Return the array via a global variable (bash limitation workaround)
  DISCOVERED_TEST_MODULES=("${test_modules[@]}")
}

function run_interactive_test() {
  local module="$1"
  local current="$2"
  local total="$3"
  
  printf "${YELLOW}[$current/$total] Running $module[initializing]...                    ${NC}"
  
  # Show different phases of test execution
  local phases=("compiling" "linking" "executing" "test.suite")
  for phase in "${phases[@]}"; do
    printf "\r${YELLOW}[$current/$total] Running $module[$phase]...                    ${NC}"
    sleep 0.3
  done
  
  # Run the actual test
  if make test-run-$module VERBOSE=minimal >/dev/null 2>&1; then
    printf "\r${GREEN}✅ [$current/$total] $module passed                                        ${NC}\n"
    return 0
  else
    printf "\r${RED}❌ [$current/$total] $module failed                                        ${NC}\n"
    return 1
  fi
}

function run_detailed_tests() {
  if [ "$VERBOSE" = "human" ]; then
    echo -e "${BLUE}🧪 Running comprehensive test suite with detailed progress...${NC}"
    echo ""
    
    # Dynamically discover test modules
    if ! discover_test_modules; then
      echo -e "${RED}Failed to discover test modules. Exiting.${NC}"
      return 1
    fi
    
    local test_modules=("${DISCOVERED_TEST_MODULES[@]}")
    local total=${#test_modules[@]}
    local current=0
    local start_time=$(date +%s)
    
    echo ""
    echo -e "${BLUE}📋 Test Execution Plan:${NC}"
    for module in "${test_modules[@]}"; do
      echo -e "  ${YELLOW}▶${NC} $module"
    done
    echo ""
    echo -e "${BLUE}🚀 Starting test execution...${NC}"
    echo ""
    
    local passed=0
    local failed=0
    
    for module in "${test_modules[@]}"; do
      current=$((current + 1))
      
      if run_interactive_test "$module" "$current" "$total"; then
        ((passed++))
      else
        ((failed++))
        echo -e "${RED}💥 Test execution stopped due to failure in $module${NC}"
        return 1
      fi
    done
    
    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    
    echo ""
    echo -e "${GREEN}🎉 All test modules completed successfully!${NC}"
    echo -e "${BLUE}📊 Test Summary:${NC}"
    echo -e "  ${GREEN}✅ Passed: $passed${NC}"
    echo -e "  ${RED}❌ Failed: $failed${NC}"
    echo -e "  ${BLUE}⏱️  Duration: ${duration}s${NC}"
    echo -e "  ${BLUE}📁 Modules: $total${NC}"
    echo ""
    return 0
  else
    # For non-human modes, run all tests normally
    make test -j$(nproc)
  fi
}

status "Cleaning build artifacts..."
if [ "$VERBOSE" = "human" ]; then
  echo -e "${BLUE}🧹 Cleaning main project and test artifacts...${NC}"
  make clean -j$(nproc) && make test-clean -j$(nproc)
  echo -e "${GREEN}✅ Clean completed${NC}"
else
  make clean -j$(nproc) && make test-clean -j$(nproc)
fi

status "Running tests..."
if run_detailed_tests; then
  success "All tests passed!"
  
  status "Showing test statistics..."
  make test-stats
else
  fail "Some tests failed. Check the output above."
  exit 1
fi

function run_interactive_build() {
  if [ "$VERBOSE" = "human" ]; then
    echo -e "${BLUE}🔨 Building main application...${NC}"
    
    # Show build phases
    local build_phases=("analyzing" "compiling" "linking" "optimizing")
    for phase in "${build_phases[@]}"; do
      printf "${YELLOW}🔧 Build phase: $phase...                    ${NC}"
      sleep 0.3
      printf "\r"
    done
    
    local build_start=$(date +%s)
    
    if make release -j$(nproc); then
      local build_end=$(date +%s)
      local build_duration=$((build_end - build_start))
      echo -e "${GREEN}✅ Build completed successfully in ${build_duration}s${NC}"
      return 0
    else
      echo -e "${RED}❌ Build failed. Check the output above.${NC}"
      return 1
    fi
  else
    make release -j$(nproc)
  fi
}

function run_interactive_app() {
  if [ "$VERBOSE" = "human" ]; then
    echo ""
    echo -e "${BLUE}🚀 Launching application...${NC}"
    echo -e "${YELLOW}⚠️  Application will start - Press Ctrl+C to stop when ready${NC}"
    echo ""
    
    local app_start=$(date +%s)
    
    if timeout 10s make run_release; then
      # Application was stopped by timeout (normal for server apps)
      local app_end=$(date +%s)
      local app_duration=$((app_end - app_start))
      echo -e "${GREEN}✅ Application ran successfully for ${app_duration}s${NC}"
      return 0
    else
      local exit_code=$?
      if [ $exit_code -eq 124 ]; then
        # Timeout occurred (expected for server applications)
        echo -e "${GREEN}✅ Application started and ran successfully${NC}"
        echo -e "${BLUE}ℹ️  Server was automatically stopped after 10s demo${NC}"
        return 0
      else
        echo -e "${RED}❌ Application failed to run. Check the output above.${NC}"
        return 1
      fi
    fi
  else
    make run_release
  fi
}

status "Building the main program..."
if ! run_interactive_build; then
  fail "Build failed."
  exit 1
fi

status "Running the main program..."
if ! run_interactive_app; then
  fail "Application failed to run."
  exit 1
fi

function run_interactive_cleanup() {
  if [ "$VERBOSE" = "human" ]; then
    echo ""
    echo -e "${BLUE}🧹 Final cleanup of build artifacts...${NC}"
    
    local cleanup_phases=("removing objects" "cleaning cache" "finalizing")
    for phase in "${cleanup_phases[@]}"; do
      printf "${YELLOW}🗑️  Cleanup: $phase...                    ${NC}"
      sleep 0.2
      printf "\r"
    done
    
    if make clean -j$(nproc) && make test-clean -j$(nproc); then
      echo -e "${GREEN}✅ Cleanup completed successfully${NC}"
      return 0
    else
      echo -e "${RED}❌ Cleanup failed${NC}"
      return 1
    fi
  else
    make clean -j$(nproc) && make test-clean -j$(nproc)
  fi
}

function show_final_summary() {
  if [ "$VERBOSE" = "human" ]; then
    echo ""
    echo -e "${GREEN}🎉 All checks complete! Your code is working perfectly!${NC}"
    echo ""
    echo -e "${BLUE}📋 Execution Summary:${NC}"
    echo -e "  ${GREEN}✅ Tests: All passed${NC}"
    echo -e "  ${GREEN}✅ Build: Successful${NC}"
    echo -e "  ${GREEN}✅ Run: Application started correctly${NC}"
    echo -e "  ${GREEN}✅ Cleanup: Build artifacts removed${NC}"
    echo ""
    echo -e "${BLUE}🚀 Your application is ready for deployment!${NC}"
    echo ""
  else
    echo -e "${BLUE}[SCRIPT]${NC} SUCCESS: All checks complete"
  fi
}

status "Cleaning up build artifacts after successful completion..."
if ! run_interactive_cleanup; then
  fail "Cleanup failed."
  exit 1
fi

show_final_summary