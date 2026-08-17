---
title: Arkoi functions, calls, and methods
description: Learn Arkoi 1.0 exact call matching, named arguments, overloads, explicit method receivers, associated functions, visibility, and function pointers.
---

# Functions, calls, and methods

Arkoi makes every call contract visible. Argument types match exactly, borrowing
and ownership are explicit, overloads are selected at compile time, and a method
receiver is an ordinary first parameter rather than an implicit object.

!!! info "Arkoi 1.0 target-language tutorial"

    Every snippet on this page illustrates the **Arkoi 1.0 target language**.
    The current compiler does not necessarily accept it. Check
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying these examples with today's executable.

## What you will learn

By the end of this chapter, you will be able to:

- call functions with exact positional, named, and mixed arguments;
- predict how Arkoi selects ordinary and fallible overloads;
- distinguish methods from associated functions;
- pass read-only, mutable, and owning receivers explicitly;
- design a public type around private fields; and
- recognize when a raw function pointer is appropriate.

## Before you start

This chapter assumes that you have read:

- [Your first Arkoi source file](basics.md), for typed bindings and
  function return contracts;
- [Data, resources, and ownership](ownership.md), for `&`, `&mut`, `@own`, and
  `move(...)`; and
- [Optionals and recoverable failures](failures.md), for postfix `!` and local
  handlers.

The examples build a small report API. Its public identifier can be inspected
directly, while its title remains private and is validated by the type's own
operations.

## Start with a type-owned API

Methods and associated functions are declared outside the aggregate. Their
qualified name says which type they belong to:

```arkoi title="Arkoi 1.0 target — reports/model.ark"
module reports.model

failure ReportFail:
    empty_title
    not_found

pub data Report:
    pub id @u64
    title @string_view

pub fun Report.create(
    id @u64,
    title @string_view,
) !ReportFail @Report:
    if length(title) == 0:
        fail ReportFail.empty_title

    return Report(
        id = id,
        title = title,
    )

pub fun Report.heading(
    self @&Report,
) @string_view:
    return self.title

pub fun Report.rename(
    self @&mut Report,
    title @string_view,
) !ReportFail:
    if length(title) == 0:
        fail ReportFail.empty_title

    self.title = title
```

`Report.create` has no `self` parameter, so it is an **associated function**.
It acts as a constructor, but constructors have no special declaration form in
Arkoi: an associated function simply returns a complete value.

`Report.heading` and `Report.rename` are **methods** because their first
parameter is named `self` and has one of the permitted receiver forms. Their
bodies can access the private `title` field because they belong to `Report` and
are declared in the same module.

## Calls use exact types

Arkoi does not insert an implicit numeric conversion, borrow, access reduction,
or resource move to make a call fit. Each argument must have the parameter's
exact type and access mode:

```arkoi title="Arkoi 1.0 target"
fun prepare_report() !ReportFail:
    report_id @u64 = 7
    report @mut Report = Report.create(report_id, "Weekly status")!

    title @string_view = Report.heading(&report)
    Report.rename(&mut report, "Published status")!
```

The first call passes two data values. The second explicitly borrows `report`
as `&Report`; the third explicitly borrows it as `&mut Report`. Binding-level
`mut` on `report` permits that mutable borrow. It does not change `Report`'s
type or create a different overload by itself.

When a numeric type differs, convert deliberately:

```arkoi title="Arkoi 1.0 target"
fun create_from_small_id() !ReportFail @Report:
    small_id @u32 = 7
    report_id @u64 = convert(small_id, u64)!
    return Report.create(report_id, "Converted identifier")!
```

## Positional and named arguments

A call may use positional arguments, named arguments, or positional arguments
followed by named arguments:

```arkoi title="Arkoi 1.0 target"
fun build_variants() !ReportFail:
    first @Report = Report.create(7, "Weekly status")!

    second @Report = Report.create(
        id = 8,
        title = "Monthly status",
    )!

    third @Report = Report.create(
        9,
        title = "Quarterly status",
    )!
```

After the first named argument, every remaining argument must also be named.
Every parameter must be supplied exactly once. Public parameter names are
therefore part of the source API: renaming `title` would break callers that use
`title = ...`.

Arguments still evaluate from left to right in their written order. Naming an
argument changes how it maps to a parameter, not when its expression runs.

## Overloads are exact, not ranked

Functions may share a name when their ordered parameter signatures differ:

```arkoi title="Arkoi 1.0 target"
pub fun describe(value @u64) @string_view:
    return "report identifier"

pub fun describe(value @&Report) @string_view:
    return Report.heading(value)

fun describe_report(report @&Report) @string_view:
    id_description @string_view = describe(report.id)
    report_description @string_view = describe(report)
    return report_description
```

The argument type selects one exact candidate. Arkoi does not rank conversions
or choose an overload from the expected return type. A literal that could fit
several overloads must receive a concrete type from context or be converted
explicitly.

An overload is distinguished by its complete name, parameter count, ordered
parameter types, ownership and reference-mutability modes, and whether it is
fallible. It is **not** distinguished by return type, parameter names,
visibility, binding-level parameter `mut`, or the exact failure-set type.

## The call spelling selects fallibility

An infallible and a fallible overload may share the same parameter signature:

```arkoi title="Arkoi 1.0 target — additional Report operations"
pub fun Report.load(id @u64) @Report:
    return Report(
        id = id,
        title = "cached report",
    )

pub fun Report.load(id @u64) !ReportFail @Report:
    if id == 0:
        fail ReportFail.not_found

    return Report(
        id = id,
        title = "fresh report",
    )
```

The directly attached failure syntax chooses between them:

```arkoi title="Arkoi 1.0 target"
fun load_reports(id @u64) !ReportFail:
    cached @Report = Report.load(id)
    fresh @Report = Report.load(id)!

    recovered @Report = Report.load(id) handle failure:
        yield Report.load(1)
```

The unmarked call selects the infallible overload. A directly attached postfix
`!` or `handle failure` selects the fallible overload when both otherwise
match. The enclosing function's effect and an expected return type do not make
that choice implicitly.

## Receiver access is part of the contract

The three method receiver forms express three different operations:

| Receiver | Meaning | Call-site form |
| --- | --- | --- |
| `self @&T` | Inspect a borrowed value | `T.inspect(&value)` |
| `self @&mut T` | Mutate through a borrow | `T.update(&mut value)` |
| `self @own T` | Take ownership of a resource | `T.consume(move(value))` |

An existing reference can be passed directly when its exact type already
matches. Arkoi neither inserts a borrow nor creates a bound-method value; the
receiver remains the ordinary first argument in every call.

### One instructive error

```arkoi title="Arkoi 1.0 target — compile-time error"
fun invalid_receiver() !ReportFail:
    report @mut Report = Report.create(7, "Draft")!

    Report.rename(report, "Published")!
    # Compile-time error: `Report.rename` requires `&mut Report`, not `Report`.
```

The correction is explicit:

```arkoi title="Arkoi 1.0 target"
fun valid_receiver() !ReportFail:
    report @mut Report = Report.create(7, "Draft")!
    Report.rename(&mut report, "Published")!
```

## Visibility makes constructors useful

Declarations and fields are private unless marked `pub`. In the report API,
`Report` and `id` are public, while `title` is private. Code outside
`reports.model` can read `report.id`, but it must call `Report.heading` to read
the title and `Report.rename` to change it.

Private-field access is not granted to every function in the same module. It is
limited to same-module methods, associated functions, and recognized hooks that
belong to the type. Because callers cannot initialize a private field directly,
the public `Report.create` associated function is the supported construction
path.

## Advanced preview: function pointers

Most Arkoi calls should remain direct. When an API genuinely needs a storable
call target, name its signature and take the raw address of one compatible
function:

```arkoi title="Arkoi 1.0 target"
type HeadingReader = fun(
    @&Report,
) @string_view

fun read_heading_indirectly() @string_view:
    report @Report = Report.load(7)
    reader @*HeadingReader = address(Report.heading)

    unsafe:
        return call(reader, &report)
```

Function-type parameters are unnamed. The expected `*HeadingReader` type lets
`address(...)` select one exact named function, including its return type,
failure effect, safety, and ABI. An indirect `call(...)` is always explicit,
positional-only, and unsafe because a raw function pointer could be null or
invalid.

A function pointer stores no captured environment and does not bind a method
receiver. It adds indirect calling, not closures, runtime overload lookup,
interface values, or dynamic dispatch.

## Check your understanding

1. Which overload does `Report.load(id)` select, and which does
   `Report.load(id)!` select?
2. Why can `Report.rename` change `title` even though the field is private?
3. Why does `Report.rename(report, "New")!` not borrow `report` automatically?
4. Does `address(Report.heading)` store a particular `Report` value?

<details markdown="1">
<summary>Show the answers</summary>

1. The unmarked call selects the infallible overload. The directly attached
   postfix `!` selects and propagates the fallible overload.
2. It is a same-module method belonging to `Report`; that is one of the
   contexts allowed to access the type's private fields.
3. Its first parameter has the exact type `&mut Report`, and ordinary calls
   never insert `&mut` implicitly. The caller must write `&mut report`.
4. No. It stores only the address of the named function. The receiver remains
   the first explicit argument to `call(...)`.

</details>

## Continue

Next, put exact call contracts to work with fixed arrays, slice views, bounds
checks, and traversal in [Collections and iteration](collections-iteration.md).

Canonical rules:

- [Functions and returns](../language-specification/1.0.0/functions-returns.md)
- [Calls and overloads](../language-specification/1.0.0/calls-overloads.md)
- [Methods and visibility](../language-specification/1.0.0/methods-visibility.md)
- [Evaluation order](../language-specification/1.0.0/evaluation-order.md)
- [Failures](../language-specification/1.0.0/failures.md)
- [Ownership and moves](../language-specification/1.0.0/ownership-moves.md)
- [Function pointers](../language-specification/1.0.0/function-pointers.md)
- [Unsafe execution](../language-specification/1.0.0/unsafe.md)
