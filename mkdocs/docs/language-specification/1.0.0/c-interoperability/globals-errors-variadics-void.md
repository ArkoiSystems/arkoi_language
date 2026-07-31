# C globals, failures, variadics, and `void`

<!-- spec-sections: 15.9, 15.10, 15.11, 15.12 -->

C globals and calls expose ordinary ABI values, not Arkoi failure or ownership
channels. This chapter defines global bindings, error translation, C variadic calls,
and the two meanings of C `void`.

!!! abstract "At a glance"

    - `import "C"` and `export "C"` bind module-level C global symbols.
    - Arkoi failures must be introduced or handled on the Arkoi side of the boundary.
    - Only imported C functions may contain a final `...` parameter.
    - A C `void` result has no Arkoi return type; `void*` uses `*c.void`.

## C global variables

### Importing a global

A module-level `import "C"` variable names the exact C object symbol:

```arkoi
import "C" library_version @c.int
import "C" errno @mut c.int
```

Without `@mut`, Arkoi can only read the imported object. With `@mut`, Arkoi can read
and write it:

```arkoi
version @c.int = library_version

errno = 0
current_error @c.int = errno
```

A correctly declared global load or store is not inherently unsafe. The import itself
is a trusted ABI contract: the programmer and build system must ensure the linked
symbol exists, denotes an object rather than a function, has compatible
representation, mutability, and storage duration, and is correctly aligned for the
declared type. A mismatch makes program behavior undefined.

Only C-compatible complete data types can be imported by value:

| Valid | Invalid |
| --- | --- |
| Core C ABI scalar | Resource |
| Raw pointer to a C-domain pointee | `string`, reference, or slice |
| `export "C"` enum | Optional or failure value |
| Complete `export "C"` data or union | Arkoi ABI scalar by value |
| Fixed array of C elements with a matching complete object type | Ordinary Arkoi aggregate or enum |
|  | Opaque imported C data by value |

Reading a pointer-valued global safely copies only its pointer value. Any later
dereference or pointer arithmetic remains unsafe:

```arkoi
import "C" global_buffer @*mut c.unsigned_char

pointer @*mut c.unsigned_char = global_buffer

unsafe:
    *pointer = 10
```

`@mut` describes Arkoi's permission to write. Omitting it does not promise that
external C code will leave the object unchanged.

### Exporting a global

`export "C"` defines a module-level Arkoi variable and exposes its Arkoi name as the
exact C symbol:

```arkoi
export "C" request_count @mut c.unsigned_int = 0
export "C" library_version @c.int = 1
```

The definition has C linkage and the target C representation. Its type must be
C-compatible, and its initializer must be a valid constant. It can use ordinary
immutable or mutable declaration syntax, but it cannot be a resource or need runtime
initialization.

`pub` independently controls whether another Arkoi module can name the declaration;
`export "C"` controls C symbol exposure.

Imported and exported globals do not imply volatile or atomic access. Memory-mapped
I/O, signal-shared state, and concurrency-sensitive C objects need dedicated
volatile or atomic operations when Arkoi specifies those features.

## Failure boundary

C-linked functions cannot declare Arkoi failure effects:

```arkoi
import "C" fun native_read(
    handle @*mut FILE,
) !IOFail @c.int
# Compile-time error: an imported C function cannot declare a failure effect

export "C" fun invalid_process(
    value @c.int,
) !ProcessFail @c.int:
    return value
# Compile-time error: an exported C function cannot declare a failure effect
```

The C ABI communicates only through C-compatible parameters, its return value,
referenced memory, and external state. Common C error channels are integer status
codes, null pointers, sentinels, output parameters, globals such as `errno`, and
library-specific error-query functions.

### Translating C errors into Arkoi failures

An ordinary wrapper explicitly inspects the C result before raising a failure:

```arkoi
import "C" fun native_open(
    path @*c.unsigned_char,
) @*mut FILE

fun open_file(
    path @*c.unsigned_char,
) !IOFail @*mut FILE:
    handle @*mut FILE

    unsafe:
        handle = native_open(path)

    if handle == null:
        fail IOFail.open_failed

    return handle
```

A C import itself is never fallible in Arkoi's type system. Its ordinary wrapper,
not the foreign declaration, introduces the Arkoi failure effect.

### Translating Arkoi failures into C results

An exported function handles every failure before returning and converts it to its
documented C convention:

```arkoi
fun perform_operation(
    value @s32,
) !ProcessFail @s32:
    if value < 0:
        fail ProcessFail.invalid_value

    return value * 2

export "C" fun process(
    value @c.int,
) @c.int:
    native_value @s32 = convert(value, s32) handle failure:
        yield -1

    native_result @s32 = perform_operation(native_value) handle failure:
        yield -1

    foreign_result @c.int = convert(native_result, c.int) handle failure:
        yield -1

    return foreign_result
```

No Arkoi failure may cross the boundary. In particular, postfix `!` cannot propagate
out of an exported function.

C unwinding, `longjmp`, hardware faults, and foreign exceptions are not Arkoi
failure effects. If foreign code violates its declared non-unwinding ABI contract or
uses an unsupported transfer across Arkoi frames, behavior is undefined.

## Imported C variadic functions

Only an `import "C" fun` can be variadic:

```arkoi
import "C" fun printf(
    format @*c.char,
    ...,
) @c.int
```

Arkoi-defined and exported functions cannot be variadic. The `...` marker:

- appears no more than once;
- is the final parameter-list entry;
- follows at least one fixed parameter;
- has neither a name nor an Arkoi type; and
- is valid only on an imported C function declaration.

Calls use ordinary syntax, but always require `unsafe`. Fixed parameters are checked
against their declared types; extra positional arguments form the variadic tail:

```arkoi
unsafe:
    written @c.int = printf(
        format,
        count,
        average,
    )
```

Every tail argument:

- is positional, never named;
- is evaluated from left to right with the other arguments;
- has a C-compatible scalar, enum, or raw-pointer representation admitted by the
  target variadic ABI;
- is not a resource, reference, slice, optional, failure value, or ordinary
  non-C-compatible aggregate; and
- is not supplied through `move(...)`, `clone(...)`, or another ownership operation.

### Default argument promotions

Arkoi applies every target C default promotion to tail arguments. This includes C
integer promotion for integer types narrower than target `c.int`, promotion from
target `c.float` to `c.double`, and any additional promotion required by the active
C ABI. Core C aliases determine the resulting target types.

```arkoi
small @c.short = 12
decimal @c.float = 3.5

unsafe:
    discard(printf(
        format,
        small,
        decimal,
    ))
```

Here the call uses the target integer promotion for `small` and passes `decimal` as
`c.double`.

The compiler validates fixed arguments and each tail argument's ABI compatibility,
but cannot prove a function-specific relationship such as agreement between a
`printf` format and its arguments. Violating that convention is undefined behavior.

An Arkoi `string` is not a C string and cannot be passed directly. A C string
argument needs a raw pointer to valid, null-terminated C-compatible storage:

```arkoi
format @*c.char = obtain_c_string_pointer()

unsafe:
    discard(printf(
        format,
        value,
    ))
```

C variadic support does not create general Arkoi variadic functions, argument packs,
tuple spreading, or variadic values.

## C `void` and `void*`

C `void` has two context-dependent mappings:

| C construct | Arkoi representation |
| --- | --- |
| `void` function result | No return type |
| `const void*` | `*c.void` |
| `void*` | `*mut c.void` |

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void

import "C" fun free(
    pointer @*mut c.void,
)
```

There is no general Arkoi value type for a C `void` result. For `void*`, the required
core `c` module supplies an opaque foreign marker, conceptually:

```arkoi
module c

import "C" data void:
    pass
```

`c.void` is provided by the core library rather than being a new built-in primitive.
It can occur only as a raw-pointer referent. It cannot exist, be constructed, copied,
passed, or returned by value; be dereferenced or indexed; appear as a by-value field;
have visible fields; or receive user-defined methods, hooks, or interface
implementations.

```arkoi
value @c.void
# Compile-time error: `c.void` cannot exist by value

export "C" data Invalid:
    field @c.void
    # Compile-time error: `c.void` cannot be an aggregate field by value
```

Pointers to `c.void` are ordinary nullable raw pointers. Converting between one and
a typed pointer requires explicit unsafe reinterpretation:

```arkoi
typed @*mut c.unsigned_char
raw @*mut c.void

unsafe:
    raw = reinterpret(typed, *mut c.void)
    typed = reinterpret(raw, *mut c.unsigned_char)
```

The conversion preserves pointee access: `*T` converts to `*c.void`, and `*mut T`
converts to `*mut c.void`. `readonly(...)` may reduce mutable access, but no safe
reinterpretation can manufacture mutable access from a read-only pointer.

## Related topics

- [C functions and ABI domains](functions-and-abi.md)
- [C data types and layout](types-and-layout.md)
- [Constants and globals](../constants-globals.md)
- [Failures](../failures.md)
- [Unsafe execution](../unsafe.md)
- [Raw pointers](../raw-pointers.md)
