---
title: Arkoi 1.0 C ownership, callbacks, addresses, and layout queries
description: Arkoi 1.0 target-language rules for foreign ownership, callback lifetimes, address operations, and C layout queries.
---

# C ownership, callbacks, addresses, and layout queries

<!-- spec-sections: 15.13, 15.14, 15.15, 15.16 -->

The C ABI carries pointer values and function addresses, but no Arkoi ownership or
lifetime metadata. Arkoi can take stable addresses and query concrete layouts; the
program must still uphold the foreign API's contract.

!!! abstract "At a glance"

    - C pointers never acquire automatic ownership or lifetime behavior.
    - C callbacks are named, C-linked function pointers without captured state.
    - `address(place)` preserves the place's mutability in its pointer type.
    - `sizeof(@T)` and `alignof(@T)` are compile-time `usize` queries for complete
      types.

## Ownership and lifetime policy

A raw pointer obtained from C is only a pointer value:

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void
```

Arkoi does not infer whether a returned pointer is owned or borrowed, points to an
array, is null-terminated, lasts until another call or for the whole program, must be
released, uses `free` or another release function, or can be invalidated by mutation
or external state.

Passing a pointer to C likewise does not transfer ownership, extend the referent's
lifetime, prevent other aliases, promise that C will not retain it, promise that C
will not mutate through a mutable pointer, or schedule cleanup after the call.

The API contract determines transfer, borrowing, retention, invalidation, and
cleanup. The programmer must ensure that:

- storage stays valid whenever C may use it;
- release uses the API's required operation and occurs exactly as required;
- owned memory is not released twice;
- borrowed memory is not released by Arkoi;
- no access occurs after the lifetime ends;
- mutable and read-only pointer contracts are honored; and
- required synchronization is present.

Breaking any of these requirements is undefined behavior.

### No ownership annotations at the boundary

C imports and exports cannot encode transfer with Arkoi ownership syntax:

```arkoi
import "C" fun consume_buffer(
    buffer @own Buffer,
)
# Compile-time error: C-linked parameters cannot use `@own`
```

`@own` is forbidden on both imported and exported parameters. A C call never inserts
`move(...)`; raw pointers remain freely copyable data values; and returning one does
not create a resource.

### Model policy in a wrapper

A higher-level wrapper can turn a documented C ownership contract into a resource:

```arkoi
failure AllocationFail:
    invalid_size
    out_of_memory

resource Allocation:
    pointer @*mut c.void

fun Allocation.__drop__(
    self @&mut Allocation,
):
    unsafe:
        free(self.pointer)

fun Allocation.create(
    size @usize,
) !AllocationFail @Allocation:
    foreign_size @c.size = convert(
        size,
        c.size,
    ) handle failure:
        fail AllocationFail.invalid_size
    pointer @*mut c.void

    unsafe:
        pointer = malloc(foreign_size)

    if pointer == null:
        fail AllocationFail.out_of_memory

    return Allocation(
        pointer = pointer,
    )
```

The wrapper defines ownership, cleanup, nullability, which operations are safe, how
long derived references or slices remain valid, and whether transfer is allowed.
The ABI declaration defines none of those policies.

An exported Arkoi function also receives or returns only a C pointer value. Its
implementation and surrounding C API must document and enforce any C-visible
ownership contract. Arkoi does not generate ownership adapters, retain/release
calls, reference counts, cleanup registration, or cross-boundary lifetime tracking.

## Callback parameters and results

First declare a module-level C ABI function type, then use a raw pointer to it:

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int

import "C" fun register_compare(
    callback @*CompareCallback,
)
```

An imported function may accept or return `*CompareCallback`. The pointer contains
no captured Arkoi environment and receives no automatic ownership or lifetime
management.

When passing an Arkoi function address, the programmer ensures that:

- the function has compatible C linkage;
- its signature matches the callback type exactly;
- C cannot call it after the program or loaded code becomes invalid;
- all API-specific lifetime and execution requirements hold; and
- the callback obeys the [C failure boundary](globals-errors-variadics-void.md#failure-boundary).

Only named function pointers are supported. Arkoi creates neither a closure context
nor a trampoline object. Threading, concurrency, reentrancy, and retained-state
obligations are specified in
[Callback execution](callback-execution-symbols.md#callback-entry-and-execution).

## Taking an address

`address(...)` returns a raw pointer to stable storage without reading or writing it:

```arkoi
immutable_value @c.int = 10
read_pointer @*c.int = address(immutable_value)

mutable_value @mut c.int = 10
write_pointer @*mut c.int = address(mutable_value)
```

An immutable place produces `*T`; a mutable place produces `*mut T`.

### Addressable places

The operand must be a stable place with storage. This includes:

- a local binding or module-level variable;
- an aggregate field;
- a fixed-array element selected with a valid index;
- a place designated by a reference, or by a dereferenced raw pointer, when the
  resulting place is otherwise valid; and
- any other place form explicitly defined as addressable.

```arkoi
point @mut Point = Point(
    x = 10,
    y = 20,
)
x_pointer @*mut c.int = address(point.x)

values @mut [4]u8 = [0, 0, 0, 0]
first @*mut u8 = address(values[0])
```

Constants, literals, temporaries, computed expressions without stable storage,
moved or uninitialized bindings, non-place property-like operations, and direct
function results are not addressable. A field or element selected from a
composite constant is also a value without storage. Store a function result in
a binding first if its address is needed.

```arkoi
pointer @*c.int = address(calculate())
# Compile-time error: a temporary function result is not an addressable place
```

```arkoi
WIDTH @const usize = 4
pointer @*usize = address(WIDTH)
# Compile-time error: a constant has no storage address
```

Taking the address is safe. Dereferencing, pointer arithmetic, raw-pointer indexing,
and pointer-to-reference conversion retain their ordinary unsafe requirements.

### Pointer validity

The result remains valid only while its storage:

- is alive;
- has not moved;
- has not been replaced in a way that changes or invalidates its address;
- has not been released or otherwise invalidated; and
- stays correctly aligned and initialized for the pointee type.

Passing the pointer to C neither extends that lifetime nor stops C from retaining the
address. Any retained pointer must not outlive or be invalidated by the Arkoi storage.

A resource value is addressable:

```arkoi
item @mut ResourceType = create_resource()
pointer @*mut ResourceType = address(item)
```

The resource cannot be moved, replaced, or dropped while foreign code can still use
the pointer. Violating that condition is undefined behavior.

### Function and data addresses

The same built-in takes named function addresses and data-place addresses:

```arkoi
function_pointer @*Operation = address(add)
data_pointer @*mut c.int = address(mutable_value)
```

The operand category and expected result type select the operation.

This also supports C output parameters:

```arkoi
import "C" fun read_value(
    output @*mut c.int,
) @c.int

fun load_value() !CoreFail @s32:
    value @mut c.int = 0

    unsafe:
        discard(read_value(address(value)))

    return convert(value, s32)!
```

## Compile-time size and alignment

`sizeof(@Type)` includes all bytes in a complete value, including representation
padding. `alignof(@Type)` returns that complete type's required storage alignment in
bytes:

```arkoi
byte_count @usize = sizeof(@Point)
alignment @usize = alignof(@Point)
```

Both built-ins take an ordinary `@Type` operand and:

- produce compile-time `usize` constants;
- evaluate no runtime value and call no user code;
- cannot fail, and cannot trap for a valid complete type;
- are not overloadable;
- can appear in ordinary constant expressions; and
- reflect the active target and ABI.

### Complete and incomplete operands

Queries are valid for every complete type with a defined representation, including:

- primitive numeric and Boolean types and `char`;
- raw pointers to C ABI or function types;
- references and slices whose Arkoi representation is defined;
- fixed arrays;
- complete data and resource aggregates;
- ordinary enums;
- complete `export "C"` data, union, and enum types;
- optionals with a defined representation; and
- any other complete type with a defined Arkoi representation.

For exported C-compatible types, the answer follows the active target C ABI:

```arkoi
array_size @usize = sizeof(@[16]c.unsigned_char)
```

The operand cannot be `c.void`, an opaque `import "C" data` type, a bare function
signature, an interface name, or any intentionally incomplete or unspecified type:

```arkoi
file_size @usize = sizeof(@FILE)
# Compile-time error: `FILE` is incomplete

operation_size @usize = sizeof(@Operation)
# Compile-time error: a function signature is not a value type

pointer_size @usize = sizeof(@*Operation)
```

The last query is valid because a raw pointer to a function type is complete.

Convert the result explicitly before supplying a C size value:

```arkoi
size @c.size = convert(
    sizeof(@Point),
    c.size,
)!
```

`usize` and `c.size` remain separate ABI-domain types even if their storage happens
to match.

These queries provide representation facts only. They allocate no storage, create
or construct no value, and do not make arbitrary bytes a valid instance of the
queried type.

## Related topics

- [C functions and ABI domains](functions-and-abi.md)
- [Callback execution and symbols](callback-execution-symbols.md)
- [Ownership and moves](../ownership-moves.md)
- [Resource lifecycle](../resource-lifecycle.md)
- [References and lifetimes](../references-lifetimes.md)
- [Raw pointers](../raw-pointers.md)
- [Function pointers](../function-pointers.md)
