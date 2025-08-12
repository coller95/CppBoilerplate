#!/bin/bash

# Token efficiency comparison - raw C++ vs lexical analysis
# Demonstrates 90%+ token reduction for LLM context optimization

set -e

SCRIPTDIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P)"
PROJECT_ROOT="$(dirname "$SCRIPTDIR")"
cd "$PROJECT_ROOT"

print_usage() {
    echo "Usage: $0 [demo|compare] [file_or_module]"
    echo ""
    echo "Actions:"
    echo "  demo     - Show token efficiency demo (default)"
    echo "  compare  - Compare specific file or module"
    echo ""
    echo "Examples:"
    echo "  $0 demo                      # Full project comparison"
    echo "  $0 compare src/main.cpp      # Compare specific file" 
    echo "  $0 compare include/Logger/   # Compare module"
    exit 1
}

# Rough token estimation (approximation: 1 token ≈ 4 characters)
estimate_tokens() {
    local text="$1"
    local char_count=$(echo "$text" | wc -c)
    echo $((char_count / 4))
}

# Count actual file content
count_file_content() {
    local file="$1"
    if [[ -f "$file" ]]; then
        cat "$file" | wc -c
    else
        echo 0
    fi
}

# Generate lexical summary for comparison
generate_summary() {
    local target="$1"
    ./scripts/lexer.sh analyze "$target" 2>/dev/null
}

# Demo mode - show project-wide efficiency
demo_efficiency() {
    echo "=== TOKEN EFFICIENCY DEMONSTRATION ==="
    echo ""
    echo "Comparing raw C++ files vs lexical analysis for LLM context optimization"
    echo ""
    
    # Analyze different components
    local components=("include/Logger" "include/WebServer" "src/main.cpp" "include/ApiRouter")
    
    local total_raw_chars=0
    local total_lexical_chars=0
    
    for component in "${components[@]}"; do
        echo "--- Component: $component ---"
        
        if [[ -f "$component" ]]; then
            # Single file
            local raw_chars=$(count_file_content "$component")
            local lexical_output=$(generate_summary "$component")
            local lexical_chars=$(echo "$lexical_output" | wc -c)
            
            echo "Raw C++ file: $raw_chars characters (~$(estimate_tokens "$(<"$component")") tokens)"
            echo "Lexical analysis: $lexical_chars characters (~$(estimate_tokens "$lexical_output") tokens)"
            
        elif [[ -d "$component" ]]; then
            # Directory
            local raw_chars=0
            for file in $(find "$component" -name "*.h" -o -name "*.cpp"); do
                raw_chars=$((raw_chars + $(count_file_content "$file")))
            done
            
            local lexical_output=$(generate_summary "$component")
            local lexical_chars=$(echo "$lexical_output" | wc -c)
            
            echo "Raw C++ files: $raw_chars characters (~$(((raw_chars / 4))) tokens)"
            echo "Lexical analysis: $lexical_chars characters (~$(estimate_tokens "$lexical_output") tokens)"
        fi
        
        if [[ $raw_chars -gt 0 ]]; then
            local reduction=$((100 - (lexical_chars * 100 / raw_chars)))
            echo "Token reduction: ${reduction}%"
        fi
        
        total_raw_chars=$((total_raw_chars + raw_chars))
        total_lexical_chars=$((total_lexical_chars + lexical_chars))
        echo ""
    done
    
    echo "=== SUMMARY ==="
    echo "Total raw content: $total_raw_chars characters (~$((total_raw_chars / 4)) tokens)"
    echo "Total lexical analysis: $total_lexical_chars characters (~$((total_lexical_chars / 4)) tokens)" 
    local total_reduction=$((100 - (total_lexical_chars * 100 / total_raw_chars)))
    echo "Overall token reduction: ${total_reduction}%"
    echo ""
    echo "🚀 Lexical analysis provides ${total_reduction}% token reduction!"
    echo "🎯 LLM can process $((100 * total_raw_chars / total_lexical_chars))x more code in same context window"
    echo "⚡ Faster analysis, surgical precision, consistent transformations"
}

# Compare specific file or module  
compare_target() {
    local target="$1"
    
    if [[ ! -e "$target" ]]; then
        echo "Error: Target '$target' does not exist"
        exit 1
    fi
    
    echo "=== COMPARISON: $target ==="
    echo ""
    
    if [[ -f "$target" ]]; then
        echo "--- RAW C++ FILE ---"
        local raw_content=$(<"$target")
        echo "$raw_content"
        echo ""
        local raw_chars=$(echo "$raw_content" | wc -c)
        echo "Raw size: $raw_chars characters (~$(estimate_tokens "$raw_content") tokens)"
        
        echo ""
        echo "--- LEXICAL ANALYSIS ---"
        local lexical_output=$(generate_summary "$target")
        echo "$lexical_output"
        echo ""
        local lexical_chars=$(echo "$lexical_output" | wc -c)
        echo "Lexical size: $lexical_chars characters (~$(estimate_tokens "$lexical_output") tokens)"
        
    elif [[ -d "$target" ]]; then
        echo "--- RAW C++ FILES ---"
        local file_count=0
        local raw_chars=0
        for file in $(find "$target" -name "*.h" -o -name "*.cpp"); do
            echo "File: $file"
            file_count=$((file_count + 1))
            raw_chars=$((raw_chars + $(count_file_content "$file")))
        done
        echo ""
        echo "Raw files: $file_count files, $raw_chars characters (~$((raw_chars / 4)) tokens)"
        
        echo ""
        echo "--- LEXICAL ANALYSIS ---"
        local lexical_output=$(generate_summary "$target")
        echo "$lexical_output"
        echo ""
        local lexical_chars=$(echo "$lexical_output" | wc -c)
        echo "Lexical size: $lexical_chars characters (~$(estimate_tokens "$lexical_output") tokens)"
    fi
    
    if [[ $raw_chars -gt 0 && $lexical_chars -gt 0 ]]; then
        local reduction=$((100 - (lexical_chars * 100 / raw_chars)))
        echo ""
        echo "=== EFFICIENCY GAIN ==="
        echo "Token reduction: ${reduction}%"
        echo "Context multiplier: $((raw_chars / lexical_chars))x"
    fi
}

# Main execution
ACTION="${1:-demo}"
TARGET="${2:-}"

case "$ACTION" in
    demo)
        demo_efficiency
        ;;
    compare)
        if [[ -z "$TARGET" ]]; then
            echo "Error: compare action requires a target file or directory"
            print_usage
        fi
        compare_target "$TARGET"
        ;;
    *)
        print_usage
        ;;
esac