---
title: Arkoi 1.0 language reference
description: The normative target-language reference for Arkoi 1.0 syntax, semantics, ownership, interfaces, modules, and C interoperability.
---

# Arkoi 1.0.0 Language Specification

<div class="spec-meta" markdown>

| Property | Value |
| --- | --- |
| Language target | **1.0.0** |
| Status | Target specification |
| Source revision | Arkoi Language Specification **v150** |
| Documentation model | One canonical page per rule, with cross-links for related behavior |

</div>

!!! info "Target language"

    These pages define the Arkoi 1.0.0 target. They do not imply that every feature is available in the [current compiler](../../getting-started/overview.md).

<div class="grid cards" markdown>

-   **Language foundations**

    ---

    Source syntax, declarations, constants, values, functions, calls, and aggregates.

    [Open the foundations guide →](foundations/index.md)

-   **Ownership and safety**

    ---

    Moves, cloning, resource lifetime, references, pointers, arrays, slices, failures, and unsafe execution.

    [Open the ownership and safety guide →](ownership-and-safety/index.md)

-   **Expressions and control**

    ---

    Numeric behavior, pipelines, comparisons, evaluation order, control flow,
    iteration, indexing, slicing, and length operations.

    [Open the expressions and control guide →](expressions-and-control/index.md)

-   **Static interfaces**

    ---

    Compile-time contracts, requirements, associated types, extension,
    implementations, and visibility.

    [Open the static interfaces guide →](static-interfaces/index.md)

-   **Modules and imports**

    ---

    Module identity, file-to-module correspondence, imports, aliases, and
    visibility across module boundaries.

    [Read about modules and imports →](modules-and-imports.md)

-   **C interoperability**

    ---

    C ABI declarations, compatible types, globals, callbacks, ownership,
    layout, arrays, unions, and symbol rules.

    [Open the C interoperability guide →](c-interoperability.md)

</div>

## Reference

- [Quick reference](quick-reference.md) — compact syntax and notation tables with links to the complete rules.
- [Glossary](glossary.md) — canonical terminology used throughout the specification.
- [Specification map](specification-map.md) — every original numbered section mapped to its reader-focused page.
- [Representative program](representative-program.md) — one illustrative program with its ownership flow explained.

## Reading the specification

Normative rules use **must**, **must not**, **may**, and **compile-time error** consistently. Runtime problems are classified as a recoverable [failure](glossary.md#failure), an unrecoverable [trap](glossary.md#trap), or [undefined behavior](glossary.md#undefined-behavior) assigned to unsafe or foreign code.

Examples labeled as compile-time errors are intentionally invalid. Other Arkoi examples follow the syntax and semantic rules defined by these pages.
