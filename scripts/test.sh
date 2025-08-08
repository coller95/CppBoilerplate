#!/bin/bash

# Script to automate the build, test, and run process

set -e  # Exit immediately if a command exits with a non-zero status

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function status() {
  echo -e "${YELLOW}==> $1${NC}"
}
function success() {
  echo -e "${GREEN}$1${NC}"
}
function fail() {
  echo -e "${RED}$1${NC}"
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

echo -e "${GREEN}All checks complete. Your code is working!${NC}"