# Copilot Instructions for Arkoi Language

## Repository Overview

**Arkoi Language** is a C++23 toy compiler project designed for learning compiler development and language design. The project implements a complete compiler pipeline from lexical analysis to x86_64 code generation.

- **Repository Size**: 2.2MB, 65 source files, ~3,275 lines of code
- **Language**: C++23 with CMake build system
- **Target Platform**: x86_64 architecture
- **Dependencies**: Google Test (via FetchContent), argparse library
- **License**: BSD 3-Clause

## Critical Build Information

### Build Process (Always follows this sequence)
```bash
# Prerequisites: GCC 13.3.0+ or equivalent C++23 compiler, CMake 3.29.6+
mkdir build && cd build
cmake ..
cmake --build .
```

### Testing
```bash
# Run tests (from build directory)
./arkoi_language_tests
# All 5 tests should pass: Snapshot.Scanner, InterferenceGraphTest (3 tests), ControlFlowGraph.IteratorRightOrder
```

### Running the Compiler
```bash
# Basic usage (from build directory)
./arkoi_language ../example/hello_world/main.ark

# With output options
./arkoi_language ../example/hello_world/main.ark --output-il --output-asm --output-cfg
```

**Build Time**: ~1-2 minutes including dependency fetch on first build

## Project Architecture

### Core Structure
```
arkoi_language/
├── src/main.cpp              # Main compiler entry point with argparse CLI
├── include/                  # Header files mirroring src structure
│   ├── ast/                  # Abstract Syntax Tree (nodes.hpp, visitor.hpp)
│   ├── front/                # Frontend: scanner.hpp, parser.hpp, token.hpp
│   ├── sem/                  # Semantic analysis: name_resolver, type_resolver, symbol_table
│   ├── il/                   # Intermediate Language: CFG, dataflow, generator, printer
│   ├── opt/                  # Optimization passes: constant folding/propagation, DCE, CFG simplification
│   ├── x86_64/               # x86_64 code generation: generator, register allocation, assembly
│   └── utils/                # Utilities: interference graph, size definitions
├── test/                     # Unit tests and snapshot testing
├── example/                  # Sample .ark programs (hello_world, test)
└── CMakeLists.txt           # Build configuration with Google Test integration
```

### Compiler Pipeline
1. **Lexical Analysis** (`front/scanner.hpp`) - Tokenizes .ark source files
2. **Parsing** (`front/parser.hpp`) - Builds AST from tokens
3. **Semantic Analysis** (`sem/`) - Name resolution and type checking
4. **IL Generation** (`il/generator.hpp`) - Creates intermediate representation with CFG
5. **Optimization** (`opt/`) - Runs optimization passes on IL
6. **Code Generation** (`x86_64/generator.hpp`) - Generates x86_64 assembly

### Key File Locations
- **Main compiler**: `src/main.cpp`
- **AST definitions**: `include/ast/nodes.hpp`
- **Parser implementation**: `src/front/parser.cpp`
- **Type system**: `include/sem/type.hpp`, `src/sem/type.cpp`
- **IL instructions**: `include/il/instruction.hpp`
- **Optimization passes**: `include/opt/` directory
- **Tests**: `test/test_*.cpp`, `test/snapshot/`

## Development Workflow

### Validation Commands
```bash
# Clean build test
rm -rf build && mkdir build && cd build
cmake .. && cmake --build .

# Verify tests pass
./arkoi_language_tests

# Test compiler functionality
./arkoi_language ../example/hello_world/main.ark
./arkoi_language ../example/test/test.ark --output-il --output-asm
```

### Common Issues and Solutions
1. **Build failures**: Ensure all required C++ includes are present in source files
2. **Missing executables**: Ensure you're in the build directory
3. **Test naming**: Test executable is `arkoi_language_tests`, not `test_arkoi_language`
4. **Memory errors**: Project uses AddressSanitizer and UBSan flags by default
5. **Clean builds**: Remove `build/` directory completely before rebuilding if issues persist

### Optional Tools
- **Graphviz**: Install `dot` command for CFG visualization (`--output-cfg` option)
- **Clang tools**: `clang-format` and `clang-tidy` available but no project configuration
- **Assembly**: Generated assembly can be assembled with standard tools (gas)

### Code Style
- **C++ Standard**: C++23 (required)
- **Compiler Flags**: `-Wall -Wextra -Werror -Wpedantic -Wswitch -fsanitize=address,undefined,leak`
- **No linting configuration**: No clang-format or clang-tidy config present
- **Headers**: Use `#pragma once` guards
- **Namespaces**: Code organized under `arkoi` namespace with subnamespaces

### Dependencies
- **External**: Google Test (v1.15.2) via FetchContent, argparse via include
- **No CI/CD**: No GitHub workflows configured
- **No package management**: Dependencies managed through CMake FetchContent

## Testing Information
- **Framework**: Google Test with GTest and GMock
- **Types**: Unit tests and snapshot testing for lexer/parser
- **Test Files**: 
  - `test/test_interference.cpp` - Graph algorithm tests
  - `test/test_cfg.cpp` - Control flow graph tests  
  - `test/snapshot/` - Lexer/parser snapshot tests
- **Coverage**: Limited to core data structures and parsing

## File Extensions and Formats
- **Source files**: `.ark` (Arkoi language source)
- **Output formats**: `.il` (intermediate language), `.asm` (assembly), `.dot` (CFG visualization)
- **Build artifacts**: Exclude `build/` directory, `cmake-build-*/` patterns

**Trust these instructions**: Only search for additional information if these instructions are incomplete or found to be incorrect. The build sequence has been validated and should work for successful compilation.