# Slices

<!-- spec-sections: 6.1 (slice representation, access, lifetimes, globals, subslices) -->

A slice is a non-owning pointer-and-length view over contiguous elements. It
copies cheaply, bounds-checks indexing, and follows the same limited lifetime
analysis as references.

!!! abstract "At a glance"

    - `[]T` reads elements; `[]mut T` also writes them.
    - A slice owns neither storage nor elements and may not outlive its backing
      storage.
    - Indexing and non-complete ranges fail with `CoreFail.out_of_range`.
    - Mutable slices may alias and overlap.
    - A mutable-to-read-only view requires explicit `readonly(...)`.
    - Safe module slices are limited to compiler-proven static array storage.

## Representation and access

A slice conceptually stores a pointer to its first element and a `usize` element
count:

```arkoi
items @[]Item
mutable_items @[]mut Item
```

Binding mutability is separate from element permission:

| Declaration | Slice binding | Elements |
| --- | --- | --- |
| `items @[]Item` | Fixed | Read-only |
| `items @mut []Item` | Reassignable | Read-only |
| `items @[]mut Item` | Fixed | Mutable |
| `items @mut []mut Item` | Reassignable | Mutable |

Assignment and argument passing copy only the descriptor. They do not copy,
move, clone, own, or clean up elements.

```arkoi
fun inspect(items @[]Item):
    value @Item = items[0]!
    display(value)

fun modify(items @[]mut Item, replacement @Item):
    items[0]! = replacement

inspect(items)
modify(mutable_items, replacement)
```

No borrowing marker is needed to pass a slice because the slice itself is a
data value.

Mutable slices may alias or overlap:

```arkoi
first @[]mut Item = mutable_items
second @[]mut Item = mutable_items

first[0]! = a
second[0]! = b
```

`[]mut T` grants mutation permission, not exclusive or `noalias` access.

## Length and indexing

`length(items)` is an infallible language built-in returning the stored element
count as `usize`:

```arkoi
count @usize = length(items)
```

It has defined operand access and is not an ordinary implicitly borrowing call.

Slice indexing is recoverably bounds-checked even when the source index is a
literal, because slice length is a runtime value:

```arkoi
value @Item = items[index]!
mutable_items[index]! = replacement
first @Item = items[0]!
```

An invalid index produces `CoreFail.out_of_range`; writing also requires
`[]mut T`. For a resource element, indexing designates an addressable place,
not a copied or moved value. See [Places and
replacement](places-replacement.md).

## Subslices

Ranges are half-open: `start` is included, `end` excluded.

```arkoi
middle @[]Item = items[start..end]!
prefix @[]Item = items[..end]!
suffix @[]Item = items[start..]!
all @[]Item = items[..]
```

Bounds must satisfy `0 <= start <= end <= length(items)`. Negative indices are
unsupported. Invalid bounds produce `CoreFail.out_of_range`. The complete range
`items[..]` is always in range and needs no postfix `!`.

A subslice keeps its source access permission:

```arkoi
mutable @[]mut Item = mutable_items[start..end]!
read_only @[]Item = readonly(mutable_items[start..end]!)
all_read_only @[]Item = readonly(mutable_items[..])
```

There is no implicit `[]mut T` to `[]T` conversion, including in a subslice
initializer. A read-only slice can never become mutable. Independently created
mutable subslices may overlap.

## Lifetimes

A slice cannot outlive its backing storage. Arkoi rejects obvious local escapes:

!!! failure "Compile-time error — slice of local storage escapes"

    ```arkoi
    fun invalid_view() @[]u32:
        values @[4]u32 = [10, 20, 30, 40]
        return values[..]
    ```

A subslice derived directly from a slice parameter may be returned when the
relationship is lexically evident:

```arkoi
fun prefix(items @[]u32, end @usize) !CoreFail @[]u32:
    return items[..end]!
```

As with references, an owner cannot be moved, replaced, or dropped while a
derived slice is obviously still live. Arkoi uses limited lexical and
obvious-last-use checking, not complete lifetime or alias analysis.

### Module slices

Safe module slices are a narrow compiler-verified initializer exception over
module fixed-array storage with provable static lifetime:

```arkoi
values @[4]u32 = [10, 20, 30, 40]
view @[]u32 = values[..]

mutable_values @mut [4]u32 = [10, 20, 30, 40]
mutable_view @[]mut u32 = mutable_values[..]
```

This does not make slices general constant values. Local, parameter, temporary,
or otherwise shorter-lived storage cannot initialize a module slice:

!!! failure "Compile-time error — non-static global view"

    ```arkoi
    view @mut []u32

    fun initialize():
        values @[4]u32 = [10, 20, 30, 40]
        view = values[..]
    ```

A global movable, replaceable, or droppable resource is not safe static backing
merely because it is global. More complex global views require unsafe
construction and programmer-guaranteed lifetime. See [Constants and module
globals](constants-globals.md).

## Raw-pointer boundary

A slice exposes its element pointer only in unsafe context:

```arkoi
unsafe:
    pointer @*Item = items.pointer()
```

Constructing a slice from a raw pointer and length is also unsafe. The
programmer must guarantee the entire range is live, correctly aligned,
contiguous, and contains valid `T` elements for the slice's lifetime and access
mode. Mutable construction must additionally justify write permission; it does
not promise uniqueness.

## Related topics

- [Arrays](arrays.md)
- [References and lifetimes](references-lifetimes.md)
- [Raw pointers](raw-pointers.md)
- [Access reduction](access-reduction.md)
- [Places and replacement](places-replacement.md)
