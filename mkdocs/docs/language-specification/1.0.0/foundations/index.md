---
title: Arkoi 1.0 language foundations
description: A task-oriented guide to Arkoi source syntax, bindings, types, aggregates, functions, calls, methods, and compiler hooks.
---

# Language foundations

<!-- spec-sections: none -->

This section defines Arkoi's source structure and the declarations used to name,
store, construct, and call values. This page is a reading guide; the linked
chapters contain the specification rules.

!!! abstract "At a glance"

    Start with blocks and typed bindings, classify each value as data or a
    resource, then add aggregates, functions, calls, visibility, and compiler
    hooks.

## Prerequisites

- Read the [Arkoi 1.0 target status](../index.md) before treating examples as
  current-compiler input.
- For a guided introduction, begin with
  [Your first Arkoi source file](../../../learn/basics.md).
- Keep the [glossary](../glossary.md) available for terms such as *binding*,
  *data*, *resource*, *place*, and *trap*.

## Choose a topic

| When you need to… | Read… |
| --- | --- |
| Format blocks, comments, lists, or multiline expressions | [Source syntax](../source-syntax.md) |
| Declare locals, control reassignment, or reason about definite initialization | [Bindings and initialization](../bindings-initialization.md) |
| Define compile-time constants or module storage | [Constants and module globals](../constants-globals.md) |
| Identify built-in types, compound type notation, value categories, text, optionals, or aliases | [Types and values](../types-values.md) |
| Declare and construct `data`, `resource`, or enum types | [Aggregates and enums](../aggregates-enums.md) |
| Define parameter and return contracts, recursion, or `main` | [Functions and returns](../functions-returns.md) |
| Resolve a call, overload, ownership mode, or fallibility choice | [Calls and overloads](../calls-overloads.md) |
| Name a function signature or take and invoke a raw function pointer | [Function pointers](../function-pointers.md) |
| Define methods, associated functions, receiver calls, or declaration visibility | [Methods and visibility](../methods-visibility.md) |
| Implement a reserved `__...__` contract used by language syntax | [Compiler hooks](../compiler-hooks.md) |

## Suggested workflow

1. Establish the file's block structure and declaration order with
   [source syntax](../source-syntax.md).
2. Give every value a category and exact type using [bindings](../bindings-initialization.md)
   and [types and values](../types-values.md).
3. Define named shapes with [aggregates and enums](../aggregates-enums.md), then
   state their operations through [functions](../functions-returns.md).
4. Check call behavior—including exact overload, ownership, and failure mode—on
   [Calls and overloads](../calls-overloads.md).
5. Add type-qualified behavior through [methods](../methods-visibility.md) and
   use a reserved [compiler hook](../compiler-hooks.md) only when its canonical
   contract matches the intended language operation.

## Related topics

- [Ownership and safety](../ownership-and-safety/index.md)
- [Expressions and control](../expressions-and-control/index.md)
- [Static interfaces](../static-interfaces/index.md)
- [Modules and imports](../modules-and-imports.md)
- [Quick reference](../quick-reference.md)
