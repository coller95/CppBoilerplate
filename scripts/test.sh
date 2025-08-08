#!/bin/bash

# Script to automate the build, test, and run process
# Usage: 
#   ./scripts/test.sh           # Default agent mode (VERBOSE=0)
#   ./scripts/test.sh human     # Human-friendly mode (VERBOSE=1)
#   ./scripts/test.sh agent     # Explicit agent mode (VERBOSE=0)
#   VERBOSE=1 ./scripts/test.sh # Environment variable override

set -e  # Exit immediately if a command exits with a non-zero status

# Handle verbose mode selection
if [ "$1" = "human" ]; then
    export VERBOSE=1
elif [ "$1" = "agent" ]; then
    export VERBOSE=0
else
    # Default to agent mode if no argument provided and VERBOSE not set
    export VERBOSE=${VERBOSE:-0}
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

function status() {
  if [ "$VERBOSE" = "1" ]; then
    echo -e "${YELLOW}==> $1${NC}"
  else
    echo -e "${BLUE}[SCRIPT]${NC} $1"
  fi
}
function success() {
  if [ "$VERBOSE" = "1" ]; then
    echo -e "${GREEN}$1${NC}"
  else
    echo -e "${BLUE}[SCRIPT]${NC} SUCCESS: $1"
  fi
}
function fail() {
  if [ "$VERBOSE" = "1" ]; then
    echo -e "${RED}$1${NC}"
  else
    echo -e "${BLUE}[SCRIPT]${NC} ERROR: $1"
  fi
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