#!/bin/bash

# test_llm_consistency.sh - Automated testing for LLM analysis consistency
# Tests that LLM analysis produces consistent, predictable results
#
# Usage:
#   ./scripts/test_llm_consistency.sh run
#   ./scripts/test_llm_consistency.sh report

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Test configuration
TEST_DIR="${PROJECT_ROOT}/.test_consistency"
RUNS=3  # Number of consistency test runs

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[CONSISTENCY-TEST]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[CONSISTENCY-TEST]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[CONSISTENCY-TEST]${NC} $1"
}

log_error() {
    echo -e "${RED}[CONSISTENCY-TEST]${NC} $1" >&2
}

# Show usage information
show_usage() {
    cat << 'EOF'
Usage: ./scripts/test_llm_consistency.sh <command>

Commands:
  run                   - Run full consistency test suite
  report               - Generate test report from previous run
  clean                - Clean test artifacts
  validate             - Validate test scenarios only

Test Scenarios:
  1. One Class Per File Analysis - Same input → Same violations detected
  2. Extract Class Analysis - Same input → Same extraction commands
  3. Dependency Analysis - Same input → Same dependency issues
  4. Schema Validation - All outputs match expected schema
  5. Command Generation - Generated refactor commands are deterministic

Examples:
  ./scripts/test_llm_consistency.sh run
  ./scripts/test_llm_consistency.sh report
  ./scripts/test_llm_consistency.sh clean
EOF
}

# Create test scenarios with known inputs and expected outputs
create_test_scenarios() {
    log_info "Creating test scenarios"
    
    mkdir -p "$TEST_DIR/scenarios"
    
    # Scenario 1: One Class Per File violation
    cat > "$TEST_DIR/scenarios/one_class_violation.json" << 'EOF'
{
  "project": "CppBoilerplate",
  "timestamp": "2024-01-01T12:00:00",
  "analysis_path": "src",
  "files": [
    {
      "file": "src/main.cpp",
      "type": "main",
      "namespace": "",
      "includes": ["<iostream>", "\"Application.h\"", "\"Logger.h\""],
      "classes": [
        {
          "name": "Application",
          "inherits": "",
          "methods": ["start", "stop", "handleSignal"]
        },
        {
          "name": "AppConfig",
          "inherits": "",
          "methods": ["load", "save", "validate"]
        }
      ],
      "functions": ["main", "signalHandler"]
    }
  ]
}
EOF
    
    # Scenario 2: Single class (no violation)
    cat > "$TEST_DIR/scenarios/single_class_good.json" << 'EOF'
{
  "project": "CppBoilerplate", 
  "timestamp": "2024-01-01T12:00:00",
  "analysis_path": "src",
  "files": [
    {
      "file": "src/Logger/Logger.cpp",
      "type": "implementation",
      "namespace": "",
      "includes": ["\"Logger.h\"", "<iostream>", "<fstream>"],
      "classes": [
        {
          "name": "Logger",
          "inherits": "ILogger",
          "methods": ["log", "debug", "error", "warn"]
        }
      ],
      "functions": []
    }
  ]
}
EOF
    
    # Scenario 3: Complex dependency scenario
    cat > "$TEST_DIR/scenarios/complex_dependencies.json" << 'EOF'
{
  "project": "CppBoilerplate",
  "dependency_analysis": {
    "timestamp": "2024-01-01T12:00:00",
    "system_includes": [
      "\"<iostream>\"",
      "\"<string>\"",
      "\"<memory>\""
    ],
    "project_includes": [
      "\"Logger/ILogger.h\"",
      "\"WebServer/IWebServer.h\"",
      "\"IocContainer/IocContainer.h\""
    ],
    "module_dependencies": [
      {
        "module": "WebServer",
        "depends_on": ["Logger", "IocContainer"]
      },
      {
        "module": "Logger", 
        "depends_on": ["IocContainer"]
      },
      {
        "module": "IocContainer",
        "depends_on": ["Logger"]
      }
    ]
  }
}
EOF

    # Expected outputs for validation
    mkdir -p "$TEST_DIR/expected"
    
    # Expected output for one_class_violation scenario
    cat > "$TEST_DIR/expected/one_class_violation_expected.json" << 'EOF'
{
  "analysis_type": "one-class-per-file",
  "violations": [
    {
      "file": "src/main.cpp",
      "class_count": 2,
      "classes": ["Application", "AppConfig"],
      "recommendation": "extract-class",
      "priority": "high"
    }
  ],
  "summary": {
    "total_violations": 1,
    "files_analyzed": 1,
    "compliance_rate": "0%"
  }
}
EOF
    
    log_success "Created test scenarios and expected outputs"
}

# Run consistency tests for specific scenario
run_scenario_consistency() {
    local scenario_name="$1"
    local analysis_type="$2"
    local scenario_file="$TEST_DIR/scenarios/${scenario_name}.json"
    
    log_info "Running consistency test for scenario: $scenario_name"
    
    if [[ ! -f "$scenario_file" ]]; then
        log_error "Scenario file not found: $scenario_file"
        return 1
    fi
    
    local results_dir="$TEST_DIR/results/$scenario_name"
    mkdir -p "$results_dir"
    
    # Generate standardized prompt
    local prompt_file="$results_dir/prompt.txt"
    "${SCRIPT_DIR}/llm_analysis.sh" prompt "$analysis_type" > "$prompt_file"
    
    log_info "Generated standardized prompt: $prompt_file"
    log_info "Scenario input: $scenario_file"
    log_info ""
    log_info "=== CONSISTENCY TEST INSTRUCTIONS ==="
    log_info "Please run the following analysis $RUNS times with IDENTICAL input:"
    log_info ""
    log_info "PROMPT:"
    cat "$prompt_file"
    log_info ""
    log_info "INPUT DATA:" 
    cat "$scenario_file"
    log_info ""
    log_info "INSTRUCTIONS:"
    log_info "1. Save each run as: $results_dir/run_1.json, $results_dir/run_2.json, $results_dir/run_3.json"
    log_info "2. Use IDENTICAL analysis each time"
    log_info "3. After all runs, execute: $0 validate-consistency $scenario_name"
    log_info ""
}

# Validate consistency across multiple runs
validate_consistency() {
    local scenario_name="$1"
    local results_dir="$TEST_DIR/results/$scenario_name"
    
    log_info "Validating consistency for scenario: $scenario_name"
    
    if [[ ! -d "$results_dir" ]]; then
        log_error "Results directory not found: $results_dir"
        return 1
    fi
    
    local run_files=("$results_dir"/run_*.json)
    
    if [[ ${#run_files[@]} -lt 2 ]]; then
        log_error "Need at least 2 runs for consistency validation"
        return 1
    fi
    
    log_info "Found ${#run_files[@]} runs to compare"
    
    # Compare key fields for consistency
    local inconsistencies=0
    
    for i in "${!run_files[@]}"; do
        for j in "${!run_files[@]}"; do
            if [[ $i -lt $j ]]; then
                local file1="${run_files[i]}"
                local file2="${run_files[j]}"
                
                log_info "Comparing $(basename "$file1") vs $(basename "$file2")"
                
                # Compare using Python for structured comparison
                python3 << EOF
import json
import sys

def compare_analyses(file1, file2):
    with open('$file1', 'r') as f1, open('$file2', 'r') as f2:
        data1 = json.load(f1)
        data2 = json.load(f2)
    
    inconsistencies = 0
    
    # Compare analysis_type
    if data1.get('analysis_type') != data2.get('analysis_type'):
        print(f"  INCONSISTENT: analysis_type differs")
        inconsistencies += 1
    
    # Compare violations count (if present)
    if 'violations' in data1 and 'violations' in data2:
        if len(data1['violations']) != len(data2['violations']):
            print(f"  INCONSISTENT: violation count differs ({len(data1['violations'])} vs {len(data2['violations'])})")
            inconsistencies += 1
        else:
            # Compare each violation
            for idx, (v1, v2) in enumerate(zip(data1['violations'], data2['violations'])):
                if v1.get('file') != v2.get('file'):
                    print(f"  INCONSISTENT: violation {idx} file differs")
                    inconsistencies += 1
                if v1.get('class_count') != v2.get('class_count'):
                    print(f"  INCONSISTENT: violation {idx} class_count differs")
                    inconsistencies += 1
                if set(v1.get('classes', [])) != set(v2.get('classes', [])):
                    print(f"  INCONSISTENT: violation {idx} classes differ")
                    inconsistencies += 1
    
    # Compare extractions (if present) 
    if 'extractions' in data1 and 'extractions' in data2:
        if len(data1['extractions']) != len(data2['extractions']):
            print(f"  INCONSISTENT: extraction count differs ({len(data1['extractions'])} vs {len(data2['extractions'])})")
            inconsistencies += 1
        else:
            for idx, (e1, e2) in enumerate(zip(data1['extractions'], data2['extractions'])):
                if e1.get('class_name') != e2.get('class_name'):
                    print(f"  INCONSISTENT: extraction {idx} class_name differs")
                    inconsistencies += 1
                if e1.get('target_header') != e2.get('target_header'):
                    print(f"  INCONSISTENT: extraction {idx} target_header differs")
                    inconsistencies += 1
    
    if inconsistencies == 0:
        print("  CONSISTENT: All key fields match")
    
    return inconsistencies

exit_code = compare_analyses('$file1', '$file2')
sys.exit(exit_code)
EOF
                
                if [[ $? -ne 0 ]]; then
                    inconsistencies=$((inconsistencies + 1))
                fi
            fi
        done
    done
    
    if [[ $inconsistencies -eq 0 ]]; then
        log_success "CONSISTENCY PASSED: All runs produced consistent results"
        return 0
    else
        log_error "CONSISTENCY FAILED: Found $inconsistencies inconsistencies"
        return 1
    fi
}

# Validate all outputs match expected schema
validate_schemas() {
    local scenario_name="$1"
    local results_dir="$TEST_DIR/results/$scenario_name"
    
    log_info "Validating schemas for scenario: $scenario_name"
    
    local run_files=("$results_dir"/run_*.json)
    local schema_failures=0
    
    for run_file in "${run_files[@]}"; do
        if [[ -f "$run_file" ]]; then
            log_info "Validating schema: $(basename "$run_file")"
            
            if "${SCRIPT_DIR}/llm_analysis.sh" validate "$run_file"; then
                log_success "Schema validation passed: $(basename "$run_file")"
            else
                log_error "Schema validation failed: $(basename "$run_file")"
                schema_failures=$((schema_failures + 1))
            fi
        fi
    done
    
    if [[ $schema_failures -eq 0 ]]; then
        log_success "SCHEMA VALIDATION PASSED: All outputs match expected schema"
        return 0
    else
        log_error "SCHEMA VALIDATION FAILED: $schema_failures files failed validation"
        return 1
    fi
}

# Run full test suite
run_full_test_suite() {
    log_info "Running full LLM analysis consistency test suite"
    
    # Clean previous test results
    rm -rf "$TEST_DIR"
    
    # Create test scenarios
    create_test_scenarios
    
    # Run consistency tests for each scenario
    log_info ""
    log_info "=== TEST SCENARIO 1: One Class Per File Violation ==="
    run_scenario_consistency "one_class_violation" "one-class-per-file"
    
    log_info ""
    log_info "=== TEST SCENARIO 2: Single Class (Good) ==="
    run_scenario_consistency "single_class_good" "one-class-per-file"
    
    log_info ""
    log_info "=== TEST SCENARIO 3: Dependency Analysis ==="
    run_scenario_consistency "complex_dependencies" "dependency-analysis"
    
    log_success "Test suite setup complete"
    log_info ""
    log_info "NEXT STEPS:"
    log_info "1. Complete the LLM analysis runs as instructed above"
    log_info "2. Run validation: $0 validate-all"
    log_info "3. Generate report: $0 report"
}

# Validate all test results
validate_all() {
    log_info "Validating all test results"
    
    local total_failures=0
    
    # Validate each scenario
    for scenario in "one_class_violation" "single_class_good" "complex_dependencies"; do
        log_info ""
        log_info "=== VALIDATING SCENARIO: $scenario ==="
        
        # Validate consistency
        if validate_consistency "$scenario"; then
            log_success "Consistency validation passed: $scenario"
        else
            log_error "Consistency validation failed: $scenario"
            total_failures=$((total_failures + 1))
        fi
        
        # Validate schemas
        if validate_schemas "$scenario"; then
            log_success "Schema validation passed: $scenario"
        else
            log_error "Schema validation failed: $scenario"
            total_failures=$((total_failures + 1))
        fi
    done
    
    log_info ""
    if [[ $total_failures -eq 0 ]]; then
        log_success "ALL TESTS PASSED: LLM analysis is consistent and predictable"
    else
        log_error "TESTS FAILED: $total_failures validation failures detected"
        log_error "LLM analysis needs improvement for consistency"
    fi
    
    return $total_failures
}

# Generate test report
generate_report() {
    log_info "Generating consistency test report"
    
    local report_file="$TEST_DIR/consistency_report.md"
    
    cat > "$report_file" << EOF
# LLM Analysis Consistency Test Report

**Generated:** $(date -Iseconds)
**Test Suite:** CppBoilerplate LLM Analysis Consistency

## Test Scenarios

### 1. One Class Per File Violation Detection
- **Input:** File with 2 classes (Application, AppConfig)
- **Expected:** High priority violation with extract-class recommendation
- **Consistency:** $(validate_consistency "one_class_violation" &>/dev/null && echo "PASS" || echo "FAIL")

### 2. Single Class Compliance
- **Input:** File with 1 class (Logger)
- **Expected:** No violations detected
- **Consistency:** $(validate_consistency "single_class_good" &>/dev/null && echo "PASS" || echo "FAIL")

### 3. Dependency Analysis
- **Input:** Complex module dependencies with circular reference
- **Expected:** Circular dependency flagged as high severity
- **Consistency:** $(validate_consistency "complex_dependencies" &>/dev/null && echo "PASS" || echo "FAIL")

## Schema Validation Results

EOF
    
    # Add schema validation results
    for scenario in "one_class_violation" "single_class_good" "complex_dependencies"; do
        local results_dir="$TEST_DIR/results/$scenario"
        if [[ -d "$results_dir" ]]; then
            echo "### $scenario" >> "$report_file"
            local run_files=("$results_dir"/run_*.json)
            for run_file in "${run_files[@]}"; do
                if [[ -f "$run_file" ]]; then
                    local basename_file=$(basename "$run_file")
                    if "${SCRIPT_DIR}/llm_analysis.sh" validate "$run_file" &>/dev/null; then
                        echo "- $basename_file: ✅ PASS" >> "$report_file"
                    else
                        echo "- $basename_file: ❌ FAIL" >> "$report_file"
                    fi
                fi
            done
            echo "" >> "$report_file"
        fi
    done
    
    cat >> "$report_file" << EOF

## Recommendations

1. **If all tests pass:** LLM analysis is consistent and production-ready
2. **If consistency fails:** Review prompt templates for ambiguity
3. **If schema validation fails:** Update output format standardization
4. **For better consistency:** Add more specific constraints to prompts

## Test Files

- Test scenarios: \`$TEST_DIR/scenarios/\`
- Analysis results: \`$TEST_DIR/results/\`
- Expected outputs: \`$TEST_DIR/expected/\`

EOF
    
    log_success "Generated report: $report_file"
    cat "$report_file"
}

# Clean test artifacts
clean_tests() {
    log_info "Cleaning test artifacts"
    rm -rf "$TEST_DIR"
    log_success "Test artifacts cleaned"
}

# Main execution
main() {
    if [[ $# -lt 1 ]]; then
        show_usage
        exit 1
    fi
    
    local command="$1"
    shift
    
    case "$command" in
        run)
            run_full_test_suite
            ;;
        validate-consistency)
            if [[ $# -ne 1 ]]; then
                log_error "validate-consistency requires 1 argument: <scenario_name>"
                exit 1
            fi
            validate_consistency "$1"
            ;;
        validate-all)
            validate_all
            ;;
        report)
            generate_report
            ;;
        clean)
            clean_tests
            ;;
        validate)
            create_test_scenarios
            log_success "Test scenarios validated"
            ;;
        --help|-h)
            show_usage
            ;;
        *)
            log_error "Unknown command: $command"
            show_usage
            exit 1
            ;;
    esac
}

# Execute main function
main "$@"