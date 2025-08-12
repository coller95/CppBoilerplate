#!/bin/bash

# Lexical analyzer for C++ codebase - generates structured JSON for LLM consumption
# Usage: ./scripts/lexer.sh [analyze|structure|dependencies] [path]

set -e

# Resolve script location and set project root
SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
PROJECT_ROOT="$(dirname "$SCRIPTDIR")"
cd "$PROJECT_ROOT"

# Default values
ACTION="${1:-analyze}"
TARGET_PATH="${2:-src}"

# Utility functions
print_usage() {
    echo "Usage: $0 [analyze|structure|dependencies] [path]"
    echo ""
    echo "Actions:"
    echo "  analyze       - Complete codebase analysis (default)"
    echo "  structure     - Project structure overview"
    echo "  dependencies  - Dependency mapping only"
    echo ""
    echo "Examples:"
    echo "  $0 analyze src/         # Analyze source code"
    echo "  $0 structure include/   # Structure overview of headers"
    echo "  $0 dependencies         # Project-wide dependencies"
    exit 1
}

# Validate action
case "$ACTION" in
    analyze|structure|dependencies) ;;
    *) print_usage ;;
esac

# Parse C++ file and extract key information
parse_cpp_file() {
    local file="$1"
    local file_type="$2"  # "header", "implementation", "main"
    
    # Determine namespace
    local namespace=$(awk '/^namespace [a-zA-Z_][a-zA-Z0-9_]*/ { gsub(/[{}]/, "", $2); print $2; exit }' "$file")
    
    # Extract includes - properly quoted for JSON
    local includes=""
    while IFS= read -r inc; do
        if [[ -n "$inc" ]]; then
            inc=$(echo "$inc" | sed 's/^#include[[:space:]]*//' | sed 's/"/\\"/g')
            if [[ -n "$includes" ]]; then
                includes="${includes},"
            fi
            includes="${includes}\"$inc\""
        fi
    done < <(grep -E '^#include' "$file" || true)
    
    # Extract classes (both declarations and definitions)
    local classes=""
    while IFS= read -r line; do
        if [[ "$line" =~ ^[[:space:]]*class[[:space:]]+([a-zA-Z_][a-zA-Z0-9_]*) ]]; then
            local class_name="${BASH_REMATCH[1]}"
            
            # Get inheritance info
            local inherits=""
            if [[ "$line" =~ :[[:space:]]*(.+)[[:space:]]*\{ ]]; then
                inherits="${BASH_REMATCH[1]}"
                inherits=$(echo "$inherits" | sed 's/public[[:space:]]*//g' | sed 's/private[[:space:]]*//g' | sed 's/protected[[:space:]]*//g' | sed 's/[[:space:]]*//g')
            fi
            
            # Extract methods for this class
            local methods=""
            local in_class=false
            local brace_count=0
            
            while IFS= read -r method_line; do
                if [[ "$method_line" =~ class[[:space:]]+$class_name ]]; then
                    in_class=true
                    brace_count=0
                fi
                
                if [[ "$in_class" == true ]]; then
                    # Count braces to track class scope
                    local open_braces=$(echo "$method_line" | tr -cd '{' | wc -c)
                    local close_braces=$(echo "$method_line" | tr -cd '}' | wc -c)
                    brace_count=$((brace_count + open_braces - close_braces))
                    
                    # Extract method declarations - simplified pattern
                    if echo "$method_line" | grep -E '^\s*(virtual\s+)?(static\s+)?.*\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(' >/dev/null; then
                        local method_name=$(echo "$method_line" | sed -E 's/^\s*(virtual\s+)?(static\s+)?.*\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(.*/\3/')
                        if [[ "$method_name" != "if" && "$method_name" != "while" && "$method_name" != "for" ]]; then
                            methods="${methods}${method_name},"
                        fi
                    fi
                    
                    # Exit class when braces balance back to 0
                    if [[ $brace_count -le 0 && "$method_line" =~ \} ]]; then
                        break
                    fi
                fi
            done < "$file"
            
            methods=$(echo "$methods" | sed 's/,$//')
            
            if [[ -n "$classes" ]]; then
                classes="${classes},"
            fi
            
            # Split methods into proper JSON array
            local methods_json="["
            if [[ -n "$methods" ]]; then
                local method_array=(${methods//,/ })
                for i in "${!method_array[@]}"; do
                    if [[ $i -gt 0 ]]; then
                        methods_json="${methods_json},"
                    fi
                    methods_json="${methods_json}\"${method_array[i]}\""
                done
            fi
            methods_json="${methods_json}]"
            
            classes="${classes}{\"name\":\"$class_name\",\"inherits\":\"$inherits\",\"methods\":$methods_json}"
        fi
    done < "$file"
    
    # Extract standalone functions (not in classes)
    local functions=""
    local in_class=false
    while IFS= read -r line; do
        # Track if we're inside a class
        if [[ "$line" =~ ^[[:space:]]*class[[:space:]] ]]; then
            in_class=true
        elif [[ "$line" =~ ^\} && "$in_class" == true ]]; then
            in_class=false
        fi
        
        # Extract functions outside classes - simplified pattern
        if [[ "$in_class" == false ]] && echo "$line" | grep -E '^[a-zA-Z_].*\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*\{' >/dev/null; then
            local func_name=$(echo "$line" | sed -E 's/^.*\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\([^)]*\)\s*\{.*/\1/')
            if [[ "$func_name" != "if" && "$func_name" != "while" && "$func_name" != "for" && "$func_name" != "namespace" ]]; then
                functions="${functions}${func_name},"
            fi
        fi
    done < "$file"
    functions=$(echo "$functions" | sed 's/,$//')
    
    # Convert functions to proper JSON array
    local functions_json="["
    if [[ -n "$functions" ]]; then
        local func_array=(${functions//,/ })
        for i in "${!func_array[@]}"; do
            if [[ $i -gt 0 ]]; then
                functions_json="${functions_json},"
            fi
            functions_json="${functions_json}\"${func_array[i]}\""
        done
    fi
    functions_json="${functions_json}]"
    
    # Generate JSON output
    cat <<EOF
{
  "file": "$file",
  "type": "$file_type",
  "namespace": "$namespace",
  "includes": [$includes],
  "classes": [$classes],
  "functions": $functions_json
}
EOF
}

# Analyze project structure
analyze_structure() {
    local path="$1"
    
    echo "{"
    echo "  \"project\": \"CppBoilerplate\","
    echo "  \"timestamp\": \"$(date -Iseconds)\","
    echo "  \"analysis_path\": \"$path\","
    
    # Find all C++ files
    local headers=($(find "$path" -name "*.h" 2>/dev/null || true))
    local sources=($(find "$path" -name "*.cpp" 2>/dev/null || true))
    
    echo "  \"summary\": {"
    echo "    \"headers\": ${#headers[@]},"
    echo "    \"sources\": ${#sources[@]},"
    echo "    \"total_files\": $((${#headers[@]} + ${#sources[@]}))"
    echo "  },"
    
    # Analyze modules (directories)
    echo "  \"modules\": ["
    local first_module=true
    for dir in $(find "$path" -mindepth 1 -maxdepth 2 -type d | sort); do
        local module_name=$(basename "$dir")
        local header_count=$(find "$dir" -maxdepth 1 -name "*.h" | wc -l)
        local source_count=$(find "$dir" -maxdepth 1 -name "*.cpp" | wc -l)
        
        if [[ $header_count -gt 0 || $source_count -gt 0 ]]; then
            if [[ "$first_module" != true ]]; then
                echo ","
            fi
            echo "    {"
            echo "      \"name\": \"$module_name\","
            echo "      \"path\": \"$dir\","
            echo "      \"headers\": $header_count,"
            echo "      \"sources\": $source_count"
            echo "    }"
            first_module=false
        fi
    done
    echo "  ],"
    
    echo "  \"files\": ["
    
    # Process headers first
    local first_file=true
    for file in "${headers[@]}"; do
        if [[ "$first_file" != true ]]; then
            echo ","
        fi
        parse_cpp_file "$file" "header" | sed 's/^/    /'
        first_file=false
    done
    
    # Process sources
    for file in "${sources[@]}"; do
        if [[ "$first_file" != true ]]; then
            echo ","
        fi
        
        local file_type="implementation"
        if [[ "$file" == *"main.cpp" ]]; then
            file_type="main"
        fi
        
        parse_cpp_file "$file" "$file_type" | sed 's/^/    /'
        first_file=false
    done
    
    echo "  ]"
    echo "}"
}

# Extract project dependencies
analyze_dependencies() {
    echo "{"
    echo "  \"project\": \"CppBoilerplate\","
    echo "  \"dependency_analysis\": {"
    echo "    \"timestamp\": \"$(date -Iseconds)\","
    
    # System includes
    echo "    \"system_includes\": ["
    find include/ src/ -name "*.h" -o -name "*.cpp" 2>/dev/null | xargs grep -h '^#include <' | sort -u | sed 's/#include </\"/' | sed 's/>/\"/' | tr '\n' ',' | sed 's/,$//'
    echo "    ],"
    
    # Project includes
    echo "    \"project_includes\": ["
    find include/ src/ -name "*.h" -o -name "*.cpp" 2>/dev/null | xargs grep -h '^#include "' | sort -u | sed 's/#include "/\"/' | sed 's/"/\"/' | tr '\n' ',' | sed 's/,$//'
    echo "    ],"
    
    # Module dependencies
    echo "    \"module_dependencies\": ["
    local first_dep=true
    for dir in $(find src/ include/ -mindepth 1 -maxdepth 1 -type d | sort); do
        local module=$(basename "$dir")
        local deps=($(find "$dir" -name "*.h" -o -name "*.cpp" | xargs grep -h '^#include "' | sed 's|#include "[^/]*/\([^/]*\)/.*|\1|' | sort -u | grep -v "$module" || true))
        
        if [[ ${#deps[@]} -gt 0 ]]; then
            if [[ "$first_dep" != true ]]; then
                echo ","
            fi
            echo "      {"
            echo "        \"module\": \"$module\","
            echo "        \"depends_on\": [$(printf '"%s",' "${deps[@]}" | sed 's/,$//')]"
            echo "      }"
            first_dep=false
        fi
    done
    echo "    ]"
    echo "  }"
    echo "}"
}

# Main execution logic
case "$ACTION" in
    analyze)
        analyze_structure "$TARGET_PATH"
        ;;
    structure)
        analyze_structure "$TARGET_PATH" | jq '.summary, .modules'
        ;;
    dependencies)
        analyze_dependencies
        ;;
esac