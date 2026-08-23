---
title: Learn Arkoi 1.0
description: A progressive ten-chapter course covering Arkoi 1.0 syntax, types, ownership, failures, calls, collections, interfaces, systems boundaries, and a capstone program.
---

# Learn Arkoi 1.0

This course develops the Arkoi 1.0 programming model from a complete source
file to a small integrated program. Ten focused chapters connect the major
ideas, show the choices they imply, and point to the canonical rules when you
need exact edge cases.

!!! info "Target-language course"

    Every Arkoi snippet in this section illustrates the **Arkoi 1.0 target
    language**. The current compiler does not necessarily accept it. Check
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md), or use
    the [current-compiler examples](../getting-started/examples.md) when you want
    code accepted by compiler 0.1.0.

## Start here

Read the chapters in order if Arkoi is new to you. If you already know systems
languages, use the map to enter at the concept you need—but read the ownership
and failure chapters before relying on later examples.

### Foundations

<div class="grid cards" markdown>

-   **1. Your first Arkoi source file**

    ---

    Build a complete module, then learn typed bindings, return contracts,
    strict Boolean conditions, branches, and loops.

    [Write the first module →](basics.md)

-   **2. Types, text, and aggregates**

    ---

    Choose exact primitive types, owned or borrowed text, optionals, data and
    resource aggregates, enums, aliases, and constants.

    [Model the data →](types-aggregates.md)

-   **3. Expressions and conversions**

    ---

    Make conversions explicit, choose arithmetic behavior, reason about
    evaluation order, use produced results, and build pipelines.

    [Evaluate expressions deliberately →](expressions.md)

</div>

### Everyday Arkoi

<div class="grid cards" markdown>

-   **4. Data, resources, and ownership**

    ---

    Separate copied data from owned resources, then use moves, clones,
    references, slices, and deterministic cleanup.

    [Build the ownership model →](ownership.md)

-   **5. Optionals and recoverable failures**

    ---

    Represent ordinary absence, declare failure effects, propagate them
    explicitly, and recover with an expression-level handler.

    [Follow every outcome →](failures.md)

-   **6. Functions, calls, and methods**

    ---

    Match exact argument types, use named arguments and overloads, pass
    receivers explicitly, control visibility, and preview function pointers.

    [Design a callable API →](calls-methods.md)

-   **7. Collections and iteration**

    ---

    Work with fixed arrays and slice views, handle bounds checks, traverse with
    `for`, and understand the custom iteration protocols.

    [Traverse values safely →](collections-iteration.md)

</div>

### Architecture and systems boundaries

<div class="grid cards" markdown>

-   **8. Interfaces and modules**

    ---

    Organize source files into qualified modules and express nominal,
    compile-time contracts with static interfaces and associated types.

    [Connect modules and contracts →](interfaces-modules.md)

-   **9. Unsafe code, hooks, and C interoperability**

    ---

    Isolate unverifiable operations, recognize reserved compiler contracts,
    and wrap foreign calls behind ordinary Arkoi types.

    [Cross the systems boundary →](systems-interop.md)

-   **10. Capstone program**

    ---

    Read one complete module that combines types, ownership, collections,
    failures, methods, and a static interface, then extend it yourself.

    [Put the model together →](capstone.md)

</div>

## How each chapter works

Each chapter states learning goals, identifies prerequisites where needed,
develops focused examples, calls out invalid or risky forms, and ends with a
short checkpoint and links to the reference. Complete modules and intentional
errors are labeled; shorter unlabeled snippets are focused fragments.

The course teaches the working model, not every specification edge case. Keep
the [quick reference](../language-specification/1.0.0/quick-reference.md) nearby,
and use the complete [Arkoi 1.0 specification](../language-specification/1.0.0/index.md)
for normative details such as overload identity, cleanup order, interface
visibility, iterator lifetimes, and ABI restrictions.

## Prefer current, target, or reference documentation?

| Your goal | Best starting point |
| --- | --- |
| Build something with compiler 0.1.0 | [Current compiler overview](../getting-started/overview.md) |
| Learn the intended Arkoi 1.0 language progressively | [Chapter 1: Your first Arkoi source file](basics.md) |
| Look up exact Arkoi 1.0 behavior | [Quick reference](../language-specification/1.0.0/quick-reference.md) |
| Understand what differs in compiler 0.1.0 | [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) |
