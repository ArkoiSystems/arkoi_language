---
title: Arkoi optional values and recoverable failures
description: Learn how Arkoi 1.0 models absence, declares recoverable failures, propagates them, and recovers locally.
---

# Optional values and recoverable failures

Arkoi makes recoverable failure part of a function's static contract. It is an
effect that a call must handle or propagate visibly, not an exception and not a
first-class result wrapper.

!!! info "Arkoi 1.0 target-language illustration"

    The snippets on this page explain the **target language**. They are not
    promised to compile with the current executable. See
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying them with compiler 0.1.0.

## What you will learn

By the end of this chapter, you will be able to:

- use `?T` and `none` when a value may be absent;
- distinguish absence, recoverable failure, and an unrecoverable trap;
- declare and produce a failure effect;
- propagate a fallible expression explicitly; and
- recover from a failure with an expression-level handler.

This chapter assumes the exact types and postfix expressions introduced in
[Types, text, and aggregates](types-aggregates.md) and
[Expressions and conversions](expressions.md).

## Model ordinary absence with an optional

`?T` contains either a `T` or `none`. Optional chaining and coalescing keep the
absence path visible without turning it into an error:

```arkoi title="fragment — Arkoi 1.0 target"
profile @?Profile = find_profile()
name @?string_view = profile?.display_name
display @string_view = name ?? "anonymous"
```

`profile?.display_name` reads the field only when a profile is present. The
result remains optional. `??` evaluates its right side lazily and uses it only
when the left side is absent.

Two postfix forms deliberately give absence different contracts:

| Form | If the optional is `none` |
| --- | --- |
| `value?` | Return `none` from the enclosing optional-returning function. |
| `value!` | Produce `CoreFail.none_access` through the enclosing failure effect. |

Use an optional when “not found” is an expected successful result. Use a
failure when the caller must acknowledge that an operation did not complete.

## Declare and produce a failure

A user failure is a flat set of qualified, payload-free members:

```arkoi
failure ScoreFail:
    negative

fun score_from(raw @s32) !ScoreFail @u32:
    if raw < 0:
        fail ScoreFail.negative

    return convert(raw, u32)!
```

The `!ScoreFail` effect appears after the parameters and before the successful
return type. `fail ScoreFail.negative` exits through that effect; the normal path
returns a `u32`.

Every user failure set includes the built-in `CoreFail` set. That is why the
checked conversion's possible `CoreFail` can be propagated from this function.

Larger APIs can combine existing sets without wrapping their values:

```arkoi title="Arkoi 1.0 target"
failure LoadFail = IOFail | ParseFail
```

The original identities remain qualified, such as `IOFail.not_found` and
`ParseFail.invalid_token`.

## Propagation is explicit

A fallible expression must attach postfix `!` or a local handler:

```arkoi
fun require_score(raw @s32) !ScoreFail @u32:
    score @u32 = score_from(raw)!
    return score
```

If `score_from` fails, postfix `!` returns that failure from `require_score`.
No successful value is produced on that path. Any initialized resources leaving
scope are still cleaned up before propagation completes.

Leaving off the postfix marker is a compile-time error because the effect would
be unhandled.

## Handle a failure as an expression

`handle failure` recovers locally, and `yield` supplies the value of the whole
expression:

```arkoi
fun score_or_zero(raw @s32) @u32:
    score @u32 = score_from(raw) handle failure:
        # `failure` is the qualified failure value produced by the call.
        yield 0

    return score
```

Every handler path must yield a compatible value or leave through another valid
terminating path. The handler sees the produced value through its `failure`
binding. Arkoi 1.0 does not define general pattern matching or failure-value
equality, so this tour applies one recovery path; an API that needs different
policies should expose an explicit discriminator or separate operations.

## Resolve nested layers one at a time

`?T` represents either a `T` or `none`. Postfix `?` propagates absence from an
optional-returning function. Postfix `!` propagates a declared failure; when
applied to an optional, it instead unwraps the value or produces
`CoreFail.none_access` where the enclosing effect permits it.

When a call is both fallible and successfully returns an optional, the layers
are resolved one at a time. `lookup()!!` first propagates the call's failure and
then treats absence as `CoreFail.none_access`.

## Failures are not traps

Recoverable operations state their failure effects and can be handled. Traps
are unrecoverable and are not caught by `handle failure`. For example, ordinary
integer division `/` traps for a zero divisor, while `/!` is the recoverable
form that produces `CoreFail.division_by_zero` and propagates it.

Choose the form that matches the contract rather than assuming that every
runtime problem becomes a failure.

## Check your understanding

1. When is `none` a better model than a failure member such as `not_found`?
2. What happens to initialized resources when postfix `!` leaves a scope?
3. What are the two layers resolved by `lookup()!!`?
4. Can `handle failure` catch an ordinary integer-division trap?

<details markdown="1">
<summary>Show the answers</summary>

1. Use `none` when absence is an ordinary successful outcome that callers may
   chain or replace with a fallback.
2. Their normal deterministic cleanup still runs before propagation completes.
3. The first `!` propagates the call's declared failure; the second unwraps the
   successful optional or produces `CoreFail.none_access`.
4. No. A trap is unrecoverable and is not part of the function's failure effect.

</details>

## Continue

Next, apply exact types, ownership modes, and effects at call sites in
[Functions, calls, and methods](calls-methods.md).

Canonical rules:

- [Failures](../language-specification/1.0.0/failures.md)
- [Optional values](../language-specification/1.0.0/types-values.md#optional-values)
- [Functions and returns](../language-specification/1.0.0/functions-returns.md)
- [Calls and overloads](../language-specification/1.0.0/calls-overloads.md)
- [Expressions and numeric operations](../language-specification/1.0.0/expressions-and-numeric-operations.md)
- [Resource lifecycle](../language-specification/1.0.0/resource-lifecycle.md)
