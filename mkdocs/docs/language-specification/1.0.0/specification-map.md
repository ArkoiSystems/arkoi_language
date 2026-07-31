# Specification map

The v150 source specification is organized as 16 long chapters. This documentation groups the same rules by reader task and gives each repeated rule one canonical home.

!!! note "How to use this map"

    Section ranges refer to the headings in `Arkoi_Language_Specification_v150.md`. A section appears in more than one row only when its original text covered several independent concepts; each concept is still defined canonically on one page.

    Where repeated source examples conflict with a later explicit rule, the
    canonical page states one normalized 1.0.0 rule and links related
    consequences back to it.

## Language foundations

| Source sections | Canonical documentation |
| --- | --- |
| 1.1–1.5 | [Source syntax](source-syntax.md) |
| 2.1 | [Bindings and initialization](bindings-initialization.md) |
| 2.2–2.6 | [Constants and globals](constants-globals.md) |
| 3.1–3.6, 3.8 | [Types and values](types-values.md) |
| 3.7, 3.9–3.10; 5.2 | [Aggregates and enums](aggregates-enums.md) |
| 4.1–4.3, 4.8–4.10 | [Functions and returns](functions-returns.md) |
| 4.4–4.6 | [Calls and overloads](calls-overloads.md) |
| 4.7 | [Function pointers](function-pointers.md) |
| 7.1–7.2 | [Methods and visibility](methods-visibility.md) |
| 7.3 | [Compiler hooks](compiler-hooks.md) |

## Ownership and memory

| Source sections | Canonical documentation |
| --- | --- |
| 5.1 — moves, consuming parameters, temporaries, and returns | [Moving resources](ownership-moves.md) |
| 5.1 — clone hooks, sources, and composite cloning | [Cloning](cloning.md) |
| 5.1 — `take`, `replace`, and `swap`; 5.5; resource-place rules from 6.1 | [Resource-valued places](places-replacement.md) |
| 5.3–5.4 | [Resource lifecycle](resource-lifecycle.md) |
| 6.1 — references and lifetime checking | [References and lifetimes](references-lifetimes.md) |
| 6.1 — raw pointers, arithmetic, and object bounds | [Raw pointers](raw-pointers.md) |
| 6.1 — slices, global views, and subslices | [Slices](slices.md) |
| 6.1 — fixed arrays, length, indexing, and repetition | [Arrays](arrays.md) |
| 6.2–6.3 | [Read-only conversion](access-reduction.md) |
| 8.1 | [Failures](failures.md) |
| 8.2 | [Unsafe execution](unsafe.md) |

## Expressions, control flow, and protocols

| Source sections | Canonical documentation |
| --- | --- |
| 9.1–9.5 | [Expressions and numeric operations](expressions-and-numeric-operations.md) |
| 9.6–9.18, 9.21 | [Comparisons and membership](comparisons-and-membership.md) |
| 9.19–9.20 | [Operator hooks](operator-hooks.md) |
| 10.1–10.2 | [Evaluation order](evaluation-order.md) |
| 10.3–10.7 | [Assignment and result use](assignment-and-result-use.md) |
| 11.1–11.2, 11.4 | [Control flow](control-flow.md) |
| 11.3, 11.14–11.15, 11.19–11.20 | [For loops](for-loops.md) |
| 11.5–11.8, 11.11–11.13, 11.18 | [Iteration protocols](iteration-protocols.md) |
| 11.9–11.10, 11.16–11.17 | [Iterator values and lifetimes](iterator-values-and-lifetimes.md) |
| 12.1–12.3 | [Indexing](indexing.md) |
| 12.4–12.7 | [Slicing](slicing.md) |
| 12.8 | [Length](length.md) |

## Static interfaces

| Source sections | Canonical documentation |
| --- | --- |
| 13.1 declaration model, 13.14, 13.20 | [Interface overview](interfaces-overview.md) |
| 13.2, 13.4–13.5, 13.9, 13.12, 13.17–13.18, 13.24, 13.26–13.27, 13.38 | [Interface requirements](interface-requirements.md) |
| 13.8, 13.13, 13.23, 13.32–13.34 type syntax | [Associated types](interface-associated-types.md) |
| 13.3, 13.22, 13.28–13.29, 13.37 | [Interface extension](interface-extension.md) |
| 13.1 implementation model, 13.6 coherence, 13.7, 13.15–13.16, 13.19, 13.35–13.36 | [Interface implementations](interface-implementations.md) |
| 13.6 visibility, 13.10–13.11, 13.21, 13.25, 13.30–13.31, 13.34 accessibility | [Interface visibility](interface-visibility.md) |

## Modules and C interoperability

| Source sections | Canonical documentation |
| --- | --- |
| 14.1–14.7 | [Modules and imports](modules-and-imports.md) |
| 15.1–15.4 | [C functions and ABI](c-interoperability/functions-and-abi.md) |
| 15.5–15.8 | [C types and layout](c-interoperability/types-and-layout.md) |
| 15.9–15.12 | [C globals, failures, variadics, and `void`](c-interoperability/globals-errors-variadics-void.md) |
| 15.13–15.16 | [C ownership, callbacks, addresses, and layout queries](c-interoperability/ownership-callbacks-addresses.md) |
| 15.17–15.18 | [C arrays and unions](c-interoperability/layout-arrays-unions.md) |
| 15.19–15.20 | [Callback execution and C symbols](c-interoperability/callback-execution-symbols.md) |
| 16 | [Representative program](representative-program.md) |
