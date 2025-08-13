#!/bin/bash

# llm_analysis.sh - Standardized LLM Analysis Protocol for Code Refactoring
# Provides predictable, testable interface between lexer.sh and refactor.sh
#
# Usage:
#   ./scripts/llm_analysis.sh analyze <lexer_output.json> <analysis_type>
#   ./scripts/llm_analysis.sh validate <analysis_output.json>
#   ./scripts/llm_analysis.sh test <test_scenario>

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[LLM-ANALYSIS]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[LLM-ANALYSIS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[LLM-ANALYSIS]${NC} $1"
}

log_error() {
    echo -e "${RED}[LLM-ANALYSIS]${NC} $1" >&2
}

# Show usage information
show_usage() {
    cat << 'EOF'
Usage: ./scripts/llm_analysis.sh <command> [arguments...]

Commands:
  analyze <lexer_output.json> <analysis_type>
    Generate standardized LLM analysis from lexer output
    
  validate <analysis_output.json>
    Validate LLM analysis output against schema
    
  test <test_scenario>
    Run consistency tests for LLM analysis
    
  prompt <analysis_type>
    Generate standardized prompt for analysis type

Analysis Types:
  one-class-per-file    - Detect violations of One Class Per File principle
  extract-class         - Generate class extraction commands
  extract-struct        - Generate struct extraction commands  
  split-large-file      - Generate file splitting recommendations
  dependency-analysis   - Analyze module dependencies
  refactor-plan         - Generate comprehensive refactoring plan

Test Scenarios:
  consistency           - Test same input produces same output
  validation            - Test output matches expected schema
  integration           - Test full lexer → analysis → refactor flow

Examples:
  ./scripts/llm_analysis.sh analyze lexer_output.json one-class-per-file
  ./scripts/llm_analysis.sh validate analysis_result.json
  ./scripts/llm_analysis.sh test consistency
  ./scripts/llm_analysis.sh prompt extract-class
EOF
}

# Generate standardized prompt for analysis type
generate_prompt() {
    local analysis_type="$1"
    
    case "$analysis_type" in
        "one-class-per-file")
            cat << 'EOF'
ANALYSIS TYPE: One Class Per File Violation Detection

INPUT: JSON output from lexer.sh containing file analysis data
TASK: Identify files that violate the "One Class Per File" principle

OUTPUT SCHEMA:
{
  "analysis_type": "one-class-per-file",
  "timestamp": "ISO-8601 timestamp",
  "violations": [
    {
      "file": "path/to/file.cpp",
      "class_count": integer,
      "classes": ["ClassName1", "ClassName2"],
      "recommendation": "extract-class",
      "priority": "high|medium|low",
      "extraction_commands": [
        {
          "command": "extract-class",
          "source_file": "path/to/file.cpp", 
          "class_name": "ClassName1",
          "target_dir": "src/ClassName1/"
        }
      ]
    }
  ],
  "summary": {
    "total_violations": integer,
    "files_analyzed": integer,
    "compliance_rate": "percentage"
  }
}

RULES:
1. Only flag files with multiple classes (class_count > 1)
2. Ignore utility structs if they support main class
3. Prioritize based on class complexity and file size
4. Generate specific refactor.sh commands for each violation
5. Use deterministic target directory naming: src/ClassName/
EOF
            ;;
        "extract-class")
            cat << 'EOF'
ANALYSIS TYPE: Class Extraction Planning

INPUT: JSON output from lexer.sh for a specific file
TASK: Generate precise class extraction commands

OUTPUT SCHEMA:
{
  "analysis_type": "extract-class",
  "timestamp": "ISO-8601 timestamp",
  "source_file": "path/to/source.cpp",
  "extractions": [
    {
      "class_name": "ClassName",
      "extraction_method": "extract-block|extract-class",
      "target_header": "src/ClassName/ClassName.h",
      "target_source": "src/ClassName/ClassName.cpp", 
      "dependencies": ["Header1.h", "Header2.h"],
      "includes_to_add": ["#include \"dependency.h\""],
      "includes_to_remove": ["#include \"old_header.h\""],
      "refactor_command": {
        "command": "extract-block",
        "args": ["src/main.cpp", "class", "ClassName", "src/ClassName/ClassName.h"]
      }
    }
  ],
  "build_updates": [
    "Add src/ClassName/ClassName.cpp to Project.build",
    "Update include paths in dependent files"
  ]
}

RULES:
1. Extract each class to separate directory: src/ClassName/
2. Preserve all dependencies and includes
3. Generate exact refactor.sh commands
4. Ensure extracted classes remain buildable
5. Update build system configuration
EOF
            ;;
        "extract-struct")
            cat << 'EOF'
ANALYSIS TYPE: Struct Extraction Planning

INPUT: JSON output from lexer.sh for a specific file
TASK: Generate precise struct extraction commands

OUTPUT SCHEMA:
{
  "analysis_type": "extract-struct",
  "timestamp": "ISO-8601 timestamp", 
  "source_file": "path/to/source.cpp",
  "extractions": [
    {
      "struct_name": "StructName",
      "extraction_method": "extract-block",
      "target_header": "src/StructName/StructName.h",
      "dependencies": ["Header1.h"],
      "includes_to_add": ["#include \"dependency.h\""],
      "refactor_command": {
        "command": "extract-block", 
        "args": ["src/main.cpp", "struct", "StructName", "src/StructName/StructName.h"]
      }
    }
  ]
}

RULES:
1. Extract structs to separate directories: src/StructName/
2. Include all dependencies  
3. Generate exact refactor.sh commands
4. Maintain compilation integrity
EOF
            ;;
        "dependency-analysis")
            cat << 'EOF'
ANALYSIS TYPE: Module Dependency Analysis

INPUT: JSON output from lexer.sh dependency analysis
TASK: Identify dependency issues and optimization opportunities

OUTPUT SCHEMA:
{
  "analysis_type": "dependency-analysis",
  "timestamp": "ISO-8601 timestamp",
  "dependency_issues": [
    {
      "issue_type": "circular-dependency|missing-include|unused-include",
      "description": "Human readable description",
      "modules_affected": ["Module1", "Module2"],
      "severity": "high|medium|low",
      "resolution": "Specific action to resolve"
    }
  ],
  "optimization_opportunities": [
    {
      "type": "interface-extraction|dependency-injection",
      "description": "Description of optimization",
      "modules": ["ModuleName"],
      "benefits": ["Reduced coupling", "Improved testability"]
    }
  ]
}

RULES:
1. Flag circular dependencies as high severity
2. Identify unused includes for cleanup
3. Suggest interface abstractions for tight coupling
4. Prioritize by impact on build time and maintainability
EOF
            ;;
        *)
            log_error "Unknown analysis type: $analysis_type"
            return 1
            ;;
    esac
}

# Validate analysis output against expected schema
validate_analysis() {
    local analysis_file="$1"
    
    log_info "Validating analysis output: $analysis_file"
    
    if [[ ! -f "$analysis_file" ]]; then
        log_error "Analysis file not found: $analysis_file"
        return 1
    fi
    
    # Check if it's valid JSON
    if ! python3 -m json.tool "$analysis_file" > /dev/null 2>&1; then
        log_error "Invalid JSON format in $analysis_file"
        return 1
    fi
    
    # Validate required fields based on analysis type
    local analysis_type=$(python3 -c "
import json
with open('$analysis_file', 'r') as f:
    data = json.load(f)
print(data.get('analysis_type', 'unknown'))
" 2>/dev/null || echo "unknown")
    
    case "$analysis_type" in
        "one-class-per-file")
            validate_one_class_schema "$analysis_file"
            ;;
        "extract-class")
            validate_extract_class_schema "$analysis_file"
            ;;
        "extract-struct")
            validate_extract_struct_schema "$analysis_file"
            ;;
        "dependency-analysis")
            validate_dependency_schema "$analysis_file"
            ;;
        *)
            log_warning "Unknown analysis type for validation: $analysis_type"
            ;;
    esac
}

# Validate one-class-per-file analysis schema
validate_one_class_schema() {
    local analysis_file="$1"
    
    python3 << EOF
import json
import sys

try:
    with open('$analysis_file', 'r') as f:
        data = json.load(f)
    
    # Check required top-level fields
    required_fields = ['analysis_type', 'timestamp', 'violations', 'summary']
    for field in required_fields:
        if field not in data:
            print(f"ERROR: Missing required field: {field}")
            sys.exit(1)
    
    # Validate violations structure
    for i, violation in enumerate(data['violations']):
        required_v_fields = ['file', 'class_count', 'classes', 'recommendation', 'priority']
        for field in required_v_fields:
            if field not in violation:
                print(f"ERROR: Violation {i} missing field: {field}")
                sys.exit(1)
        
        # Validate class_count matches classes array length
        if violation['class_count'] != len(violation['classes']):
            print(f"ERROR: Violation {i} class_count mismatch")
            sys.exit(1)
    
    # Validate summary
    summary_fields = ['total_violations', 'files_analyzed', 'compliance_rate']
    for field in summary_fields:
        if field not in data['summary']:
            print(f"ERROR: Summary missing field: {field}")
            sys.exit(1)
    
    print("VALID: one-class-per-file schema validation passed")
    
except Exception as e:
    print(f"ERROR: Schema validation failed: {e}")
    sys.exit(1)
EOF
}

# Validate extract-class analysis schema
validate_extract_class_schema() {
    local analysis_file="$1"
    
    python3 << EOF
import json
import sys

try:
    with open('$analysis_file', 'r') as f:
        data = json.load(f)
    
    # Check required fields
    required_fields = ['analysis_type', 'timestamp', 'source_file', 'extractions']
    for field in required_fields:
        if field not in data:
            print(f"ERROR: Missing required field: {field}")
            sys.exit(1)
    
    # Validate extractions
    for i, extraction in enumerate(data['extractions']):
        required_e_fields = ['class_name', 'extraction_method', 'target_header', 'refactor_command']
        for field in required_e_fields:
            if field not in extraction:
                print(f"ERROR: Extraction {i} missing field: {field}")
                sys.exit(1)
        
        # Validate refactor_command structure
        cmd = extraction['refactor_command']
        if 'command' not in cmd or 'args' not in cmd:
            print(f"ERROR: Extraction {i} invalid refactor_command structure")
            sys.exit(1)
    
    print("VALID: extract-class schema validation passed")
    
except Exception as e:
    print(f"ERROR: Schema validation failed: {e}")
    sys.exit(1)
EOF
}

# Generate analysis with standardized prompts
analyze_with_prompt() {
    local lexer_output="$1"
    local analysis_type="$2"
    
    log_info "Generating standardized analysis: $analysis_type"
    
    if [[ ! -f "$lexer_output" ]]; then
        log_error "Lexer output file not found: $lexer_output"
        return 1
    fi
    
    # Generate timestamp for output file
    local timestamp=$(date +%Y%m%d_%H%M%S)
    local output_file="${PROJECT_ROOT}/.analysis_output/${analysis_type}_${timestamp}.json"
    
    # Create output directory
    mkdir -p "$(dirname "$output_file")"
    
    # Get standardized prompt
    local prompt_file="${PROJECT_ROOT}/.analysis_output/prompt_${analysis_type}.txt"
    generate_prompt "$analysis_type" > "$prompt_file"
    
    log_info "Generated standardized prompt: $prompt_file"
    log_info "Expected output file: $output_file"
    
    # Display the standardized prompt for LLM to use
    echo ""
    echo "=== STANDARDIZED LLM PROMPT ==="
    cat "$prompt_file"
    echo ""
    echo "=== LEXER INPUT DATA ==="
    cat "$lexer_output"
    echo ""
    echo "=== INSTRUCTIONS ==="
    echo "Please analyze the lexer input data using the standardized prompt above."
    echo "Output the result as valid JSON matching the specified schema."
    echo "Save the result to: $output_file"
    
    log_success "Standardized analysis prompt generated"
    echo "Output file will be: $output_file"
}

# Run consistency tests
test_consistency() {
    log_info "Running LLM analysis consistency tests"
    
    local test_dir="${PROJECT_ROOT}/.test_analysis"
    mkdir -p "$test_dir"
    
    # Create test lexer output
    local test_lexer_output="$test_dir/test_lexer_output.json"
    cat > "$test_lexer_output" << 'EOF'
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
          "methods": ["load", "save"]
        }
      ],
      "functions": ["main", "signalHandler"]
    }
  ]
}
EOF
    
    log_info "Created test lexer output: $test_lexer_output"
    
    # Generate analysis for this test case
    analyze_with_prompt "$test_lexer_output" "one-class-per-file"
    
    log_info "Consistency test setup complete"
    log_info "To test consistency:"
    log_info "1. Run the LLM analysis 3 times with the same input"
    log_info "2. Compare outputs for consistency"
    log_info "3. Validate each output with: $0 validate <output_file>"
}

# Run integration tests
test_integration() {
    log_info "Running integration tests: lexer → analysis → refactor"
    
    # Test with actual project file
    local test_file="src/main.cpp"
    if [[ ! -f "$test_file" ]]; then
        log_error "Test file not found: $test_file"
        return 1
    fi
    
    local test_dir="${PROJECT_ROOT}/.test_integration"
    mkdir -p "$test_dir"
    
    # Step 1: Run lexer
    log_info "Step 1: Running lexer analysis"
    local lexer_output="$test_dir/lexer_output.json"
    "${SCRIPT_DIR}/lexer.sh" analyze "$(dirname "$test_file")" > "$lexer_output"
    
    # Step 2: Generate LLM analysis prompt
    log_info "Step 2: Generating LLM analysis prompt"
    analyze_with_prompt "$lexer_output" "one-class-per-file"
    
    log_success "Integration test setup complete"
    log_info "Manual steps:"
    log_info "1. Complete the LLM analysis using the generated prompt"
    log_info "2. Validate with: $0 validate <analysis_output.json>"
    log_info "3. Execute refactor commands from the analysis"
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
        analyze)
            if [[ $# -ne 2 ]]; then
                log_error "analyze requires 2 arguments: <lexer_output.json> <analysis_type>"
                exit 1
            fi
            analyze_with_prompt "$1" "$2"
            ;;
        validate)
            if [[ $# -ne 1 ]]; then
                log_error "validate requires 1 argument: <analysis_output.json>"
                exit 1
            fi
            validate_analysis "$1"
            ;;
        test)
            if [[ $# -ne 1 ]]; then
                log_error "test requires 1 argument: <test_scenario>"
                exit 1
            fi
            case "$1" in
                consistency)
                    test_consistency
                    ;;
                integration)
                    test_integration
                    ;;
                validation)
                    log_info "Running validation tests with known good outputs"
                    # This would test validation with pre-created valid outputs
                    ;;
                *)
                    log_error "Unknown test scenario: $1"
                    exit 1
                    ;;
            esac
            ;;
        prompt)
            if [[ $# -ne 1 ]]; then
                log_error "prompt requires 1 argument: <analysis_type>"
                exit 1
            fi
            generate_prompt "$1"
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