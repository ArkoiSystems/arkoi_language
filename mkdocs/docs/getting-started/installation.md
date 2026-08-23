---
title: Install the current Arkoi compiler
description: Set up the supported Linux toolchain, build dependencies, compile Arkoi, and run its tests.
---

# Install and build the current compiler

This is the local equivalent of the repository's Linux CI build. It builds
`pretty_diagnostics` first, then configures Arkoi to find that installation.

## Supported build environment

| Requirement | Current expectation |
| --- | --- |
| Operating system and target | Ubuntu 24.04 on x86-64 |
| CMake | 4.2 or newer |
| C++ compiler | GCC 14 with C++23 support |
| Build backend | GNU Make |
| Native tools | GNU `as` and `ld`, supplied by `binutils` |
| Source control | Git |
| Required library | `pretty_diagnostics`, built and installed below |
| Optional visualization | Graphviz, for rendering emitted `.dot` files |

The compiler emits Linux x86-64 assembly and ELF entry-point code directly.
Building or running generated programs on macOS, Windows, ARM, or another ABI is
not currently supported.

On Ubuntu, install Git, GCC, GNU Make, and binutils with:

```bash
sudo apt-get update
sudo apt-get install --yes git gcc-14 g++-14 make binutils
```

Install CMake 4.2 or newer using a package source or official distribution that
provides that version. Graphviz is optional:

```bash
sudo apt-get install --yes graphviz
```

Check the tools before continuing:

```bash
cmake --version
g++-14 --version
make --version
as --version
ld --version
```

## Build from a fresh workspace

Run these commands from an empty working directory. They create
`pretty_diagnostics` and `arkoi_language` as sibling directories, matching the
dependency order used in CI.

### 1. Build and install `pretty_diagnostics`

```bash
git clone https://github.com/Excse/pretty_diagnostics.git
cmake -S pretty_diagnostics -B pretty_diagnostics/build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=gcc-14 \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DBUILD_TESTING=OFF \
  -DMAKE_INSTALLABLE=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_INSTALL_PREFIX="$PWD/pretty_diagnostics/install"
cmake --build pretty_diagnostics/build --config Release
cmake --install pretty_diagnostics/build --config Release
```

### 2. Configure and build Arkoi

```bash
git clone https://github.com/ArkoiSystems/arkoi_language.git
cmake -S arkoi_language -B arkoi_language/build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=gcc-14 \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DBUILD_TESTING=ON \
  -DBUILD_EXECUTABLE=ON \
  -DMAKE_INSTALLABLE=ON \
  -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_INSTALL_PREFIX="$PWD/arkoi_language/install" \
  -DCMAKE_PREFIX_PATH="$PWD/pretty_diagnostics/install"
cmake --build arkoi_language/build --config Release
```

With tests enabled, CMake also downloads GoogleTest while configuring. Set
`-DBUILD_TESTING=OFF` when you only need the compiler and want to skip that
test dependency.

The compiler executable is now
`arkoi_language/build/arkoi_language_app`.

## Run tests and verify the build

```bash
ctest --test-dir arkoi_language/build -C Release --output-on-failure
cd arkoi_language
./build/arkoi_language_app --version
./build/arkoi_language_app \
  example/hello_world/hello_world.ark \
  --run --emit-asm
```

The final command should end with:

```text
Executed with exit code: 0
```

## Configure an existing checkout

If this repository is already checked out, build `pretty_diagnostics` anywhere
you choose and pass its absolute install directory to CMake:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=gcc-14 \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DBUILD_TESTING=ON \
  -DCMAKE_PREFIX_PATH=/absolute/path/to/pretty_diagnostics/install
cmake --build build --config Release
```

If CMake reports that it cannot find `pretty_diagnostics`, confirm that the
prefix points to the install directory—not its source or build directory—and
then reconfigure.

## Next

Use the [quickstart](quickstart.md) for a first run, consult the
[CLI reference](cli.md) for output controls, or follow the
[compiler pipeline](pipeline.md).
