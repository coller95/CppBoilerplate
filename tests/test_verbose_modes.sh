#!/bin/bash

# TDD: Test script for multi-tiered verbose mode system
# Tests the new VERBOSE modes: minimal, standard, debug, human, silent

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0

function test_assert() {
    local description="$1"
    local command="$2"
    local expected_pattern="$3"
    
    echo "Testing: $description"
    
    # Run command and capture output
    local output=$(eval "$command" 2>&1 || true)
    
    if echo "$output" | grep -q "$expected_pattern"; then
        echo -e "${GREEN}✓ PASS${NC}: $description"
        ((PASS_COUNT++))
    else
        echo -e "${RED}✗ FAIL${NC}: $description"
        echo "  Expected pattern: $expected_pattern"
        echo "  Actual output: $output"
        ((FAIL_COUNT++))
    fi
    echo
}

echo "=== Multi-Tiered Verbose Mode Tests ==="
echo

# Test 1: Default mode should be minimal
test_assert \
    "Default mode is minimal" \
    "make clean VERBOSE=" \
    "\[BUILD\] CLEAN"

# Test 2: Minimal mode produces ultra-concise output
test_assert \
    "Minimal mode ultra-concise" \
    "make debug VERBOSE=minimal" \
    "\[BUILD\] hello_world"

# Test 3: Standard mode shows more context
test_assert \
    "Standard mode shows build target" \
    "make debug VERBOSE=standard" \
    "\[BUILD\] BUILT.*hello_world"

# Test 4: Debug mode shows compilation steps
test_assert \
    "Debug mode shows compilation" \
    "make debug VERBOSE=debug" \
    "\[BUILD\] COMPILE.*src/"

# Test 5: Human mode shows emojis and colors
test_assert \
    "Human mode shows decorative elements" \
    "make debug VERBOSE=human" \
    "🔨\|✅\|Successfully"

# Test 6: Silent mode shows minimal output
test_assert \
    "Silent mode minimal output" \
    "make clean VERBOSE=silent" \
    "^$\|\[ERROR\]"

# Test 7: Backward compatibility - VERBOSE=0 maps to minimal
test_assert \
    "Backward compatibility VERBOSE=0" \
    "make clean VERBOSE=0" \
    "\[BUILD\] CLEAN"

# Test 8: Backward compatibility - VERBOSE=1 maps to human  
test_assert \
    "Backward compatibility VERBOSE=1" \
    "make clean VERBOSE=1" \
    "🧹\|Cleaning"

echo "=== Test Results ==="
echo "Passed: $PASS_COUNT"
echo "Failed: $FAIL_COUNT"

if [ $FAIL_COUNT -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi