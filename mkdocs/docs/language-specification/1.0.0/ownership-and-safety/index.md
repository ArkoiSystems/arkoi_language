---
title: Arkoi 1.0 ownership and safety
description: A task-oriented guide to Arkoi 1.0 target-language resources, moves, cleanup, references, pointers, arrays, slices, failures, and unsafe execution.
---

# Ownership and safety

<!-- spec-sections: none -->

This section defines how Arkoi distinguishes copied data from uniquely owned
resources, how non-owning access is represented, and where explicit failure or
unsafe responsibility begins. This page is a reading guide; the linked chapters
contain the specification rules.

!!! abstract "At a glance"

    Determine whether a type is data or a resource, make every ownership change
    explicit, keep views within their backing lifetime, and isolate operations
    whose safety the compiler cannot prove.

## Prerequisites

- Understand [bindings and initialization](../bindings-initialization.md),
  especially binding-level `mut` and definite initialization.
- Read the data/resource classification and compound type notation in
  [Types and values](../types-values.md).
- For a guided introduction, use [Data, resources, and
  ownership](../../../learn/ownership.md).

## Choose a topic

| When you need to… | Read… |
| --- | --- |
| Transfer a named resource, accept ownership, or return an owned value | [Ownership and moves](../ownership-moves.md) |
| Create an independent owned resource without consuming its source | [Cloning](../cloning.md) |
| Read, replace, take, or swap a resource field or indexed element | [Places and replacement](../places-replacement.md) |
| Define construction and cleanup or audit scope-exit order | [Resource lifecycle](../resource-lifecycle.md) |
| Borrow stable storage through `&T` or `&mut T` and check obvious lifetimes | [References and lifetimes](../references-lifetimes.md) |
| Represent nullable addresses or cross into explicit pointer operations | [Raw pointers](../raw-pointers.md) |
| Work with non-owning contiguous views | [Slices](../slices.md) |
| Own a fixed number of elements and distinguish static from dynamic bounds checks | [Arrays](../arrays.md) |
| Explicitly remove one layer of mutation permission | [Access reduction](../access-reduction.md) |
| Declare, propagate, or handle recoverable effects | [Failures](../failures.md) |
| Mark a region whose invariants the compiler cannot fully verify | [Unsafe execution](../unsafe.md) |

## Suggested workflow

1. Classify each value with [Types and values](../types-values.md). Data copies;
   resources do not.
2. Choose whether a resource changes owner through [move](../ownership-moves.md)
   or gains another independent owner through [clone](../cloning.md).
3. Use [references](../references-lifetimes.md) or [slices](../slices.md) for
   non-owning access, and keep the backing storage live through the view's last
   use.
4. For fields and elements, select ordinary assignment, `take`, `replace`, or
   `swap` from [Places and replacement](../places-replacement.md) without leaving
   a container partly initialized.
5. Audit [cleanup](../resource-lifecycle.md) on every exit, including failure
   propagation.
6. Keep [raw-pointer](../raw-pointers.md) and other unverifiable operations in a
   narrow [unsafe context](../unsafe.md) with their invariants stated by the
   surrounding code.

## Related topics

- [Language foundations](../foundations/index.md)
- [Optionals and recoverable failures learning chapter](../../../learn/failures.md)
- [Assignment and result use](../assignment-and-result-use.md)
- [Iterator values and lifetimes](../iterator-values-and-lifetimes.md)
- [C ownership and callback boundaries](../c-interoperability/ownership-callbacks-addresses.md)
