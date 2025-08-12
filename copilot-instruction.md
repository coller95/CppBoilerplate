# Copilot Instructions for CppBoilerplate Project

## **CRITICAL: LLM Context Optimization**

This project implements **dual optimization strategies** for maximum LLM efficiency:

### **1. Multi-Tiered Output System**
Optimized output modes with 90-95% token reduction in minimal mode.

### **2. One Class Per File Architecture**
**MANDATORY**: Each class must be in its own separate file for optimal LLM context usage:

**Benefits for LLM Efficiency:**
- **Precise targeting**: Read/edit individual classes without irrelevant context
- **Minimal file size**: Smaller files = fewer tokens per operation
- **Clear boundaries**: File scope matches logical class scope
- **Focused analysis**: Analyze single classes without distraction
- **Efficient modifications**: Edit specific classes without loading unrelated code

**File Organization Rules:**
```cpp
// ✅ CORRECT - One class per file
src/Logger/Logger.h          // Logger class interface
src/Logger/Logger.cpp        // Logger class implementation
src/WebServer/WebServer.h    // WebServer class interface  
src/WebServer/WebServer.cpp  // WebServer class implementation

// ❌ INCORRECT - Multiple classes in one file
src/Utilities.h             // Contains Logger, WebServer, ApiRouter classes
src/BigFile.cpp              // Multiple implementations in one file
```

### **Default Mode Usage**

**ALWAYS use minimal mode by default:**
```bash
# Optimal LLM performance (90-95% token reduction)
make debug                    # Uses minimal mode automatically
./scripts/test.sh            # Uses minimal mode automatically
make test                    # Uses minimal mode automatically
```

### **Multi-Tiered Output Modes**

**Available modes (ordered by context efficiency):**

1. **`minimal`** (DEFAULT - BEST PERFORMANCE)
   - 90-95% token reduction vs human mode
   - 5-10x faster LLM processing
   - Use for: All routine operations, builds, tests
   - Output: `[BUILD] hello_world`

2. **`standard`** (MODERATE CONTEXT)
   - 80% token reduction vs human mode  
   - 5x faster processing
   - Use for: When moderate debugging context needed
   - Output: `[BUILD] BUILT debug hello_world x86_64-native`

3. **`debug`** (TROUBLESHOOTING ONLY)
   - 60% token reduction vs human mode
   - 2.5x faster processing
   - Use for: Complex debugging scenarios only
   - Output: `[BUILD] COMPILE src/main.cpp` + `[BUILD] LINK debug hello_world`

4. **`human`** (AVOID IN LLM CONTEXTS)
   - 100% token usage (baseline)
   - Use for: Human developers only, never for LLM interactions
   - Output: `🔨 Building debug target...` + `✅ Successfully built...`

5. **`silent`** (MAXIMUM EFFICIENCY)
   - 95%+ token reduction
   - Use for: When only errors matter
   - Output: (nothing unless errors occur)

### **Command Examples**

```bash
# ✅ OPTIMAL - Use these by default
make debug                         # Minimal mode (best performance)
make test                          # Minimal mode
./scripts/test.sh                  # Minimal mode

# ✅ ACCEPTABLE - When more context needed
make debug VERBOSE=standard        # Moderate context
./scripts/test.sh debug           # Troubleshooting

# ⚠️ EMERGENCY ONLY - Heavy context usage
make debug VERBOSE=human           # For human developers only

# 🚫 AVOID - Never use in LLM contexts
./scripts/test.sh human           # Wastes 10-20x more tokens
```

### **Context Budget Strategy**

**Token Usage Comparison:**
- **Minimal**: ~50-100 tokens per build/test cycle
- **Standard**: ~200-300 tokens per build/test cycle  
- **Debug**: ~500-800 tokens per build/test cycle
- **Human**: ~1000-2000 tokens per build/test cycle

**Best Practices:**
- **File-level operations**: Read/edit individual class files for maximum efficiency
- Start with `minimal` mode for all operations
- Escalate to `standard` only when debugging
- Use `debug` mode only for complex troubleshooting
- Never use `human` mode in LLM interactions
- **Target specific classes**: Use `Read` tool on individual class files, not large multi-class files

### **Architecture Overview**

This project follows **Test-Driven Development (TDD)** with modular architecture:

**Core Components:**
- **IoC Container**: Dependency injection with service registration
- **WebServer**: Self-contained backends (no external polling required)
- **ApiRouter**: HTTP endpoint management with auto-registration
- **Logger**: Structured logging with remote/local display

**Key Principles:**
- **One Class Per File**: MANDATORY - Each class must be in its own separate file for LLM efficiency
- **Interface Segregation**: Clean interfaces without leaky abstractions
- **Self-Contained Components**: Internal threading, no external dependencies
- **TDD First**: RED-GREEN-REFACTOR cycle mandatory for all changes
- **Context Optimization**: Multi-tiered output for LLM efficiency
- **File-Level Modularity**: File boundaries match logical class boundaries

### **Build Commands (Optimized)**

```bash
# TDD Workflow (minimal context)
make test-run-ModuleName          # Run specific module tests
make debug                        # Build debug version
make release                      # Build release version

# Test Management
./scripts/test.sh                 # Complete test + build + run cycle
make test                         # Run all tests
make clean                        # Clean build artifacts

# Module Generation
./scripts/create_endpoint.sh create EndpointName
./scripts/create_service.sh create ServiceName
./scripts/create_module.sh create ModuleName
```

### **Error Handling**

**In minimal/standard mode, errors are clearly indicated:**
```bash
[ERROR] Test failed: ModuleTest 3/5      # Clear failure indication
[BUILD] FAILED: Compilation error        # Build failure
[SCRIPT] ERROR: Main program failed      # Runtime error
```

**Always use minimal mode unless specific debugging context is required.**

### **LLM Workflow Optimization**

**Efficient Class-Level Operations:**
```bash
# ✅ OPTIMAL - Target specific class files
Read src/Logger/Logger.h                    # Read only Logger interface (50-100 tokens)
Edit src/Logger/Logger.cpp                  # Modify only Logger implementation
Read src/WebServer/WebServer.h              # Read only WebServer interface

# ❌ INEFFICIENT - Large multi-class files
Read src/AllClasses.h                       # Reads multiple classes (500-1000 tokens)
Read src/main.cpp                           # Contains multiple concerns
```

**Context-Efficient Analysis:**
- **Single class focus**: Analyze one class at a time for precise understanding
- **Targeted modifications**: Edit specific class files without loading irrelevant code
- **Clear scope**: File boundaries provide natural analysis boundaries
- **Reduced context pollution**: No unrelated classes in the same file

### **Integration with Development Tools**

- **GitHub Copilot**: Use minimal output + single-class files for optimal code generation
- **Claude Code**: Default minimal mode + file-level targeting optimizes token usage
- **CI/CD**: Standard mode provides adequate logging for automation
- **Human Review**: Human mode available for code review sessions

### **Backward Compatibility**

Legacy commands still work:
```bash
VERBOSE=0 make debug              # Maps to minimal mode
VERBOSE=1 make debug              # Maps to human mode  
./scripts/test.sh agent           # Maps to standard mode
```

## **Remember: Dual LLM Optimization Strategy! 🚀**

### **Always Apply Both Optimizations:**
1. **Minimal Mode by Default**: 90-95% token reduction in build output
2. **One Class Per File**: Precise targeting of individual classes for context efficiency

### **Maximum LLM Efficiency Achieved Through:**
- **Output optimization**: Minimal mode for build/test operations
- **File organization**: Single-class files for focused analysis/modification
- **Combined effect**: 10-20x overall context efficiency improvement