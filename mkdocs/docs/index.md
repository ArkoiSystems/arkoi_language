# Arkoi

Arkoi is a statically typed systems language with explicit ownership, recoverable failures, deterministic resource cleanup, static interfaces, and direct C interoperability.

<div class="grid cards" markdown>

-   **Arkoi 1.0.0 language**

    ---

    Read the complete target-language documentation, organized by concept with compact rule tables and linked examples.

    [Open the 1.0.0 specification →](language-specification/1.0.0/index.md)

-   **Current compiler**

    ---

    Build and use the compiler that exists today. Its implemented language subset is smaller than the Arkoi 1.0.0 target.

    [Use the current compiler →](getting-started/overview.md)

</div>

## Documentation map

| Area | Use it for |
| --- | --- |
| [Arkoi 1.0.0 specification](language-specification/1.0.0/index.md) | Language syntax, semantics, ownership, interfaces, modules, and C interoperability |
| [Current compiler](getting-started/overview.md) | Implemented features, installation, CLI commands, and runnable examples |
| [Compiler API](arkoi_language/classes.md) | Generated C++ API documentation for compiler contributors |

!!! info "Target versus implementation"

    The 1.0 documentation defines the target language. A documented 1.0 feature may not yet be accepted by the current compiler; implementation pages call out the supported subset separately.
