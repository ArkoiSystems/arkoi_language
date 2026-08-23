---
title: Arkoi 1.0 C data types and layout
description: Arkoi 1.0 target-language rules for C-compatible scalar, pointer, aggregate, enum, union, and layout declarations.
---

# C data types and layout

<!-- spec-sections: 15.5, 15.6, 15.7, 15.8 -->

Arkoi can define complete C-compatible structs and enums or name an externally
defined type without claiming to know its layout. C `const` maps to raw-pointer
pointee access, not to binding mutability.

!!! abstract "At a glance"

    - Use `export "C" data` to transcribe a complete C `struct`.
    - Use `import "C" data Name: pass` for an opaque, incomplete C type.
    - Map `const T*` to `*T` and `T*` to `*mut T`.
    - Use `export "C" enum` only when the complete C enum is known.

## Complete C-compatible aggregates

`export "C"` gives a `data` aggregate the active target's C struct representation:

```arkoi
export "C" data Point:
    x @c.int
    y @c.int
```

That representation guarantees:

- declaration-order fields;
- target C field alignment, padding, and aggregate alignment;
- no hidden Arkoi runtime fields;
- the same layout as the equivalent C `struct` made from corresponding field types
  on the same target; and
- eligibility for by-value and raw-pointer use in C imports and exports.

Only `data` aggregates can use this form:

```arkoi
export "C" resource Handle:
    value @usize
# Compile-time error: only data aggregates can request C struct layout
```

### Field matrix

Every field must itself have a complete C-compatible representation.

| Permitted by value | Forbidden by value |
| --- | --- |
| C ABI scalar | Resource |
| Raw pointer to a C ABI pointee | `string` or `string_view` |
| Fixed array of C ABI elements | Reference or slice |
| `export "C"` enum | Optional or failure value |
| Complete `export "C"` data | Arkoi ABI scalar |
| Complete `export "C"` union | Ordinary Arkoi aggregate |
|  | Fixed array of Arkoi ABI elements |
|  | Any unspecified or non-C-compatible representation |

```arkoi
data Internal:
    value @u32

export "C" data Invalid:
    field @Internal
    # Compile-time error: ordinary Arkoi aggregates have no C-compatible layout
```

An exported C aggregate is freely copyable and can be stored in local bindings. It
may have external methods and associated functions, implement interfaces, and apply
ordinary Arkoi field visibility. It still belongs to the C ABI domain: a native
Arkoi function cannot pass or return it by value, and an ordinary Arkoi-layout
aggregate cannot embed it by value. A native wrapper instead uses local storage,
references, raw pointers, or an explicit conversion to a separate native
representation.

An ordinary `data` aggregate has no guaranteed C layout, so it cannot cross a C
boundary by value or as a raw pointer to a complete aggregate. Arkoi has no separate
`repr`, packed-layout, custom-alignment, or field-offset syntax.

## Opaque imported data

Use an incomplete type when C owns the definition:

```arkoi
import "C" data FILE:
    pass
```

The declaration is module-level and gives Arkoi the C type name, but no visible
fields, size, alignment, or complete representation. `pass` merely satisfies the
non-empty-block grammar; it does not define an empty C struct or otherwise supply a
layout.

| Operation | Opaque `import "C" data` |
| --- | --- |
| Construct, copy, pass, or return by value | Forbidden |
| Store as an aggregate field by value | Forbidden |
| Field access, indexing, or any complete-type operation | Forbidden |
| Use behind a raw pointer | Allowed |
| Use in a C function signature | Allowed only behind a raw pointer |

```arkoi
import "C" fun fopen(
    path @*c.char,
    mode @*c.char,
) @*FILE

import "C" fun fclose(
    stream @*FILE,
) @c.int

export "C" data FileSlot:
    file @*FILE
```

A raw pointer to an opaque type follows the ordinary pointer rules: its value can be
copied, compared, passed, and checked for `null` safely, while dereferencing is
unsafe. Incompleteness means even a dereference cannot expose fields or construct a
complete value.

```arkoi
file @FILE
# Compile-time error: an incomplete type cannot exist by value

unsafe:
    field @u32 = (*pointer).field
    # Compile-time error: an opaque type has no visible fields

export "C" data InvalidSlot:
    file @FILE
    # Compile-time error: an incomplete type cannot be a by-value field
```

The opaque declaration body must contain only `pass`:

```arkoi
import "C" data FILE:
    descriptor @c.int
    # Compile-time error: an imported opaque C type cannot declare fields
```

Arkoi does not import complete C aggregate definitions. Transcribe a known complete
definition with `export "C" data`; otherwise keep it opaque and pointer-only.

## C `const` and pointee access

C pointee qualification maps directly to Arkoi's two raw-pointer forms:

| C | Arkoi | Access through the pointer |
| --- | --- | --- |
| `const T*` | `*T` | Read-only |
| `T*` | `*mut T` | Mutable |

The two forms have the same pointer-sized runtime representation for a given target;
Arkoi enforces their access difference in the type system.

```arkoi
import "C" fun strlen(
    text @*c.char,
) @c.size

import "C" fun memset(
    destination @*mut c.void,
    value @c.int,
    count @c.size,
) @*mut c.void
```

Binding mutability is a separate concern:

```arkoi
write_pointer @*mut c.unsigned_char
# Immutable binding containing a mutable-pointee pointer

reassignable_write_pointer @mut *mut c.unsigned_char
# Mutable binding containing a mutable-pointee pointer

reassignable_read_pointer @mut *c.unsigned_char
# Mutable binding containing a read-only-pointee pointer
```

Nullability, safe pointer-value operations, unsafe memory operations, write
requirements, and the one-way `readonly(...)` conversion are general Arkoi rules;
see [Raw pointers](../raw-pointers.md) and
[Access reduction](../access-reduction.md).

## C-compatible enums

`export "C" enum` selects the target platform's C enum ABI:

```arkoi
export "C" enum Status:
    success = 0
    failure = 1
```

Such an enum:

- contains at least one member;
- has no payload-bearing members;
- uses explicit or implicit integer discriminants under ordinary enum rules;
- has only discriminants representable by the target-selected C enum representation;
- can be a C-linked parameter, result, or `export "C" data` field;
- remains a distinct Arkoi enum with qualified members and ordinary equality; and
- does not implicitly convert to or from an integer.

```arkoi
import "C" fun set_status(
    status @Status,
)

export "C" data Result:
    status @Status
    code @c.int
```

An ordinary enum has no guaranteed C representation:

```arkoi
enum InternalStatus:
    ready
    failed

import "C" fun consume(
    status @InternalStatus,
)
# Compile-time error: an ordinary Arkoi enum cannot cross the C boundary
```

The ABI determines an exported enum's width and signedness. Use a fixed-width C
integer type instead where the boundary requires fixed width. If a C API supplies
only integer constants rather than a complete enum definition, use the appropriate
core C integer alias and declare ordinary Arkoi constants.

Arkoi supplies no separate enum-representation attribute, explicit underlying
integer type for exported enums, packed or nonstandard C enum control, or opaque
imported C enum declaration.

## Visibility and representation

`pub` controls Arkoi module visibility independently of C representation:

```arkoi
pub export "C" data PublicPoint:
    x @c.int
    y @c.int

pub export "C" enum PublicStatus:
    success = 0
    failure = 1
```

Here `export "C"` selects the C layout for each type; `pub` makes the declarations
visible to other Arkoi modules.

## Related topics

- [C functions and ABI domains](functions-and-abi.md)
- [Arrays and unions](layout-arrays-unions.md)
- [Types and values](../types-values.md)
- [Raw pointers](../raw-pointers.md)
- [Methods and visibility](../methods-visibility.md)
