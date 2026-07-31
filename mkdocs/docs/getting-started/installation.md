# Installation

This page covers the minimum needed to build Arkoi locally and run the compiler against an example program.

## Requirements

- Git
- CMake
- A C++23 compiler

## Quick path

If you have the prerequisites, the shortest path is:

```bash
git clone https://github.com/ArkoiSystems/arkoi_language
cd arkoi_language
cmake -S . -B build
cmake --build build
./build/bin/arkoi_language example/hello_world/hello_world.ark
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

Read the [current compiler overview](overview.md), learn the [CLI](cli.md), or browse the [repository examples](examples.md). For the target language design, use the [Arkoi 1.0.0 specification](../language-specification/1.0.0/index.md).
