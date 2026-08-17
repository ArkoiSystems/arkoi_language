---
title: Arkoi 1.0 static interfaces
description: A task-oriented guide to Arkoi interface requirements, associated types, extension, implementation, and visibility.
---

# Static interfaces

<!-- spec-sections: none -->

Arkoi interfaces are nominal compile-time contracts for named user aggregates.
They describe associated types and required behavior without creating runtime
interface values or dynamic dispatch. This page is a reading guide; the linked
chapters contain the specification rules.

!!! abstract "At a glance"

    Declare a contract, bind it explicitly to an eligible concrete type, provide
    exact external definitions, and validate extension and visibility after all
    associated types are substituted.

## Prerequisites

- Know how named aggregates are declared in [Aggregates and enums](../aggregates-enums.md).
- Understand external receiver functions and access control from [Methods and
  visibility](../methods-visibility.md).
- Read [Modules and imports](../modules-and-imports.md) before placing an
  implementation across module boundaries.
- For a guided introduction, use [Interfaces and
  modules](../../../learn/interfaces-modules.md).

## Choose a topic

| When you need to… | Read… |
| --- | --- |
| Understand the compile-time model, `Self`, marker interfaces, or valid name positions | [Static-interface model](../interfaces-overview.md) |
| Declare methods, associated functions, hooks, overloads, or exact matching rules | [Interface requirements](../interface-requirements.md) |
| Leave a related concrete type open and bind it during conformance | [Associated types](../interface-associated-types.md) |
| Inherit or combine contracts and reason about shared parent paths | [Extension and inheritance](../interface-extension.md) |
| Declare conformance, check eligible types, or apply ownership and uniqueness rules | [Interface implementations](../interface-implementations.md) |
| Expose a contract or validate concrete definitions and bound types across modules | [Interface visibility](../interface-visibility.md) |

## Suggested workflow

1. Confirm that a compile-time contract—not a runtime polymorphic value—is the
   intended model on [Static interfaces](../interfaces-overview.md).
2. Write the smallest complete set of [requirements](../interface-requirements.md),
   using an [associated type](../interface-associated-types.md) only when the
   concrete type genuinely varies by implementation.
3. Decide whether shared contracts should use [extension](../interface-extension.md)
   and check every inherited path for one consistent conformance.
4. Provide ordinary external functions on the concrete type, then declare the
   nominal [implementation](../interface-implementations.md) and its associated
   bindings.
5. Audit the flattened contract, bound types, and implementing definitions with
   [Interface visibility](../interface-visibility.md).

Implementation blocks contain associated-type bindings only. Executable bodies
remain ordinary concrete methods, associated functions, or hooks outside the
block.

## Related topics

- [Language foundations](../foundations/index.md)
- [Expressions and control](../expressions-and-control/index.md)
- [Compiler hooks](../compiler-hooks.md)
- [Iteration protocols](../iteration-protocols.md)
- [Modules and imports](../modules-and-imports.md)
