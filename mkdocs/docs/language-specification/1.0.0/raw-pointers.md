---
title: Arkoi 1.0 raw pointers
description: Arkoi 1.0 target-language rules for raw-pointer types, creation, arithmetic, dereferencing, nullability, and unsafe use.
---

# Raw pointers

<!-- spec-sections: 6.1 (raw pointers, dereference, arithmetic, provenance) -->

Raw pointers are nullable, copyable address values. Safe code may transport and
compare them; dereference, arithmetic, and representation-changing operations
require `unsafe`.

!!! abstract "At a glance"

    - `*T` provides read-only pointee access; `*mut T` permits writes.
    - Pointer binding mutability is independent from pointee mutability.
    - Raw unary `*`, indexing, and arithmetic are unsafe.
    - Arithmetic uses `offset` and `distance`, not numeric operators.
    - Object provenance bounds arithmetic to one allocation and its one-past
      position.
    - Non-null does not imply valid, aligned, live, or in-bounds.

## Pointer types and safe operations

```arkoi
pointer @*Item
mutable_pointer @*mut Item
reassignable @mut *Item
```

`*T` and `*mut T` describe pointee access; a preceding binding-level `mut`
controls whether the pointer value can be reassigned.

Without entering `unsafe`, a raw pointer may be:

- declared, copied, stored, passed, or returned;
- compared with another compatible pointer for equality; or
- compared with `null`.

Raw pointers are non-owning data and never clean up a pointee.

## Unsafe pointer operations

Unsafe context is required for:

- dereferencing and writing through `*mut T`;
- pointer indexing, arithmetic, and distance;
- integer-to-pointer conversion;
- reinterpretation between unrelated pointer types; and
- pointer-to-reference conversion.

```arkoi
pointer @*u32 = get_pointer()
mutable_pointer @*mut u32 = get_mutable_pointer()

unsafe:
    value @u32 = *pointer
    *mutable_pointer = 42
```

Unary `*` applies only to raw pointers and requires an unsafe context.

### Field access

Raw pointers never dereference implicitly for field access:

```arkoi
unsafe:
    handle @u64 = (*file_pointer).handle
    (*mutable_file_pointer).handle = 42
```

Arkoi has no separate `->` operator. Values and references use
`reference.field`; raw pointers use visibly unsafe `(*pointer).field`.
Method receivers remain explicit arguments and must have the declared reference
or ownership type.

## Element-based arithmetic

Use dedicated functions rather than `+` or `-`:

```arkoi
unsafe:
    next @*Item = offset(pointer, 1)
    previous @*Item = offset(pointer, -1)
    count @ssize = distance(begin, end)
```

`offset(pointer, amount)` advances by a signed number of **elements**, not bytes,
and preserves the input pointer type. `distance(begin, end)` requires equal
pointee types and returns the signed element count from `begin` to `end` as
`ssize`.

Indexing is unsafe shorthand for offset then dereference:

```arkoi
unsafe:
    value @Item = pointer[index]
    mutable_pointer[index] = replacement
```

Conceptually, `pointer[index]` is `*offset(pointer, index)`.

!!! danger "Compile-time errors — numeric pointer operators"

    ```arkoi
    next @*Item = pointer + 1
    previous @*Item = pointer - 1
    count @ssize = end - begin
    ```

## Object-bounded provenance

Pointer arithmetic retains the allocated object from which the pointer
originated. `offset` may produce:

- an address of an element within that same object; or
- the one-past address immediately after it.

```arkoi
unsafe:
    end @*Item = offset(begin, length)
```

A one-past pointer may be compared, transported, or offset back into the object,
but not dereferenced. Producing an address before the allocation or beyond its
one-past position violates the unsafe contract.

`distance(begin, end)` is valid only for positions in the same allocation,
including one-past. Raw pointers carry no runtime bounds. The compiler may
reject obvious violations, but unsafe code must maintain the associated length
or end pointer. Violations are undefined behavior; machine-address overflow
during `offset` is an unrecoverable trap.

## Flat machine addresses

Code intentionally leaving object-bounded arithmetic converts explicitly:

```arkoi
unsafe:
    address @usize = pointer_address(pointer)
    adjusted @usize = address + byte_offset
    result @*Item = pointer_from_address(adjusted)
```

Converting through an integer does not preserve ordinary object provenance. The
programmer is responsible for the resulting pointer's validity, alignment,
provenance, lifetime, and accessible range. A non-null result proves none of
those conditions.

## Pointers and slices

Obtaining a raw pointer from a slice and constructing a slice from a pointer and
length are unsafe boundary operations. Their range validity requirements are
defined in [Slices](slices.md).

Use `readonly(pointer)` to remove `*mut T` write permission without changing its
address; see [Access reduction](access-reduction.md).

## Related topics

- [References and lifetimes](references-lifetimes.md)
- [Slices](slices.md)
- [Access reduction](access-reduction.md)
- [Unsafe execution](unsafe.md)
- [Function pointers](function-pointers.md)
