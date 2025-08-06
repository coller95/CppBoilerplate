
# CppBoilerplate

A minimal, modern C++ project template with a simple Makefile-based build system. This boilerplate demonstrates a clean structure for C++ projects, including support for external and third-party libraries, and is ready for use with Visual Studio Code.

---

## 📁 Project Structure

```
CppBoilerplate/
├── Makefile            # Main build file
├── Makefile.build      # Build configuration and variables
├── include/            # Project headers
│   └── printHello.h    # Example header
├── external/           # External libraries (with source)
│   └── foo/
│       ├── include/    # External library headers
│       │   ├── foo.h      # C++ header
│       │   └── foo_c.h    # C header
│       └── src/       # External library sources
│           ├── foo.cpp    # C++ implementation
│           └── foo_c.c    # C implementation
├── lib/                # Pre-built third-party libraries
│   └── include/        # Third-party headers
└── src/                # Main project sources
    ├── main.cpp        # Entry point
    └── printHello.cpp  # Example implementation
```

---

## 🛠️ Building

Build the project (release by default):

```sh
make
```

For a debug build:

```sh
make debug
```

Build artifacts are placed in `bin/<arch-vendor>/{release,debug}/`.

---

## 🚀 Running

After building, run the executable (replace `<arch-vendor>` with your architecture, e.g., `x86_64-native`):

```sh
# Release build
./bin/<arch-vendor>/release/hello_world

# Debug build
./bin/<arch-vendor>/debug/hello_world
```

**Example Output:**

```sh
$ ./bin/x86_64-native/debug/hello_world
Hello from printHello!
External library says: Hello from the external C++ library!
C library says: Hello from the external C library!
```

---

## ➕ Adding Source Files

**Main project:**
- Add `.cpp` files to `src/`
- Add headers to `include/`

**External libraries:**
- Create `external/<libname>/`
- Add sources to `external/<libname>/src/` (`.cpp` for C++, `.c` for C)
- Add headers to `external/<libname>/include/`
  - For C headers, use `extern "C"` for C++ compatibility

**Example C header for C++ compatibility:**

```c
#ifndef FOO_C_H
#define FOO_C_H

#ifdef __cplusplus
extern "C" {
#endif

void external_print_c(const char* message);

#ifdef __cplusplus
}
#endif

#endif // FOO_C_H
```

**Example C++ header:**

```cpp
#pragma once
void external_print(const char* message);
```

**Example usage in C++:**

```cpp
#include "foo.h"
#include "foo_c.h"

int main() {
    external_print("Hello from C++!");
    external_print_c("Hello from C!");
    return 0;
}
```

---

## 📦 External & Third-Party Libraries

**External (with source):**
- Place in `external/<libname>/`
- Structure: `include/` for headers, `src/` for sources
- No Makefile changes needed—sources are auto-detected
- C files use gcc, C++ files use g++

**Pre-built third-party:**
- Place `.a`/`.so` in `lib/`
- Place headers in `lib/include/<libname>/`
- Update `Makefile` to link new libraries

---

## 🏗️ Build System Features

- Auto-detects C/C++ sources
- Separate rules for `.c` and `.cpp`
- Correct compiler selection (gcc/g++)
- Dependency tracking
- Debug/Release configs
- Architecture-specific output

**Output structure:**

```
bin/
└── x86_64-native/
    ├── debug/
    │   ├── src/
    │   ├── external/
    │   │   └── foo/
    │   │       └── src/
    │   └── hello_world
    └── release/
        └── (same as debug)
```

---

## 🐞 Debugging in VS Code

- Pre-configured for GDB debugging (`.vscode/launch.json`)
- Build with `make debug` (`.vscode/tasks.json`)
- Source-level debugging for C & C++
- Full symbol loading for external libs
- Mixed C/C++ stack traces & variable inspection

**How to debug:**
1. Open in VS Code
2. Press `F5` or select **(gdb) Launch Debug**
3. The project builds in debug mode and launches the debugger
4. Set breakpoints, step, and inspect variables

Debug binary default path:

```sh
./bin/x86_64-native/debug/hello_world
```

If your architecture differs, update `.vscode/launch.json` accordingly.

---

Feel free to use this as a starting point for your own modern C++ projects!
