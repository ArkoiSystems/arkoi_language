# Unsafe execution

<!-- spec-sections: 8.2 -->

Unsafe contexts permit operations whose invariants the compiler cannot fully
verify. They relax specific safety checks without disabling static typing.

!!! abstract "At a glance"

    - Use an `unsafe:` block for a local unsafe region.
    - An `unsafe fun` makes its complete body an unsafe context.
    - Calling an unsafe function requires another unsafe context.
    - Unsafe code remains type-checked.
    - A safe wrapper is valid only when it enforces every unsafe invariant.

## Unsafe blocks

```arkoi
unsafe:
    value @u32 = *pointer
```

The block permits explicitly unsafe operations such as raw-pointer dereference.
Like every block, an intentionally empty unsafe block would contain `pass`.

## Unsafe functions

```arkoi
unsafe fun read_raw(pointer @*u8) @u8:
    return *pointer
```

The complete body is an unsafe context. Calling this function is allowed only
from an unsafe block or another unsafe function:

```arkoi
unsafe:
    byte @u8 = read_raw(pointer)
```

!!! failure "Compile-time error — unsafe call from safe context"

    ```arkoi
    byte @u8 = read_raw(pointer)
    ```

## What unsafe does—and does not do

Unsafe permits operations the compiler cannot prove safe. It does not disable
type checking, make invalid memory access defined, or turn a violated unsafe
contract into a recoverable failure.

A safe function may contain a carefully scoped unsafe block and expose a safe
interface only when it validates and maintains all necessary invariants:

```arkoi
fun read_first(items @[]u8) !CoreFail @u8:
    if length(items) == 0:
        fail CoreFail.out_of_range

    unsafe:
        pointer @*u8 = pointer(items)
        return *pointer
```

Here the safe interface is responsible for bounds, lifetime, alignment, and
element validity before dereferencing. The exact obligations depend on the
unsafe operation being wrapped.

## Related topics

- [Raw pointers](raw-pointers.md)
- [Function pointers](function-pointers.md)
- [Slices](slices.md)
- [Failures](failures.md)
- [Source syntax](source-syntax.md)
