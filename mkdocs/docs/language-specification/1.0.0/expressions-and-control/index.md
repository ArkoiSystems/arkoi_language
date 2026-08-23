---
title: Arkoi 1.0 expressions and control
description: A task-oriented guide to Arkoi 1.0 target-language expressions, evaluation, assignment, control flow, iteration, indexing, slicing, and length.
---

# Expressions and control

<!-- spec-sections: none -->

This section defines how Arkoi evaluates expressions, changes stored values,
branches, loops, and invokes built-in or user-defined container protocols. This
page is a reading guide; the linked chapters contain the specification rules.

!!! abstract "At a glance"

    Select the exact operation and failure mode first, preserve defined
    evaluation order, then choose statement control flow or the appropriate
    iteration, indexing, slicing, and length protocol.

## Prerequisites

- Know the built-in and compound types from [Types and values](../types-values.md).
- Understand successful and failing function paths from [Functions and
  returns](../functions-returns.md) and [Failures](../failures.md).
- For a guided start with branches and loops, read [Values, functions, and
  control flow](../../../learn/basics.md).

## Choose a topic

| When you need to… | Read… |
| --- | --- |
| Use strict Booleans, numeric conversion, arithmetic modes, division, remainder, or shifts | [Expressions and numeric operations](../expressions-and-numeric-operations.md) |
| Thread a value through explicit receiver or placeholder calls | [Pipeline expressions](../pipelines.md) |
| Compare values or test membership | [Comparisons and membership](../comparisons-and-membership.md) |
| Provide user-defined operator behavior or resolve reflected operations | [Operator hooks](../operator-hooks.md) |
| Determine operand, argument, or partial-construction order | [Evaluation order](../evaluation-order.md) |
| Assign to a place, replace a resource, or handle an otherwise unused result | [Assignment and result use](../assignment-and-result-use.md) |
| Branch with `if` or repeat with `while`, `loop`, and named targets | [Control flow](../control-flow.md) |
| Iterate with `for` or `for!` and select a source access mode | [For loops](../for-loops.md) |
| Implement the canonical compile-time iteration contracts | [Iteration protocols](../iteration-protocols.md) |
| Return iterator values or reason about receiver-tied lifetime | [Iterator values and lifetimes](../iterator-values-and-lifetimes.md) |
| Select one or more elements and audit bounds behavior | [Indexing](../indexing.md) |
| Select a half-open range and preserve source access | [Slicing](../slicing.md) |
| Enable or call `length(...)` | [Length](../length.md) |

## Suggested workflow

1. Identify the operand types and choose the exact built-in or hook-backed
   operation on [Expressions and numeric operations](../expressions-and-numeric-operations.md),
   [Comparisons and membership](../comparisons-and-membership.md), or
   [Operator hooks](../operator-hooks.md).
2. Check [evaluation order](../evaluation-order.md) before an expression mixes
   mutation, failure, construction, or several calls.
3. Use [assignment](../assignment-and-result-use.md) for state changes and
   [control flow](../control-flow.md) for statement-level decisions.
4. For traversal, first select the [for-loop source mode](../for-loops.md), then
   inspect [iteration protocols](../iteration-protocols.md) only when a named
   aggregate supplies custom behavior.
5. Treat [indexing](../indexing.md), [slicing](../slicing.md), and
   [length](../length.md) as separate protocols with their own access, failure,
   and lifetime rules.

## Related topics

- [Language foundations](../foundations/index.md)
- [Ownership and safety](../ownership-and-safety/index.md)
- [Static interfaces](../static-interfaces/index.md)
- [Calls and overloads](../calls-overloads.md)
- [Quick reference](../quick-reference.md#control-flow-and-protocols)
