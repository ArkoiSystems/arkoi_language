# C functions, symbols, and ABI domains

<!-- spec-sections: 15.1, 15.2, 15.3, 15.4 -->

C linkage is attached to individual module-level declarations. The boundary uses
target C representations, while native Arkoi values remain in a separate ABI domain.

!!! abstract "At a glance"

    - `import "C" fun` declares a bodyless C function; calling it is unsafe.
    - `export "C" fun` defines an Arkoi body with a C-facing signature.
    - Only `"C"` is a supported foreign ABI name.
    - A linked function's Arkoi name is its exact C symbol name.
    - Values and pointees at the boundary must belong to the C ABI domain.

## C-linked function declarations

Both directions are declared per function:

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void

import "C" fun free(
    pointer @*mut c.void,
)

export "C" fun arkoi_process(
    value @c.int,
) @c.int:
    return value * 2
```

| Property | `import "C" fun` | `export "C" fun` |
| --- | --- | --- |
| Implementation | External C function; declaration has no Arkoi body | Ordinary Arkoi definition with a body |
| Call and linkage | Target C calling convention and C symbol linkage | Target C calling convention and a C-linkage symbol |
| Signature | C-compatible parameter and result types only | C-compatible parameter and result types only |
| Arkoi body rules | Not applicable | All ordinary Arkoi body rules apply |
| Safety | Every call requires an `unsafe` context | Entry from C follows the foreign API contract |

```arkoi
pointer @*mut c.void

unsafe:
    pointer = malloc(size)
```

The C boundary cannot expose Arkoi ownership annotations, resources, references,
slices, optionals, or any other non-C-compatible ABI type. Its failure boundary is
defined once in [Globals, failures, variadics, and `void`](globals-errors-variadics-void.md#failure-boundary).

### Placement and modifier restrictions

`import "C"` and `export "C"` are mutually exclusive:

```arkoi
import "C" export "C" fun invalid()
# Compile-time error: a function cannot be both imported and exported
```

Both modifiers are valid only on module-level functions. They are forbidden on
methods, associated functions, interface requirements, reserved hooks, local
functions, and `main`.

Arkoi has no foreign-declaration blocks. Every function carries its own linkage:

```arkoi
import "C":
    fun malloc(size @c.size) @*mut c.void
# Compile-time error: C linkage must be declared on each function
```

## Foreign ABI name

The ABI string selects the calling convention, symbol-linkage rules, and permitted
boundary representations. `"C"` is the only ABI name in this specification. It
means target-platform C calling convention and representations, C symbol linkage,
Arkoi's C-interop restrictions, and no Arkoi name mangling.

```arkoi
import "C++" fun create_object()
# Compile-time error: unsupported foreign ABI `C++`

import "system" fun platform_call()
# Compile-time error: unsupported foreign ABI `system`

import "wasm" fun host_call()
# Compile-time error: unsupported foreign ABI `wasm`
```

## Function symbol names

For an imported or exported function, its Arkoi declaration name is exactly the C
symbol name. `malloc` imports `malloc`; `arkoi_process` exports `arkoi_process`.

Arkoi provides no foreign-name alias, link-name attribute, import/export renaming,
or way to expose several C symbols for one Arkoi function:

```arkoi
import "C" fun allocate(
    size @c.size,
) @*mut c.void as "malloc"
# Compile-time error: C symbols cannot be renamed

export "C" "arkoi_process_v1" fun process(
    value @c.int,
) @c.int:
    return value * 2
# Compile-time error: C export aliases are unsupported
```

Use an ordinary Arkoi wrapper when application code needs a friendlier name:

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void

fun allocation_succeeded(
    size @usize,
) !CoreFail @bool:
    foreign_size @c.size = convert(
        size,
        c.size,
    )!

    unsafe:
        return malloc(foreign_size) != null
```

Whole-program collision rules are separate from name mapping; see
[C symbol uniqueness](callback-execution-symbols.md#c-symbol-uniqueness).

## ABI domains

Every Arkoi type belongs to an ABI domain:

| Domain | Declaration form | Purpose |
| --- | --- | --- |
| Native Arkoi | Ordinary type declaration, for example `type Count = u32` | Arkoi code and native layouts |
| C | `import "C" type` | Target C representation and calling convention |

`import "C"` on a type selects the C ABI domain; it does not import a linker symbol.
An exported C function also has a C ABI signature even though its implementation is
written in Arkoi.

### Core C scalar types

The required core library supplies a `c` interoperability module whose scalar types
are selected for the active target:

| Category | Required C ABI aliases |
| --- | --- |
| Characters | `c.char`, `c.signed_char`, `c.unsigned_char` |
| Integers | `c.short`, `c.unsigned_short`, `c.int`, `c.unsigned_int`, `c.long`, `c.unsigned_long`, `c.long_long`, `c.unsigned_long_long` |
| Floating point | `c.float` (resolved to `f32`), `c.double` (resolved to `f64`) |
| Other scalar types | `c.bool`, `c.size`, `c.ptrdiff` |

The integer, character, Boolean, size, and pointer-difference aliases resolve to
target-defined signed, unsigned, or Boolean integer storage as required. The core
library may additionally package target C fixed-width aliases corresponding to such
definitions as `uint8_t`, `uint32_t`, and `int64_t`; their precise Arkoi names and
packaging are core-library details.

For every core C scalar, the compiler and core library guarantee the target C ABI's
size, signedness, alignment, value representation, and calling convention. User code
does not implement target selection for these canonical types.

A C scalar has the numeric or Boolean behavior of its resolved storage kind, but it:

- remains in the C ABI domain;
- is not type-identical to a representation-equivalent Arkoi primitive;
- has a separate overload identity;
- never converts implicitly to or from an Arkoi ABI value; and
- may be converted explicitly under the ordinary conversion rules.

Even where `c.int` and `s32` share storage, assignment across the domains is invalid:

```arkoi
foreign_value @c.int = 10
native_value @s32 = foreign_value
# Compile-time error: ABI-domain mismatch
```

```arkoi
foreign_value @c.int = 10
native_value @s32 = convert(
    foreign_value,
    s32,
)!
```

### By-value compatibility

Non-pointer values from different ABI domains cannot be mixed by value in a
callable signature or composite type:

| Invalid composition | Reason |
| --- | --- |
| C scalar parameter or result in a native Arkoi function | Native callable signatures require native by-value types |
| Arkoi scalar parameter or result in a C-linked function | C signatures require C by-value types |
| Non-pointer C value field in an ordinary Arkoi-layout aggregate | The containing layout is native |
| Non-pointer Arkoi value field in an `export "C"` aggregate | The containing layout is C-compatible |

```arkoi
fun invalid_native(
    value @c.int,
) @c.int:
    return value
# Compile-time error: a native signature cannot use C values by value

import "C" fun invalid_foreign(
    value @s32,
) @s32
# Compile-time error: a C signature cannot use Arkoi values by value

data InvalidNative:
    value @c.int
# Compile-time error: a native aggregate cannot contain a C value field

export "C" data InvalidC:
    value @s32
# Compile-time error: a C aggregate cannot contain an Arkoi value field
```

A type alias stays in its target's ABI domain:

```arkoi
import "C" type ForeignInt = c.int

type InvalidAlias = c.int
# Compile-time error: an ordinary alias cannot move a C type into the Arkoi ABI
```

These restrictions concern by-value composition and signatures. A function body may
hold local values from either domain, allowing wrappers to convert before and after
a foreign call:

```arkoi
fun wrapper(
    value @s32,
) !CoreFail @s32:
    foreign_input @c.int = convert(value, c.int)!
    foreign_result @c.int

    unsafe:
        foreign_result = native_operation(foreign_input)

    return convert(foreign_result, s32)!
```

Native Arkoi aggregates and resources have one deliberate handle exception:
they may store a raw pointer whose pointee belongs to the C ABI domain. The
pointer field embeds only an address, not a C value object, and it does not make
the containing type C-compatible. The wrapper remains responsible for
nullability, ownership, lifetime, cleanup, and synchronization:

```arkoi
resource Allocation:
    pointer @*mut c.void
```

This exception does not permit a pointer to an Arkoi-domain pointee in a C
declaration. See [ownership wrappers](ownership-callbacks-addresses.md#model-policy-in-a-wrapper).

Function types can also be C ABI types:

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int
```

## Pointer and array ABI domains

A raw pointer inherits its ABI domain from its pointee for type identity and C
boundary checking. Representation equality does not make `*u8` interchangeable
with `*c.unsigned_char`. The native-wrapper handle exception above permits
storage of a C-domain pointer in a native aggregate, but a raw pointer in a C
function, global, aggregate field, union field, or callback signature must point
to one of these C-domain categories:

- a scalar declared with `import "C" type`;
- `c.void`;
- an opaque `import "C" data` type;
- a complete `export "C"` data aggregate or union;
- an `export "C"` enum;
- a fixed array whose element belongs to the C ABI; or
- a function type declared with `import "C" type`.

```arkoi
import "C" fun fill(
    data @*mut c.unsigned_char,
    count @c.size,
)

import "C" fun invalid(
    data @*mut u8,
)
# Compile-time error: a C boundary pointer cannot point to an Arkoi ABI type
```

When representations, alignment, lifetime, and the foreign API contract really are
compatible, an explicit unsafe reinterpretation can cross pointer domains:

```arkoi
buffer @mut [4]u8 = [0, 0, 0, 0]
native_pointer @*mut u8 = address(buffer[0])
foreign_pointer @*mut c.unsigned_char

unsafe:
    foreign_pointer = reinterpret(
        native_pointer,
        *mut c.unsigned_char,
    )
```

This changes only the pointer type. It neither converts stored values nor pins or
extends the lifetime of the storage, and it cannot make an invalid representation
valid. A pointer to an ordinary Arkoi aggregate or resource can be exposed only by
explicitly reinterpreting it as an opaque C pointer such as `*c.void` or
`*mut c.void`; the programmer then maintains its ownership and lifetime contract.

References and slices always remain Arkoi ABI types and never occur in C declarations.
A native function may nevertheless use a reference locally because that reference
does not cross the boundary:

```arkoi
fun inspect_foreign(
    value @&c.int,
):
    pass
```

Fixed arrays take their domain from their element:

```arkoi
[16]c.unsigned_char  # C ABI array
[16]u8               # Arkoi ABI array
```

Thus, a fixed array stored by value in a C aggregate or global needs a C-domain
element. An Arkoi `u8` pointer likewise requires the explicit, justified
reinterpretation shown above before it can serve as the corresponding C byte pointer.

## Related topics

- [C interoperability overview](../c-interoperability.md)
- [Data types and layout](types-and-layout.md)
- [Raw pointers](../raw-pointers.md)
- [Calls and overloads](../calls-overloads.md)
- [Function pointers](../function-pointers.md)
- [Failures at the C boundary](globals-errors-variadics-void.md#failure-boundary)
