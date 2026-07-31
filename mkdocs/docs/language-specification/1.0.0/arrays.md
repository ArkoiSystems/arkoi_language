# Arrays

<!-- spec-sections: 6.1 (fixed arrays, length, indexing, resource elements, repetition) -->

`[N]T` is an owning fixed-size array whose length is part of its type. It can
produce a slice view explicitly, but an array and a slice are never the same
kind of value.

!!! abstract "At a glance"

    - `[N]T` owns exactly `N` elements; arrays of different lengths differ in
      type.
    - `N` is a nonnegative compile-time integer representable as `usize`.
    - Constant in-range indexing is infallible; dynamic indexing is recoverable.
    - `[0]T` is valid.
    - An array is a resource exactly when its element type is a resource.
    - `[value; N]` repetition syntax is not supported.

## Types, literals, and slice views

```arkoi
values @[4]u32 = [10, 20, 30, 40]
```

The length belongs to the type, so `[4]u32` and `[8]u32` are distinct. An array
owns its elements; a slice only views storage:

| Type | Meaning |
| --- | --- |
| `[4]u32` | Owns four `u32` values |
| `[]u32` | Non-owning read-only view |
| `[]mut u32` | Non-owning mutable view |

Create a full view with `[..]`:

```arkoi
view @[]u32 = values[..]

mutable_values @mut [4]u32 = [10, 20, 30, 40]
mutable_view @[]mut u32 = mutable_values[..]
```

A mutable array is required for mutable element access. Converting a mutable
view to read-only remains explicit with `readonly(...)`.

## Length expressions

`N` must be a compile-time integer expression. The compiler evaluates it using
compile-time integer rules, then requires a nonnegative result representable as
`usize`:

```arkoi
WIDTH @const usize = 4
HEIGHT @const usize = 3

type Pixels = [WIDTH * HEIGHT]u32
type Header = [2 + 6]u8
```

Every array consequently has statically known size and layout.

!!! failure "Compile-time error — runtime array length"

    ```arkoi
    size @usize = 4
    values @[size]u32
    ```

`length(array)` is an infallible language built-in returning `N` as a `usize`
compile-time value. It observes its operand through defined built-in access and
is not an ordinary implicitly borrowing call.

## Zero-length arrays

`[0]T` has no elements and requires no element initialization or cleanup:

```arkoi
empty @[0]u8 = []
view @[]u8 = empty[..]
```

It preserves `T` and its alignment requirements but contains no element
storage. `[0]Resource` owns no resource instances and performs no element
cleanup. The full slice is an ordinary empty slice.

Any compile-time index is invalid:

!!! failure "Compile-time error — indexing a zero-length array"

    ```arkoi
    value @u8 = empty[0]
    ```

A dynamic index compiles as a recoverable bounds check and always produces
`CoreFail.out_of_range`.

## Indexing

A compile-time-known index is checked statically:

```arkoi
first @u32 = values[0]
values[2] = 42
```

An in-range constant access is infallible. An out-of-range one is a compile-time
error.

!!! failure "Compile-time error — constant index outside `[4]u32`"

    ```arkoi
    value @u32 = values[4]
    ```

A runtime index is recoverably checked:

```arkoi
value @u32 = values[index]!
values[index]! = 42
```

Failure produces `CoreFail.out_of_range`; writing requires mutable array
access. This is the key array/slice distinction:

```arkoi
array_value @u32 = values[0]
dynamic_value @u32 = values[index]!
slice_value @u32 = view[0]!
```

## Data and resource elements

`[N]T` is data when `T` is data and a resource when `T` is a resource. Data
elements copy normally:

```arkoi
number @u32 = numbers[0]
```

For a resource element, indexing identifies an existing owning place. It cannot
implicitly copy the element or move it out and leave the array partly
initialized. The whole resource array may move:

```arkoi
other @[2]File = move(files)
```

Resource elements may be borrowed, assigned, replaced, swapped, or taken when
optional. Dynamic access first performs the array bounds check. The canonical
rules for prohibited reads and moves, destination failure, `take`, `replace`,
`swap`, and construct-first assignment are defined once under [Places and
replacement](places-replacement.md).

Resource arrays clone and clean up element-by-element under the rules in
[Cloning](cloning.md) and [Resource lifecycle](resource-lifecycle.md).

## Array literals and repetition

Every element must be written in an array literal or produced by explicit
construction code. Arkoi has no `[value; N]` repetition form.

!!! failure "Compile-time error — array repetition"

    ```arkoi
    repeated @[4]u32 = [0; 4]
    ```

## Related topics

- [Slices](slices.md)
- [Constants and module globals](constants-globals.md)
- [Places and replacement](places-replacement.md)
- [Cloning](cloning.md)
- [Resource lifecycle](resource-lifecycle.md)
