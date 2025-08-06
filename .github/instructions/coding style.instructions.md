---
applyTo: '**'
---

# C++ Coding Style Guidelines

This document defines the coding style guidelines for this C++ project. All code must strictly follow Microsoft's camelCase naming conventions and these established patterns.

## Naming Conventions

### Variables and Functions
- Use **camelCase** for all variables and functions
- Start with lowercase letter
- Examples:
  ```cpp
  int myVariable = 0;
  std::string userName = "john";
  void calculateSum(int firstNumber, int secondNumber);
  bool isFileExists(const std::string& fileName);
  ```

### Classes and Structs
- Use **PascalCase** (UpperCamelCase) for class and struct names
- Start with uppercase letter
- Examples:
  ```cpp
  class MyClass {};
  struct DataBuffer {};
  class FileManager {};
  ```

### Member Variables
- Use **camelCase** with optional prefix
- Private members may use underscore prefix for clarity
- Examples:
  ```cpp
  class MyClass {
  private:
      int _memberVariable;  // preferred with underscore
      std::string _fileName;
  public:
      int publicMember;     // alternative without underscore
  };
  ```

### Constants and Enums
- Use **PascalCase** for enum types
- Use **PascalCase** for enum values
- Use **PascalCase** for constants
- Prefer `constexpr` over `const` when possible
- Always use `enum class` over plain `enum`
- Examples:
  ```cpp
  constexpr int MaxBufferSize = 1024;
  constexpr std::string_view DefaultFileName = "output.txt";
  
  // Avoid plain enums
  enum Status {  // Avoid this
      Success,
      Failure,
      Pending
  };
  
  // Prefer scoped enums
  enum class FileType {
      Text,
      Binary,
      Compressed
  };
  
  enum class Status {
      Success,
      Failure,
      Pending
  };
  ```

### Namespaces
- Use **lowercase** with underscores if needed
- Keep concise and descriptive
- Examples:
  ```cpp
  namespace utils {}
  namespace file_manager {}
  ```

### Macros and Preprocessor Directives
- Use **UPPER_CASE** with underscores
- Examples:
  ```cpp
  #define MAX_BUFFER_SIZE 1024
  #define ENABLE_LOGGING
  ```

## File Naming
- Use **camelCase** for source files
- Header files: `.h` extension
- Source files: `.cpp` extension
- Examples:
  ```
  myClass.h
  myClass.cpp
  fileManager.h
  fileManager.cpp
  ```

## Code Formatting

### Indentation
- Use 4 spaces (no tabs)
- Consistent indentation for all nested blocks

### Braces
- Opening brace on same line for functions, classes, and control structures
- Examples:
  ```cpp
  void myFunction() {
      if (condition) {
          // code here
      }
  }
  
  class MyClass {
  public:
      void method() {
          // implementation
      }
  };
  ```

### Line Length
- Maximum 120 characters per line
- Break long lines appropriately

### Spacing
- Space after keywords: `if (`, `for (`, `while (`
- Space around operators: `x + y`, `a == b`
- No space before semicolons or commas
- Space after commas: `function(a, b, c)`

## Comments and Documentation

### Function Comments
```cpp
/**
 * Calculates the sum of two integers
 * @param firstNumber The first integer to add
 * @param secondNumber The second integer to add
 * @return The sum of the two numbers
 */
int calculateSum(int firstNumber, int secondNumber);
```

### Class Comments
```cpp
/**
 * Manages file operations including reading, writing, and validation
 */
class FileManager {
    // class implementation
};
```

### Inline Comments
- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Keep comments concise and meaningful

## Include Guards
- Use `#pragma once` for header files
- Example:
  ```cpp
  #pragma once
  
  class MyClass {
      // class definition
  };
  ```

## Error Handling and Memory Management
- **Prefer exceptions** over error codes for exceptional conditions
- **Use descriptive exception messages** with context
- **Follow RAII principles** - resources should be managed by objects
- **Avoid manual memory management** - prefer smart pointers
- **Use stack allocation** when possible
- **Check for nullptr** when using raw pointers
- **Prefer standard library containers** over C-style arrays

### Examples:
```cpp
// Good: RAII with smart pointers
class ResourceManager {
private:
    std::unique_ptr<Resource> _resource;
public:
    ResourceManager() : _resource(std::make_unique<Resource>()) {}
    // Destructor automatically called, resource cleaned up
};

// Good: Exception with context
void openFile(std::string_view fileName) {
    if (fileName.empty()) {
        throw std::invalid_argument("File name cannot be empty");
    }
    // file operations...
}

// Good: Container usage
std::vector<int> numbers{1, 2, 3, 4, 5};  // Better than int numbers[5]
```

## Modern C++ Features
- **Use `auto`** when type is obvious from context
- **Prefer smart pointers** over raw pointers (`std::unique_ptr`, `std::shared_ptr`)
- **Use range-based for loops** when iterating over containers
- **Use `const` and `constexpr`** where applicable
- **Prefer `std::string_view`** for read-only string parameters
- **Use uniform initialization** with braces `{}`
- **Avoid C-style casts**, use C++ static_cast, dynamic_cast, etc.
- **Use lambda expressions** for short, local functions
- **Prefer algorithms** from `<algorithm>` over manual loops
- **Use `nullptr`** instead of `NULL` or `0`
- **Use `using` aliases** instead of `typedef`
- **Embrace RAII** (Resource Acquisition Is Initialization)

### Examples:
```cpp
// Good: Modern C++ practices
auto myVector = std::vector<int>{1, 2, 3, 4, 5};
auto uniquePtr = std::make_unique<MyClass>(42);
constexpr auto Pi = 3.14159;

void processStrings(std::string_view input) {  // Better than const std::string&
    for (const auto& item : myVector) {  // Range-based for
        std::cout << item << " ";
    }
}

using IntVector = std::vector<int>;  // Better than typedef

// Good: Lambda for simple operations
auto isEven = [](int n) { return n % 2 == 0; };
std::count_if(myVector.begin(), myVector.end(), isEven);

// Good: Algorithm usage
std::sort(myVector.begin(), myVector.end());
```

## Example Code Structure
```cpp
#pragma once

#include <string>
#include <vector>

/**
 * Example class demonstrating the coding style
 */
class ExampleClass {
private:
    int _memberVariable;
    std::string _memberString;

public:
    ExampleClass(int initialValue, const std::string& initialString);
    
    /**
     * Gets the current value
     * @return The current integer value
     */
    int getValue() const;
    
    /**
     * Sets a new value with validation
     * @param newValue The new value to set
     * @return true if successful, false otherwise
     */
    bool setValue(int newValue);
    
    /**
     * Processes data using modern C++ features
     * @param inputData Vector of data to process
     * @return Processed results
     */
    std::vector<int> processData(const std::vector<int>& inputData) const;
};
```

## Guidelines Summary
1. **Always use camelCase** for variables and functions
2. **Always use PascalCase** for classes, structs, and constants
3. **Be consistent** with naming throughout the project
4. **Write clear, descriptive names** that explain purpose
5. **Follow spacing and formatting rules** for readability
6. **Document public interfaces** with clear comments
7. **Use modern C++ features** appropriately
8. **Maintain consistency** with existing codebase patterns
9. **Prefer `constexpr` over `const`** when compile-time evaluation is possible
10. **Use `enum class`** instead of plain enums for type safety
11. **Apply RAII principles** for automatic resource management
12. **Prefer standard library algorithms** over manual loops
13. **Use smart pointers** to manage dynamic memory
14. **Embrace `auto`** for obvious types to improve maintainability

## Performance Considerations
- **Pass large objects by const reference** (`const std::string&`) or `std::string_view`
- **Use move semantics** when transferring ownership (`std::move`)
- **Prefer `emplace_back()` over `push_back()`** for containers
- **Use `reserve()` for vectors** when size is known
- **Avoid unnecessary copies** with perfect forwarding
- **Use `noexcept` specifier** for functions that don't throw

### Examples:
```cpp
// Good: Performance-aware code
void processData(std::string_view data) noexcept {  // string_view + noexcept
    std::vector<int> results;
    results.reserve(data.size());  // Reserve space
    
    for (char c : data) {  // Range-based for
        results.emplace_back(static_cast<int>(c));  // emplace_back
    }
}

// Good: Move semantics
class DataProcessor {
private:
    std::vector<std::string> _data;
public:
    void addData(std::string data) {
        _data.emplace_back(std::move(data));  // Move instead of copy
    }
};
```

These guidelines ensure code maintainability, readability, and consistency across the entire project.