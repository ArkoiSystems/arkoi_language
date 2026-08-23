---
title: Arkoi programming language
description: Learn Arkoi, try the current compiler, or use the complete Arkoi 1.0 target-language reference.
---

# Arkoi

Arkoi is a statically typed systems language with explicit ownership, recoverable failures, deterministic resource cleanup, static interfaces, and direct C interoperability. The compiler is experimental; the 1.0 reference describes the language it is growing toward.

<p class="arkoi-status-row">
  <span class="arkoi-status arkoi-status--current">Compiler 0.1.0</span>
  <span class="arkoi-status arkoi-status--target">Target language 1.0</span>
</p>

<div class="grid cards" markdown>

-   **Try the current compiler**

    ---

    Build the compiler, run a small program, and inspect the generated pipeline artifacts.

    [Start the quickstart →](getting-started/quickstart.md)

-   **Learn Arkoi 1.0**

    ---

    Follow a ten-chapter course from your first module through ownership,
    failures, interfaces, systems boundaries, and a capstone program.

    [Begin the language tour →](learn/index.md)

-   **Use the language reference**

    ---

    Look up exact syntax and semantics, ownership rules, interfaces, modules, and C interoperability.

    [Open the Arkoi 1.0 reference →](language-specification/1.0.0/index.md)

</div>

## Arkoi 1.0 at a glance

This target-language illustration shows typed values, a recoverable failure effect,
borrowing, and an explicit ownership transfer:

```arkoi
fun load(path @string_view) !IOFail @Buffer:
    file @File = File.open(path)!
    inspect(&file)
    return File.into_buffer(move(file))!
```

!!! warning "Target-language example"

    The current compiler implements a smaller and sometimes syntactically different
    subset. Use the [compatibility guide](getting-started/compatibility.md) before
    trying 1.0 examples locally.

## Documentation map

| Area | Use it for |
| --- | --- |
| [Quickstart](getting-started/quickstart.md) | Invoke compiler 0.1.0 and run one small verified example |
| [Learn Arkoi 1.0](learn/index.md) | Progressive explanations and small target-language illustrations |
| [Arkoi 1.0 reference](language-specification/1.0.0/index.md) | Language syntax, semantics, ownership, interfaces, modules, and C interoperability |
| [Current compiler](getting-started/quickstart.md) | Invoke compiler 0.1.0 and run one small verified example |
| [Compiler development](development/index.md) | Detailed CLI and pipeline behavior, example fixtures, architecture, and generated C++ API documentation |

!!! info "Target versus implementation"

    The 1.0 documentation defines the target language. A documented 1.0 feature may not yet be accepted by the current compiler; implementation pages call out the supported subset separately.
