---
title: Your first Arkoi source file
description: Build a complete Arkoi 1.0 module, then learn typed bindings, function contracts, strict Boolean conditions, branches, and loops.
---

# Your first Arkoi source file

Arkoi starts with a named module, explicitly typed values, fixed function
contracts, and indentation-based statement blocks. This chapter builds those
pieces into a small calculation and shows where each declaration belongs.

!!! info "Arkoi 1.0 target-language illustration"

    The snippets on this page explain the **target language**. They are not
    promised to compile with the current executable. See
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying them with compiler 0.1.0.

## What you will learn

By the end of this chapter, you will be able to:

- recognize the shape of a complete Arkoi source file;
- declare typed, immutable, and mutable local bindings;
- write a function whose return contract is complete;
- use strict Boolean conditions in branches and loops; and
- explain why initialization and result use are checked statically.

## Begin with a complete module

Every Arkoi source file starts with exactly one `module` declaration. A runnable
program also provides `main`. Here is a complete target-language source file:

```arkoi title="learn/first_program.ark — complete Arkoi 1.0 target module"
module learn.first_program

fun triangular(limit @u32) @u32:
    total @mut u32 = 0
    next @mut u32 = 1

    while next <= limit:
        total = total + next
        next = next + 1

    return total

fun main():
    discard(triangular(10))
```

The module name gives the file a stable project identity. `main` is the entry
point. `triangular(10)` produces a result, so `discard(...)` says explicitly
that this small program does not need it. Arkoi rejects silently ignored
results.

The shorter blocks below are fragments taken from functions like these; they
omit the surrounding module only to keep one rule in view.

## Typed bindings

A binding places its type after `@`. Add binding-level `mut` only when the name
must be reassigned:

```arkoi
limit @u32 = 10
total @mut u32 = 0
next @mut u32 = 1
```

`limit` is initialized once. `total` and `next` may be assigned again. Arkoi
does not provide implicit zero or default initialization, and a use must be
definitely initialized on every path that reaches it.

Numeric literals receive a concrete type from context. Numbers, Booleans,
characters, string views, and `data` aggregates are data values; ordinary
assignment copies them. The next chapters develop the type model before moving
on to ownership-controlled resources.

## Functions state their result contract

A return type follows the parameter list. Every reachable normal path through a
value-returning function must return a compatible value:

```arkoi
fun clamp(
    value @s32,
    lower @s32,
    upper @s32,
) @s32:
    if value < lower:
        return lower
    elif value > upper:
        return upper
    else:
        return value
```

Parameters are immutable local bindings by default. A function with no declared
return type either reaches the body end or uses bare `return`; it cannot return
a value.

Colons begin blocks, indentation determines their contents, and every block
creates a lexical scope. Use `pass` when a permitted block is intentionally
empty.

## Conditions are strict Booleans

`if` is a statement and does not produce a value. Its condition must already be
`bool`; Arkoi has no truthiness conversion for numbers, text, pointers, or
optionals. Boolean operators are the keywords `and`, `or`, and `not`.

Branches participate in definite-initialization checking. An immutable binding
can be initialized once along every continuing branch:

```arkoi
fun rating(points @u32) @string_view:
    label @string_view

    if points >= 90:
        label = "high"
    elif points >= 60:
        label = "medium"
    else:
        label = "low"

    return label
```

Removing the `else` would leave a path on which `label` is uninitialized.

## Loops make mutation visible

`while` checks a `bool` before each iteration and may run zero times:

```arkoi
fun triangular(limit @u32) @u32:
    total @mut u32 = 0
    next @mut u32 = 1

    while next <= limit:
        total = total + next
        next = next + 1

    return total
```

Ordinary integer arithmetic has the specification's trapping overflow behavior;
recoverable and wrapping spellings are separate operations. Use `loop:` for
intentional indefinite iteration. `break` and `continue` target the nearest loop
unless a named loop is selected explicitly.

## Keep the model straight

| Question | Arkoi rule |
| --- | --- |
| Can this name be assigned again? | The binding has `@mut T`. |
| Can this condition be tested? | Its type is exactly `bool`. |
| Can this value-returning function finish here? | That path returns a value or leaves through another specified terminating path. |
| Does an inner declaration replace an outer one? | No. Local names are unique throughout a function; shadowing is forbidden. |

## Check your understanding

1. Why is `if 1:` invalid even though some languages treat `1` as true?
2. What becomes invalid if the final `else` is removed from `rating`?
3. Why do `total` and `next` use `@mut u32`, while `limit` uses `@u32`?

<details markdown="1">
<summary>Show the answers</summary>

1. An Arkoi condition must have the exact type `bool`; there is no numeric
   truthiness conversion.
2. `label` would not be definitely initialized on the path where neither test
   succeeds, so returning it would be rejected.
3. The loop reassigns `total` and `next`. `limit` is initialized once and only
   read afterward.

</details>

## Continue

Next, learn how Arkoi models built-in values, owned text, optionals, aggregates,
and enums in [Types, text, and aggregates](types-aggregates.md).

Canonical rules:

- [Source syntax](../language-specification/1.0.0/source-syntax.md)
- [Bindings and initialization](../language-specification/1.0.0/bindings-initialization.md)
- [Types and values](../language-specification/1.0.0/types-values.md)
- [Functions and returns](../language-specification/1.0.0/functions-returns.md)
- [Expressions and numeric operations](../language-specification/1.0.0/expressions-and-numeric-operations.md)
- [Control flow](../language-specification/1.0.0/control-flow.md)
