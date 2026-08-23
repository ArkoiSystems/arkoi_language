---
title: Current compiler quickstart
description: Invoke experimental Arkoi compiler 0.1.0 and run one small checked example.
---

# Run compiler 0.1.0

This page intentionally covers only the current invocation and one small
example. It assumes the repository has already been built; contributors who
need the supported toolchain and build procedure can use the detailed
[installation guide](installation.md).

From the repository root, run:

```bash
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

Detailed modes, retained artifacts, and pipeline behavior belong to the
[compiler-development CLI reference](cli.md).
