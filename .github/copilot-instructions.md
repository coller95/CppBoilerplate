# AI Coding Instructions for CppBoilerplate

This is a **modular monolithic architecture** C++ project with clean separation of concerns while maintaining single-deployment simplicity.

## Architecture Overview

- **Modular Structure**: Each logical component (`Logger`, `printHello`, etc.) has dedicated files in `include/` and `src/`
- **Single Binary**: All modules compile into one executable (`hello_world`) despite modular organization
- **External Libraries**: Third-party code in `external/` with auto-detection (no manual Makefile changes needed)
- **Clean Interfaces**: Headers define module contracts, implementations are isolated

## Key Patterns

### Naming Convention (Microsoft camelCase + PascalCase)
- Files: `Logger.h`, `Logger.cpp` (PascalCase)
- Variables/Functions: `logInfo()`, `userName` (camelCase)
- Classes: `Logger`, `Application` (PascalCase)
- Constants: `MaxBufferSize` (PascalCase with `constexpr`)

### Module Structure
```
include/ModuleName.h    # Public interface
src/ModuleName.cpp      # Implementation
tests/moduleName/       # Unit tests (camelCase folder)
```

## Build System Workflows

### Main Application
```bash
make debug              # Debug build (default for development)
make                    # Release build
./bin/x86_64-native/debug/hello_world
```

### Unit Tests (Google Test)
```bash
cd tests && make test   # Build and run ALL tests
cd tests/printHello && make  # Build specific test
./bin/printHelloTest         # Run specific test
```

**Test Structure**: Each test module has `obj/` (objects) and `bin/` (executables) to keep source clean.

## Adding New Modules

1. **Create module files**: `include/NewModule.h`, `src/NewModule.cpp`
2. **Auto-detected** by build system (no Makefile changes)
3. **Add unit tests**: `tests/newModule/` with own Makefile (copy from existing)
4. **Update** `tests/Makefile` SUBDIRS to include new test folder

## External Dependencies

- Place in `external/libname/` with `include/` and `src/` subfolders
- C files automatically use `gcc`, C++ files use `g++`
- Use `extern "C"` in headers for C libraries used by C++

## Development Workflow

- **Use PascalCase** for new files to match existing convention
- **Debug symbols** always included (`-g` flag)
- **VS Code debugging** pre-configured for `F5` launch
- **Modular tests** - one executable per logical component
- **RAII principles** - prefer smart pointers, avoid manual memory management

Focus on maintaining the modular monolithic pattern: clear module boundaries with single deployment target.
