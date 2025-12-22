<h1 align="center" id="title">Arkoi Language</h1>

![State](https://img.shields.io/github/actions/workflow/status/ArkoiSystems/arkoi_language/cmake-single-platform.yml?style=flat&label=Build%2FTest)
![Last Commit](https://img.shields.io/github/last-commit/ArkoiSystems/arkoi_language?style=flat&label=Last%20Commit)
![Contributors](https://img.shields.io/github/contributors/ArkoiSystems/arkoi_language?style=flat&label=Contributors)
![Visitors](https://api.visitorbadge.io/api/visitors?path=https%3A%2F%2Fgithub.com%2FArkoiSystems%2Farkoi_language&label=Repository%20Visits&countColor=%232ccce4&style=flat&labelStyle=none)
![License](https://img.shields.io/github/license/ArkoiSystems/arkoi_language?style=flat&label=License)
![Stars](https://img.shields.io/github/stars/ArkoiSystems/arkoi_language)

<p align="center" id="description">
Arkoi Language is a small toy compiler project designed to provide a hands-on experience in developing a custom programming language and its ecosystem. This project serves as both an educational tool and an exploration of compiler optimizations and language design principles.
</p>

---

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

---

## Introduction
Arkoi Language is a work-in-progress toy compiler written primarily in C++. It is aimed at developers and enthusiasts who are interested in:
- Understanding compiler internals.
- Experimenting with custom language features and optimizations.
- Building a small custom ecosystem around a toy programming language.

While the project is in its early stages, it is designed to be a learning platform for anyone curious about language and compiler design.

---

## Features
- **Custom Syntax**: A unique and intuitive syntax design for the Arkoi Language.
- **Compiler Development**: Learn how a compiler is structured, from parsing to code generation.
- **Optimization Techniques**: Explore basic optimization strategies for compiled code.
- **Assembly Integration**: Includes low-level Assembly code for understanding machine-level interactions.

---

## Installation
To build and run the Arkoi compiler, follow the steps below:

### Prerequisites
- A C++23 compatible compiler (e.g., GCC, Clang, or MSVC).
- CMake 3.29.6 or higher.
- A supported build system (e.g., make, Ninja).

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/Excse/arkoi_language.git
   cd arkoi_language
   ```
2. Create a build directory and run CMake:
   ```bash
   cmake -S . -B build
   ```
3. Build the project:
   ```bash
   cmake --build build
   ```

4. Run the compiler:
   ```bash
   ./arkoi_language <input_file>
   ```

5. Run the tests:
   ```bash
   ctest --test-dir build --output-on-failure
   ```

---

## Usage
Once the Arkoi compiler is built, you can use it to compile Arkoi source files into executable binaries or intermediate representations.

### CLI Options
```bash
Usage: arkoi_language [--help] [--version] [--output VAR] [-v] [-S] [-c] [-r] [-print-asm] [-print-cfg] [-print-il] inputs...

The Arkoi Compiler is a lightweight experimental compiler for the Arkoi
Programming Language, designed to explore a mix of Python and C programming
principles. It is primarily a learning and experimentation tool for testing
new language features, compiler techniques, and language design concepts.

Positional arguments:
  inputs        All input files that should be compiled
                [nargs: 1 or more] 

Optional arguments:
  -h, --help    Shows the help message and exits 
  --version     Prints version information and exits 

Input and output control (detailed usage):
  -o, --output  The output file name of the compiled files
                [nargs=0..1] [default: "a.out"]
  -v            Print (on the standard error output) the commands executed to run the stages of compilation 

Compilation modes (detailed usage):
  -S            Only compile but do not assemble.
                For each source an assembly file ".s" is generated 
  -c            Only compile and assemble, but do not link.
                For each source an object file ".o" is generated 
  -r            Compile, assemble, link and run the program afterwards 

Output control of compilation stages (detailed usage):
  -print-asm    Print the assembly code of each source to a file ending in ".s" 
  -print-cfg    Print the Control-Flow-Graph of each source to a file ending in ".dot" 
  -print-il     Print the Intermediate Language of each source to a file ending in ".il" 
```

---

## Project Structure
Here is an overview of the repository structure:

```
arkoi_language/
├── CMakeLists.txt      # CMake configuration for the project
├── src/                # The source directory of the repository
│   ├── main.cpp        # Main entry of the compiler
│   └── ...             # Other source files (same structure as the include directory)
├── include/            # Same structure as the source directory
│   ├── ast/            # Abstract Syntax Tree (nodes, visitor)
│   ├── front/          # Frontend (parser, scanner, tokens)
│   ├── sem/            # Semantic Analysis (name and type resolution)
│   ├── il/             # Intermediate Language (dataflow, control flow graph, generator, printer, instructions, operands, visitor)
│   ├── opt/            # Optimization Passes
│   ├── x86_64/         # x86_64 Code Generation (generator, mapper, operands)
│   └── utils/          # Some useful utility functions
│── test/               # Unit tests for specific code parts
│   ├── e2e/            # End to end tests (lexer, parser, semantic analysis, optimization, code generation)
│   ├── il/             # Intermediate Language tests (dataflow, control flow graph, generator)
│   ├── utils/          # Some utility functions that are tested
│   ├── snapshot/       # A suit for snapshot testing (lexer, parser, etc.)
│   └── CMakeLists.txt  # CMake configuration for the tests
└── example/            # Some examples to showcase the Arkoi Language
    ├── hello_world/    # The main hello world program
    ├── test/           # An example that demonstrates every Arkoi feature
    └── ...             # Some other examples (test for calling convention etc.)
```

---

## Roadmap
There is a separate [ROADMAP.md]() file that lists the planned features and improvements for the compiler.

---

## Contributing
We welcome contributions to improve the Arkoi Language project! Here’s how you can get involved:

1. Clone the repository and create a new branch for your feature or bugfix.
2. Write clear and concise commit messages explaining your changes.
3. Submit a pull request to the `main` branch.

Please make sure to follow the [contribution guidelines](CODE_OF_CONDUCT.md).

---

## Credits
This repository uses the following open-source projects:
- [Google Test](https://github.com/google/googletest)
- [argparse](https://github.com/p-ranav/argparse/)

---

## License
This project is licensed under the BSD 3-Clause License. See the [LICENSE](LICENSE.txt) file for details.

---

<p align="center">
Happy coding with Arkoi Language! 🚀
</p>
