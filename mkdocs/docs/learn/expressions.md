---
title: Arkoi expressions, conversions, and evaluation
description: Learn Arkoi 1.0 conversions, Boolean and numeric operations, evaluation order, assignment, result use, and pipelines.
---

# Expressions, conversions, and evaluation

Arkoi expressions produce typed values, while statements control where those
values go. The language makes conversion, arithmetic failure, evaluation order,
and discarded results explicit so that a compact expression still has a clear
contract.

!!! info "Arkoi 1.0 target-language illustration"

    The snippets on this page explain the **target language**. The current
    executable does not necessarily accept them. See
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying them today.

## Learning goals

By the end of this chapter, you should be able to:

- choose between implicit conversion, `convert`, `truncate`, and `bitcast`;
- compose strict Boolean and comparison expressions;
- select trapping, recoverable, or wrapping integer arithmetic;
- predict left-to-right evaluation and destination-first assignment;
- use or explicitly discard every produced value; and
- read and write a pipeline without hiding borrowing or ownership transfer.

## Exact types make conversion a decision

An implicit numeric conversion is permitted only when **every** possible value
of the source type is exactly representable by the destination. That includes
same-signedness integer widening, an unsigned-to-signed conversion whose target
covers the whole source range, and `f32` to `f64`:

```arkoi
small @u16 = 500
wide @u32 = small

sample @f32 = 1.25
precise @f64 = sample
```

Every other numeric conversion states its intent:

| Intent | Form | What can happen |
| --- | --- | --- |
| Preserve the numeric value when representable | `convert(value, T)!` | Checked; may produce `CoreFail.out_of_range` or `CoreFail.invalid_conversion` |
| Deliberately discard numeric information | `truncate(value, T)` | Integer narrowing keeps the low-order bits |
| Preserve equal-sized bits rather than the numeric value | `bitcast(value, T)` | No conversion or validation; safe only for the specified fixed-width numeric cases |

```arkoi
fun checked_count(raw @s32) !CoreFail @u32:
    return convert(raw, u32)!

word @u32 = 0x1234
low_byte @u8 = truncate(word, u8)

sample @f32 = 1.0
bits @u32 = bitcast(sample, u32)
```

Numbers never convert implicitly to `bool`, and Booleans never convert to
numbers. A `bitcast` involving validity-constrained types such as `bool`,
`char`, enums, pointers, references, or aggregates requires an `unsafe`
context and proof of all destination invariants.

### A conversion checklist

| Question | Choose |
| --- | --- |
| Does the destination represent the source type's complete range exactly? | Let the lossless implicit conversion apply. |
| Should an out-of-range value be recoverable? | Use `convert(value, T)!`. |
| Is information loss deliberate and part of the algorithm? | Use `truncate(value, T)`. |
| Do the bits themselves carry the meaning? | Use an allowed `bitcast(value, T)`. |

## Boolean logic and comparisons

Conditions and operands of Boolean operators must already be `bool`. Arkoi uses
the keywords `and`, `or`, and `not`; it does not use `&&`, `||`, or logical
`!`.

```arkoi
fun is_visible(
    value @s32,
    lower @s32,
    upper @s32,
    enabled @bool,
) @bool:
    return enabled and lower <= value and value <= upper
```

`and` evaluates its right side only when its left side is `true`; `or`
evaluates its right side only when its left side is `false`. This short-circuit
behavior is useful for guarding later work, but the guarded expression must
still have type `bool`.

One comparison expression contains exactly one comparison operator. Write a
range test as `lower <= value and value <= upper`; chained syntax such as
`lower <= value <= upper` is invalid.

| Operand kind | Available comparison behavior |
| --- | --- |
| Compatible numbers | Compare after the permitted lossless common-type conversion |
| Enums | `==` and `!=` between values of the same enum |
| Data aggregates | Structural equality and lexicographic ordering when all fields support the requested operator |
| Resource aggregates | Only operators supplied by their explicit comparison hooks |
| Arrays and slices | Element/content comparison; ordering is lexicographic |
| Optionals | `none` compares before present values, then the contained type decides |
| References | Compare their referent values, not their addresses |
| Raw pointers | Address equality only; no ordering |

Each comparison operator is enabled independently. Supporting `==` does not
automatically supply `!=` or any ordering operator.

Membership is a separate Boolean-producing operation. Write `item in
container` or `item not in container`; the container's `__contains__` hook
defines the test. A fallible hook instead uses `in!` or `not in!`, where the
`!` belongs to the membership operator. Arkoi evaluates the item before the
container.

## Integer arithmetic states its failure mode

Integer arithmetic behaves the same in debug and release builds. For addition,
subtraction, and multiplication, the spelling selects the contract:

| Mode | Add | Subtract | Multiply | Behavior |
| --- | --- | --- | --- | --- |
| Trapping | `+` | `-` | `*` | Stops execution on overflow |
| Recoverable | `+!` | `-!` | `*!` | Produces and propagates `CoreFail.out_of_range` |
| Wrapping | `+%` | `-%` | `*%` | Computes modulo the integer type's range |

```arkoi
fun extend_budget(base @u32, extra @u32) !CoreFail @u32:
    return base +! extra

fun next_machine_counter(current @u32) @u32:
    return current +% 1
```

`+!` is one recoverable operator spelling; its `!` is not a second postfix
operator. A surrounding `handle failure` may recover locally instead of
propagating the failure.

Integer `/` and `%` trap for a zero divisor; `/!` and `%!` make that condition
recoverable. Division can also overflow for signed minimum divided by `-1`.
There are no wrapping division or remainder forms.

Shifts use `<<` and `>>`, with recoverable `<<!` and `>>!` variants. An invalid
shift count traps or fails according to the selected form; left shift also
checks whether significant bits would be discarded. Floating-point arithmetic
does not use these integer overflow modes.

### Choosing an arithmetic mode

| Situation | Suitable mode |
| --- | --- |
| Overflow means the program violated an invariant | Ordinary trapping operator |
| Input-dependent overflow is an expected recoverable outcome | Recoverable `...!` operator |
| Modulo arithmetic is the intended algorithm | Wrapping `...%` operator |

## Evaluation follows the written order

Arkoi evaluates binary operands, call arguments, aggregate fields, and array
elements from left to right. Named call arguments follow their **textual**
order, not parameter-declaration order:

```arkoi
record_route(
    announce = announce_start(),
    origin = load_origin(),
    destination = load_destination(),
)
```

Here `announce_start()` runs first, then `load_origin()`, then
`load_destination()`, and only then does the call occur. Method receivers are
ordinary explicit first arguments and follow the same rule.

If an earlier expression fails, traps, returns, or otherwise exits control
flow, later expressions in that sequence do not run. Already-created temporary
resources receive their normal deterministic cleanup.

Postfix chains evaluate the base first and continue outward. Pipelines evaluate
their initial expression once and then each stage once from left to right.

## Assignment is destination-first and produces no value

Assignment is a statement, not an expression. A mutable binding may be assigned
again, but assignment cannot appear in a condition, argument, return, or another
assignment:

```arkoi
count @mut u32 = 0
count = 3
count += 1

first @mut u32 = 0
second @mut u32 = 0
first = second = 0 # Compile-time error: assignment does not chain.
```

For a computed destination, Arkoi resolves and validates the destination before
evaluating the replacement:

```arkoi
items[calculate_index()]! = create_value()
```

The base and index run first, and bounds are checked. Only then does
`create_value()` run. For a resource destination, Arkoi fully constructs the
replacement before dropping the old value, so failed construction leaves the
old resource intact.

A compound assignment resolves its destination exactly once:

```arkoi
values[index()]! += amount()
```

It then reads the old value, evaluates `amount()`, applies `+`, and stores the
result. Arithmetic mode remains explicit in forms such as `+!=` and `+%=`.
Arkoi has no increment or decrement expressions; use an appropriate compound
assignment instead.

## Produced results must have a destination

A value-producing expression cannot stand alone. Bind it, assign it, return it,
pass it onward, compose it into another expression, or discard it explicitly:

```arkoi
total @u32 = calculate_total()
send_total(total)
discard(calculate_preview())
```

`discard(expression)` evaluates its argument exactly once. A data result is
ignored; a successfully created resource result is cleaned up immediately.
A call that returns no value may stand alone without `discard`.

This rule catches accidentally ignored computations and resource-producing
calls while preserving an explicit escape hatch when the side effect is the
only desired result.

## Pipelines carry one visible value

A pipeline starts with one carrier and threads it through stages. Every stage
contains exactly one `_` placeholder:

```arkoi
fun clamp(value @s32, lower @s32, upper @s32) @s32:
    if value < lower:
        return lower
    elif value > upper:
        return upper
    else:
        return value

fun to_u32(value @s32) !CoreFail @u32:
    return convert(value, u32)!

fun scale(value @u32, factor @u32) !CoreFail @u32:
    return value *! factor

fun display_level(raw @s32) !CoreFail @u32:
    return (
        raw
        |> clamp(_, 0, 100)
        |> to_u32(_)!
        |> scale(_, 10)!
    )
```

The return value of a stage becomes the next carrier. A stage that returns no
value preserves the existing carrier, which makes in-place mutation compose
without a special return convention.

| Placeholder form | Meaning |
| --- | --- |
| `_` | Use the current data value or place it in the selected argument position |
| `&_` | Borrow the current carrier read-only |
| `&mut _` | Borrow it for mutation; a no-value stage preserves it |
| `move(_)` | Consume a resource carrier; a later stage needs the returned replacement |

The placeholder is not a binding, wildcard, or discard marker. A named
resource enters by ownership only through an explicit `move(name)` or a later
`move(_)` stage.

`|>` has lower precedence than every other expression operator and groups from
left to right. Multiline pipelines use parentheses because expressions continue
across lines only inside parentheses or brackets.

## Checkpoint: predict the calls

Assume every helper below logs when it runs. In what order do the calls occur?

```arkoi
result @u32 = (
    source()
    |> clamp(_, lower(), upper())
    |> to_u32(_)!
    |> scale(_, factor())!
)
```

<details markdown="1">
<summary>Show the answer</summary>

The order is `source()`, `lower()`, `upper()`, `clamp(...)`, `to_u32(...)`,
`factor()`, then `scale(...)`. The initial carrier runs once; each stage then
uses ordinary left-to-right argument evaluation before making its call.

</details>

## Continue

Now that value production and evaluation are explicit, continue to
[Data, resources, and ownership](ownership.md) to see how copying, movement,
borrowing, and cleanup affect those values.

Canonical rules:

- [Expressions and numeric operations](../language-specification/1.0.0/expressions-and-numeric-operations.md)
- [Comparisons and membership](../language-specification/1.0.0/comparisons-and-membership.md)
- [Evaluation order](../language-specification/1.0.0/evaluation-order.md)
- [Assignment and result use](../language-specification/1.0.0/assignment-and-result-use.md)
- [Pipeline expressions](../language-specification/1.0.0/pipelines.md)
- [Types and values](../language-specification/1.0.0/types-values.md)
- [Failures](../language-specification/1.0.0/failures.md)
