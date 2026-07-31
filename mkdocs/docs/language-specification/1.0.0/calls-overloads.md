# Calls and overloads

<!-- spec-sections: 4.4–4.6 -->

Arkoi resolves overloaded functions, methods, associated functions, and hooks
at compile time by exact parameter signature. Named arguments are part of the
source API, but not type or ABI identity.

!!! abstract "At a glance"

    - Overloads differ by complete name and ordered parameter signature.
    - Arkoi performs no implicit numeric conversion or best-match ranking.
    - Return type, failure effect, visibility, parameter names, and local
      parameter `mut` do not distinguish overloads.
    - Positional arguments precede named arguments; every parameter is supplied
      exactly once.
    - Parameter names in a public API are source-compatibility commitments.

## Overload identity

```arkoi
fun parse(value @u32) @Node:
    return parse_number(value)

fun parse(value @&string) @Node:
    return parse_text(value)
```

An overload's identity consists of:

- its complete qualified name;
- its parameter count;
- its ordered parameter types; and
- parameter ownership and reference-mutability modes.

For a method, the receiver is one of those parameters.

| Does distinguish | Does not distinguish |
| --- | --- |
| `T` versus `&T` versus `&mut T` | Parameter name |
| Data parameter versus `@own Resource` | Return type |
| Ordered parameter type sequence | Failure effect |
| Receiver access mode | Visibility |
| | Binding-level parameter `@mut` |
| | Function-body behavior |

!!! failure "Compile-time errors — identical parameter signatures"

    ```arkoi
    fun convert_value(value @u32) @u64:
        return 0

    fun convert_value(value @u32) @f64:
        return 0.0
    ```

    The return type cannot distinguish these definitions.

    ```arkoi
    fun load(path @&string) @File:
        return open_infallibly(path)

    fun load(source @&string) !IOFail @File:
        return File.open(source)!
    ```

    Parameter names and the failure effect also do not distinguish definitions.

Similarly, changing only `value @T` to `value @mut T` creates no overload because
that `mut` affects only the callee's local binding.

## Resolution algorithm

At a call site, the compiler:

1. resolves the complete function or member name;
2. collects visible declarations;
3. maps positional and named arguments to each candidate;
4. rejects candidates with incompatible count, names, or exact types; and
5. requires exactly one candidate to remain.

No candidate or multiple candidates is a compile-time error. Arkoi has no
preferred-type, conversion-cost, or “best match” ranking.

### Exact types and conversions

```arkoi
fun process(value @u32):
    print_u32(value)

fun process(value @u64):
    print_u64(value)

small @u16 = 10
process(convert(small, u32)!)
```

!!! failure "Compile-time error — no implicit numeric conversion"

    ```arkoi
    small @u16 = 10
    process(small)
    ```

Context-dependent literals can match multiple overloads:

!!! failure "Compile-time error — ambiguous literal"

    ```arkoi
    fun store(value @u32):
        print_u32(value)

    fun store(value @u64):
        print_u64(value)

    store(10)
    ```

Use a typed binding or explicit conversion to select one. Transparent aliases
preserve identity, so an alias and its target cannot define distinct overloads.

!!! failure "Compile-time error — transparent alias collision"

    ```arkoi
    type UserId = u64

    fun find(value @u64):
        print_u64(value)

    fun find(value @UserId):
        print_u64(value)
    ```

Methods, associated functions, and reserved hooks use this same mechanism.
Operator resolution first chooses its normal or reverse hook family and then
performs exact overload selection.

Overload sets are compile-time name-resolution structures, not values. To
obtain a pointer to one named function, use `address(...)` with enough expected
function-pointer type context; see [Function pointers](function-pointers.md).

## Positional and named arguments

```arkoi
fun copy_file(
    source @&string,
    destination @&string,
    overwrite @bool,
) !IOFail:
    perform_copy(source, destination, overwrite)!
```

Calls may be positional, named, or positional followed by named:

```arkoi
copy_file(&source, &destination, true)!

copy_file(
    source = &source,
    destination = &destination,
    overwrite = true,
)!

copy_file(
    &source,
    destination = &destination,
    overwrite = true,
)!
```

After the first named argument, no positional argument may appear. Unknown
names, duplicate arguments, omitted parameters, and extra arguments are
compile-time errors. Every declared parameter must be supplied exactly once.
Named and positional arguments obey identical borrowing and ownership rules:

```arkoi
store(file = move(file))
```

## Parameter names as API

Named arguments must use the declaration's parameter names. Renaming a public
parameter is therefore a breaking source change for named-argument callers.

Parameter names are not part of runtime function type, ABI identity, calling
convention, or function-pointer compatibility. Private parameters may be
renamed when all call sites are updated.

## Fallible calls

Failure propagation is explicit:

```arkoi
file @File = File.open(&path)!
```

For a fallible call whose successful type is optional, the first `!` propagates
the call's failure and a second `!` unwraps `none`:

```arkoi
item @Item = find_optional(id)!!
```

The second operation may propagate `CoreFail.none_access`. Local handling is
described in [Failures](failures.md).

## Related topics

- [Functions and returns](functions-returns.md)
- [Function pointers](function-pointers.md)
- [Methods and visibility](methods-visibility.md)
- [Compiler hooks](compiler-hooks.md)
- [Failures](failures.md)
