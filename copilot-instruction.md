# Copilot Instructions for CppBoilerplate Project

## **CRITICAL: LLM Context Optimization**

This project implements **triple optimization strategies** for maximum LLM efficiency:

### **1. Multi-Tiered Output System**
Optimized output modes with 90-95% token reduction in minimal mode.

### **2. Lexical Analysis for Codebase Understanding**
**NEW**: Advanced C++ lexical analysis with 85-95% token reduction for codebase analysis.

### **3. One Class Per File Architecture**
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

### **Lexical Analysis Commands (PREFERRED for Large Codebase Analysis)**

**ALWAYS use lexical analysis for codebase understanding:**
```bash
# 🚀 MAXIMUM EFFICIENCY - Use these for codebase analysis
./scripts/lexer.sh analyze src/               # Complete codebase analysis (85-95% token reduction)
./scripts/lexer.sh structure include/         # Module structure overview  
./scripts/lexer.sh dependencies               # Project dependency mapping

# Token efficiency demonstration
./scripts/token_comparison.sh demo            # Project-wide efficiency statistics
./scripts/token_comparison.sh compare file    # Compare specific file/module
```

**Why Lexical Analysis is Superior:**
- **85-95% token reduction** vs reading raw C++ files
- **Structured JSON output** enables surgical precision targeting
- **Consistent parsing** eliminates LLM syntax errors
- **Context multiplier**: Analyze entire codebase in minimal token budget
- **Ready for automation**: Prepared for refactoring command workflows

### **NEW: Automated Refactoring Tools (CRITICAL for "One Class Per File")**

**MANDATORY**: Use refactoring tools to enforce "One Class Per File" architecture:

```bash
# 🚀 REFACTORING ANALYSIS - Detect violations first
./scripts/refactor.sh analyze src/main.cpp           # Detects multi-class files and oversized files

# 🔧 CLASS/STRUCT EXTRACTION - Preserves comments automatically  
./scripts/refactor.sh extract-class src/main.cpp Application src/Application/
./scripts/refactor.sh extract-struct src/main.cpp AppConfig src/AppConfig/
./scripts/refactor.sh extract-block src/main.cpp function signalHandler src/SignalHandler.cpp

# ✂️ PRECISE LINE OPERATIONS - For surgical code movement
./scripts/refactor.sh move-lines src/main.cpp 100 150 src/RouteSetup.cpp
./scripts/refactor.sh copy-lines src/main.cpp 200 250 backup/main_section.cpp

# 🔄 BUILD SYSTEM SYNC - Auto-updates Project.build
./scripts/refactor.sh update-build-system           # Detects new sources and updates configs
```

**Critical Refactoring Features:**
- **Comment Preservation**: Automatically includes documentation comments above functions/classes
- **Backup Safety**: All operations create automatic backups before modifications
- **"One Class Per File" Detection**: Identifies violations and suggests specific extractions
- **LLM Integration**: Follows lexer.sh → LLM analysis → refactor.sh execution workflow  
- **Build Integration**: Updates Project.build and Tests.build automatically

**LLM Refactoring Workflow (OPTIMAL EFFICIENCY):**
```bash
# 1. DISCOVER: Analyze file structure with minimal tokens
./scripts/refactor.sh analyze src/main.cpp          # Identifies refactoring needs

# 2. UNDERSTAND: Use lexical analysis for extraction planning  
./scripts/lexer.sh analyze src/main.cpp             # JSON structure for precise planning

# 3. EXECUTE: LLM commands refactor.sh with specific operations
./scripts/refactor.sh extract-class src/main.cpp Application src/Application/

# 4. VERIFY: Ensure changes compile correctly
make debug VERBOSE=minimal                          # Test extraction success
```

**Why Refactoring Tools are Essential for LLM Efficiency:**
- **Enforces optimal file organization**: Automatically creates single-class files
- **Preserves documentation**: Comments and formatting maintained during extraction
- **Safe transformation**: Backup system prevents data loss during refactoring
- **Context reduction**: Large files split into focused, single-purpose files
- **LLM-friendly output**: Structured operations reduce context pollution

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

**Priority 1: Lexical Analysis (MAXIMUM EFFICIENCY)**
```bash
# 🚀 OPTIMAL - Use lexical analysis for codebase understanding
./scripts/lexer.sh analyze src/             # Entire codebase: 1,500 tokens vs 10,000+ raw
./scripts/lexer.sh structure include/       # Module overview: 200 tokens vs 2,000+ raw
./scripts/lexer.sh dependencies             # Dependencies: 300 tokens vs 5,000+ raw

# Use lexical output for planning before making changes
```

**Priority 2: Efficient Class-Level Operations**
```bash
# ✅ OPTIMAL - Target specific class files (after lexical analysis)
Read src/Logger/Logger.h                    # Read only Logger interface (50-100 tokens)
Edit src/Logger/Logger.cpp                  # Modify only Logger implementation
Read src/WebServer/WebServer.h              # Read only WebServer interface

# ❌ INEFFICIENT - Large multi-class files
Read src/AllClasses.h                       # Reads multiple classes (500-1000 tokens)
Read src/main.cpp                           # Contains multiple concerns
```

**Context-Efficient Analysis:**
- **Lexical analysis first**: Use structured JSON summaries before reading raw C++
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

## **MEMORY MANAGEMENT & CONTEXT CLEARING**

### **Claude Code Memory Management**

**MANDATORY Clear Memory Scenarios:**

1. **Major Module Transitions** (ALWAYS clear):
   ```bash
   # When switching between unrelated modules
   Logger → WebServer → ApiRouter
   EndpointHello → IocContainer → different endpoint
   ```

2. **Architecture Changes** (ALWAYS clear):
   - Modifying core interfaces (ILogger, IWebServer, IApiRouter)
   - TDD cycles affecting multiple interconnected classes
   - Dependency injection changes across modules

3. **Context Pollution Indicators** (CLEAR immediately):
   - Claude references non-existent files/classes
   - Confusion about recent TDD state or module structure
   - Outdated build/test suggestions
   - References to removed or renamed components

**OPTIONAL Clear Memory Scenarios:**

4. **Performance Optimization** (CLEAR for efficiency):
   - Long sessions (30+ tool calls or 1-2+ hours)
   - Slower response times indicating context window pressure
   - Complex debugging sessions with accumulated context

5. **Clean Development Start** (CLEAR for clarity):
   - Beginning new major features
   - After completing and testing significant changes
   - Fresh development sessions

### **Memory Clearing Commands**

```bash
/clear                              # Complete conversation reset
```

### **Context State Assessment**

**Good Context State (KEEP memory):**
- ✅ Accurate module structure understanding
- ✅ Correct file paths and class references
- ✅ Current TDD cycle awareness
- ✅ Build commands work as expected
- ✅ Understands recent architectural decisions

**Poor Context State (CLEAR memory):**
- ❌ References deleted/renamed files
- ❌ Suggests outdated patterns
- ❌ Confusion about module dependencies
- ❌ TDD state misunderstanding
- ❌ Build failures due to incorrect assumptions

### **Memory-Efficient Development Patterns**

**Single Module Focus Strategy:**
```bash
# ✅ OPTIMAL - Work within module boundaries
make test-run-LoggerTest            # Focus on Logger module
# ... complete all Logger-related TDD work ...
/clear                             # Clear before switching modules
make test-run-WebServerTest         # Switch to WebServer module
```

**TDD Memory Patterns:**
- **RED-GREEN-REFACTOR within context**: Complete cycles without clearing
- **Module transition points**: Clear memory between different modules
- **Interface changes**: Clear before/after major interface modifications

## **Remember: Quadruple LLM Optimization Strategy! 🚀**

### **Always Apply All Four Optimizations:**

1. **Lexical Analysis First**: 85-95% token reduction for codebase understanding 
2. **Minimal Mode by Default**: 90-95% token reduction in build output
3. **One Class Per File**: Precise targeting of individual classes for context efficiency  
4. **Automated Refactoring**: Enforce file organization with comment preservation
5. **Strategic Memory Management**: Clear context at optimal transition points

### **Maximum LLM Efficiency Achieved Through:**
- **Lexical preprocessing**: Structured JSON analysis instead of raw C++ parsing
- **Output optimization**: Minimal mode for build/test operations
- **File organization**: Single-class files for focused analysis/modification
- **Automated refactoring**: Tools enforce optimal structure with comment preservation
- **Context optimization**: Strategic memory clearing prevents context pollution
- **Combined effect**: 20-50x overall context efficiency improvement

### **Optimal Development Workflow:**
```bash
# 1. DISCOVER: Check for refactoring needs first
./scripts/refactor.sh analyze src/main.cpp    # Identify violations and oversized files

# 2. ANALYZE: Use lexical analysis for understanding (maximum efficiency)
./scripts/lexer.sh analyze src/               # 1,500 tokens vs 10,000+ raw C++

# 3. REFACTOR: Extract classes if violations found
./scripts/refactor.sh extract-class src/main.cpp ClassName src/ClassName/

# 4. PLAN: Review structured JSON output for precise targeting

# 5. FOCUS: Target single module with minimal output
make test-run-ModuleName                      # Minimal mode by default

# 6. TDD: Complete RED-GREEN-REFACTOR cycle within module context
# Work within same context without clearing memory

# 7. TRANSITION: Clear memory at module boundaries  
/clear                                        # Clear before switching modules

# 8. TARGET: Read individual class files after analysis
Read src/NextModule/ClassName.h              # Single-class targeting

# Repeat cycle for maximum efficiency with optimal file organization
```