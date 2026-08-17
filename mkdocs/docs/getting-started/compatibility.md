---
title: Current compiler and Arkoi 1.0 compatibility
description: Translate common syntax between the executable compiler subset and the target Arkoi 1.0 language.
---

# Current compiler vs. Arkoi 1.0

The current compiler is an implementation snapshot. The
[Arkoi 1.0 specification](../language-specification/1.0.0/index.md) is the
language target, not a promise that every documented construct compiles today.

Use current-compiler syntax for repository examples and experiments. Use 1.0
syntax when discussing or designing the eventual language.

## Common differences

| Area | Current compiler | Arkoi 1.0 target |
| --- | --- | --- |
| Entry point | Introductory examples use a parameterless `main` with a numeric return that becomes the process status; entry-point signature validation is incomplete | Exactly one parameterless, value-less `fun main():` |
| Function returns | Every function declares a return type and `return` has a value | Functions may be value-less and use bare `return` |
| Reassignment | A typed local such as `count @u32 = 0` can be reassigned | Bindings are immutable by default; use `count @mut u32 = 0` |
| Conditional chain | `else if condition:` | `elif condition:` |
| Boolean operators | `&&` and `||`; current conversions also allow boolean/numeric mixtures | `and`, `or`, and `not`, with strict `bool` operands |
| Control flow | `if`, `else if`, `else`, `while`, and `return` | Also specifies `loop`, `for`, `break`, `continue`, named loops, and failure flow |
| Values and types | Fixed-width integers, `usize`, `ssize`, floats, and `bool` | Also specifies characters, strings, optionals, aggregates, enums, references, slices, pointers, and more |
| Conversion | The current subset performs its own implicit conversions and accepts postfix type casts such as `value @u32` | Lossless implicit conversion plus explicit `convert`, `truncate`, and `bitcast` operations |
| Larger language systems | Not implemented as a complete set | Modules, ownership, resources, interfaces, failures, unsafe code, and C interoperability are specified |

This table highlights learner-facing differences; it is not an exhaustive
implementation-status matrix. The checked-in examples and tests remain the
authority for what the current executable accepts.

## The same idea in both dialects

Current compiler form:

```arkoi
fun classify(value @s32) @u32:
    result @u32 = 0
    if value > 0:
        result = 1
    else if value < 0:
        result = 2
    return result
```

Arkoi 1.0 target form:

```arkoi
fun classify(value @s32) @u32:
    result @mut u32 = 0
    if value > 0:
        result = 1
    elif value < 0:
        result = 2
    return result
```

The current introductory examples use an entry point with an observable numeric
return value:

```arkoi
fun main() @s32:
    return 0
```

The 1.0 entry point is value-less:

```arkoi
fun main():
    run_application()
```

## Where to look next

- For code you can run now, start with the [current examples](examples.md).
- For current flags and artifacts, use the [CLI reference](cli.md).
- For target syntax, use the [1.0 quick reference](../language-specification/1.0.0/quick-reference.md).
- For the complete target rules behind the examples above, see
  [functions and returns](../language-specification/1.0.0/functions-returns.md),
  [bindings and initialization](../language-specification/1.0.0/bindings-initialization.md),
  [control flow](../language-specification/1.0.0/control-flow.md), and
  [numeric expressions](../language-specification/1.0.0/expressions-and-numeric-operations.md).
