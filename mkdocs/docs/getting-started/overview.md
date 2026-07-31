# Current Compiler

The current compiler implements an early Arkoi subset. Use this section when building or running the repository today; use the [Arkoi 1.0.0 specification](../language-specification/1.0.0/index.md) when designing against the target language.

!!! warning "Not the complete 1.0.0 language"

    Modules, interfaces, resources, ownership operations, failure effects, and C interoperability belong to the 1.0.0 target but are not all implemented by the current compiler.

## Implemented language subset

| Area | Current support |
| --- | --- |
| Programs | Top-level `fun` definitions and a `main` entry point |
| Values | Explicitly typed variables, parameters, and return values |
| Primitive types | Signed and unsigned integers, floating-point values, and `bool` |
| Expressions | Arithmetic, comparisons, boolean operations, calls, and grouping |
| Control flow | `if`, `else if`, `else`, `while`, and `return` |
| Toolchain | Parsing, intermediate output, control-flow graphs, assembly, and native compilation |

The examples and tests in the repository are the authority for this implemented subset. Syntax in the target specification may be accepted only as the compiler evolves toward 1.0.

## Start here

1. [Install and build the compiler](installation.md).
2. [Learn the CLI](cli.md).
3. [Run the repository examples](examples.md).

## Language design

For the complete language, begin with the [1.0.0 specification overview](../language-specification/1.0.0/index.md) or use its [quick reference](../language-specification/1.0.0/quick-reference.md).
