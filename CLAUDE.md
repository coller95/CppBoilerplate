# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Quick Development Workflow
```bash
# Complete build, test, and run cycle
./scripts/test.sh                    # Agent mode (concise output)
./scripts/test.sh human             # Human mode (colorful, detailed output)

# Individual build commands
make debug                          # Build debug version
make release                        # Build optimized release version
make run_debug                      # Build and run debug version
make run                           # Build and run release version

# Clean build artifacts
make clean                         # Clean main project build artifacts
make test-clean                    # Clean all test artifacts
```

### Testing Commands
```bash
# Run all tests
make test                          # Run all project tests
make test-help                     # Show all available test targets

# Individual module testing
make test-run-<ModuleName>         # Build and run tests for specific module
make test-make-<ModuleName>        # Build tests only (no execution)
make test-clean-<ModuleName>       # Clean artifacts for specific module
make test-debug-<ModuleName>       # Show dependency configuration

# Project statistics and debugging
make test-stats                    # Show test project statistics
make test-debug-all               # Debug all module configurations

# Examples
make test-run-LoggerTest          # Run Logger module tests
make test-run-EndpointHelloTest   # Run EndpointHello module tests
make test-run-IocContainerTest    # Run IoC Container tests
```

### Multi-Tiered Output Modes & LLM Context Optimization

**CRITICAL**: The build system supports **5 distinct output modes** for optimal LLM context efficiency:

**Available Modes (ordered by context usage):**
- **`minimal`** - Ultra-minimal output (DEFAULT, best performance, 90-95% token reduction)
- **`standard`** - Balanced agent output (moderate context usage)
- **`debug`** - Agent + debugging context (troubleshooting scenarios)
- **`human`** - Full human-friendly output (colorful, verbose)
- **`silent`** - Critical errors only (absolute minimum)

**Usage Examples:**
```bash
# Default optimal performance (minimal mode)
make debug                         # Uses minimal mode by default
./scripts/test.sh                  # Uses minimal mode by default

# Explicit mode selection
make debug VERBOSE=minimal         # Ultra-minimal (best performance)
make debug VERBOSE=standard        # Moderate context
make debug VERBOSE=debug           # Troubleshooting context
make debug VERBOSE=human           # Full human-friendly
make debug VERBOSE=silent          # Errors only

# Script mode selection
./scripts/test.sh minimal          # Ultra-minimal (default)
./scripts/test.sh standard         # Balanced output
./scripts/test.sh debug            # Debugging context
./scripts/test.sh human            # Human-friendly
./scripts/test.sh silent           # Critical errors only

# Backward compatibility (legacy support)
VERBOSE=0 make debug               # Maps to minimal mode
VERBOSE=1 make debug               # Maps to human mode
./scripts/test.sh agent            # Maps to standard mode
```

**Context Efficiency Comparison:**
```bash
# Human mode (100% token usage):
🔨 Building debug target for x86_64 architecture...
✅ Successfully compiled: src/main.cpp
🎯 Linking executable: hello_world  
🚀 Build completed successfully in 2.3 seconds!

# Standard mode (~20% token usage):
[BUILD] LINK debug hello_world
[BUILD] BUILT debug hello_world x86_64-native

# Minimal mode (~5-10% token usage - OPTIMAL):
[BUILD] hello_world

# Silent mode (~2-5% token usage):
# (no output unless errors occur)
```

**LLM Performance Benefits:**
- **Minimal mode**: 90-95% token reduction, 5-10x faster processing
- **Structured prefixes**: `[BUILD]`, `[TEST]`, `[SCRIPT]`, `[ERROR]` for efficient parsing
- **Context budget optimization**: More operations possible within token limits
- **Scalable complexity**: Handle multi-step tasks efficiently

### Module Generation
```bash
# Create new modules using generation scripts
./scripts/create_endpoint.sh create EndpointName    # HTTP endpoints
./scripts/create_service.sh create ServiceName      # Business logic services  
./scripts/create_module.sh create ModuleName        # Utility/infrastructure modules

# Remove modules safely with confirmation
./scripts/create_endpoint.sh remove EndpointName
./scripts/create_service.sh remove ServiceName
./scripts/create_module.sh remove ModuleName
```

### **NEW: Lexical Analysis for LLM Context Optimization**

**CRITICAL**: Use lexical analysis for maximum LLM efficiency when working with large codebases:

```bash
# Lexical analysis commands (90%+ token reduction)
./scripts/lexer.sh analyze src/               # Complete codebase analysis  
./scripts/lexer.sh structure include/         # Module structure overview
./scripts/lexer.sh dependencies               # Project dependency mapping

# Token efficiency demonstration  
./scripts/token_comparison.sh demo            # Project-wide efficiency stats
./scripts/token_comparison.sh compare file    # Compare specific file/module
```

**Why Use Lexical Analysis:**
- **85-95% token reduction** - process 10x more code in same context
- **Structured JSON output** - surgical precision targeting
- **Consistent parsing** - eliminates LLM syntax parsing errors  
- **Context multiplier** - analyze entire codebase efficiently
- **Integration ready** - prepared for automated refactoring workflows

**LLM Workflow Integration:**
```bash
# 1. ANALYZE: Generate structured summary (200 tokens vs 10,000+ raw)
./scripts/lexer.sh analyze src/ > codebase_summary.json

# 2. STANDARDIZED ANALYSIS: Use predictable LLM analysis protocol
./scripts/llm_analysis.sh analyze codebase_summary.json one-class-per-file

# 3. VALIDATE: Ensure analysis output is consistent and correct
./scripts/llm_analysis.sh validate analysis_output.json

# 4. EXECUTE: Apply validated refactoring commands
./scripts/refactor.sh extract-class src/main.cpp Application src/Application/

# 5. VERIFY: Ensure build still works after refactoring
make debug VERBOSE=minimal
```

### **NEW: Standardized LLM Analysis Protocol**

**CRITICAL**: Use standardized LLM analysis for predictable, testable refactoring workflows:

```bash
# Standardized LLM analysis commands (95% consistency guarantee)
./scripts/llm_analysis.sh analyze lexer_output.json one-class-per-file  # Detect violations
./scripts/llm_analysis.sh analyze lexer_output.json extract-class       # Plan extractions  
./scripts/llm_analysis.sh analyze lexer_output.json dependency-analysis # Analyze dependencies

# Validation and consistency testing
./scripts/llm_analysis.sh validate analysis_output.json                 # Validate schema
./scripts/test_llm_consistency.sh run                                   # Test consistency
./scripts/test_llm_consistency.sh report                                # Generate report
```

**Why Use Standardized LLM Analysis:**
- **95% Consistency** - standardized prompts eliminate analysis variance
- **JSON Schema Validation** - ensures output correctness and compatibility
- **Automated Testing** - verify LLM analysis produces consistent results
- **Predictable Output** - deterministic refactoring commands every time
- **Error Detection** - catch inconsistencies before executing refactor operations
- **Continuous Improvement** - test-driven validation enables protocol refinement

### **NEW: Automated Refactoring Tools**

**CRITICAL**: Use refactoring tools for LLM-driven code extraction and reorganization:

```bash
# Refactoring analysis and execution (preserves comments and documentation)
./scripts/refactor.sh analyze src/main.cpp           # Analyze for "One Class Per File" violations
./scripts/refactor.sh extract-class src/main.cpp Application src/Application/
./scripts/refactor.sh extract-struct src/main.cpp AppConfig src/AppConfig/
./scripts/refactor.sh extract-block src/main.cpp function signalHandler src/SignalHandler.cpp

# Precise line-based operations
./scripts/refactor.sh move-lines src/main.cpp 100 150 src/RouteSetup.cpp
./scripts/refactor.sh copy-lines src/main.cpp 200 250 backup/main_section.cpp

# Build system integration
./scripts/refactor.sh update-build-system          # Auto-detect new sources and update configs
```

**Why Use Refactoring Tools:**
- **Comment Preservation** - automatically includes documentation comments above functions/classes
- **"One Class Per File" Enforcement** - detects violations and suggests extraction
- **Standardized Integration** - follows lexer.sh → llm_analysis.sh → refactor.sh workflow
- **Safe Operations** - automatic backups before any file modifications
- **Build System Sync** - updates Project.build and Tests.build configurations
- **Context Efficiency** - enables large-scale refactoring within LLM token limits
- **Validated Execution** - only executes refactor commands that pass consistency validation

**LLM Refactoring Workflow:**
```bash
# 1. DISCOVER: Find files that need refactoring
./scripts/refactor.sh analyze src/main.cpp

# 2. ANALYZE: Use lexical analysis for structured understanding
./scripts/lexer.sh analyze src/main.cpp > lexer_output.json

# 3. STANDARDIZED PLANNING: Generate predictable LLM analysis
./scripts/llm_analysis.sh analyze lexer_output.json extract-class

# 4. VALIDATE: Ensure analysis output is consistent and executable
./scripts/llm_analysis.sh validate analysis_output.json

# 5. EXECUTE: Apply validated refactoring commands  
./scripts/refactor.sh extract-class src/main.cpp Application src/Application/

# 6. VERIFY: Ensure build still works after refactoring
make debug VERBOSE=minimal
```

## **CRITICAL: Test-Driven Development (TDD) Requirements**

⚠️ **MANDATORY TDD DISCIPLINE** - This project strictly enforces Test-Driven Development:

1. **RED-GREEN-REFACTOR CYCLE**:
   - 🔴 **RED**: Write failing test first (`make test-run-ModuleName`)
   - 🟢 **GREEN**: Write minimal code to make test pass
   - 🔵 **REFACTOR**: Improve code while keeping tests green

2. **STRICT RULES**:
   - **NEVER write implementation code without a failing test first**
   - **NO exceptions** - applies to new features, bug fixes, and refactoring
   - **ONE CLASS PER FILE** - mandatory for LLM context efficiency
   - Tests must be comprehensive and cover edge cases
   - All auto-registration functionality must be verified in tests

3. **TDD WORKFLOW**:
   ```bash
   # 1. Create failing test in tests/ModuleNameTest/cases/
   make test-run-ModuleName  # Should fail (RED)
   
   # 2. Implement minimal code to pass
   make test-run-ModuleName  # Should pass (GREEN)
   
   # 3. Refactor and ensure tests still pass
   make test-run-ModuleName  # Should remain green (REFACTOR)
   ```

4. **WHY TDD IS MANDATORY**:
   - Ensures robust, testable architecture
   - Prevents regression bugs  
   - Documents expected behavior
   - Enables confident refactoring
   - Maintains code quality at scale
   - **Drives clean design** - TDD led to removing `poll()` method for cleaner interfaces
   - **Enforces one-class-per-file** - TDD encourages focused, testable classes

## High-Level Architecture

This project uses a **modular monolithic architecture** with the following key principles:

### Core Architecture Components

**Application Class (`src/main.cpp`)**: The main application orchestrates:
- IoC Container initialization and service registration
- WebServer configuration and startup
- ApiRouter initialization for HTTP endpoint management
- Service lifecycle management (startup, shutdown)

**IoC Container (`IocContainer/`)**: Dependency injection container that:
- Manages service registration and resolution
- Supports singleton and instance-based services
- Enables loose coupling and testability
- Provides service discovery and type management

**WebServer (`WebServer/`)**: HTTP server abstraction that:
- **Self-contained backends** - each backend handles its own event processing internally
- **Clean interfaces** - no backend-specific methods (like `poll()`) leak to interfaces
- **Internal threading** - Mongoose backend uses dedicated thread for event processing
- **Pluggable architecture** - supports multiple backends via factory pattern
- Integrates with ApiRouter for request routing
- Handles MIME types, static content, and JSON responses

**ApiRouter (`ApiRouter/`)**: API endpoint management system that:
- Manages HTTP endpoint registration and routing
- Supports automatic module registration
- Provides global access to registered endpoints
- Integrates seamlessly with WebServer for request handling

**Logger (`Logger/`)**: Comprehensive logging system that:
- Supports both local display and remote logging
- Integrates with IoC container for dependency injection
- Provides structured logging with different severity levels
- Handles connection failures gracefully

### Architectural Best Practices

**File Organization Principles:**
- **One Class Per File**: MANDATORY - Each class must be in its own separate file
- **LLM Context Efficiency**: Single-class files enable precise file reading and modification
- **Modular Analysis**: AI assistants can analyze individual classes without irrelevant context
- **Clear Boundaries**: File boundaries match logical class boundaries for better understanding

**Interface Design Principles:**
- **No Leaky Abstractions**: Interfaces must NOT expose implementation-specific methods
- **Self-Contained Components**: Each component handles its own internal processes (e.g., event loops, threading)
- **Clean Separation**: Consumer code should never need to know about backend implementation details
- **Example**: WebServer backends handle event processing internally - no external `poll()` methods

**Threading and Event Processing:**
- **Internal Threading**: Components requiring event processing use internal threads (e.g., Mongoose backend)
- **Atomic Operations**: Thread-safe state management using `std::atomic` types
- **Graceful Shutdown**: Components handle their own cleanup and thread joining
- **No External Dependencies**: Main application doesn't handle component-specific event processing

**Interface Segregation:**
- **Minimal Interfaces**: Only expose methods that ALL implementations can meaningfully support
- **No Optional Methods**: Avoid methods that some implementations treat as no-ops
- **Behavioral Consistency**: Interface methods should have consistent behavior across implementations

### Module Organization Principles

**One Module, One Responsibility**: Each module has a clear, single purpose:
- **Endpoints** (`EndpointHello/`): Handle specific HTTP requests
- **Services**: Implement business logic with IoC integration
- **Infrastructure**: Provide system-level functionality (WebServer, Logger)

**Interface-Based Design**: All modules implement clear interfaces:
- `ILogger`, `IWebServer`, `IWebServerBackend`, `IApiRouter`
- Enables mocking and testing isolation
- Supports dependency injection patterns

**Auto-Registration Pattern**: Services and endpoints automatically register:
- Endpoints register with ApiRouter during initialization
- Services register with IoC Container
- No manual wiring required in main application

**Test Isolation**: Each module has comprehensive, isolated test suites:
- Tests in `tests/ModuleNameTest/cases/` subdirectories
- Mock implementations for all interfaces
- Verification of auto-registration functionality

### Build System Architecture

**Immutable Makefile Design**: Core build logic never changes:
- Main `Makefile` contains generic build patterns
- `Project.build` contains project-specific source/library configuration
- `Platform.build` handles cross-compilation and architecture detection
- `Tests.build` provides test-related targets at project level

**Flexible Test Framework**: Each test module uses configurable Makefiles:
- Dependencies specified declaratively: `DEPENDENCIES = Logger IocContainer`
- Special folder handling: `ModuleName:FolderName` format
- External library support via `EXTERNAL_SOURCES`, `EXTERNAL_INCLUDES`, `EXTERNAL_LIBS`
- Automatic compilation rules and object file organization

**Multi-Tiered Output System**: Optimized for surgical LLM context control:
- **Minimal mode** (default): Ultra-minimal output, 90-95% token reduction
- **Standard mode**: Balanced agent output, 80% token reduction  
- **Debug mode**: Agent + debugging context, 60% token reduction
- **Human mode**: Colorful, descriptive output for humans
- **Silent mode**: Critical errors only, maximum token efficiency
- **Structured prefixes**: `[BUILD]`, `[TEST]`, `[SCRIPT]`, `[ERROR]` for parsing
- **Context budget optimization**: Surgical precision for complex tasks

### LLM Context Optimization Guidelines

**CRITICAL: Always Minimize Context Usage for AI Efficiency**

**1. Use Minimal Mode by Default:**
```bash
# ALWAYS use minimal mode for best LLM performance
./scripts/test.sh                 # Defaults to minimal mode (ultra-concise)
make test                         # Defaults to minimal mode
make debug                        # Defaults to minimal mode

# Escalate verbosity only when needed
make debug VERBOSE=standard       # When moderate context needed
make debug VERBOSE=debug          # When troubleshooting
```

**2. Output Efficiency Principles:**
- **Essential Information Only**: Print only what's necessary for decision-making
- **Structured Format**: Use consistent prefixes `[BUILD]`, `[TEST]`, `[ERROR]`
- **No Decorative Elements**: Eliminate colors, emojis, ASCII art, progress bars
- **Compressed Status**: `PASSED/FAILED` instead of verbose descriptions
- **Token Reduction**: 80-90% less context usage compared to human mode

**3. Context-Efficient Command Examples:**
```bash
# ✅ OPTIMAL - Minimal mode (5-10% tokens)
[BUILD] hello_world
[TEST] ModuleTest 5/5

# ✅ GOOD - Standard mode (15-20% tokens)
[BUILD] BUILT debug hello_world x86_64-native
[TEST] RUN ModuleTest
[TEST] RESULT ModuleTest 5/5

# ⚠️ ACCEPTABLE - Debug mode (40% tokens, troubleshooting only)
[BUILD] COMPILE src/main.cpp
[BUILD] LINK debug hello_world
[BUILD] BUILT debug hello_world x86_64-native

# ❌ BAD - Human mode (100% tokens, wastes context)
🧪 Running comprehensive test suite for ModuleName...
✅ Test case 1: BasicFunctionality - PASSED
✅ Test case 2: ErrorHandling - PASSED  
🎯 All 5 test cases completed successfully!
🔨 Compiling with optimizations for x86_64 architecture...
```

**4. LLM Interaction Best Practices:**
- **One Class Per File Architecture**: Each class in separate file enables precise context targeting
- **Read individual classes**: Target specific class files instead of large multi-class files
- **Use limits**: `Read` tool with `offset` and `limit` parameters when files are large
- **Single-class modifications**: Edit individual class files for minimal context usage
- **Batch operations**: Multiple tool calls in single response when possible
- **Targeted searches**: `Grep` with specific patterns instead of broad searches
- **Class-level granularity**: Analyze/modify at class level for maximum efficiency

**5. Multi-Tiered Context Budget Management:**
- **Minimal mode saves ~10-20x tokens** compared to human mode
- **Standard mode saves ~5x tokens** - use for moderate complexity
- **Debug mode saves ~2.5x tokens** - use only for troubleshooting
- **Essential for complex tasks** that require multiple tool calls
- **Enables deeper analysis** within context limits
- **Faster processing** due to reduced token overhead
- **Surgical precision** - exact verbosity control per task

### External Dependencies

**Google Test Framework** (`external/googletest/`): Complete testing infrastructure
- Integrated with all test modules via build system
- Includes Google Mock for dependency mocking
- Configured in `bootstrap.sh` for automatic setup

**Mongoose Web Server** (`external/mongoose/`): HTTP server backend
- C library with C++ wrapper integration
- Source-based inclusion for cross-platform compatibility
- Abstracted through WebServer interface for pluggability

### Development Workflow Integration

**Test-Driven Development (TDD)**: **MANDATORY** for all code changes and implementations:
- **ALWAYS write failing tests FIRST** before any implementation code
- Tests must be written in `cases/` subdirectories for each module
- Implement **minimal code only** to make tests pass (Red-Green-Refactor cycle)
- Use `make test-run-ModuleName` for rapid TDD iteration
- **NO CODE CHANGES WITHOUT TESTS** - this is a strict requirement
- Comprehensive test coverage including auto-registration verification
- All new features, bug fixes, and refactoring must follow TDD discipline

**Module Generation Scripts**: Consistent structure across all modules:
- `create_endpoint.sh`: HTTP endpoints with ApiRouter integration
- `create_service.sh`: Business services with IoC registration
- `create_module.sh`: Utility modules with interface/implementation patterns

**Enhanced Test Management**: Comprehensive test operations from project root:
- Individual module testing without changing directories
- Project-wide statistics and debugging information
- Intelligent error handling with module name suggestions

### Key Architectural Benefits

**High Cohesion, Low Coupling**: Each module is self-contained but integrates cleanly:
- **Clean interfaces** with no leaky abstractions (no backend-specific methods like `poll()`)
- **Self-contained components** handle their own internal processing (threading, event loops)
- **Interface segregation** ensures minimal, universally-implementable methods
- Clear interfaces enable swappable implementations
- Dependency injection eliminates hard dependencies
- Auto-registration reduces boilerplate and manual wiring

**TDD-First Testability**: Every component designed through Test-Driven Development:
- **ALL code written following RED-GREEN-REFACTOR TDD cycle**
- Interface-based design enables complete mocking and test isolation
- Test isolation prevents cross-module interference
- Auto-registration verification catches integration issues
- **Tests written BEFORE implementation ensures robust design**

**Scalability**: Architecture supports growing complexity:
- New modules integrate automatically via established patterns
- Test framework scales with configurable dependency management
- Build system handles increasing source file complexity

**Developer Experience**: Tooling optimized for rapid development:
- Generation scripts eliminate boilerplate and ensure consistency
- Dual-mode output adapts to human vs. automated workflows
- Enhanced test management provides comprehensive project visibility

This architecture enables rapid development of new features while maintaining high code quality, comprehensive test coverage, and clear separation of concerns across all system components.

**Multi-tiered output optimization ensures maximum LLM efficiency** - the `minimal` mode default provides 90-95% token reduction while preserving all essential information for AI-assisted development workflows.

## **MEMORY MANAGEMENT & CONTEXT CLEARING**

### **When to Clear Claude Code Memory**

**MANDATORY Clear Memory Scenarios:**

1. **Major Context Shift** (ALWAYS clear):
   - Switching from one module to completely different module (e.g., Logger → WebServer → ApiRouter)
   - Moving between unrelated features or bug fixes
   - Starting new TDD cycles for different components

2. **Architecture Changes** (ALWAYS clear):
   - Modifying core interfaces (ILogger, IWebServer, IApiRouter)
   - Changing dependency relationships between modules
   - Refactoring that affects multiple interconnected classes

3. **Error Recovery** (CLEAR when needed):
   - Build failures that require significant debugging context
   - When Claude Code seems confused about project state
   - After failed TDD attempts that left incomplete understanding

4. **Context Pollution** (CLEAR when noticed):
   - Responses reference outdated/incorrect information
   - Claude mentions files/classes that don't exist
   - Confusion about current module structure or recent changes

**OPTIONAL Clear Memory Scenarios:**

5. **Long Sessions** (CLEAR for performance):
   - After 30+ tool calls in single conversation
   - When context window approaches limits (slower responses)
   - Working sessions longer than 1-2 hours

6. **Clean Slate Development** (CLEAR for clarity):
   - Beginning major new features from scratch
   - Starting fresh development sessions
   - After completing and testing major changes

### **Memory Clearing Commands**

```bash
# Clear Claude Code memory
/clear                              # Complete conversation reset

# Alternative: Start fresh conversation
# Close current session and start new one
```

### **Best Practices for Memory Management**

**DO Clear Memory When:**
- ✅ Context becomes stale or incorrect
- ✅ Major architectural changes planned
- ✅ Switching between unrelated modules
- ✅ Build failures require fresh perspective
- ✅ Long development sessions (1-2+ hours)

**DON'T Clear Memory When:**
- ❌ Working on same module/feature incrementally
- ❌ Following TDD cycle within same class
- ❌ Making small related changes
- ❌ Context is still accurate and helpful

**Memory-Efficient Development Patterns:**
- **Single module focus**: Work on one module at a time to maintain context
- **TDD discipline**: Complete RED-GREEN-REFACTOR cycles before switching contexts
- **Incremental changes**: Small, related changes maintain context efficiency
- **Clear module boundaries**: Use memory clearing as module transition points
- **Lexical analysis first**: Use `./scripts/lexer.sh` to understand codebase before making changes
- **Standardized analysis**: Use `./scripts/llm_analysis.sh` for predictable refactoring workflows
- **Consistency validation**: Always validate LLM analysis output before executing commands
- **Structured summaries**: Prefer lexical JSON over reading multiple raw C++ files

### **Context State Indicators**

**Good Context State (keep memory):**
- Claude accurately describes current module structure
- References correct file paths and class names
- Understands recent changes and TDD state
- Build commands work as expected

**Poor Context State (clear memory):**
- Claude references non-existent files/classes
- Confusion about module dependencies
- Suggests outdated approaches or patterns
- Repeated build/test failures due to misunderstanding

## **FINAL REMINDERS**

### **TDD is MANDATORY**
⚠️ Every code change in this project MUST follow Test-Driven Development:
- 🔴 **RED**: Write failing test FIRST  
- 🟢 **GREEN**: Implement minimal code to pass
- 🔵 **REFACTOR**: Improve while keeping tests green
- **NO CODE WITHOUT TESTS** - This is non-negotiable

### **LLM Context Optimization is DEFAULT**
⚡ **Always use minimal mode for optimal AI performance:**
- `make debug` (defaults to minimal mode - 90-95% token reduction)
- `./scripts/test.sh` (defaults to minimal mode for best performance)
- Escalate verbosity only when debugging: `VERBOSE=standard` or `VERBOSE=debug`
- **Never use `VERBOSE=human` in LLM contexts** - wastes 10-20x more tokens

### **Standardized LLM Analysis is MANDATORY**
🚀 **Use standardized LLM analysis for predictable, efficient workflows:**
- `./scripts/lexer.sh analyze` for codebase understanding (85-95% token reduction)
- `./scripts/llm_analysis.sh analyze` for consistent refactoring decisions (95% repeatability)
- `./scripts/llm_analysis.sh validate` to ensure output correctness before execution
- `./scripts/test_llm_consistency.sh run` to verify analysis consistency
- Structured JSON analysis instead of reading raw C++ files
- Essential for large refactoring and architectural analysis

### **Memory Management is CRITICAL**
🧠 **Clear memory strategically for optimal development:**
- Clear between major module transitions or architectural changes
- Clear when context becomes stale or incorrect
- Keep memory for incremental work within same module/feature
- Use `/clear` command when context state indicators suggest confusion
