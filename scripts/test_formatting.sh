#!/bin/bash

# Test script to verify carriage return formatting
export VERBOSE=human

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🧪 Running test formatting demo with carriage return...${NC}"

test_modules=("LoggerTest" "IocContainerTest" "WebServerTest")
total=${#test_modules[@]}
current=0

for module in "${test_modules[@]}"; do
  current=$((current + 1))
  printf "${YELLOW}[$current/$total] Running $module...${NC}"
  
  # Simulate test execution time
  sleep 1
  
  # Overwrite the "Running..." line with result
  printf "\r${GREEN}✅ [$current/$total] $module passed${NC}\n"
done

echo -e "${GREEN}🎉 All test modules completed successfully!${NC}"