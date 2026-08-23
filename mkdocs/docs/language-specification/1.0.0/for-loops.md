---
title: Arkoi 1.0 for loops
description: Arkoi 1.0 target-language rules for built-in and custom for loops, binding forms, ownership, and loop lifetimes.
---

# For loops

<!-- spec-sections: 11.3, 11.14–11.15, 11.19–11.20 -->

This page defines `for` syntax, source-mode selection, temporary lifetime behavior, and built-in array and slice iteration.

!!! abstract "At a glance"
    A source is evaluated once. Direct values and read-only references select read-only iteration, explicit mutable access selects mutable iteration, and ownership transfer selects consuming iteration. Named resources need `move(...)`; resource temporaries consume automatically.

## Loop syntax and bindings

```arkoi
for value @u32 in values:
    process(value)
```

The binding type is explicit and its scope is the loop body. `break` exits the nearest loop; `continue` starts the next iteration. Python-style `for ... else` is unsupported.

For data elements, a built-in array or slice may copy values or lend references:

```arkoi
for value @u32 in values:
    process(value)

for value @&u32 in values:
    inspect(value)
```

Resource elements cannot be copied or implicitly moved out of built-in arrays or slices. Borrow them:

```arkoi
for file @&File in files:
    inspect(file)

for file @&mut File in &mut files:
    File.flush(file)!
```

Mutable access requires a mutable array place, mutable slice, or existing mutable reference. An owned resource binding is valid only when a custom iterator explicitly yields an owned resource value.

## Source-selection matrix

For a named aggregate `T`, source syntax chooses exactly one receiver mode:

| Source expression | Selected mode | Canonical protocol |
| --- | --- | --- |
| direct data value or named binding of type `T` | Read-only | `Iterable` or `FallibleIterable` |
| named resource binding of type `T` | Read-only; never an implicit move | `Iterable` or `FallibleIterable` |
| `&place` or an expression of type `&T` | Read-only on referent `T` | read-only protocol for `T` |
| `&mut place` | Mutable | `MutableIterable` or `FallibleMutableIterable` |
| expression already of type `&mut T` | Mutable on referent `T` | mutable protocol for `T` |
| `move(named_resource)` | Consuming; source becomes uninitialized | `OwningIterable` or `FallibleOwningIterable` |
| resource temporary | Consuming automatically | owning protocol |
| data temporary | Read-only, with loop lifetime extension | read-only protocol |

`&mut place` must designate a stable mutable receiver. A direct mutable binding still selects read-only iteration unless the complete source is explicitly borrowed with `&mut`.

References transparently expose their referents for `for` protocol selection:
`&T` and `&mut T` do not themselves implement iteration interfaces.
Implementing an interface for a reference type is invalid. A reference
expression never selects consuming iteration.

The source reference expression is evaluated exactly once. The hidden iterator uses its existing receiver-derived access and does not copy the referenced aggregate. The referent must be a named aggregate with the selected canonical protocol, or a built-in array or slice with compiler-defined iteration.

If the selected mode has no protocol, the loop is a compile-time error. In particular, `&T` cannot select mutable iteration. The loop binding type must exactly match the selected iterator's `Item` type, apart from the binding-level `@mut` modifier described on [Iterator values and lifetimes](iterator-values-and-lifetimes.md).

## Resource temporaries

A resource temporary enters owning iteration without `move(...)`:

```arkoi
for item @Item in create_collection():
    process(item)
```

The expression runs once and its ownership transfers directly into the owning protocol. By contrast, a named resource always needs `move(...)`:

```arkoi
collection @Collection = create_collection()

for item @Item in move(collection):
    process(item)
```

Without `move(...)`, a named resource selects read-only iteration when available. Arkoi never inserts an implicit move from a named binding.

If creation or advancement is fallible, use `for!`:

```arkoi
for! item @Item in create_fallible_collection():
    process(item)
```

Failure from producing the temporary itself still uses ordinary postfix propagation:

```arkoi
for! item @Item in load_collection()!:
    process(item)
```

Here postfix `!` belongs to `load_collection()`; `for!` covers iterator creation or advancement. Transfer occurs only after the temporary expression succeeds.

`move(...)` accepts a whole named resource binding, so this is invalid:

```arkoi
for item @Item in move(create_collection()):  # Compile-time error
    process(item)
```

A resource temporary with only a non-consuming protocol must first be stored in a named binding. Direct resource-temporary iteration always selects consuming mode.

## Data temporary lifetime

A data temporary used as the iterable is evaluated once and kept alive for the entire `for` or `for!`:

```arkoi
for item @&Item in create_data_collection():
    inspect(item)
```

Conceptually, Arkoi stores it in a hidden immutable binding before constructing the iterator. The binding remains alive while the hidden iterator exists and is cleaned up when the loop exits by exhaustion, `break`, `return`, failure propagation, or any other control-flow exit.

This fixed lifetime lets the iterator borrow the temporary or yield references into it. It is specific to loop iterable expressions and does not extend temporary lifetimes generally:

```arkoi
item @&Item = &create_data_collection().first  # Compile-time error
```

Arkoi does not prove general iterator-reference validity. A yielded reference that escapes and outlives the hidden temporary is programmer error and may cause undefined behavior. Resource temporaries instead transfer ownership into the hidden iterator and do not use this borrowed-source model.

## Built-in arrays and slices

Fixed arrays and slices have compiler-defined, infallible iteration:

- read-only arrays and slices support ordinary `for`;
- mutable arrays and `[]mut T` slices support explicit mutable iteration;
- traversal and bounds follow their built-in semantics;
- resource elements are yielded only by reference and never implicitly moved;
- no source-level interface implementation or synthetic iterator aggregate is visible.

Users cannot inspect, override, replace, or conflict with this behavior. A compiler may internally share the canonical protocol's lowering or helper machinery, but arrays and slices do not nominally conform to its interfaces, and similarly shaped user interfaces cannot affect them.

The compiler-defined behavior also applies through references to arrays and slices:

```arkoi
values @&[16]u32 = get_array()

for value @u32 in values:
    process(value)
```

## Related topics

- [Iteration protocols](iteration-protocols.md)
- [Iterator values and lifetimes](iterator-values-and-lifetimes.md)
- [Control flow](control-flow.md)
- [References and lifetimes](references-lifetimes.md)
- [Ownership and moves](ownership-moves.md)
- [Failures](failures.md)
