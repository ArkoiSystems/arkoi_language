---
title: Arkoi 1.0 constants and module globals
description: Arkoi 1.0 target-language rules for constant expressions, module globals, initialization, visibility, and ownership.
---

# Constants and module globals

<!-- spec-sections: 2.2–2.6 -->

Compile-time constants and module variables are initialized at compile time, but
they have different runtime semantics. A constant is an addressless value. A
module variable is stable program-lifetime storage, even when it is immutable.

!!! abstract "At a glance"

    - Write constants as `NAME @const T = value`; the initializer is mandatory.
    - Constants have no address, storage identity, or runtime object.
    - Module variables have stable storage and may be immutable or `mut`.
    - Module variables must have data types and static initializers.
    - A narrow exception permits a module slice over compiler-proven static
      array storage.
    - Initializer dependencies are order-independent but must be acyclic.
    - Module declarations are private unless prefixed with `pub`.

## Compile-time constants

```arkoi
WIDTH @const usize = 4
MAX_RETRIES @const u32 = 5
pub BUFFER_SIZE @const usize = WIDTH * 256

type Buffer = [BUFFER_SIZE]u8
```

A constant:

- requires an explicit type and initializer;
- is declared as `NAME @const T = expression`;
- cannot combine `const` with `mut` or `own`;
- cannot be reassigned, use delayed initialization, or be shadowed;
- may be declared at module scope or inside a lexical block;
- is private at module scope unless `pub` precedes its name;
- may be used in array lengths, module initializers, and ordinary expressions;
- must evaluate completely at compile time; and
- must produce a value representable by its declared type.

Local constants remain visible only in their lexical block and follow the
function-wide no-shadowing rule.

## Constants have no address

A constant declaration does not allocate storage and never denotes a place.
Using a constant in a runtime expression produces its value without giving the
declaration an address. Consequently, a constant cannot be assigned, borrowed,
passed to `address(...)`, or used as static slice backing storage.

This applies recursively to composite constant values: `VALUES[0]` below is a
value, not an array element place with stable storage.

!!! danger "Compile-time error — an addressless constant"

    ```arkoi
    VALUES @const [3]u32 = [10, 20, 30]

    reference @&u32 = &VALUES[0]
    pointer @*u32 = address(VALUES[0])
    ```

## Constant expressions

| Allowed | Not allowed |
| --- | --- |
| Literals and other constants | Runtime state |
| Arithmetic, bitwise, comparison, and Boolean operators | User-defined calls or `const fun` |
| `convert`, `truncate`, safe numeric `bitcast` | Allocation, I/O, or resources |
| Constant optionals, fixed arrays, and data aggregates | References, raw pointers, or general slices |
| Literal-backed `string_view` values | Runtime-borrowed `string_view` values |
| Field access and statically valid array indexing | Unsafe operations |
| | Failure propagation or handlers |

```arkoi
WIDTH @const usize = 4
AREA @const usize = WIDTH * 8
ENABLED @const bool = WIDTH > 2
MASK @const u32 = 0xff00 | 0x00ff

ORIGIN @const Point = Point(
    x = 0.0,
    y = 0.0,
)

VALUES @const [3]u32 = [10, 20, 30]
SECOND @const u32 = VALUES[1]
```

Compile-time arithmetic has the same semantics as runtime arithmetic. A trap,
invalid operation, or unrepresentable result during evaluation is a compile-time
error.

!!! danger "Compile-time error — runtime-dependent constant"

    ```arkoi
    size @usize = read_size()!
    BUFFER_SIZE @const usize = size
    ```

Constant values cannot contain resources, references, raw pointers, slices, or
other runtime-storage or lifetime-dependent values. A `string_view` whose
storage derives entirely from literals is permitted.

## Module variables

```arkoi
version @u32 = 1
pub counter @mut u32 = 0

INITIAL_COUNT @const u32 = 10
requests @mut u32 = INITIAL_COUNT
```

A module variable:

- may be immutable or mutable using normal binding syntax;
- is private unless prefixed with `pub`;
- must have a data type; and
- normally requires a valid static initializer.

Unlike a constant, every module variable is an addressable place with stable
program-lifetime storage. An immutable module variable cannot be reassigned, but
it can be borrowed or passed to `address(...)`. Reading it in ordinary code is a
load from storage, not a constant expression.

```arkoi
version @u32 = 1

fun inspect_version():
    reference @&u32 = &version
    pointer @*u32 = address(version)
```

A static initializer is a constant expression extended with references to the
initial values of other module data variables. This extension applies only while
the compiler builds initial module storage; it does not turn those variables
into constants.

Local variables do not have the compile-time initializer restriction.
Resource-typed globals are forbidden; create resources inside functions and
pass them explicitly. `File`, `Socket`, and `Buffer` in examples are
illustrative user or library resource types, not language built-ins.

!!! danger "Compile-time error — resource global"

    ```arkoi
    global_file @File = File.open(path)!
    ```

This restriction removes runtime module initialization order, startup failure,
global resource cleanup, and long-lived resource-borrowing problems.

## Static slice initializer exception

One narrow exception permits a compiler-verified module slice over
module-level fixed-array storage with static lifetime. This does not make slices
general constant values. The complete backing-storage, mutability, and unsafe
fallback rules are defined once under [module slices](slices.md#module-slices).

## Dependency order and cycles

Module constants and module static initializers may refer to later declarations.
The compiler evaluates their dependency graph rather than relying on textual
order:

```arkoi
WIDTH @const usize = HEIGHT * 2
HEIGHT @const usize = 4
```

When one static initializer names another module variable, it denotes that
declaration's initial value rather than performing a runtime load. The graph
must be acyclic. Both direct and indirect cycles are rejected, and the
diagnostic identifies participating declarations.

!!! danger "Compile-time error — initializer cycle"

    ```arkoi
    FIRST @const usize = SECOND + 1
    SECOND @const usize = FIRST + 1
    ```

Function recursion is unrelated: calls are unavailable in constant and module
static initializers.

## Related topics

- [Bindings and initialization](bindings-initialization.md)
- [Types and values](types-values.md)
- [Slices](slices.md)
- [Arrays](arrays.md)
