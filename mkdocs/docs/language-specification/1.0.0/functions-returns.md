# Functions and returns

<!-- spec-sections: 4.1–4.3, 4.8–4.10 -->

Arkoi functions have fixed, statically typed parameter lists and explicit return
contracts. The compiler verifies every reachable value-returning path.

!!! abstract "At a glance"

    - Declare functions with `fun`; the return type follows the parameter list.
    - Parameters are immutable local bindings unless marked `@mut`.
    - `@mut` on a parameter does not mutate the caller's value.
    - A declared return type requires `return value` on every normal path.
    - Recursion is allowed; tail-call optimization is not guaranteed.
    - An executable has exactly one parameterless, value-less module `main`.

## Functions and parameters

```arkoi
fun add(left @u32, right @u32) @u32:
    return left + right
```

Parameters are immutable local bindings by default. `@mut` permits reassignment
of only that local parameter binding:

```arkoi
fun increment_copy(value @mut u32) @u32:
    value = value + 1
    return value
```

Caller-visible mutation requires mutable access, commonly a mutable reference.
A reference parameter transparently designates its referent:

```arkoi
fun increment(value @&mut u32):
    value = value + 1
```

Binding-level parameter mutability is not part of function type identity and
does not distinguish overloads. Reference mutability and resource ownership
modes, as well as infallible versus fallible mode, do; see [Calls and
overloads](calls-overloads.md).

## Return forms

### No declared return type

A function without a return type may reach the end or exit with bare `return`:

```arkoi
fun log_value(value @u32):
    if value == 0:
        return

    print_u32(value)
```

It cannot return a value.

!!! failure "Compile-time error — value from a value-less function"

    ```arkoi
    fun invalid():
        return 42
    ```

### Declared return type

A value-returning function uses `return value`. Bare `return` is invalid.

```arkoi
fun answer() @u32:
    return 42
```

!!! failure "Compile-time error — missing return value"

    ```arkoi
    fun answer() @u32:
        return
    ```

## Complete return-path checking

No reachable path of a value-returning function may reach the body end. Each
path must:

1. return a compatible value;
2. leave through `fail` or failure/absence propagation;
3. enter a loop with no reachable normal exit; or
4. terminate through an explicit unrecoverable operation.

```arkoi
fun classify(value @s32) @u32:
    if value < 0:
        return 0
    else:
        return 1
```

!!! failure "Compile-time error — incomplete return paths"

    ```arkoi
    fun classify(value @s32) @u32:
        if value < 0:
            return 0
    ```

A failure path needs no successful return:

```arkoi
fun load(id @u32) !LoadFail @Item:
    if id == 0:
        fail LoadFail.invalid_id

    return find_item(id)!
```

Likewise, a loop that cannot complete normally satisfies the rule:

```arkoi
fun run_forever() @u32:
    loop:
        process_events()
```

`pass` never counts as a return.

## Recursion

Direct and mutual recursion are valid, independent of module declaration order:

```arkoi
fun factorial(value @u32) @u32:
    if value == 0:
        return 1

    return value * factorial(value - 1)
```

Arkoi does not guarantee tail-call optimization. Exhausting the call stack is
an unrecoverable trap. When a recursive frame exits, its resources follow the
ordinary lexical cleanup rules.

## Fixed arity

Every Arkoi-defined function has a fixed parameter list. Homogeneous
variable-length input is passed as an array or slice.

```arkoi
fun print_values(values @[]u32):
    for value @u32 in values:
        print_u32(value)
```

!!! failure "Compile-time error — Arkoi variadic parameter"

    ```arkoi
    fun log(values @...):
        pass
    ```

Heterogeneous Arkoi variadics are not supported. Imported C functions have a
separate, restricted C-variadic facility.

## Program entry point

An executable defines exactly one `main`:

```arkoi
fun main():
    run_application()
```

`main` must be module-level, named exactly `main`, have no parameters or return
value, and be neither a method nor associated function. It is compiler
discovered and therefore is not declared `pub`.

The entry point may declare a failure effect:

```arkoi
fun main() !AppFail:
    run_application()!
```

A failure propagated out of `main` terminates the program unsuccessfully.

## Related topics

- [Calls and overloads](calls-overloads.md)
- [Failures](failures.md)
- [Methods and visibility](methods-visibility.md)
- [Resource lifecycle](resource-lifecycle.md)
