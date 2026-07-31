# Failures

<!-- spec-sections: 8.1 -->

Arkoi models recoverable failure as an explicit function effect rather than an
exception or first-class `Result` wrapper. Calls must propagate or handle that
effect visibly.

!!! abstract "At a glance"

    - A `failure` is a flat set of qualified, payload-free discriminants.
    - Every user failure set includes `CoreFail`.
    - `!FailureSet` appears between a function's parameters and return type.
    - `fail` produces a failure; postfix `!` propagates one.
    - `handle failure` recovers at expression level and `yield`s a value.
    - Recoverable failures and unrecoverable traps are distinct.

## Failure declarations

```arkoi
failure IOFail:
    not_found
    permission_denied
```

Failure members are compact, flat, enum-like discriminants. They carry no
message, payload, source exception, or stack trace and remain qualified by
their defining set, such as `IOFail.not_found`.

## `CoreFail`

The built-in set contains language-level recoverable failures:

```arkoi
failure CoreFail:
    none_access
    out_of_range
    invalid_conversion
    division_by_zero
    out_of_memory
```

Every user-defined failure set includes `CoreFail` through a failure-set
inclusion rule, not through object-oriented inheritance.

`CoreFail.out_of_memory` represents recoverable allocation failure. An
allocating operation must expose allocation failure through its declared effect
or handle it internally; allocation never adds an undeclared effect. In
particular, `clone(value)` exposes exactly its selected clone hook or contained
clone effect. An infallible clone hook must handle allocation failure itself.

## Declaring and producing effects

The effect follows the parameter list and precedes the return type:

```arkoi
fun read_file(path @&string) !IOFail @File:
    if length(path) == 0:
        fail IOFail.not_found

    return File.open(path)!
```

`fail IOFail.not_found` exits through the recoverable failure path. A successful
return remains governed by the function's declared return type.

## Explicit propagation

Failure propagation is never automatic:

```arkoi
file @File = read_file(&path)!
```

A fallible expression must use postfix `!` or be handled locally.

!!! failure "Compile-time error — unhandled failure effect"

    ```arkoi
    file @File = read_file(&path)
    ```

Postfix operators resolve one layer at a time. If a fallible expression succeeds
with an optional, the first `!` propagates its declared failure and the second
unwraps `none` as `CoreFail.none_access`:

```arkoi
item @Item = find_optional(id)!!
```

Cleanup still runs when propagation exits a scope.

## Combined failure sets

```arkoi
failure LoadFail = IOFail | ParseFail
```

Combination is transitive and creates no runtime wrapper. Values keep their
original qualified identities, such as `IOFail.not_found` or
`ParseFail.invalid_token`.

## Expression-level handling

```arkoi
profile @Profile = read_profile(id) handle failure:
    log_failure(failure)
    yield default_profile()
```

Within the handler, `failure` is the produced failure value. `yield` supplies
the result of the whole handler expression. Every handler path must:

- yield a compatible value;
- return from the enclosing function;
- produce or propagate a failure; or
- leave through another valid terminating construct.

## Failures versus traps

Failures are recoverable and part of static function effects. Traps are
unrecoverable and are not caught by `handle failure`. Features that can trap
state that behavior explicitly in their own chapters.

## Related topics

- [Functions and returns](functions-returns.md)
- [Calls and overloads](calls-overloads.md)
- [Types and values](types-values.md)
- [Cloning](cloning.md)
- [Resource lifecycle](resource-lifecycle.md)
