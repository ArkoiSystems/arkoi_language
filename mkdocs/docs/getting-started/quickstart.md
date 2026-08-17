---
title: Current compiler quickstart
description: Build the experimental Arkoi compiler and run a first program on Linux x86-64.
---

# Five-minute current compiler quickstart

This path builds the current compiler and runs a real Arkoi program. It assumes
Linux x86-64, CMake 4.2+, GCC 14, Git, and GNU binutils are installed; see
[installation](installation.md) for platform details and package setup.

Run the following from an empty working directory:

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

git clone https://github.com/ArkoiSystems/arkoi_language.git
cmake -S arkoi_language -B arkoi_language/build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=gcc-14 \
  -DCMAKE_CXX_COMPILER=g++-14 \
  -DBUILD_TESTING=OFF \
  -DCMAKE_PREFIX_PATH="$PWD/pretty_diagnostics/install"
cmake --build arkoi_language/build --config Release

cd arkoi_language
./build/arkoi_language_app \
  example/hello_world/hello_world.ark \
  --run --emit-asm
```

After the build messages, the compiler prints:

```text
Executed with exit code: 0
```

`--emit-asm` is currently required to refresh the assembly before the assemble
and link stages. The driver leaves the generated `.s` and `.o` files beside the
example; the [CLI reference](cli.md) records this and other current limitations.

The example is deliberately minimal:

```arkoi
--8<-- "example/hello_world/hello_world.ark"
```

`main` returns `0`, which becomes the process exit status. The program does not
print “Hello, world!” because the current subset has no text-output facility.

## Try one change

Change `return 0` to `return 7`, then run the same compiler command. It should
report `Executed with exit code: 7`. A nonzero Arkoi program result is also the
compiler command's exit status, so shells and CI correctly treat it as nonzero.

Next, explore the [complete examples](examples.md), learn what every
[CLI mode](cli.md) keeps, or inspect the [compiler pipeline](pipeline.md).
