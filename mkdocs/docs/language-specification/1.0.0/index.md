# Arkoi 1.0.0 Language Specification

<div class="spec-meta" markdown>

| | |
| --- | --- |
| Language target | **1.0.0** |
| Status | Target specification |
| Source | Arkoi Language Specification v150, language version 1 |
| Documentation model | One canonical page per rule, with cross-links for related behavior |

</div>

!!! info "Target language"

    These pages define the Arkoi 1.0.0 target. They do not imply that every feature is available in the [current compiler](../../getting-started/overview.md).

<div class="grid cards" markdown>

-   **Language foundations**

    ---

    Source syntax, declarations, constants, values, functions, calls, and aggregates.

    [Start with source syntax →](source-syntax.md)

-   **Ownership and safety**

    ---

    Moves, cloning, resource lifetime, references, pointers, arrays, slices, failures, and unsafe execution.

    [Learn the ownership model →](ownership-moves.md)

-   **Expressions and protocols**

    ---

    Numeric behavior, pipelines, comparisons, evaluation order, control flow,
    iteration, indexing, and static interfaces.

    [Explore expressions →](expressions-and-numeric-operations.md)

-   **Modules and C**

    ---

    Module identity, imports, C ABI declarations, compatible types, callbacks, layout, and symbol rules.

    [Read about modules →](modules-and-imports.md)

</div>

## Reference

- [Quick reference](quick-reference.md) — compact syntax and notation tables with links to the complete rules.
- [Glossary](glossary.md) — canonical terminology used throughout the specification.
- [Specification map](specification-map.md) — every original numbered section mapped to its reader-focused page.
- [Representative program](representative-program.md) — one illustrative program with its ownership flow explained.

## Reading the specification

Normative rules use **must**, **must not**, **may**, and **compile-time error** consistently. Runtime problems are classified as a recoverable [failure](glossary.md#failure), an unrecoverable [trap](glossary.md#trap), or [undefined behavior](glossary.md#undefined-behavior) assigned to unsafe or foreign code.

Examples labeled as compile-time errors are intentionally invalid. Other Arkoi examples follow the syntax and semantic rules defined by these pages.
