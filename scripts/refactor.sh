#!/bin/bash

# refactor.sh - LLM-driven code refactoring execution tool
# This script executes refactoring operations based on LLM analysis and commands
#
# Usage:
#   ./scripts/refactor.sh extract-class <source_file> <class_name> <target_dir>
#   ./scripts/refactor.sh extract-struct <source_file> <struct_name> <target_dir>
#   ./scripts/refactor.sh extract-function <source_file> <function_name> <target_file>
#   ./scripts/refactor.sh split-file <source_file> <split_config_json>
#   ./scripts/refactor.sh move-lines <source_file> <start_line> <end_line> <target_file>
#   ./scripts/refactor.sh execute-json <refactor_commands.json>

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
    echo -e "${BLUE}[REFACTOR]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[REFACTOR]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[REFACTOR]${NC} $1"
}

log_error() {
    echo -e "${RED}[REFACTOR]${NC} $1" >&2
}

# Usage information
show_usage() {
    cat << EOF
Usage: $0 <command> [arguments...]

Commands:
  extract-class <source_file> <class_name> <target_dir>
    Extract a class from source file to target directory
    Creates: target_dir/ClassName.h and target_dir/ClassName.cpp

  extract-struct <source_file> <struct_name> <target_dir>  
    Extract a struct from source file to target directory
    Creates: target_dir/StructName.h and target_dir/StructName.cpp

  extract-function <source_file> <function_name> <target_file>
    Extract a function from source file to target file
    
  split-file <source_file> <split_config_json>
    Split file based on JSON configuration
    
  move-lines <source_file> <start_line> <end_line> <target_file>
    Move specific lines from source to target file
    
  copy-lines <source_file> <start_line> <end_line> <target_file>
    Copy specific lines from source to target file (keeps original)
    
  extract-block <source_file> <block_type> <block_name> <target_file>
    Extract code block (class/struct/function) using pattern matching
    
  analyze <source_file>
    Analyze file for refactoring opportunities using lexical analysis
    
  execute-json <refactor_commands.json>
    Execute multiple refactoring operations from JSON file

Examples:
  $0 extract-class src/main.cpp Application src/Application/
  $0 extract-struct src/main.cpp AppConfig src/AppConfig/
  $0 move-lines src/main.cpp 31 38 src/AppConfig/AppConfig.h
  $0 copy-lines src/main.cpp 100 150 backup/main_section.cpp
  $0 extract-block src/main.cpp struct AppConfig src/AppConfig/AppConfig.h
  $0 analyze src/main.cpp
  $0 execute-json refactor_plan.json
EOF
}

# Create backup of file
create_backup() {
    local file="$1"
    local backup_dir="${PROJECT_ROOT}/.refactor_backups/$(date +%Y%m%d_%H%M%S)"
    
    if [[ -f "$file" ]]; then
        mkdir -p "$backup_dir"
        cp "$file" "$backup_dir/$(basename "$file")"
        log_info "Backed up $file to $backup_dir/"
    fi
}

# Extract class from file based on line ranges
extract_class() {
    local source_file="$1"
    local class_name="$2"
    local target_dir="$3"
    
    log_info "Extracting class $class_name from $source_file"
    
    # Validate inputs
    if [[ ! -f "$source_file" ]]; then
        log_error "Source file $source_file does not exist"
        return 1
    fi
    
    # Create target directory
    mkdir -p "$target_dir"
    
    # Create backup
    create_backup "$source_file"
    
    # Define target files
    local header_file="${target_dir}/${class_name}.h"
    local cpp_file="${target_dir}/${class_name}.cpp"
    
    log_info "Creating $header_file and $cpp_file"
    
    # Extract class definition (this is a simplified version)
    # In practice, this would use the lexical analysis data to find exact line ranges
    
    # For now, create template files that LLM can populate
    cat > "$header_file" << EOF
#pragma once

// TODO: Add necessary includes from original file

/**
 * $class_name class extracted from $source_file
 */
class $class_name
{
    // TODO: Move class definition here
    // LLM should populate this based on lexical analysis
};
EOF

    cat > "$cpp_file" << EOF
#include "${class_name}.h"

// TODO: Add necessary includes from original file

// TODO: Move class implementation here
// LLM should populate this based on lexical analysis
EOF
    
    log_success "Created template files for $class_name"
    log_warning "Files created as templates - LLM should populate with actual extracted code"
}

# Extract struct from file
extract_struct() {
    local source_file="$1"
    local struct_name="$2"
    local target_dir="$3"
    
    log_info "Extracting struct $struct_name from $source_file"
    
    # Validate inputs
    if [[ ! -f "$source_file" ]]; then
        log_error "Source file $source_file does not exist"
        return 1
    fi
    
    # Create target directory
    mkdir -p "$target_dir"
    
    # Create backup
    create_backup "$source_file"
    
    # Define target files
    local header_file="${target_dir}/${struct_name}.h"
    local cpp_file="${target_dir}/${struct_name}.cpp"
    
    log_info "Creating $header_file and $cpp_file"
    
    # Create template files
    cat > "$header_file" << EOF
#pragma once

// TODO: Add necessary includes from original file

/**
 * $struct_name structure extracted from $source_file
 */
struct $struct_name
{
    // TODO: Move struct definition here
    // LLM should populate this based on lexical analysis
};
EOF

    cat > "$cpp_file" << EOF
#include "${struct_name}.h"

// TODO: Add necessary includes from original file
// TODO: Add any struct-related functions or methods here
EOF
    
    log_success "Created template files for $struct_name"
}

# Move specific lines from one file to another
move_lines() {
    local source_file="$1"
    local start_line="$2"
    local end_line="$3"
    local target_file="$4"
    
    log_info "Moving lines $start_line-$end_line from $source_file to $target_file"
    
    # Validate inputs
    if [[ ! -f "$source_file" ]]; then
        log_error "Source file $source_file does not exist"
        return 1
    fi
    
    if ! [[ "$start_line" =~ ^[0-9]+$ ]] || ! [[ "$end_line" =~ ^[0-9]+$ ]]; then
        log_error "Line numbers must be integers"
        return 1
    fi
    
    if [[ $start_line -gt $end_line ]]; then
        log_error "Start line must be less than or equal to end line"
        return 1
    fi
    
    # Create backups
    create_backup "$source_file"
    if [[ -f "$target_file" ]]; then
        create_backup "$target_file"
    fi
    
    # Extract the lines
    local extracted_lines=$(sed -n "${start_line},${end_line}p" "$source_file")
    
    # Append to target file (create directory if needed)
    mkdir -p "$(dirname "$target_file")"
    echo "$extracted_lines" >> "$target_file"
    
    # Remove from source file
    sed -i "${start_line},${end_line}d" "$source_file"
    
    log_success "Moved lines $start_line-$end_line from $source_file to $target_file"
}

# Copy specific lines from one file to another (without removing from source)
copy_lines() {
    local source_file="$1"
    local start_line="$2"
    local end_line="$3"
    local target_file="$4"
    
    log_info "Copying lines $start_line-$end_line from $source_file to $target_file"
    
    # Validate inputs
    if [[ ! -f "$source_file" ]]; then
        log_error "Source file $source_file does not exist"
        return 1
    fi
    
    if ! [[ "$start_line" =~ ^[0-9]+$ ]] || ! [[ "$end_line" =~ ^[0-9]+$ ]]; then
        log_error "Line numbers must be integers"
        return 1
    fi
    
    if [[ $start_line -gt $end_line ]]; then
        log_error "Start line must be less than or equal to end line"
        return 1
    fi
    
    # Create backup of target if it exists
    if [[ -f "$target_file" ]]; then
        create_backup "$target_file"
    fi
    
    # Extract the lines
    local extracted_lines=$(sed -n "${start_line},${end_line}p" "$source_file")
    
    # Append to target file (create directory if needed)
    mkdir -p "$(dirname "$target_file")"
    echo "$extracted_lines" >> "$target_file"
    
    log_success "Copied lines $start_line-$end_line from $source_file to $target_file"
}

# Find the start of comments preceding a code block
find_comment_start() {
    local source_file="$1"
    local code_start_line="$2"
    
    local comment_start_line="$code_start_line"
    local current_line=$((code_start_line - 1))
    
    # Look backwards for comments, blank lines, or documentation
    while [[ $current_line -gt 0 ]]; do
        local line_content=$(sed -n "${current_line}p" "$source_file")
        
        # Check if line is a comment, blank, or documentation
        if [[ "$line_content" =~ ^[[:space:]]*(/\*|\*|//|\*/) ]] || [[ -z "$line_content" ]] || [[ "$line_content" =~ ^[[:space:]]*$ ]]; then
            comment_start_line=$current_line
            current_line=$((current_line - 1))
        else
            # Found non-comment, non-blank line - stop searching
            break
        fi
    done
    
    echo "$comment_start_line"
}

# Extract code block based on pattern matching (for classes, structs, functions)
extract_code_block() {
    local source_file="$1"
    local block_type="$2"  # "class", "struct", "function"
    local block_name="$3"
    local target_file="$4"
    
    log_info "Extracting $block_type '$block_name' from $source_file to $target_file"
    
    # Validate inputs
    if [[ ! -f "$source_file" ]]; then
        log_error "Source file $source_file does not exist"
        return 1
    fi
    
    # Create backups
    create_backup "$source_file"
    if [[ -f "$target_file" ]]; then
        create_backup "$target_file"
    fi
    
    # Find the block using pattern matching
    local code_start_line end_line
    
    case "$block_type" in
        "struct")
            # Find struct definition
            code_start_line=$(grep -n "^struct $block_name" "$source_file" | cut -d: -f1)
            if [[ -z "$code_start_line" ]]; then
                log_error "Struct '$block_name' not found in $source_file"
                return 1
            fi
            # Find the closing brace (simplified - assumes proper formatting)
            end_line=$(awk -v start="$code_start_line" 'NR >= start && /^};/ {print NR; exit}' "$source_file")
            ;;
        "class")
            # Find class definition
            code_start_line=$(grep -n "^class $block_name" "$source_file" | cut -d: -f1)
            if [[ -z "$code_start_line" ]]; then
                log_error "Class '$block_name' not found in $source_file"
                return 1
            fi
            # Find the closing brace (simplified)
            end_line=$(awk -v start="$code_start_line" 'NR >= start && /^};/ {print NR; exit}' "$source_file")
            ;;
        "function")
            # Find function definition (more flexible pattern)
            code_start_line=$(grep -n -E "^[a-zA-Z_].*$block_name\s*\(" "$source_file" | cut -d: -f1)
            if [[ -z "$code_start_line" ]]; then
                log_error "Function '$block_name' not found in $source_file"
                return 1
            fi
            # Find the closing brace (simplified)
            end_line=$(awk -v start="$code_start_line" 'NR >= start && /^}/ {print NR; exit}' "$source_file")
            ;;
        *)
            log_error "Unknown block type: $block_type"
            return 1
            ;;
    esac
    
    if [[ -z "$end_line" ]]; then
        log_error "Could not find end of $block_type '$block_name'"
        return 1
    fi
    
    # Find the start of any preceding comments
    local start_line=$(find_comment_start "$source_file" "$code_start_line")
    
    log_info "Found $block_type '$block_name' at lines $start_line-$end_line (including comments)"
    
    # Extract the lines including comments
    local extracted_lines=$(sed -n "${start_line},${end_line}p" "$source_file")
    
    # Create target directory if needed
    mkdir -p "$(dirname "$target_file")"
    
    # Write to target file
    echo "$extracted_lines" > "$target_file"
    
    # Remove from source file
    sed -i "${start_line},${end_line}d" "$source_file"
    
    log_success "Extracted $block_type '$block_name' with comments to $target_file"
}

# Execute refactoring operations from JSON file
execute_json() {
    local json_file="$1"
    
    log_info "Executing refactoring operations from $json_file"
    
    if [[ ! -f "$json_file" ]]; then
        log_error "JSON file $json_file does not exist"
        return 1
    fi
    
    # Validate JSON format
    if ! python3 -m json.tool "$json_file" > /dev/null 2>&1; then
        log_error "Invalid JSON format in $json_file"
        return 1
    fi
    
    log_info "JSON file is valid, executing operations..."
    
    # Parse and execute operations
    # This would parse the JSON and call appropriate functions
    # For now, show what operations would be executed
    
    log_info "Operations to execute:"
    python3 -c "
import json
import sys

with open('$json_file', 'r') as f:
    data = json.load(f)

if 'operations' in data:
    for i, op in enumerate(data['operations'], 1):
        print(f'  {i}. {op.get(\"type\", \"unknown\")}: {op.get(\"description\", \"no description\")}')
else:
    print('  No operations found in JSON file')
"
    
    log_warning "JSON execution not fully implemented - this would execute the parsed operations"
}

# Update build system files after refactoring
update_build_system() {
    log_info "Updating build system configuration..."
    
    # Scan for new source files in src/
    local new_sources=$(find "${PROJECT_ROOT}/src" -name "*.cpp" -newer "${PROJECT_ROOT}/Project.build" 2>/dev/null || true)
    
    if [[ -n "$new_sources" ]]; then
        log_info "Found new source files:"
        echo "$new_sources" | while read -r file; do
            log_info "  - $file"
        done
        
        # Update Project.build with comment about new sources
        if ! grep -q "# Auto-added sources from refactoring" "${PROJECT_ROOT}/Project.build"; then
            echo "" >> "${PROJECT_ROOT}/Project.build"
            echo "# Auto-added sources from refactoring" >> "${PROJECT_ROOT}/Project.build"
            echo "# ADDITIONAL_SOURCES += \$(wildcard src/NewModule/*.cpp)" >> "${PROJECT_ROOT}/Project.build"
        fi
        
        log_success "Project.build updated with refactoring comments"
    else
        log_info "No new source files detected"
    fi
    
    # Check if we need to update test configurations
    local new_test_dirs=$(find "${PROJECT_ROOT}/tests" -name "*Test" -type d -newer "${PROJECT_ROOT}/Tests.build" 2>/dev/null || true)
    
    if [[ -n "$new_test_dirs" ]]; then
        log_info "Found new test directories:"
        echo "$new_test_dirs" | while read -r dir; do
            log_info "  - $dir"
        done
        log_warning "New test modules may need manual configuration in Tests.build"
    fi
}

# Auto-detect refactoring requirements from lexical analysis
analyze_refactoring_needs() {
    local source_file="$1"
    
    log_info "Analyzing refactoring needs for $source_file"
    
    # Run lexical analysis
    local lexer_output=$(${SCRIPT_DIR}/lexer.sh analyze "$source_file" 2>/dev/null || echo "{}")
    
    # Check for violations of "One Class Per File"
    local class_count=$(echo "$lexer_output" | python3 -c "
import json
import sys
try:
    data = json.load(sys.stdin)
    if 'files' in data and len(data['files']) > 0:
        classes = data['files'][0].get('classes', [])
        print(len(classes))
    else:
        print(0)
except:
    print(0)
" 2>/dev/null || echo "0")
    
    if [[ "$class_count" -gt 1 ]]; then
        log_warning "File $source_file contains $class_count classes (violates One Class Per File)"
        log_info "Consider using: $0 extract-class $source_file <ClassName> <target_dir>"
    elif [[ "$class_count" -eq 1 ]]; then
        log_success "File $source_file follows One Class Per File principle"
    else
        log_info "File $source_file contains no classes"
    fi
    
    # Check file size
    local line_count=$(wc -l < "$source_file")
    if [[ "$line_count" -gt 300 ]]; then
        log_warning "File $source_file is large ($line_count lines) - consider splitting"
        log_info "Use lexical analysis to identify split points"
    fi
}

# Verify that refactored code still compiles
verify_build() {
    log_info "Verifying that refactored code compiles..."
    
    cd "$PROJECT_ROOT"
    
    if make clean && make debug VERBOSE=minimal; then
        log_success "Build verification passed"
        return 0
    else
        log_error "Build verification failed"
        return 1
    fi
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
        extract-class)
            if [[ $# -ne 3 ]]; then
                log_error "extract-class requires 3 arguments: <source_file> <class_name> <target_dir>"
                exit 1
            fi
            extract_class "$1" "$2" "$3"
            ;;
        extract-struct)
            if [[ $# -ne 3 ]]; then
                log_error "extract-struct requires 3 arguments: <source_file> <struct_name> <target_dir>"
                exit 1
            fi
            extract_struct "$1" "$2" "$3"
            ;;
        extract-function)
            if [[ $# -ne 3 ]]; then
                log_error "extract-function requires 3 arguments: <source_file> <function_name> <target_file>"
                exit 1
            fi
            log_warning "extract-function not yet implemented"
            ;;
        split-file)
            if [[ $# -ne 2 ]]; then
                log_error "split-file requires 2 arguments: <source_file> <split_config_json>"
                exit 1
            fi
            log_warning "split-file not yet implemented"
            ;;
        move-lines)
            if [[ $# -ne 4 ]]; then
                log_error "move-lines requires 4 arguments: <source_file> <start_line> <end_line> <target_file>"
                exit 1
            fi
            move_lines "$1" "$2" "$3" "$4"
            ;;
        copy-lines)
            if [[ $# -ne 4 ]]; then
                log_error "copy-lines requires 4 arguments: <source_file> <start_line> <end_line> <target_file>"
                exit 1
            fi
            copy_lines "$1" "$2" "$3" "$4"
            ;;
        extract-block)
            if [[ $# -ne 4 ]]; then
                log_error "extract-block requires 4 arguments: <source_file> <block_type> <block_name> <target_file>"
                exit 1
            fi
            extract_code_block "$1" "$2" "$3" "$4"
            ;;
        analyze)
            if [[ $# -ne 1 ]]; then
                log_error "analyze requires 1 argument: <source_file>"
                exit 1
            fi
            analyze_refactoring_needs "$1"
            ;;
        execute-json)
            if [[ $# -ne 1 ]]; then
                log_error "execute-json requires 1 argument: <refactor_commands.json>"
                exit 1
            fi
            execute_json "$1"
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
    
    log_success "Refactoring operation completed"
}

# Execute main function with all arguments
main "$@"