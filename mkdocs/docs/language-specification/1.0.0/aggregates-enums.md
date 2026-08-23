---
title: Arkoi 1.0 aggregates and enums
description: Arkoi 1.0 target-language rules for aggregate declarations, fields, initialization, anonymous aggregates, enums, and unions.
---

# Aggregates and enums

<!-- spec-sections: 3.7, 3.9–3.10, 5.2 -->

Named aggregates collect fields; enums define one of a fixed set of
payload-free cases. Both use indentation blocks but have different
construction and membership rules.

!!! abstract "At a glance"

    - `data` aggregates contain data fields; `resource` aggregates may contain
      data and resources.
    - Aggregate construction accepts positional, named, or mixed arguments and
      initializes every field exactly once.
    - Empty aggregates are valid and use `pass`; empty enums are invalid.
    - Enum members are qualified, payload-free data values.
    - Enum discriminants are logical integers, not a storage-layout promise.

## Aggregate declarations and construction

```arkoi
data Point:
    x @f32
    y @f32

point @Point = Point(
    10.0,
    y = 20.0,
)

other @Point = Point(
    y = 20.0,
    x = 10.0,
)
```

Positional arguments initialize fields in declaration order. After the first
named argument, every remaining argument must be named. Named arguments may
initialize the remaining fields in any order.

Every field must be supplied exactly once with its exact declared type. A
missing, unknown, inaccessible, or duplicate field is a compile-time error.
Expressions evaluate from left to right in written order, and partially
initialized aggregates are never observable.

Field declaration order is part of the source API for positional construction.
Named construction avoids depending on that order and is usually clearer when
an aggregate has several fields of the same type.

!!! danger "Compile-time errors — invalid argument order or duplication"

    ```arkoi
    duplicate @Point = Point(10.0, x = 20.0)
    invalid @Point = Point(x = 10.0, 20.0)
    ```

### Resource fields

A named resource binding must be moved or cloned explicitly when placed in an
aggregate; a temporary resource enters the field directly:

```arkoi
user @User = User(
    name = move(name),
    file = File.open(path)!,
)
```

For complete ownership rules, see [Ownership and moves](ownership-moves.md).

### Fields, destructuring, and visibility

Fields are accessed individually:

```arkoi
point @Point = get_point()
x @f32 = point.x
y @f32 = point.y
```

Aggregate destructuring is not part of Arkoi. In particular, a resource
aggregate cannot be destructured to move out resource fields; use the
addressable-place operations described in [Places and
replacement](places-replacement.md).

!!! danger "Compile-time error — aggregate destructuring"

    ```arkoi
    Point(x, y) = point
    ```

Construction must also respect field visibility. Positional syntax cannot
bypass a private field. Code without access to every field uses a public
associated constructor; see [Methods and visibility](methods-visibility.md).

## Empty aggregates

Both categories may be fieldless:

```arkoi
data Marker:
    pass

resource CapabilityToken:
    pass
```

An empty aggregate remains:

- a distinct named type with its declared data or resource category;
- eligible for methods, associated functions, permitted hooks, and interfaces;
- usable in otherwise-valid compound types and exact overload resolution; and
- unspecified in size and binary layout.

An empty data aggregate copies normally. An empty resource aggregate still
obeys move, cleanup, and hook rules despite having no visible fields. Such a
type can represent nominal identity, a capability, or a synchronization token.

## Enum declarations

```arkoi
pub enum Status:
    ready
    waiting
    failed

status @Status = Status.ready
```

Enums are flat, payload-free data types. Their values copy normally, and members
must be referenced through the declaring type.

Visibility applies to the enum as a whole. When the type is accessible, all
members are accessible through qualified names; individual members cannot have
separate `pub` or private modifiers.

Every enum must contain at least one member. Comments and `pass` do not count.

!!! danger "Compile-time error — empty enum"

    ```arkoi
    enum Never:
        pass
    ```

A one-member enum is valid. For nominal identity with no cases, use an empty
aggregate or marker interface.

## Discriminants

```arkoi
enum Code:
    first  = 5
    second # 6
    third  = 10
    fourth # 11
```

| Rule | Definition |
| --- | --- |
| Expression | Compile-time integer constant expression |
| First implicit member | `0` |
| Later implicit member | Previous discriminant plus one |
| Uniqueness | No two members may share a value |
| Range | Representable in Arkoi's compile-time integer model |
| Layout | Storage size and ABI representation remain unspecified |

Explicit discriminants define logical values only. Enums neither implicitly
convert to integers nor accept implicit integer conversion.

```arkoi
number @u32 = convert(Code.first, u32)!
code @Code = convert(number, Code)!
```

Integer-to-enum conversion produces `CoreFail.invalid_conversion` if no member
has that discriminant. Enum-to-integer conversion produces
`CoreFail.out_of_range` when the destination cannot represent the discriminant.

## Enum operations

Enums support `==` and `!=`, but not `<`, `<=`, `>`, or `>=`. Convert explicitly
to a numeric type when numeric ordering is intended:

```arkoi
left_number @s32 = convert(left, s32)!
right_number @s32 = convert(right, s32)!

if left_number < right_number:
    report_order()
```

Exact enum comparison and `in`/`not in` behavior are described with the
language's operator rules.

## Related topics

- [Types and values](types-values.md)
- [Methods and visibility](methods-visibility.md)
- [Ownership and moves](ownership-moves.md)
- [Places and replacement](places-replacement.md)
- [Constants and module globals](constants-globals.md)
