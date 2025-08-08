# Flexible Test Makefile Usage Guide

This guide explains how to use the new flexible Makefile template for test modules in CppBoilerplate.

## Overview

The new Makefile template provides an easy way to configure dependencies for test modules without manually editing compilation rules. You only need to modify the configuration section at the top of the Makefile.

## Configuration Section

At the top of each test Makefile, you'll find this configuration section:

```makefile
# ============================================================================
# CONFIGURATION SECTION - Modify this section to add/remove dependencies
# ============================================================================

ROOTDIR = ../..
MODULE_NAME = EndpointHello

# Primary module being tested (automatically included)
PRIMARY_MODULE = $(MODULE_NAME)

# Additional dependencies - add any services, modules, or utilities needed
# Format: ModuleName:FolderName (if folder differs from module name, otherwise just ModuleName)
DEPENDENCIES = ApiModules:ApiModule

# External dependencies (from external/ folder) - uncomment if needed
# EXTERNAL_DEPS = mongoose foo
```

## Usage Examples

### 1. Testing a Simple Service

For a service that only depends on IoCContainer:

```makefile
MODULE_NAME = ServiceA
PRIMARY_MODULE = $(MODULE_NAME)
DEPENDENCIES = IoCContainer
```

### 2. Testing an Endpoint

For an endpoint that depends on ApiModules:

```makefile
MODULE_NAME = EndpointHello
PRIMARY_MODULE = $(MODULE_NAME)
DEPENDENCIES = ApiModules:ApiModule
```

### 3. Testing a Complex Module

For a module with multiple dependencies:

```makefile
MODULE_NAME = ComplexService
PRIMARY_MODULE = $(MODULE_NAME)
DEPENDENCIES = ServiceA ServiceB IoCContainer Logger
```

### 4. Testing with External Dependencies

For a module that uses external libraries:

```makefile
MODULE_NAME = WebServer
PRIMARY_MODULE = $(MODULE_NAME)
DEPENDENCIES = Logger IoCContainer
EXTERNAL_DEPS = mongoose
```

### 5. Testing a Module with Custom Folder Structure

When the class name differs from the folder name:

```makefile
MODULE_NAME = SomeModule
PRIMARY_MODULE = $(MODULE_NAME)
DEPENDENCIES = ApiModules:ApiModule IoCContainer:IoCContainer
```

## Dependency Format

### Simple Dependencies
For modules where the class name matches the folder name:
```makefile
DEPENDENCIES = ServiceA ServiceB Logger
```

This automatically resolves to:
- `../../src/ServiceA/ServiceA.cpp`
- `../../src/ServiceB/ServiceB.cpp`
- `../../src/Logger/Logger.cpp`

### Custom Folder Dependencies
For modules where the class name differs from the folder name:
```makefile
DEPENDENCIES = ApiModules:ApiModule
```

This resolves to:
- `../../src/ApiModule/ApiModules.cpp`

### External Dependencies
For dependencies from the `external/` folder:
```makefile
EXTERNAL_DEPS = mongoose foo
```

This resolves to:
- `../../external/mongoose/src/mongoose.cpp`
- `../../external/foo/src/foo.cpp`

## Debugging

Use the `debug-config` target to see how your configuration is resolved:

```bash
make debug-config
```

This will show:
- All configured dependencies
- Resolved source file paths
- Object file names
- Final binary name

## Common Patterns

### Service Tests
```makefile
MODULE_NAME = ServiceName
DEPENDENCIES = IoCContainer
```

### Endpoint Tests
```makefile
MODULE_NAME = EndpointName
DEPENDENCIES = ApiModules:ApiModule
```

### Integration Tests
```makefile
MODULE_NAME = IntegrationTest
DEPENDENCIES = ServiceA ServiceB IoCContainer Logger ApiModules:ApiModule
EXTERNAL_DEPS = mongoose
```

### Utility Tests
```makefile
MODULE_NAME = UtilityName
DEPENDENCIES = Logger
```

## Migration from Old Makefiles

To migrate an existing test Makefile:

1. **Identify current dependencies** - Look at the explicit rules in your current Makefile
2. **Update configuration section** - Replace the old explicit variables with the new format
3. **Remove explicit rules** - The new template handles all compilation automatically
4. **Test the build** - Run `make clean && make` to verify everything works

Example migration:

**Old format:**
```makefile
SERVICE = ServiceA
TEST_SRC = cases/*.cpp TestMain.cpp $(ROOTDIR)/src/$(SERVICE)/$(SERVICE).cpp $(ROOTDIR)/src/IoCContainer/IoCContainer.cpp
# ... explicit rules for each dependency
```

**New format:**
```makefile
MODULE_NAME = ServiceA
DEPENDENCIES = IoCContainer
```

## Benefits

1. **Easy Configuration** - Just modify the top section, no need to understand Make patterns
2. **Automatic Rules** - Generic compilation rules work for any module
3. **Consistency** - All test Makefiles follow the same pattern
4. **Maintainable** - Adding/removing dependencies is trivial
5. **Debuggable** - Built-in debug target shows resolved configuration
6. **Extensible** - Easy to add support for new dependency types

## Tips

- Always use `make debug-config` first when adding new dependencies
- Use `make clean` before rebuilding after configuration changes
- Keep dependencies minimal - only include what's actually needed for the tests
- Use the `cases/` folder for all test case files - they're automatically included
