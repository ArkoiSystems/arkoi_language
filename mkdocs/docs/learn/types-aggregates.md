---
title: Arkoi types, text, aggregates, and enums
description: Learn Arkoi 1.0 built-in types, literal context, text ownership, optionals, aggregates, enums, aliases, and constants.
---

# Types, text, aggregates, and enums

Arkoi makes the category and exact type of every value visible. This chapter
starts with built-in values, then combines them into named domain types for a
small route-reporting example.

!!! info "Arkoi 1.0 target-language illustration"

    The snippets on this page explain the **target language**. The current
    executable does not necessarily accept them. See
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying them with compiler 0.1.0.

## Learning goals

By the end of this chapter, you should be able to:

- choose a concrete built-in type and give literals enough context;
- distinguish copied data from ownership-controlled resources;
- choose between `string_view` and `string`;
- model absence with `?T`;
- declare and construct `data`, `resource`, and `enum` types; and
- recognize when an alias or compile-time constant is the right tool.

## Built-in values have exact types

Arkoi has no untyped runtime numbers and no implicit truthiness. Pick a type
whose range and meaning fit the value:

| Family | Built-in types | Typical choice |
| --- | --- | --- |
| Unsigned integers | `u8`, `u16`, `u32`, `u64`, `usize` | Counts, bit patterns, and sizes that cannot be negative |
| Signed integers | `s8`, `s16`, `s32`, `s64`, `ssize` | Values whose domain includes negatives |
| Floating point | `f32`, `f64` | Measurements that need fractional values |
| Boolean | `bool` | Exactly `true` or `false`; conditions require this type |
| Character | `char` | One Unicode scalar value |
| Text | `string_view`, `string` | Borrowed or owned valid UTF-8 text |

`usize` and `ssize` match the target pointer width. A byte is a `u8`; `char`
is a Unicode scalar rather than an encoded byte.

### Literals get their type from context

Integer and floating literals remain compile-time values until their context
selects a concrete type. The selected value must be representable:

```arkoi
stop_count @u32 = 12
mask @u16 = 0xff00
permissions @u16 = 0o755
route_bits @u8 = 0b1010
distance_km @f64 = 18.75
ratio @f32 = 2.5e-4
enabled @bool = true
marker @char = '\u{1f6a9}'
name @string_view = "North ridge"
```

Digits may contain `_` separators between valid digits. Integer literals also
support decimal, hexadecimal, binary, and octal notation; floating literals
support decimal, scientific, and hexadecimal notation. A context-free numeric
literal expression is invalid, and Arkoi does not silently infer a runtime
integer or floating type for it.

The destination is not the only possible context. A typed parameter can also
select a literal's type:

```arkoi
fun reserve_stops(count @usize):
    pass

fun plan_route():
    reserve_stops(32) # `32` is a `usize` in this call.
```

Conversions between concrete numeric types are covered in
[Expressions and conversions](expressions.md).

## Preview: data copies, resources move

Every Arkoi value belongs to one of two semantic categories:

| Category | Ordinary behavior | Examples |
| --- | --- | --- |
| Data | Copies through assignment, arguments, and returns | Numbers, `bool`, `char`, `string_view`, enums, references, slices, and `data` aggregates |
| Resource | Has one owning context and never copies implicitly | `string`, `resource` aggregates, and resource-containing arrays or optionals |

Binding mutability is a separate question. `name @mut string_view` is still a
copied data value; `name @string` is still a resource even though that binding
cannot be reassigned.

The category also follows some type constructors. `?T` and `[N]T` are resources
exactly when `T` is a resource. References, slices, and string views are
non-owning data values. The ownership chapter explains moves, clones, borrows,
and cleanup in detail.

## Text: view it or own it

String literals have type `string_view`. A view is copied cheaply and borrows
read-only UTF-8 storage; it does not own or mutate the bytes. Literal storage
lives for the whole program:

```arkoi
REPORT_TITLE @const string_view = "Route report"
```

`string` owns UTF-8 storage and is a resource. Converting between the two forms
is explicit:

```arkoi
fun measured_title() !CoreFail @usize:
    owned @string = string(REPORT_TITLE)!
    view @string_view = string_view(owned)
    return length(view)
```

`string(REPORT_TITLE)!` allocates an independent owned copy and may fail with
`CoreFail.out_of_memory`. `string_view(owned)` only borrows; it neither
allocates nor copies bytes. The view cannot outlive, or keep being used after a
move or replacement of, its backing string.

`length(text)` reports the UTF-8 **byte** count. `bytes(text)` provides a
read-only zero-copy `[]u8` view. Neither text type supports direct indexing,
because an arbitrary byte index need not identify a Unicode character.

### A practical text choice

| Need | Choose | Why |
| --- | --- | --- |
| Read a literal or caller-owned text during a call | `string_view` | No allocation or ownership transfer |
| Store independent, dynamically created text | `string` | The value owns its UTF-8 storage |
| Give another owner a named `string` | `move(name)` | Resources never transfer implicitly |
| Create independent owned text from a view | `string(view)!` | Allocation and ownership creation are explicit and may fail |

## Optionals model absence

`?T` contains either a `T` or `none`. It is distinct from a nullable raw
pointer and does not become true or false in a condition.

```arkoi
data Stop:
    name @string_view
    sequence @u32

fun visible_name(stop @?Stop) @string_view:
    selected @?string_view = stop?.name
    return selected ?? "unnamed"
```

`stop?.name` accesses the field only when a `Stop` is present and produces
another optional. `??` evaluates its fallback lazily and returns it only for
`none`. Later chapters introduce postfix `?` for propagating absence and
postfix `!` for treating absence as `CoreFail.none_access` in a fallible
function.

Use an optional when absence is a normal successful state. Use a failure effect
when the caller must distinguish a recoverable operation that did not succeed.

## Aggregates give values domain structure

A `data` aggregate may contain only data fields. It copies structurally like
its fields. The `Stop` declared above is one such aggregate:

```arkoi
start @Stop = Stop(
    name = "Trailhead",
    sequence = 1,
)
```

A `resource` aggregate may contain data and resource fields and always follows
resource ownership rules:

```arkoi
resource RouteReport:
    title @string
    stop_count @u32

fun create_report() !CoreFail @RouteReport:
    return RouteReport(
        title = string("Alpine route")!,
        stop_count = 3,
    )
```

Construction must initialize every field exactly once with its exact declared
type. Positional arguments follow field declaration order. After the first
named argument, all remaining arguments must be named. Named construction is
usually clearest when several fields share a type.

Declaring a type as `resource` can also express semantic ownership when its
visible fields happen to be data. Arkoi never silently changes a declaration's
category based on its fields or later use.

## Enums describe one of several cases

An enum is a flat, payload-free data type:

```arkoi
enum RouteState:
    planned
    active
    complete

state @RouteState = RouteState.planned

fun show_state(state @RouteState):
    if state == RouteState.active:
        show_live_route()
```

Members are always qualified by their type. Enum values copy and support `==`
and `!=`, but not ordering. An enum must have at least one member; use an empty
aggregate when you need a distinct marker type without cases.

Members may have explicit compile-time integer discriminants. Those numbers
are logical values, not a storage-layout promise, and integer/enum conversion
must use the explicit conversion operations.

## Aliases rename; constants compute

A type alias is transparent—it creates a convenient spelling, not a distinct
type identity:

```arkoi
type StopCount = u32
type MaybeStop = ?Stop

MAX_STOPS @const StopCount = 32
DEFAULT_STOP @const Stop = Stop(
    name = "unknown",
    sequence = 0,
)
```

`StopCount` and `u32` are the same type, so an alias cannot distinguish
overloads or add ownership rules. It can name compound data or resource types.

A `const` declaration is an addressless compile-time value. It requires an
explicit type and initializer, cannot be reassigned or borrowed, and may be
used in other constant expressions. Constants may contain optionals, arrays,
data aggregates, and literal-backed string views, but not resources or values
that depend on runtime storage.

### Which declaration fits?

| You need to model… | Use |
| --- | --- |
| A copied record of related fields | `data Name:` |
| A value with ownership or cleanup semantics | `resource Name:` |
| One qualified case from a fixed set | `enum Name:` |
| A shorter, transparent spelling for an existing type | `type Name = T` |
| An addressless value computed entirely at compile time | `NAME @const T = expression` |
| Stable runtime storage | A normal module or local binding, not `const` |

## Checkpoint: classify a route model

Which declarations below are invalid, and why?

```arkoi
data SavedRoute:
    title @string

state @RouteState = 1
type RouteId = u64
```

<details markdown="1">
<summary>Show the answer</summary>

The first declaration is invalid because a `data` aggregate cannot contain the
resource type `string`; declare `SavedRoute` as `resource`. The second is
invalid because integers do not implicitly convert to enums; use a qualified
member such as `RouteState.planned` or an explicit checked conversion. The
alias is valid, but `RouteId` remains exactly the same type as `u64`.

</details>

## Continue

Next, learn how exact types combine through conversion, comparison, arithmetic,
assignment, and pipelines in [Expressions and conversions](expressions.md).

Canonical rules:

- [Types and values](../language-specification/1.0.0/types-values.md)
- [Aggregates and enums](../language-specification/1.0.0/aggregates-enums.md)
- [Constants and module globals](../language-specification/1.0.0/constants-globals.md)
- [Bindings and initialization](../language-specification/1.0.0/bindings-initialization.md)
- [Ownership and moves](../language-specification/1.0.0/ownership-moves.md)
- [Failures](../language-specification/1.0.0/failures.md)
