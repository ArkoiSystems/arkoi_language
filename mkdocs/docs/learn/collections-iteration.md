---
title: Arkoi collections and iteration
description: Learn Arkoi 1.0 arrays, slices, checked indexing, views, lifetimes, and built-in and custom iteration.
---

# Collections and iteration

Arrays own a fixed number of elements. Slices provide lightweight views over
contiguous storage. Arkoi keeps the difference visible, especially when code
indexes, mutates, or iterates over their elements.

!!! info "Arkoi 1.0 target-language tutorial"

    Every snippet on this page illustrates the **Arkoi 1.0 target language**.
    Several forms are not implemented by the current compiler. Check the
    [current compiler versus Arkoi 1.0](../getting-started/compatibility.md)
    before trying an example locally.

## Before you begin

This chapter builds on:

- typed bindings and functions from [Your first Arkoi source file](basics.md);
- ownership and borrowing from [Data, resources, and ownership](ownership.md);
- postfix failure propagation from [Optionals and recoverable
  failures](failures.md); and
- explicit calls and receiver modes from [Calls and methods](calls-methods.md).

By the end, you will be able to:

- choose an owning array or a non-owning slice;
- predict when indexing or slicing needs postfix `!`;
- create read-only and mutable views without copying elements;
- iterate by copy, read-only reference, or mutable reference; and
- recognize when a custom collection needs hooks or an iteration protocol.

## Arrays own; slices view

The length in `[N]T` is part of an array's type and must be known at compile
time. The array owns exactly those elements:

```arkoi
readings @mut [5]u32 = [18, 21, 19, 23, 20]
```

`[5]u32` and `[6]u32` are different types. `[0]u32` is also valid. Arkoi does
not provide a repetition spelling such as `[0; 5]`; write each literal element
or construct the array explicitly.

A slice stores a view of contiguous elements and a runtime length. It owns
neither the backing storage nor the elements:

| Type | Storage relationship | Element access |
| --- | --- | --- |
| `[5]u32` | Owns five elements | Follows the array place |
| `[]u32` | Borrows contiguous storage | Read-only |
| `[]mut u32` | Borrows contiguous storage | Mutable |

Create a full mutable view of the array, then explicitly reduce its access when
a consumer needs a read-only view:

```arkoi
editable @[]mut u32 = readings[..]
visible @[]u32 = readonly(editable)
```

Copying `visible` copies only its pointer-and-length descriptor. It does not
copy the five numbers. `readonly(...)` is also zero-copy: it removes one layer
of mutation permission and leaves `editable` available.

## Length, indexing, and slicing

`length(...)` is infallible for both arrays and slices and returns `usize`:

```arkoi
array_count @usize = length(readings)  # Compile-time value: 5.
view_count @usize = length(visible)    # Runtime length stored in the slice.
```

Bounds behavior depends on what the compiler knows:

| Expression | Check | Failure handling |
| --- | --- | --- |
| `readings[0]` | Constant array index checked at compile time | No `!` |
| `readings[position]!` | Dynamic array index checked at runtime | May produce `CoreFail.out_of_range` |
| `visible[0]!` | Slice index checked at runtime, even when literal | May produce `CoreFail.out_of_range` |
| `visible[..]` | Complete range is always valid | No `!` |
| `visible[start..end]!` | Half-open runtime range | May produce `CoreFail.out_of_range` |

The `!` belongs to ordinary failure propagation; it is not special indexing
syntax. The enclosing function must permit `CoreFail`, or the expression must
handle the failure locally.

This function uses all three access forms against the same collection:

```arkoi
fun inspect_window(
    values @[]u32,
    position @usize,
) !CoreFail @u32:
    first @u32 = values[0]!
    selected @u32 = values[position]!
    window @[]u32 = values[1..4]!

    return first + selected + window[0]!
```

Ranges are half-open: `1..4` contains indices 1, 2, and 3. Bounds must satisfy
`0 <= start <= end <= length(values)`; negative indices are unsupported.

For an array, a constant out-of-range index is rejected before the program can
run:

!!! failure "Compile-time error — constant array index"

    ```arkoi
    readings @[5]u32 = [18, 21, 19, 23, 20]
    impossible @u32 = readings[5]
    ```

    Valid indices are 0 through 4. Postfix `!` cannot turn a statically invalid
    index into a runtime operation.

## Mutation and view lifetimes

Mutable slice access is carried by `[]mut T`, independently of whether the
slice binding can be reassigned:

```arkoi
fun revise(
    values @[]mut u32,
    position @usize,
) !CoreFail:
    values[position]! += 1

    tail @[]mut u32 = values[position..]!
    tail[0]! = 0
```

`values` is a fixed slice binding, but its type permits writing the viewed
elements. Use `values @mut []mut u32` only when the slice descriptor itself must
be rebound to a different range.

A view cannot outlive its backing storage. Arkoi also rejects obvious movement,
replacement, or cleanup of the owner while a derived view remains live:

!!! failure "Compile-time error — replacing borrowed storage"

    ```arkoi
    readings @mut [5]u32 = [18, 21, 19, 23, 20]
    editable @[]mut u32 = readings[..]
    visible @[]u32 = readonly(editable)

    readings = [0, 0, 0, 0, 0]
    print_sum(visible)  # The whole backing owner was replaced while still viewed.
    ```

Arkoi performs limited lexical and obvious-last-use checking rather than a full
borrow proof. Mutable references and slices may alias, so code that creates
several views remains responsible for using their shared storage sensibly.

## Iterate over arrays and slices

Built-in arrays and slices have compiler-defined, infallible iteration. The
source expression is evaluated once, and a fresh loop binding is created for
every element.

For data elements, iterate by value to copy each element:

```arkoi
fun sum(values @[]u32) @u32:
    total @mut u32 = 0

    for value @u32 in values:
        total += value

    return total
```

No index is exposed, so ordinary built-in traversal performs no recoverable
bounds operation. The addition still follows Arkoi's ordinary trapping overflow
rule.

Use a read-only reference when copying would be undesirable or when the element
is a resource:

```arkoi
for value @&u32 in visible:
    inspect(value)

for file @&File in files:
    inspect_file(file)
```

Built-in arrays and slices never implicitly copy or move resource elements.
They lend those elements by reference.

To update elements, select mutable iteration explicitly and bind each element
as a mutable reference:

```arkoi
readings @mut [5]u32 = [18, 21, 19, 23, 20]

for reading @&mut u32 in &mut readings:
    reading += 1
```

Binding-level `mut` means something different:

```arkoi
for reading @mut u32 in readings:
    reading += 1
```

Here each `reading` is a mutable local **copy**. The array elements do not
change. To mutate an element, the loop must yield `&mut u32` as in the previous
example.

## Preview: consuming and fallible iteration

Arrays and slices use infallible built-in traversal and never consume their
resource elements. A custom aggregate can expose additional modes:

```arkoi
# Read-only custom iteration.
for entry @&Entry in ledger:
    inspect_entry(entry)

# Consuming iteration over a named resource.
for entry @Entry in move(queue):
    archive(move(entry))

# Iterator creation or advancement may fail.
for! record @&Record in database:
    process(record)
```

A named resource requires `move(...)` to select consuming iteration; a fresh
resource temporary enters that mode directly. `for!` propagates failure from
custom iterator creation or advancement. It is required when either operation
is fallible and rejected when both are infallible. Failure while evaluating the
source expression still uses ordinary postfix `!`.

## Preview: collection hooks and protocols

Named `data` and `resource` aggregates can participate in collection syntax by
providing exact compiler-recognized contracts:

| Feature | Contract to explore |
| --- | --- |
| `length(container)` | `__length__(self @&Type) @usize` |
| Read-only `container[index]` | `__index__` |
| Mutable `container[index]` | `__index_mut__` |
| Read-only ranges | `__slice__` |
| Explicit mutable ranges | `__slice_mut__` |
| `for` / `for!` | Canonical creation and advancement interfaces |

These hooks are statically selected external functions, not dynamic methods.
Custom iteration uses nominal interfaces such as `Iterable`, `Iterator`, and
their mutable, owning, or fallible counterparts. Built-in arrays and slices
keep compiler-defined behavior and do not nominally implement those interfaces.
The [next chapter](interfaces-modules.md) develops the interface model that
makes these protocols possible.

## Check your understanding

1. Which of `array[0]`, `array[index]`, `slice[0]`, and `slice[..]` require
   failure propagation?
2. Does `for value @mut u32 in numbers` modify `numbers`?
3. How do you pass a `[]mut u32` to a function requiring `[]u32`?
4. Why can a built-in array of resources iterate as `@&File` but not `@File`?

<details markdown="1">
<summary>Show the answers</summary>

1. An in-range constant array index and a complete slice range are infallible.
   A dynamic array index and every slice index need propagation or handling.
2. No. It mutates the fresh local copy. Use a loop item of `@&mut u32` and
   explicitly select mutable iteration to update elements.
3. Pass `readonly(mutable_slice)`. Access reduction is explicit and copies no
   elements.
4. The array owns every resource element. Yielding `@File` would implicitly copy
   or move it out and leave the array incomplete; `@&File` preserves ownership.

</details>

## Continue

Next, organize these protocols and other contracts with
[Interfaces and modules](interfaces-modules.md).

Canonical rules:

- [Arrays](../language-specification/1.0.0/arrays.md)
- [Slices](../language-specification/1.0.0/slices.md)
- [Access reduction](../language-specification/1.0.0/access-reduction.md)
- [For loops](../language-specification/1.0.0/for-loops.md)
- [Iterator values and lifetimes](../language-specification/1.0.0/iterator-values-and-lifetimes.md)
- [Indexing](../language-specification/1.0.0/indexing.md), [slicing](../language-specification/1.0.0/slicing.md), and [length](../language-specification/1.0.0/length.md)
- [Iteration protocols](../language-specification/1.0.0/iteration-protocols.md)
