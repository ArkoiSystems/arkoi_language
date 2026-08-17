---
title: Arkoi compiler development
description: Contributor orientation for the current compiler architecture, tests, stage boundaries, and generated C++ API.
---

# Compiler development

This page orients contributors to the current C++ compiler and its generated API
reference. It is separate from the Arkoi 1.0 target-language specification.

!!! info "Current implementation"

    Compiler headers and generated API pages describe the code that exists in
    this repository. Language-design work should also check the
    [Arkoi 1.0 target specification](../language-specification/1.0.0/index.md),
    because the current compiler does not implement every target rule.

## Choose a starting point

| When you want to… | Start with… |
| --- | --- |
| Configure the project, build it, and run its tests | [Installation](../getting-started/installation.md) |
| Exercise compiler modes and emitted artifacts | [CLI usage](../getting-started/cli.md) |
| Find a C++ type | [Generated class index](../arkoi_language/classes.md) |
| Browse a compiler subsystem | [Generated namespace index](../arkoi_language/namespaces.md) |
| Locate declarations by header | [Generated file index](../arkoi_language/files.md) |

## Compiler areas

The generated API is built from the public headers under
`include/arkoi_language`. Their namespaces provide a practical map:

| Namespace | Header area |
| --- | --- |
| `arkoi::front` | Tokens, scanning, and parsing |
| `arkoi::ast` | Syntax-tree nodes, visitors, and printing |
| `arkoi::sem` | Symbols, types, name resolution, and type resolution |
| `arkoi::il` | Intermediate-language operands and instructions, generation, control-flow graphs, analyses, SSA, visitors, and printing |
| `arkoi::opt` | Optimization passes, including propagation, folding, dead-code elimination, and CFG simplification |
| `arkoi::x86_64` | x86-64 operands, assembly, generation, resolution, and allocation |
| `arkoi::utils` | Diagnostics, driver operations, and shared utilities |

The command-line driver coordinates compilation, optional IL and CFG output,
x86-64 assembly generation, assembly, linking, and optional execution. Use the
[CLI page](../getting-started/cli.md) for the modes currently exposed to users.

## Contributor workflow

1. Build the project and establish a passing test baseline with the commands on
   [Installation](../getting-started/installation.md).
2. Find the relevant namespace or header in the generated API before changing a
   stage boundary.
3. Put coverage at the matching level: snapshot tests exercise scanner and
   parser output, IL tests cover control-flow and intermediate representation,
   utility tests cover shared components, and end-to-end tests exercise the
   compiler pipeline.
4. Run the test suite again and inspect any emitted artifact relevant to the
   change.
5. If behavior or a user-facing command changes, update the current-compiler
   guide. If the language contract changes, update the canonical Arkoi 1.0
   reference page as a separate, explicit decision.

## API shortcuts

- [Classes and structs](../arkoi_language/classes.md)
- [Namespaces](../arkoi_language/namespaces.md)
- [Header files](../arkoi_language/files.md)
