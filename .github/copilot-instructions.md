
# AI Coding Instructions for CppBoilerplate

This project uses a **modular monolithic architecture** in C++ with strict Microsoft naming conventions and clean separation of concerns.

## Architecture Overview

- **Modular Structure**: Each logical component (e.g., `Logger`, `PrintHello`, etc.) has dedicated files in `include/` and `src/`.
- **Single Binary**: All modules are compiled into one executable (`hello_world`).
- **External Libraries**: Third-party code is placed in `external/` and auto-detected by the build system.
- **Clean Interfaces**: Public headers define module contracts; implementations are isolated in source files.

## Naming Conventions (Microsoft Style)

- **All source, header, and test files and folders must use PascalCase** to match class/struct names and Microsoft C++ guidelines.
  - Examples: `RestfulServer.cpp`, `RestfulServerTest.cpp`, `LoggerTest.cpp`, `RestfulServerTest/`
- **Classes/structs:** PascalCase (e.g., `RestfulServerTest`)
- **Variables/functions:** camelCase (e.g., `isRunning`, `registerHandler`)
- **Constants/enums:** PascalCase (e.g., `MaxBufferSize`, `enum class Status`)
- **No camelCase file or folder names**—use PascalCase everywhere for clarity and consistency.

### Module Structure
```
include/ModuleName.h    # Public interface
src/ModuleName.cpp      # Implementation
tests/ModuleNameTest/   # Unit tests (PascalCase folder)
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
cd tests/PrintHelloTest && make  # Build specific test
./bin/PrintHelloTest         # Run specific test
```

**Test Structure**: Each test module has `obj/` (objects) and `bin/` (executables) to keep source clean.

## Adding New Modules

1. **Create module files**: `include/NewModule.h`, `src/NewModule.cpp`
2. **Auto-detected** by build system (no Makefile changes)
3. **Add unit tests**: `tests/NewModuleTest/` with its own Makefile (copy from existing)
4. **Update** `tests/Makefile` SUBDIRS to include the new test folder (PascalCase)

## External Dependencies

- Place in `external/LibName/` with `include/` and `src/` subfolders
- C files automatically use `gcc`, C++ files use `g++`
- Use `extern "C"` in headers for C libraries used by C++

## Development Workflow

- **Use PascalCase** for all new files and folders
- **Debug symbols** always included (`-g` flag)
- **VS Code debugging** pre-configured for `F5` launch
- **Modular tests**: one executable per logical component
- **RAII principles**: prefer smart pointers, avoid manual memory management

**Focus:** Maintain the modular monolithic pattern—clear module boundaries, single deployment target, and strict Microsoft naming conventions throughout the codebase.
