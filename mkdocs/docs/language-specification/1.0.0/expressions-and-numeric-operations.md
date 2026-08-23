---
title: Arkoi 1.0 expressions and numeric operations
description: Arkoi 1.0 target-language rules for expression results, numeric conversions, arithmetic, overflow, and explicit operations.
---

# Expressions and numeric operations

<!-- spec-sections: 9.1–9.5 -->

This page defines Arkoi's boolean expressions, numeric conversions, integer arithmetic modes, division, remainder, and shifts.

!!! abstract "At a glance"
    Arkoi has strict booleans and no truthiness conversion. Implicit numeric conversion is lossless only. Integer arithmetic makes trapping, recoverable, and wrapping behavior explicit in the operator spelling.

## Boolean expressions

`bool` has the values `true` and `false`. Conditions and boolean operands must already have type `bool`; numbers, pointers, text values, and optionals do not convert to booleans.

The boolean operators are keywords:

| Operation | Syntax | Evaluation |
| --- | --- | --- |
| Conjunction | `left and right` | Evaluates `right` only when `left` is `true` |
| Disjunction | `left or right` | Evaluates `right` only when `left` is `false` |
| Negation | `not value` | Produces the opposite boolean |

Arkoi does not use `&&`, `||`, or logical `!`.

## Numeric conversion

An implicit numeric conversion is allowed only when **every** source value is exactly representable in the destination type:

- widening between integers of the same signedness;
- unsigned to signed when the signed destination covers the source's complete range;
- `f32` to `f64`.

All other numeric conversions are explicit. Numbers and booleans never implicitly convert to one another.

| Operation | Example | Contract |
| --- | --- | --- |
| Checked conversion | `converted @u32 = convert(value, u32)!` | Safe and fallible; may produce `CoreFail.out_of_range` or `CoreFail.invalid_conversion` |
| Truncation | `small @u8 = truncate(value, u8)` | Intentionally discards information; integer narrowing keeps the low-order bits |
| Bit reinterpretation | `bits @u32 = bitcast(number, u32)` | Preserves the exact bits, performs no numeric conversion, and requires equal-sized types |

### Bitcasts

Safe `bitcast` is limited to equal-sized fixed-width integer and floating-point types for which every source bit pattern is a valid destination value.

```arkoi
bits @u32 = bitcast(number, u32)
number @f32 = bitcast(bits, f32)
signed @s32 = bitcast(bits, s32)
```

Types with additional validity requirements require an [unsafe context](unsafe.md). This includes booleans, characters, restricted-discriminant enums, references, raw pointers, and aggregates.

```arkoi
unsafe:
    flag @bool = bitcast(byte, bool)
    character @char = bitcast(bits, char)
    pointer @*Item = bitcast(address, *Item)
    reference @&Item = bitcast(pointer, &Item)
    item @Item = bitcast(raw, Item)
```

Unsafe bitcasts still require equal sizes and a compile-time-known layout. They perform no validation. The programmer must satisfy every destination validity, alignment, lifetime, initialization, and ownership requirement; violating one is undefined behavior.

A named resource must be transferred explicitly:

```arkoi
unsafe:
    converted @OtherResource = bitcast(move(resource), OtherResource)
```

The source becomes uninitialized and the destination becomes the sole owner of the transferred bits. Unsafe `bitcast` never implicitly copies a resource.

## Integer arithmetic modes

Integer behavior is identical in debug and release builds. These arithmetic variants do not apply to floating-point values.

| Operation | Trapping | Recoverable | Wrapping |
| --- | --- | --- | --- |
| Addition | `+` | `+!` | `+%` |
| Subtraction | `-` | `-!` | `-%` |
| Multiplication | `*` | `*!` | `*%` |

Ordinary arithmetic traps on overflow. Recoverable arithmetic produces `CoreFail.out_of_range`; the `!` is part of the operator and propagates the failure, although the expression may instead use `handle failure`. Wrapping arithmetic is modular.

## Division and remainder

| Operation | Trapping form | Recoverable form | Failure or trap |
| --- | --- | --- | --- |
| Division | `/` | `/!` | Zero divisor; also signed minimum divided by `-1` when the quotient is unrepresentable |
| Remainder | `%` | `%!` | Zero divisor |

Signed integer division truncates toward zero, and remainder follows that division convention. Signed minimum remainder `-1` is `0` and does not trap. Recoverable division produces `CoreFail.division_by_zero` or `CoreFail.out_of_range`; recoverable remainder produces only `CoreFail.division_by_zero`.

There are no wrapping division or remainder operators.

## Shifts

```arkoi
shifted_left @u32 = value << count
shifted_right @u32 = value >> count
```

A shift count must be nonnegative and less than the left operand's bit width. Ordinary `<<` and `>>` trap on an invalid count. Left shift also traps when it would discard significant bits or produce an unrepresentable mathematical result.

The recoverable forms `<<!` and `>>!` produce `CoreFail.out_of_range`. There are no `<<%` or `>>%` operators.

Signed right shift is arithmetic; unsigned right shift is logical. To shift a signed value logically, first `bitcast` it to the corresponding unsigned type.

## Related topics

- [Types and values](types-values.md)
- [Failure handling](failures.md)
- [Unsafe execution](unsafe.md)
- [Evaluation order](evaluation-order.md)
- [Operator hooks](operator-hooks.md)
