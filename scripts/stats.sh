#!/bin/bash

# Script to show project statistics
# Usage: ./scripts/stats.sh [minimal|standard|debug|human|silent]

set -e

# Handle verbose mode selection
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
        export VERBOSE=${VERBOSE:-minimal} ;;
esac

echo "📊 Project Statistics:"
make test-stats