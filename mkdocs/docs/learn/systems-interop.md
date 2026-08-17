---
title: Arkoi systems boundaries, hooks, and C interoperability
description: Learn how Arkoi 1.0 confines raw-pointer operations, validates compiler hooks, and wraps C ABI declarations behind safe native APIs.
---

# Systems boundaries, hooks, and C interoperability

Arkoi keeps unverifiable operations visible. Raw pointers are ordinary values
until code accesses memory through them, compiler hooks activate only exact
language-defined contracts, and a C declaration describes representation—not
ownership, lifetime, or failure policy.

!!! info "Arkoi 1.0 target-language tutorial"

    Every snippet on this page illustrates the **Arkoi 1.0 target language**.
    The current compiler does not necessarily accept it. Check
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying these examples with today's executable.

## What you will learn

By the end of this chapter, you will be able to:

- separate safe raw-pointer transport from unsafe memory access;
- keep unsafe operations inside a small, checked wrapper;
- recognize reserved compiler hooks and their owning contracts;
- declare C functions with C-domain types and explicit conversions; and
- audit a safe Arkoi API layered over a foreign ABI.

## Before you start

You should already understand:

- resources, cleanup, references, and slices from
  [Data, resources, and ownership](ownership.md);
- propagation and local handling from
  [Optionals and recoverable failures](failures.md);
- exact calls and receiver contracts from
  [Functions, calls, and methods](calls-methods.md); and
- nominal compile-time contracts from
  [Interfaces and modules](interfaces-modules.md).

Interfaces are contracts that user types opt into. Compiler hooks are narrower:
the language recognizes a fixed set of reserved names and validates each one's
exact signature.

## Safe code can transport a raw pointer

`*T` and `*mut T` are nullable, copyable, non-owning data values. Safe code may
store, copy, pass, return, and compare them, including comparison with `null`:

```arkoi title="Arkoi 1.0 target"
fun has_storage(pointer @*c.void) @bool:
    return pointer != null
```

`*T` permits only read-only pointee access and `*mut T` permits mutation, but
that permission does not make access safe. Binding-level `mut` independently
controls whether the pointer value itself can be reassigned:

```arkoi title="Arkoi 1.0 target"
fun pointer_roles(
    read_pointer @*u32,
    write_pointer @*mut u32,
):
    reassignable_pointer @mut *u32 = read_pointer
```

A raw pointer owns no pointee and schedules no cleanup. Non-null also says
nothing about alignment, lifetime, provenance, bounds, or whether a valid `T`
exists at the address.

## Memory access is explicitly unsafe

Dereference, mutation through a pointer, pointer indexing, arithmetic, distance,
representation-changing conversions, and pointer-to-reference conversion
require an unsafe context:

```arkoi title="Arkoi 1.0 target"
unsafe fun replace_and_read_next(
    read_pointer @*u32,
    write_pointer @*mut u32,
) @u32:
    first @u32 = *read_pointer
    *write_pointer = 42
    next @*u32 = offset(read_pointer, 1)
    return *next
```

`offset(pointer, amount)` advances in elements, not bytes. It must remain within
the originating object or its one-past position; one-past may be transported or
compared but not dereferenced. `distance(begin, end)` likewise requires
positions in the same allocation. Unsafe code must uphold these contracts—the
pointer carries no runtime bounds.

An `unsafe:` block relaxes only the checks assigned to unsafe operations. It
does not disable static typing, make invalid memory access defined, or turn
undefined behavior into a recoverable failure. An `unsafe fun` makes its whole
body an unsafe context, and calling it requires another unsafe context.

## Build a narrow safe wrapper

Safe inputs often carry the facts needed to justify one small unsafe region.
This function checks a slice's length before deriving and dereferencing its raw
pointer:

```arkoi title="Arkoi 1.0 target"
fun read_second(items @[]u8) !CoreFail @u8:
    if length(items) < 2:
        fail CoreFail.out_of_range

    unsafe:
        begin @*u8 = pointer(items)
        second @*u8 = offset(begin, 1)
        return *second
```

The slice establishes live, aligned, contiguous `u8` storage; the length check
establishes that element `1` exists. The unsafe block performs only the
operations those facts justify. A wrapper that could not establish every
required invariant would not be safe merely because its `unsafe` block is
small.

## Compiler hooks are reserved contracts

A hook is an ordinary qualified external function or method whose reserved name
connects it to language syntax. The compiler checks its receiver, parameters,
result, safety, and failure effect. Selection is static and uses exact overload
resolution; hooks create neither runtime method tables nor dynamic dispatch.

| Language feature | Principal hook contracts |
| --- | --- |
| Resource lifecycle | `__clone__`, `__drop__` |
| Comparisons | `__eq__`, `__ne__`, `__lt__`, `__le__`, `__gt__`, `__ge__` |
| Arithmetic and bitwise operators | `__add__`, `__sub__`, `__mul__`; normal/reverse families and checked or wrapping variants where defined |
| Membership | `__contains__` |
| Indexing | `__index__`, `__index_mut__` |
| Slicing | `__slice__`, `__slice_mut__` |
| Length | `__length__` |
| Iteration | `__iterate__`, `__iterate_mut__`, `__into_iterator__`, `__next__` |

Each feature's canonical page defines the complete signature. Programs normally
use `clone(value)`, `left + right`, `container[index]`, `length(container)`, or
`for` and let that syntax select the hook. Inventing another double-underscore
name creates no new syntax or compiler protocol.

Known reserved names must match their contracts exactly. This declaration is
deliberately invalid:

```arkoi title="Arkoi 1.0 target — compile-time error"
resource Handle:
    value @u64

fun Handle.__drop__(self @&Handle) !CoreFail:
    pass

# Compile-time error: `__drop__` requires `self @&mut Handle` and cannot
# declare or propagate a recoverable failure.
```

Hooks belong to their qualified type, follow its module and private-field
access rules, and do not let another module add behavior to a type it does not
own.

## A C declaration is an ABI contract

Attach C linkage to each module-level declaration. Imported functions have no
Arkoi body, use their exact C symbol name, and accept only C-compatible boundary
types:

```arkoi title="Arkoi 1.0 target — memory/allocation.ark"
module memory.allocation

import "C" fun malloc(
    size @c.size,
) @*mut c.void

import "C" fun free(
    pointer @*mut c.void,
)
```

Core C types such as `c.int`, `c.size`, and `c.void` use the active target's C
representation and calling convention. They remain distinct from native Arkoi
types even when their storage happens to match, so conversion is explicit:

```arkoi title="Arkoi 1.0 target"
fun round_trip_size(native_size @usize) !CoreFail @bool:
    foreign_size @c.size = convert(native_size, c.size)!
    restored @usize = convert(foreign_size, usize)!
    return restored == native_size
```

A C-linked signature cannot expose Arkoi resources, references, slices,
optionals, failure effects, `@own` parameters, or native Arkoi values by value.
Calls to imported C functions require `unsafe`; the declaration and linker must
actually agree about symbol kind, representation, alignment, and calling
convention.

## Put ownership and failure policy in Arkoi

The C ABI does not say whether a pointer is borrowed, owned, retained, nullable,
or which operation releases it. An ordinary Arkoi resource can encode that
policy and translate C results into a native failure contract:

```arkoi title="Arkoi 1.0 target — safe allocation wrapper"
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
    if size == 0:
        fail AllocationFail.invalid_size

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

The two unsafe calls remain local. `Allocation.create` rejects an unsupported
size, converts ABI domains explicitly, translates `null` into a declared
failure, and exposes only a fully initialized resource. `__drop__` pairs every
successfully created owner with `free`; because cleanup is automatic, it is
infallible and handles no recoverable result.

This wrapper is valid only under the real C contract that successful `malloc`
results are owned allocations releasable exactly once with `free`. Arkoi cannot
infer or verify that foreign policy from the declarations.

## Safe-wrapper checklist

Before exposing a safe function around unsafe or foreign operations, verify:

- **ABI:** every linked symbol, calling convention, parameter, result, and
  layout matches the foreign declaration.
- **Validity:** every pointer used for access is non-null when required, aligned,
  live, correctly typed, and within its permitted object range.
- **Lifetime:** borrowed storage outlives every foreign use, including retained
  pointers and callbacks.
- **Ownership:** acquisition and release rules are explicit; no value leaks,
  double-frees, or becomes usable after release.
- **Access:** read-only and mutable pointee permissions match the C contract;
  required synchronization is supplied.
- **Failures:** sentinels, status codes, `null`, and foreign error state are
  translated intentionally; no Arkoi failure crosses a C-linked signature.
- **Surface:** callers cannot violate the unsafe invariants through the safe API.

## Check your understanding

1. Which raw-pointer operations are permitted outside `unsafe`?
2. Does a non-null pointer prove that dereferencing it is valid?
3. Why can `__drop__` not declare a failure effect?
4. What does `import "C" fun` specify about ownership?
5. Why does `Allocation.create` convert `usize` to `c.size` explicitly?

<details markdown="1">
<summary>Show the answers</summary>

1. Safe code may declare, copy, store, pass, return, and compare compatible raw
   pointers, including comparison with `null`. Memory access, arithmetic, and
   representation-changing operations require `unsafe`.
2. No. It proves only that the stored address is not the null address; validity,
   alignment, lifetime, provenance, and bounds remain separate obligations.
3. Automatic cleanup must not replace an already propagating failure or change
   normal control flow. A drop hook is therefore infallible.
4. Nothing. It specifies C linkage and ABI representation. Ownership, lifetime,
   retention, and cleanup come from the external API contract and its wrapper.
5. Native Arkoi and C ABI types remain distinct even when their machine storage
   matches. The conversion also exposes an unrepresentable size as a failure
   that the wrapper can translate.

</details>

## Continue

Finish the course by combining the language model in one
[complete capstone module](capstone.md).

Canonical rules:

- [Raw pointers](../language-specification/1.0.0/raw-pointers.md)
- [Unsafe execution](../language-specification/1.0.0/unsafe.md)
- [Compiler hooks](../language-specification/1.0.0/compiler-hooks.md)
- [Operator hooks](../language-specification/1.0.0/operator-hooks.md)
- [C interoperability overview](../language-specification/1.0.0/c-interoperability.md)
- [C functions, symbols, and ABI domains](../language-specification/1.0.0/c-interoperability/functions-and-abi.md)
- [C data types and layout](../language-specification/1.0.0/c-interoperability/types-and-layout.md)
- [C globals, failures, variadics, and `void`](../language-specification/1.0.0/c-interoperability/globals-errors-variadics-void.md)
- [C ownership, callbacks, addresses, and layout queries](../language-specification/1.0.0/c-interoperability/ownership-callbacks-addresses.md)
