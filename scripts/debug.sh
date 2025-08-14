#!/bin/bash

# Script for quick development workflow: test, build debug, and run
# Usage:
#   ./scripts/debug.sh           # Default minimal mode (best performance)
#   ./scripts/debug.sh minimal   # Ultra-minimal output (best performance)
#   ./scripts/debug.sh standard  # Standard agent output (moderate context)
#   ./scripts/debug.sh debug     # Agent + debugging context (troubleshooting)
#   ./scripts/debug.sh human     # Human-friendly output (verbose)
#   ./scripts/debug.sh silent    # Errors only (critical only)

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
      echo -e "${BLUE}[DEBUG]${NC} $1" ;;
    "standard")
      echo -e "${BLUE}[DEBUG]${NC} $1" ;;
    "minimal")
      echo "[DEBUG] $1" ;;
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
      echo -e "${BLUE}[DEBUG]${NC} SUCCESS: $1" ;;
    "standard") 
      echo -e "${BLUE}[DEBUG]${NC} SUCCESS: $1" ;;
    "minimal")
      echo "[DEBUG] SUCCESS: $1" ;;
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
      echo -e "${BLUE}[DEBUG]${NC} ERROR: $1" ;;
    "silent")
      echo "[ERROR] $1" ;;
  esac
}

function run_quick_tests() {
  if [ "$VERBOSE" = "human" ]; then
    echo -e "${BLUE}🧪 Running quick test validation...${NC}"
    if make test -j$(nproc); then
      echo -e "${GREEN}✅ All tests passed - ready for debug session${NC}"
      return 0
    else
      echo -e "${RED}❌ Tests failed - please fix before debugging${NC}"
      return 1
    fi
  else
    # For non-human modes, run tests quietly
    make test -j$(nproc)
  fi
}

function build_debug() {
  if [ "$VERBOSE" = "human" ]; then
    echo -e "${BLUE}🔨 Building debug version...${NC}"
    
    local build_start=$(date +%s)
    
    if make debug -j$(nproc); then
      local build_end=$(date +%s)
      local build_duration=$((build_end - build_start))
      echo -e "${GREEN}✅ Debug build completed in ${build_duration}s${NC}"
      return 0
    else
      echo -e "${RED}❌ Debug build failed${NC}"
      return 1
    fi
  else
    make debug -j$(nproc)
  fi
}

function run_debug() {
  if [ "$VERBOSE" = "human" ]; then
    echo ""
    echo -e "${BLUE}🚀 Starting debug session...${NC}"
    echo -e "${GREEN}🔧 Debug mode features:${NC}"
    echo -e "  • Symbol information for debugging"
    echo -e "  • Detailed logging and error information"
    echo -e "  • No compiler optimizations"
    echo ""
    echo -e "${YELLOW}📝 Server will start at http://127.0.0.1:8080${NC}"
    echo -e "${YELLOW}⚡ Press Ctrl+C to stop the server${NC}"
    echo ""
    
    # Run in normal mode (not test mode) so it stays running
    make run_debug
  else
    # For non-human modes, just start the debug server
    make run_debug
  fi
}

# Quick development workflow
status "Running quick test validation..."
if ! run_quick_tests; then
  fail "Tests failed. Fix tests before debugging."
  exit 1
fi

status "Building debug version..."
if ! build_debug; then
  fail "Debug build failed."
  exit 1
fi

status "Starting debug session..."
if [ "$VERBOSE" = "human" ]; then
  echo ""
  echo -e "${GREEN}🎉 Ready for debugging!${NC}"
  echo ""
fi

run_debug