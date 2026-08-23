---
title: Arkoi 1.0 learning capstone
description: Read one complete Arkoi 1.0 target module that combines data modeling, ownership, views, failures, methods, iteration, and a static interface.
---

# Capstone: a complete report module

This final chapter brings the tour together in one module. The program builds
an owned report, updates and views its fixed readings, finds an optional
highlight, calls a static interface requirement, and finally transfers the
report to a consuming function.

!!! info "Arkoi 1.0 target-language module"

    This is a complete illustration of the **Arkoi 1.0 target language**, not a
    promise of support in compiler 0.1.0. Review
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying it locally.

## What the program demonstrates

By the end of the walkthrough, you should be able to trace:

- a module and its parameterless, value-less `main` entry point;
- copied `data` values inside a `resource` that owns a `string`;
- associated functions and methods with explicit receiver arguments;
- read-only and mutable borrows, a moved resource, and deterministic cleanup;
- a fixed array, read-only slices, and `for` iteration by value or reference;
- a normal optional absence path and a recoverable failure effect; and
- nominal conformance to a static interface with no dynamic dispatch.

## The complete module

```arkoi title="tutorial/report_capstone.ark — Arkoi 1.0 target"
module tutorial.report_capstone

failure ReportFail:
    empty_title

interface Counted:
    fun count(
        self @&Self,
    ) @usize

pub data Reading:
    pub value @u32
    pub accepted @bool

pub resource Report:
    title @string
    pub readings @[4]Reading

implements Counted for Report

fun Report.create(
    title @string_view,
    readings @[4]Reading,
) !ReportFail @Report:
    if length(title) == 0:
        fail ReportFail.empty_title

    owned_title @string = string(title)!

    return Report(
        title = move(owned_title),
        readings = readings,
    )

fun Report.count(
    self @&Report,
) @usize:
    return length(self.readings)

fun Report.title_bytes(
    self @&Report,
) @usize:
    return length(self.title)

fun Report.raise_all(
    self @&mut Report,
    amount @u32,
):
    for reading @&mut Reading in &mut self.readings:
        reading.value += amount

fun Report.total(
    self @&Report,
) @u32:
    total @mut u32 = 0

    for reading @Reading in self.readings:
        total += reading.value

    return total

fun first_accepted(
    readings @[]Reading,
    minimum @u32,
) @?Reading:
    for reading @Reading in readings:
        if reading.accepted and reading.value >= minimum:
            return reading

    return none

fun validate_and_finish(
    report @own Report,
    highlight @?Reading,
    expected_count @usize,
):
    actual_count @usize = Counted.count(&report)

    if actual_count != expected_count:
        return

    if Report.title_bytes(&report) == 0:
        return

    selected @u32 = highlight?.value ?? 0
    total @u32 = Report.total(&report)

    if selected > total:
        return

fun main() !ReportFail:
    initial @[4]Reading = [
        Reading(value = 18, accepted = true),
        Reading(value = 21, accepted = false),
        Reading(value = 19, accepted = true),
        Reading(value = 23, accepted = true),
    ]

    report @mut Report = Report.create(
        "Morning readings",
        initial,
    )!

    Report.raise_all(&mut report, 1)

    all @[]Reading = readonly(report.readings[..])
    preview @[]Reading = all[..3]!
    highlight @?Reading = first_accepted(preview, 20)

    count @usize = Counted.count(&report)
    validate_and_finish(move(report), highlight, count)
```

The module calls no platform or library functions, so all names used by the
example are either declared in the file or provided by the language. It does
not print; its final observable language-level work is validation followed by
deterministic cleanup of the consumed report.

## Map the declarations to the model

| Declaration | Role |
| --- | --- |
| `module tutorial.report_capstone` | Gives the file its absolute module identity. |
| `ReportFail` | Adds a domain failure while automatically including `CoreFail`. |
| `Counted` | Describes a static receiver requirement; it is not a runtime type. |
| `Reading` | A copied data aggregate containing only data fields. |
| `Report` | A resource because it owns a `string`; its array field contains copied data. |
| `Report.create` | An associated constructor with no `self` parameter. |
| `Report.raise_all` | A mutable method selected by its explicit `&mut Report` receiver. |
| `Report.total` | A read-only method that copies each `Reading` loop item. |
| `first_accepted` | Returns `none` for ordinary absence rather than failing. |
| `Report.count` | The concrete external function satisfying `Counted.count`. |
| `validate_and_finish` | Takes ownership through `@own Report` and ends its lifetime. |

`implements Counted for Report` establishes nominal conformance. The matching
function would still be an ordinary `Report.count` method without that line,
but it would not satisfy the interface. `Counted.count(&report)` uses the
receiver's concrete `Report` type to select the implementation entirely at
compile time.

## Follow ownership and views

The report has one owner throughout each step:

1. `initial` is a data array. Passing it to `Report.create` copies its four
   `Reading` values into the parameter, and constructing `Report` copies them
   once more into the array field.
2. `string(title)!` constructs a fresh owned `string`. The named
   `owned_title` resource then moves into the returned `Report`, leaving its
   local source uninitialized.
3. `Report.raise_all(&mut report, 1)` borrows the report mutably. Its loop lends
   each array element as `&mut Reading`; it neither copies nor moves the report.
4. `report.readings[..]` creates a mutable slice view because `report` permits
   mutable access. `readonly(...)` explicitly produces `all @[]Reading`, and
   `all[..3]!` creates the half-open `preview` view.
5. `first_accepted` iterates over `preview` by value. Each `Reading` is data, so
   the loop copies it. The returned `?Reading` is therefore independent of the
   report's storage.
6. `preview` and `all` are no longer used when `move(report)` runs. Arkoi's
   obvious-last-use analysis can permit the transfer because no live use of a
   derived view follows it.
7. `validate_and_finish` becomes the report's owner. When that function exits,
   the report's owned title is cleaned up automatically. The moved source in
   `main` is not cleaned up a second time.

`Report` needs no custom `__drop__` hook here. Resource fields still receive
their ordinary cleanup when their containing resource's lifetime ends.

## Follow success, absence, and failure

The program keeps three different paths distinct:

- `first_accepted` returning `none` is a successful result meaning “no matching
  reading.” Safe member access and `?? 0` handle that absence locally.
- An empty title produces `ReportFail.empty_title` from `Report.create`.
- Allocating the owned title or slicing `all[..3]` can produce a built-in
  `CoreFail`. Every user failure set includes `CoreFail`, so `main` can
  propagate both operations through its `!ReportFail` effect.

If slicing fails after `report` has been initialized, propagation exits `main`
only after cleaning up that initialized report. If report construction fails,
no incomplete `Report` escapes. Ordinary `u32` addition in `raise_all` and
`total` retains its separate trapping-overflow behavior; a trap is not caught
or converted by the failure effect.

## Check your understanding

1. Why does `Report.create` write `move(owned_title)` but pass `readings`
   normally?
2. Why is `Counted.count(&report)` statically dispatched even though the call
   is qualified by an interface?
3. Why may `highlight` still be used after `report` moves?
4. Which owner cleans up the report's `string` on the successful path?

<details markdown="1">
<summary>Show the answers</summary>

1. `string` is a resource and a named resource transfer must be explicit.
   `[4]Reading` is data because `Reading` is data, so it copies normally.
2. The explicit receiver has the concrete type `&Report`. The compiler uses
   that type and the nominal `implements` declaration to select
   `Report.count`; no interface value or runtime lookup exists.
3. Iteration copied a `Reading` into the optional. `highlight` does not borrow
   the report or its slice backing storage.
4. `validate_and_finish` owns the moved parameter. Its lexical scope performs
   cleanup when the function exits; `main`'s moved source is uninitialized.

</details>

## Extension exercises

1. Change `first_accepted` to return the first matching `@&Reading` rather than
   a copied `Reading`. Identify how that changes the point at which `report` may
   be moved.
2. Replace the postfix propagation on `Report.create(...)!` with
   `handle failure` and yield a report whose title is `"Untitled"`.
3. Move `Counted` into `contracts.counted` and the report into
   `reports.reading`; add the required import and qualification without
   introducing a dependency cycle.

<details markdown="1">
<summary>Show exercise hints</summary>

1. The function's successful type becomes `@?&Reading`, and the loop binding
   becomes `reading @&Reading`. A present result still borrows the report's
   storage, so use it before `move(report)`; copying the optional descriptor
   does not copy its referent.
2. Attach `handle failure` directly to the fallible call. Every handler path
   must `yield` a compatible `Report` or leave through another valid terminating
   path. Constructing the fallback title may itself still fail and can use
   postfix `!` because `main` remains fallible.
3. Put `pub interface Counted` in `contracts/counted.ark`. In the report module,
   write `import contracts.counted as counted`, then use
   `implements counted.Counted for Report` and
   `counted.Counted.count(&report)`. The implementation belongs in either the
   interface's module or the concrete type's module.

</details>

## Continue with the reference

Keep the [Arkoi 1.0 quick reference](../language-specification/1.0.0/quick-reference.md)
nearby, or continue into the complete
[Arkoi 1.0 specification](../language-specification/1.0.0/index.md).

Canonical rules behind the capstone:

- [Modules and imports](../language-specification/1.0.0/modules-and-imports.md)
- [Aggregates and enums](../language-specification/1.0.0/aggregates-enums.md)
- [Functions and returns](../language-specification/1.0.0/functions-returns.md)
- [Methods and visibility](../language-specification/1.0.0/methods-visibility.md)
- [Ownership and moves](../language-specification/1.0.0/ownership-moves.md)
- [References and lifetimes](../language-specification/1.0.0/references-lifetimes.md)
- [Arrays](../language-specification/1.0.0/arrays.md), [slices](../language-specification/1.0.0/slices.md), and [for loops](../language-specification/1.0.0/for-loops.md)
- [Failures](../language-specification/1.0.0/failures.md)
- [Static interfaces](../language-specification/1.0.0/interfaces-overview.md)
- [Interface implementations](../language-specification/1.0.0/interface-implementations.md)
