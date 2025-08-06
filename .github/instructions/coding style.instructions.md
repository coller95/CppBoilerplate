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
- Examples:
  ```cpp
  const int MaxBufferSize = 1024;
  const std::string DefaultFileName = "output.txt";
  
  enum Status {
      Success,
      Failure,
      Pending
  };
  
  enum class FileType {
      Text,
      Binary,
      Compressed
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

## Error Handling
- Prefer exceptions over error codes where appropriate
- Use descriptive exception messages
- Follow RAII principles

## Modern C++ Features
- Use `auto` when type is obvious
- Prefer smart pointers over raw pointers
- Use range-based for loops when appropriate
- Use `const` and `constexpr` where applicable

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

These guidelines ensure code maintainability, readability, and consistency across the entire project.