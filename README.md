# CppBoilerplate

A minimal C++ project boilerplate with a simple build system using Makefiles. This project demonstrates a basic structure for C++ projects, including source files, headers, and support for third-party libraries.

## Project Structure

```
CppBoilerplate/
├── Makefile            # Main build file
├── Makefile.build      # Build configuration and variables
├── include/            # Project header files
│   └── printHello.h    # Example header
├── external/           # External libraries with source
│   └── foo/           # Example external library
│       ├── include/   # External library headers
│       │   ├── foo.h  # C++ header
│       │   └── foo_c.h# C header
│       └── src/       # External library sources
│           ├── foo.cpp# C++ implementation
│           └── foo_c.c# C implementation
├── lib/                # Third-party libraries and headers
│   └── include/        # Third-party headers
└── src/                # Source files
    ├── main.cpp        # Entry point
    └── printHello.cpp  # Example implementation
```

## Building the Project

To build the project, simply run:

```bash
make
```

This will build the release target by default. For a debug build, run:

```bash
make debug
```

The binaries will be placed in the `bin/` directory, organized by architecture and build type.

## Usage

After building, run the executable (replace `<arch-vendor>` with your detected architecture and vendor, e.g., `x86_64-native`):

```bash
# Run release build
./bin/<arch-vendor>/release/hello_world

# Run debug build
./bin/<arch-vendor>/debug/hello_world
```

### Example Output
```bash
$ ./bin/x86_64-native/debug/hello_world
Hello from printHello!
External library says: Hello from the external C++ library!
C library says: Hello from the external C library!
```

## Adding Source Files

### Main Project Sources
- Place new `.cpp` files in the `src/` directory
- Place new headers in the `include/` directory

### External Library Sources
- Create a new directory in `external/<libname>/`
- Add source files in `external/<libname>/src/`
  - Use `.cpp` extension for C++ files
  - Use `.c` extension for C files
- Add headers in `external/<libname>/include/`
  - For C headers, use `extern "C"` blocks for C++ compatibility

#### Example C/C++ Header Integration
For C headers that need to be used in C++:
```c
/* foo_c.h */
#ifndef FOO_C_H
#define FOO_C_H

#ifdef __cplusplus
extern "C" {
#endif

/* C-style function declarations */
void external_print_c(const char* message);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif /* FOO_C_H */
```

For C++ headers:
```cpp
/* foo.h */
#pragma once

// C++ function declarations
void external_print(const char* message);
```

#### Using Mixed C/C++ Code
In your main C++ code:
```cpp
#include "foo.h"      // C++ header
#include "foo_c.h"    // C header

int main() {
    // Call C++ function
    external_print("Hello from C++!");
    
    // Call C function
    external_print_c("Hello from C!");
    return 0;
}

## External and Third-Party Libraries

### External Libraries with Source
The project supports external libraries with their source code in both C and C++:

- Place external libraries in `external/<libname>/`
- Structure each external library with:
  - `include/` for headers (both `.h` and `.hpp`)
  - `src/` for source files (both `.c` and `.cpp`)
- Headers and sources are automatically detected and compiled
- C files are compiled with gcc
- C++ files are compiled with g++
- No Makefile changes needed for new external libraries

Example:
```
external/foo/
├── include/
│   ├── foo.h      # C++ or C headers
│   └── foo_c.h    # C headers (with extern "C")
└── src/
    ├── foo.cpp    # C++ implementation
    └── foo_c.c    # C implementation
```

### Third-Party Libraries (Pre-built)

- Place library files (`.a` or `.so`) in the `lib/` directory
- Place third-party headers in `lib/include/<libname>/`
- Update the `Makefile` to link against new libraries as needed


## Building Details

### Build System Features
- Automatic detection of C and C++ source files
- Separate compilation rules for C (`.c`) and C++ (`.cpp`) files
- Proper compiler selection (gcc for C, g++ for C++)
- Dependency tracking for both C and C++ files
- Debug and Release build configurations
- Architecture-specific output directories

### Build Output Structure
```
bin/
└── x86_64-native/          # Architecture-vendor
    ├── debug/              # Debug build
    │   ├── src/           # Main project objects
    │   │   ├── main.o
    │   │   └── *.o
    │   ├── external/      # External library objects
    │   │   └── foo/
    │   │       └── src/
    │   │           ├── foo.o
    │   │           └── foo_c.o
    │   └── hello_world    # Debug executable
    └── release/           # Release build
        └── (same structure as debug)
```

## Debugging with VS Code

This project includes pre-configured VS Code settings for building and debugging:

- `.vscode/launch.json` for debugging with GDB
- `.vscode/tasks.json` for building with `make debug`

### Debugging Mixed C/C++ Code
- Source-level debugging for both C and C++ files
- Proper symbol loading for external libraries
- Stack traces show both C and C++ frames
- Variable inspection works for both C and C++ types

### How to Debug

1. Open this project in VS Code.
2. Press `F5` or go to the Run & Debug panel and select **(gdb) Launch Debug**.
3. The project will be built in debug mode and the debugger will start.
4. You can set breakpoints, step through code, and inspect variables.

The debug binary is expected at:

```bash
./bin/x86_64-native/debug/hello_world
```

If your architecture is different, adjust the path in `.vscode/launch.json` accordingly.

---

Feel free to use this as a starting point for your own C++ projects!
