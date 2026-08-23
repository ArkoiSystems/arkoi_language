---
title: Arkoi data, resources, and ownership
description: Learn how Arkoi 1.0 copies data, moves resources, borrows storage, and performs deterministic cleanup.
---

# Data, resources, and ownership

Arkoi separates values that copy safely from values that represent ownership.
The distinction is a property of the type, not of whether a local binding is
mutable.

!!! info "Arkoi 1.0 target-language illustration"

    The snippets on this page explain the **target language**. They are not
    promised to compile with the current executable. See
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying them with compiler 0.1.0.

## What you will learn

By the end of this chapter, you will be able to:

- tell data values from ownership-controlled resources;
- choose between copying, moving, cloning, and borrowing;
- create read-only and mutable references without transferring ownership;
- explain why arrays own elements while slices only view them; and
- predict when deterministic resource cleanup runs.

This chapter builds on [Types, text, and aggregates](types-aggregates.md) and
[Expressions and conversions](expressions.md).

## Data copies; resources have one owner

| Category | Ordinary value behavior | Examples |
| --- | --- | --- |
| Data | Copies by assignment, argument, and return | Numbers, `bool`, `char`, `string_view`, references, slices, and `data` aggregates |
| Resource | Never copies implicitly | Built-in `string` and user or library `resource` aggregates |

A `data` aggregate may contain only data fields. A `resource` aggregate may
contain both data and resource fields:

```arkoi
data Point:
    x @f32
    y @f32

resource Document:
    id @u64
    title @string
```

The declaration fixes the category. Arkoi never silently turns one category
into the other based on how a value is later used.

## Moving and cloning are different choices

`move(name)` transfers a whole named resource without allocation or copying and
leaves the source uninitialized. `clone(value)` asks the resource's clone
contract for a new independent owner. User resources opt into cloning with an
explicit hook:

```arkoi title="fragment — Arkoi 1.0 target"
resource Ticket:
    id @u64

fun Ticket.__clone__(self @&Ticket) @Ticket:
    return Ticket(id = self.id)

original @Ticket = Ticket(id = 42)
backup @Ticket = clone(original)
forwarded @Ticket = move(original)
```

After the move, `original` cannot be used again. `backup` and `forwarded` are
separate initialized owners. Every resource can move, but a resource can clone
only when its type supplies a valid `__clone__` contract; cloning may be
fallible.

An ownership-taking parameter makes transfer part of the function contract:

```arkoi title="fragment — Arkoi 1.0 target"
fun consume_ticket(ticket @own Ticket) @u64:
    return ticket.id

ticket_id @u64 = consume_ticket(move(forwarded))
```

The function owns `ticket`. Because it does not move the value onward, cleanup
runs when the parameter's lexical lifetime ends. A fresh resource temporary can
enter an owning destination directly and must not be wrapped in `move(...)`.

## References borrow stable storage

Use an explicit reference when a function should observe or mutate a value
without taking ownership:

```arkoi
fun increment(value @&mut u32):
    value = value + 1

counter @mut u32 = 0
increment(&mut counter)

view @&u32 = &counter
```

`&T` grants read-only access and `&mut T` grants mutable access to the
referent. A reference is non-null, owns nothing, and is initialized once; it is
not a reassignable pointer. Arkoi requires explicit `&` or `&mut` at ordinary
call sites and rejects obvious lifetime escapes such as returning a reference
to a local binding.

Mutable references may alias. They grant mutation permission rather than
Rust-style exclusivity.

## Arrays own; slices view

A fixed array owns its elements and includes its length in its type. A slice is
a copied, non-owning pointer-and-length view:

```arkoi
fun revise_values() !CoreFail:
    values @mut [4]u32 = [10, 20, 30, 40]
    editable @[]mut u32 = values[..]

    editable[0]! = 42
    visible @[]u32 = readonly(editable)
    count @usize = length(visible)
```

Even the literal index on a slice is checked at runtime and can produce
`CoreFail.out_of_range`, so the example declares and propagates that failure.
`readonly(...)` explicitly removes one layer of mutation permission without
copying the elements.

A slice or reference cannot outlive its backing storage. An owner also cannot
be moved, replaced, or dropped while a derived view is obviously still live.

The postfix `!` in the array example propagates a recoverable bounds failure.
The [next chapter](failures.md) develops that contract in full; for now, read it
as “leave this function if the checked operation fails.”

## Cleanup follows initialized ownership

Initialized resources are cleaned up deterministically on normal block
completion and on exits such as `return`, `fail`, propagation, `break`, and
`continue`. Local resources clean up in reverse successful-initialization order.
A moved source is no longer initialized and is not cleaned up there.

Resource fields and array elements remain complete owning places. Borrow them
or use `take`, `replace`, and `swap` when their specified behavior fits; do not
move one out and leave its container partly initialized. Raw pointers and
operations whose invariants the compiler cannot prove belong in an explicit
`unsafe` context.

## Check your understanding

1. Does `move(original)` copy the resource before invalidating `original`?
2. Why can `view @&u32 = &counter` coexist with ownership of `counter`?
3. Does copying `visible @[]u32` copy the four array elements?
4. Which value is cleaned up after `consume_ticket(move(forwarded))` returns?

<details markdown="1">
<summary>Show the answers</summary>

1. No. It transfers the existing owned value and leaves the source binding
   uninitialized.
2. `view` borrows storage; it does not become an owner or extend the backing
   storage's lifetime.
3. No. A slice copy duplicates only its pointer-and-length descriptor.
4. The owning parameter `ticket` is cleaned up at the end of the call.
   `forwarded` was moved and is no longer initialized at its old location.

</details>

## Continue

The postfix `!` in this chapter is deliberate. Learn what it propagates in
[Optionals and recoverable failures](failures.md).

Canonical rules:

- [Types and values](../language-specification/1.0.0/types-values.md)
- [Ownership and moves](../language-specification/1.0.0/ownership-moves.md)
- [Cloning](../language-specification/1.0.0/cloning.md)
- [Resource lifecycle](../language-specification/1.0.0/resource-lifecycle.md)
- [References and lifetimes](../language-specification/1.0.0/references-lifetimes.md)
- [Arrays](../language-specification/1.0.0/arrays.md) and [slices](../language-specification/1.0.0/slices.md)
- [Places and replacement](../language-specification/1.0.0/places-replacement.md)
