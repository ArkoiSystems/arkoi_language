# Installation

This page covers the minimum needed to build Arkoi locally.

## Requirements

- Git
- CMake
- A C++23 compiler

## Build

```bash
git clone https://github.com/ArkoiSystems/arkoi_language
cd arkoi_language
cmake -S . -B build
cmake --build build
```

## Run the compiler

The build produces an `arkoi_language` executable. Run it against one or more `.ark` files:

```bash
./build/bin/arkoi_language example/hello_world/hello_world.ark
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Next

Read the [syntax overview](syntax.md) to see the current language shape, or go to [CLI usage](cli.md) for command examples.
