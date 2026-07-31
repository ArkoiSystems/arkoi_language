# C arrays and unions

<!-- spec-sections: 15.17, 15.18 -->

Fixed arrays preserve their contiguous layout at C-compatible boundaries, while C
unions use explicitly overlapping storage. Neither construct adds C's implicit
array decay or active-union-field tracking to Arkoi.

!!! abstract "At a glance"

    - `[N]T` is C-compatible when `T` is C-compatible and the complete fixed layout
      is known.
    - C array parameters become raw pointers in Arkoi signatures.
    - Arrays never decay implicitly and cannot be returned directly by C-linked
      functions.
    - `export "C" union` defines complete overlapping C-compatible storage.

## Fixed arrays

A fixed `[N]T` array is C-compatible when its element type is C-compatible, `N` is a
compile-time constant, and its complete representation is defined for the active
target ABI. Its elements use ordinary contiguous fixed-array layout, matching the
corresponding C array object representation for C-compatible elements.

Complete arrays can be fields or global objects:

```arkoi
export "C" data Packet:
    bytes @[256]c.unsigned_char

import "C" global_buffer @mut [256]c.unsigned_char
```

### C array parameters

C adjusts function parameters written as arrays into pointer parameters. Therefore:

```c
void process(const unsigned char data[256]);
```

is declared as:

```arkoi
import "C" fun process(
    data @*c.unsigned_char,
)

import "C" fun fill(
    data @*mut c.unsigned_char,
    count @c.size,
)
```

The read-only C parameter maps to `*c.unsigned_char`; a mutable parameter maps to
`*mut c.unsigned_char`.

Arkoi never decays a fixed array to a pointer. Take the address of its first element
explicitly:

```arkoi
import "C" mutable_buffer @mut [256]c.unsigned_char
import "C" immutable_buffer @[256]c.unsigned_char

fun call_array_functions():
    count @c.size = 256

    unsafe:
        fill(
            address(mutable_buffer[0]),
            count,
        )

        process(address(immutable_buffer[0]))
```

The imported globals denote initialized C storage; `address(...)` cannot target
an uninitialized binding. Each pointer describes only the first element and
carries no length. The caller must provide valid storage, enough elements for
the documented count, correct mutability and alignment, and a lifetime covering
all use or retention by C. A violation is undefined behavior.

### Boundary limits

A C-linked function cannot return a fixed array directly:

```arkoi
import "C" fun invalid_array_result() @[16]c.unsigned_char
# Compile-time error: a C-linked function cannot return a fixed array by value
```

A complete C-compatible aggregate that contains an array may still be returned when
the target C ABI supports that aggregate return:

```arkoi
export "C" data Block:
    bytes @[16]c.unsigned_char

import "C" fun make_block() @Block
```

Fixed arrays cannot be C variadic-tail arguments. Arkoi also has no implicit
array-to-pointer conversion, C flexible-array-member syntax, variable-length array
types, array-parameter length contract in the type system, or automatic
null-termination for character arrays.

## C unions

`export "C" union` defines a complete type whose fields overlap and begin at the same
address:

```arkoi
export "C" union Value:
    integer @c.int
    decimal @c.double
```

Its size, alignment, padding, field alignment, and by-value calling convention all
follow the active target C ABI.

### Union fields

| Permitted | Forbidden |
| --- | --- |
| C ABI scalar | Resource |
| Raw pointer to C ABI data | Reference, slice, `string`, or `string_view` |
| Raw pointer to a C ABI function type | Optional or failure value |
| Fixed array of C ABI elements | Ordinary Arkoi-layout aggregate |
| Complete `export "C"` data or union | Incomplete opaque C type by value |
| `export "C"` enum | Function signature by value |
|  | Interface type |

At least one field is required:

```arkoi
export "C" union Empty:
    pass
# Compile-time error: a C union must contain a field
```

Field names use ordinary aggregate naming and visibility. `pub` may independently
expose the union to other Arkoi modules:

```arkoi
pub export "C" union PublicValue:
    integer @c.int
    decimal @c.double
```

### Unsafe field access

Every union-field read, write, or address operation requires `unsafe`:

```arkoi
value @mut Value
integer_pointer @*mut c.int

unsafe:
    value.integer = 10
    integer @c.int = value.integer
    integer_pointer = address(value.integer)
```

Arkoi does not record an active field. The surrounding C API contract must establish
that a field's current object representation is valid before it is read; otherwise
behavior is undefined. Writing one field replaces the shared storage and can
invalidate the value previously represented by another.

A union whose fields are all valid C-compatible data types is copyable. Copying it
copies the entire target-ABI object representation, including padding.

### Where unions can appear

A complete C union can be:

- a C-linked parameter or return type when the target ABI supports it;
- a field of an exported C aggregate or another exported C union;
- the type of an imported or exported C global; or
- used behind a raw pointer.

```arkoi
import "C" fun inspect_value(
    value @Value,
) @c.int
```

Arkoi provides no native-layout `union`, tagged semantics for `union`, automatic
active-field tracking, C bit-fields, flexible array members, packed unions, custom
alignment controls, or direct import of a complete C union definition.

Manually transcribe a known definition with `export "C" union`. If its layout is not
available, declare an opaque `import "C" data Name: pass` and use it only behind raw
pointers.

## Related topics

- [C data types and layout](types-and-layout.md)
- [C globals and variadics](globals-errors-variadics-void.md)
- [Taking addresses and querying layout](ownership-callbacks-addresses.md)
- [Raw pointers](../raw-pointers.md)
- [Unsafe execution](../unsafe.md)
