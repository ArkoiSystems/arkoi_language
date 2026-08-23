---
title: Arkoi 1.0 operator hooks
description: Arkoi 1.0 target-language rules for operator-hook selection, signatures, overload resolution, failures, and results.
---

# Operator hooks

<!-- spec-sections: 9.19–9.20 -->

This page defines user-provided arithmetic, bitwise, shift, and unary operations for named aggregates.

!!! abstract "At a glance"
    Operator hooks are statically resolved, same-module functions. They borrow aggregate operands read-only: operator syntax can create a replacement result, but it can never silently mutate or consume an operand.

## Binary resolution

For `left * right`, Arkoi resolves:

1. a built-in operation or applicable normal hook owned by the left operand type;
2. otherwise, an applicable reverse hook owned by the right operand type;
3. otherwise, a compile-time error.

The left-side operation wins whenever both sides could provide one.

```arkoi
data Vector:
    x @f64
    y @f64

fun Vector.__mul__(
    self @&Vector,
    scalar @f64,
) @Vector:
    return Vector(x = self.x * scalar, y = self.y * scalar)

fun Vector.__r_mul__(
    self @&Vector,
    scalar @f64,
) @Vector:
    return Vector(x = scalar * self.x, y = scalar * self.y)

right_scaled @Vector = vector * 2.0
left_scaled @Vector = 2.0 * vector
```

Resolution is compile-time and searches only the identified normal or reverse overload set. It does not search unrelated functions or introduce implicit conversions merely to make a hook applicable. Ordinary parameter, return-type, and overload checking applies.

An aggregate hook must be declared in the same module as its owning type; an unrelated module cannot add hooks to a built-in type. A malformed declaration using a reserved hook name is a compile-time error.

## Hook catalog

Normal and reverse binary hooks follow a regular pairing:

| Operation | Normal | Reverse |
| --- | --- | --- |
| `+` | `__add__` | `__r_add__` |
| `-` | `__sub__` | `__r_sub__` |
| `*` | `__mul__` | `__r_mul__` |
| `/` | `__div__` | `__r_div__` |
| `%` | `__rem__` | `__r_rem__` |
| `&` | `__bit_and__` | `__r_bit_and__` |
| `\|` | `__bit_or__` | `__r_bit_or__` |
| `^` | `__bit_xor__` | `__r_bit_xor__` |
| `<<` | `__shl__` | `__r_shl__` |
| `>>` | `__shr__` | `__r_shr__` |

Unary hooks are `__neg__` for unary `-`, `__pos__` for unary `+`, and `__bit_not__` for `~`.

Trapping, recoverable, and wrapping arithmetic variants are independent:

| Family | Trapping | Recoverable | Wrapping |
| --- | --- | --- | --- |
| Add | `__add__` | `__add_checked__` | `__add_wrapping__` |
| Subtract | `__sub__` | `__sub_checked__` | `__sub_wrapping__` |
| Multiply | `__mul__` | `__mul_checked__` | `__mul_wrapping__` |

Reverse variants insert `r_`, for example `__r_add_checked__`. Division, remainder, and shifts likewise use distinct checked hooks for their recoverable operator forms. Arkoi derives no variant from another; a type may implement any subset.

## Failure effects

An operator hook may declare a failure effect. The resulting operator expression must be propagated or handled like any other fallible expression.

```arkoi
result @Matrix = (matrix / divisor)!
```

The checked built-in operator spelling and a user hook's declared failure effect are separate parts of the selected operation's contract.

## Operand contract

User-defined operator hooks do not consume or mutate operands. The owning aggregate is always a read-only reference:

```arkoi
fun Vector.__add__(
    self @&Vector,
    other @&Vector,
) @Vector:
    return Vector(x = self.x + other.x, y = self.y + other.y)
```

For a normal hook, `self` is the left operand; for a reverse hook, it is the right operand. Operator parameters:

- cannot use `@own` or `&mut`;
- cannot implicitly move from or mutate either operand;
- may receive freely copyable data, such as `f64`, by value;
- must receive aggregate and resource operands through read-only references;
- retain the ordinary rule that by-value parameters are immutable local bindings.

Both source operands remain initialized and unchanged after an operator expression. A hook may return a newly constructed data or resource value, which follows ordinary ownership and cleanup rules.

Mutating and consuming operations must instead have explicit names:

```arkoi
Vector.scale_in_place(&mut vector, scalar)
Buffer.combine(move(first), move(second))
```

## Compound assignment

Compound assignment uses the corresponding binary hook to construct a replacement:

| Statement | Hook |
| --- | --- |
| `value += other` | `__add__` |
| `value +!= other` | `__add_checked__` |
| `value +%= other` | `__add_wrapping__` |

The hook still receives read-only operands. Mutation is performed by the assignment statement after the result exists. Destination evaluation, single resolution, and construct-first resource replacement are defined on [Assignment and result use](assignment-and-result-use.md).

Ordinary binary operators never consume a resource operand. A consuming operation must remain an explicitly named function that receives `move(...)`.

## Related topics

- [Numeric operations](expressions-and-numeric-operations.md)
- [Comparisons and membership](comparisons-and-membership.md)
- [Assignment and result use](assignment-and-result-use.md)
- [Calls and overloads](calls-overloads.md)
- [Ownership and moves](ownership-moves.md)
- [Compiler hooks](compiler-hooks.md)
