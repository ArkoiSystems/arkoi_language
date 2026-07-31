# Constants and module globals

<!-- spec-sections: 2.2–2.6 -->

Compile-time constants and module variables share Arkoi's restricted initializer
model. Module storage is deliberately limited so startup cannot allocate, fail,
or depend on runtime initialization order.

!!! abstract "At a glance"

    - `const` always has an explicit type and a compile-time initializer.
    - Module variables must have data types and compile-time initializers.
    - A narrow exception permits a module slice over compiler-proven static
      array storage.
    - Initializer dependencies are order-independent but must be acyclic.
    - Module declarations are private unless prefixed with `pub`.

## Compile-time constants

```arkoi
const WIDTH @usize = 4
const MAX_RETRIES @u32 = 5
pub const BUFFER_SIZE @usize = WIDTH * 256

type Buffer = [BUFFER_SIZE]u8
```

A constant:

- requires an explicit type and cannot be `mut`, reassigned, or shadowed;
- may be declared at module scope or inside a lexical block;
- is private at module scope unless declared `pub const`;
- may be used in array lengths, module initializers, and ordinary expressions;
- must evaluate completely at compile time; and
- must produce a value representable by its declared type.

Local constants remain visible only in their lexical block and follow the
function-wide no-shadowing rule.

## Constant expressions

| Allowed | Not allowed |
| --- | --- |
| Literals and other constants | Runtime state |
| Arithmetic, bitwise, comparison, and Boolean operators | User-defined calls or `const fun` |
| `convert`, `truncate`, safe numeric `bitcast` | Allocation, I/O, or resources |
| Constant optionals, fixed arrays, and data aggregates | References, raw pointers, or general slices |
| Field access and statically valid array indexing | Unsafe operations |
| | Failure propagation or handlers |

```arkoi
const WIDTH @usize = 4
const AREA @usize = WIDTH * 8
const ENABLED @bool = WIDTH > 2
const MASK @u32 = 0xff00 | 0x00ff

const ORIGIN @Point = Point(
    x = 0.0,
    y = 0.0,
)

const VALUES @[3]u32 = [10, 20, 30]
const SECOND @u32 = VALUES[1]
```

Compile-time arithmetic has the same semantics as runtime arithmetic. A trap,
invalid operation, or unrepresentable result during evaluation is a compile-time
error.

!!! failure "Compile-time error — runtime-dependent constant"

    ```arkoi
    size @usize = read_size()!
    const BUFFER_SIZE @usize = size
    ```

Constant values cannot contain resources, references, raw pointers, slices, or
other runtime-storage or lifetime-dependent values.

## Module variables

```arkoi
version @u32 = 1
pub counter @mut u32 = 0

const INITIAL_COUNT @u32 = 10
requests @mut u32 = INITIAL_COUNT
```

A module variable:

- may be immutable or mutable using normal binding syntax;
- is private unless prefixed with `pub`;
- must have a data type; and
- normally requires an initializer valid as a constant expression.

Local variables do not have the compile-time initializer restriction.
Resource-typed globals are forbidden; create resources inside functions and
pass them explicitly. `File`, `Socket`, and `Buffer` in examples are
illustrative user or library resource types, not language built-ins.

!!! failure "Compile-time error — resource global"

    ```arkoi
    global_file @File = File.open(&path)!
    ```

This restriction removes runtime module initialization order, startup failure,
global resource cleanup, and long-lived resource-borrowing problems.

## Static slice initializer exception

One narrow exception permits a compiler-verified module slice over
module-level fixed-array storage with static lifetime. This does not make slices
general constant values. The complete backing-storage, mutability, and unsafe
fallback rules are defined once under [module slices](slices.md#module-slices).

## Dependency order and cycles

Module constants and module data initializers may refer to later declarations.
The compiler evaluates their dependency graph rather than relying on textual
order:

```arkoi
const WIDTH @usize = HEIGHT * 2
const HEIGHT @usize = 4
```

The graph must be acyclic. Both direct and indirect cycles are rejected, and
the diagnostic identifies participating declarations.

!!! failure "Compile-time error — initializer cycle"

    ```arkoi
    const FIRST @usize = SECOND + 1
    const SECOND @usize = FIRST + 1
    ```

Function recursion is unrelated: calls are unavailable in constant and module
data initializers.

## Related topics

- [Bindings and initialization](bindings-initialization.md)
- [Types and values](types-values.md)
- [Slices](slices.md)
- [Arrays](arrays.md)
