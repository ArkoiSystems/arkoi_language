---
title: Current compiler overview
description: Understand the subset, platform, and learning paths available in the Arkoi compiler today.
---

# Current compiler overview

The repository contains an experimental Arkoi compiler and a separate description
of the intended Arkoi 1.0 language. This section documents the compiler you can
build and run today.

!!! warning "Current implementation, not the complete 1.0 language"

    Runnable examples in this section are accepted by the current compiler.
    The [compatibility guide](compatibility.md) deliberately includes labeled
    target-language examples, and the
    [Arkoi 1.0 specification](../language-specification/1.0.0/index.md) may use
    syntax that does not compile yet.

## What you can do today

The current compiler supports:

- top-level, fixed-arity `fun` definitions;
- explicitly typed parameters, local variables, and return values;
- `u8`–`u64`, `s8`–`s64`, `usize`, `ssize`, `f32`, `f64`, and `bool`;
- arithmetic, comparisons, `&&`, `||`, calls, and grouping;
- `if`, `else if`, `else`, `while`, assignment, and `return`;
- IL, control-flow graph, and x86-64 assembly output; and
- assembly, native linking, and execution on Linux x86-64.

Here is the smallest repository example:

```arkoi
--8<-- "example/hello_world/hello_world.ark"
```

Despite its directory name, this program does not print text. Its return value
becomes the native process exit status.

## Choose a path

| Goal | Start here |
| --- | --- |
| Build and run one program | [Five-minute quickstart](quickstart.md) |
| Set up every dependency or run the tests | [Installation and build](installation.md) |
| Choose stages and output files | [CLI reference](cli.md) |
| Learn from complete programs | [Current compiler examples](examples.md) |
| Follow source through IL, CFG, and assembly | [Compiler pipeline tutorial](pipeline.md) |
| Translate between current and 1.0 syntax | [Current compiler vs. Arkoi 1.0](compatibility.md) |

## Scope and stability

The executable currently produces Linux x86-64 ELF programs and invokes GNU
`as` and `ld`. The compiler and its accepted subset are evolving, so the
checked-in examples and tests are the best executable record of current support.

For language design work, use the [1.0 specification
overview](../language-specification/1.0.0/index.md) and its
[quick reference](../language-specification/1.0.0/quick-reference.md).
