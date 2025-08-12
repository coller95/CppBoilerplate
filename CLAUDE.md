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

### Output Modes
The build system supports two output modes:
- **Human Mode (VERBOSE=1)**: Colorful, detailed output with emojis and progress indicators
- **Agent Mode (VERBOSE=0)**: Ultra-concise, structured output optimized for AI processing

Set via environment variable or command argument:
```bash
VERBOSE=1 make debug               # Human-friendly build
VERBOSE=0 make debug               # Agent-friendly build
./scripts/test.sh human            # Human-friendly test script
./scripts/test.sh agent            # Agent-friendly test script
```

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
- Supports pluggable backends (currently Mongoose)
- Integrates with ApiRouter for request routing
- Provides factory pattern for backend selection
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

**Dual-Mode Output System**: Optimized for both humans and AI agents:
- Human mode: Colorful, descriptive output with comprehensive error context
- Agent mode: Ultra-concise, structured output for AI context efficiency
- Consistent prefixes: `[BUILD]`, `[TEST]`, `[SCRIPT]`

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

**Test-Driven Development (TDD)**: Mandatory for all code changes:
- Write failing tests first in `cases/` subdirectories
- Implement minimal code to pass tests
- Use `make test-run-ModuleName` for rapid iteration
- Comprehensive test coverage with auto-registration verification

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
- Clear interfaces enable swappable implementations
- Dependency injection eliminates hard dependencies
- Auto-registration reduces boilerplate and manual wiring

**Testability First**: Every component designed for comprehensive testing:
- Interface-based design enables complete mocking
- Test isolation prevents cross-module interference
- Auto-registration verification catches integration issues

**Scalability**: Architecture supports growing complexity:
- New modules integrate automatically via established patterns
- Test framework scales with configurable dependency management
- Build system handles increasing source file complexity

**Developer Experience**: Tooling optimized for rapid development:
- Generation scripts eliminate boilerplate and ensure consistency
- Dual-mode output adapts to human vs. automated workflows
- Enhanced test management provides comprehensive project visibility

This architecture enables rapid development of new features while maintaining high code quality, comprehensive test coverage, and clear separation of concerns across all system components.