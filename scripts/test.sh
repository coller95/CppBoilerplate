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

status "Cleaning build artifacts..."
make clean -j$(nproc) && make test-clean -j$(nproc)

status "Running tests..."
if make test -j$(nproc); then
  success "All tests passed!"
else
  fail "Some tests failed. Check the output above."
  exit 1
fi

status "Building and running the main program..."
if make run -j$(nproc); then
  success "Main program built and ran successfully!"
else
  fail "Main program failed to build or run."
  exit 1
fi

status "Cleaning up build artifacts after successful completion..."
make clean -j$(nproc) && make test-clean -j$(nproc)

if [ "$VERBOSE" = "1" ]; then
  echo -e "${GREEN}All checks complete. Your code is working! Build artifacts cleaned up.${NC}"
else
  echo -e "${BLUE}[SCRIPT]${NC} SUCCESS: All checks complete"
fi