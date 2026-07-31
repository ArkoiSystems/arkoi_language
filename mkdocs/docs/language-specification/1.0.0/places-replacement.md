# Places and replacement

<!-- spec-sections: 5.1 (take, replace, swap); 5.5; 6.1 (resource places and direct assignment) -->

A *place* is existing addressable storage: a local binding, field, array or
slice element, or another mutable location. Resource places own their current
value and must remain fully initialized.

!!! abstract "At a glance"

    - Resource fields and elements may be borrowed or replaced, but never copied
      or moved out directly.
    - `take` replaces an optional place with `none`.
    - `replace` installs a new value and returns the old one.
    - `swap` exchanges two initialized places without allocation or cleanup.
    - Direct resource assignment constructs first, then drops and installs.
    - A named resource replacement requires `move(...)`.

## Resource-valued places

Reading a data place may copy its value. Reading a resource place as an ordinary
value would imply an ownership copy and is invalid:

!!! failure "Compile-time error — copying a resource element"

    ```arkoi
    file @File = files[0]
    ```

Borrowing preserves ownership:

```arkoi
inspect(&files[0])
modify(&mut files[0])
```

Directly moving a resource field or element is also invalid because its
container would become partly initialized:

!!! failure "Compile-time error — moving out of a resource place"

    ```arkoi
    first @File = move(files[0])
    ```

Move a whole owning binding or use one of the operations below, which restores
the destination immediately.

## `take`: empty an optional place

`take(...)` accepts a mutable place of type `?T`, whether `T` is data or a
resource. It returns the previous optional and writes `none`:

```arkoi
current @mut ?File = File.open(&path)!
taken @?File = take(&mut current)
```

When present, a resource transfers into the returned optional; data returns
normally. The operation itself is infallible after its place has been resolved.
Any indexing failure belongs to resolving that place.

Postfix `!` can require a present result:

```arkoi
file @File = take(&mut current)!
```

If the old value is `none`, this propagates `CoreFail.none_access`. Semantically:

```arkoi
taken @?T = replace(&mut optional, none)
```

## `replace`: return the old value

`replace(place, replacement)` works for data and resources:

```arkoi
old_count @u32 = replace(&mut count, 42)
old_file @File = replace(
    &mut file,
    File.open(&new_path)!,
)
```

A named replacement resource must transfer ownership:

```arkoi
old_file @File = replace(&mut file, move(replacement))
```

The destination may be a mutable local, field, array or slice element, or
another addressable mutable place. Resolution and replacement follow this
order:

1. resolve the destination place, including any bounds check;
2. fully evaluate and construct the replacement;
3. if construction fails, leave the old value unchanged;
4. remove the old value; and
5. install the new value.

The place is never observably uninitialized. For resources, the caller owns the
returned old value. If it is not stored or moved elsewhere, temporary cleanup
drops it normally.

## `swap`: exchange two places

```arkoi
swap(&mut first, &mut second)
swap(&mut files[0], &mut files[1])
```

Both arguments must be fully initialized mutable places of exactly the same
type. `swap(...)` is infallible, works for data and resources, allocates
nothing, and neither clones nor drops either value. Resource ownership simply
changes places; no `move` marker is needed.

Swapping the exact same typed place is valid and does nothing:

```arkoi
swap(&mut value, &mut value)
swap(&mut items[0], &mut items[0])
```

Distinct places may alias under Arkoi's permissive mutable-reference and slice
rules. If unsafe code supplies differently described places that partially
overlap without being the exact same typed place, using `swap` has invalid
unsafe behavior.

## Direct resource assignment

A mutable resource binding or place supports construct-first,
drop-and-replace assignment:

```arkoi
file @mut File = File.open(&first_path)!
file = File.open(&second_path)!

files @mut [2]File = [
    File.open(&first_path)!,
    File.open(&second_path)!,
]
files[0] = File.open(&replacement_path)!
```

Direct assignment drops the old resource only after the replacement is ready,
so failure preserves the destination. The complete evaluation sequence is
defined once under [resource replacement
assignment](assignment-and-result-use.md#resource-replacement).

A named replacement resource still requires `move(...)`:

```arkoi
replacement @File = File.open(&replacement_path)!
files[0] = move(replacement)

user.file = File.open(&path)!
files[index]! = move(replacement)
mutable_files[index]! = File.open(&path)!
```

Direct assignment drops the old value. `replace(...)` differs because it
returns ownership of that old value.

### Assignment from a moved value

For `first = move(second)`, the conceptual order is:

1. prepare the move from `second`;
2. drop the old value in `first`;
3. install the moved value in `first`; and
4. mark `second` uninitialized.

Self-move assignment is rejected.

!!! failure "Compile-time error — self-move assignment"

    ```arkoi
    file = move(file)
    ```

Assignment to a moved, uninitialized mutable binding is instead ordinary
reinitialization and has no old value to drop.

## Place operations at a glance

| Operation | Destination after | Old value |
| --- | --- | --- |
| `place = replacement` | Replacement | Dropped |
| `replace(&mut place, replacement)` | Replacement | Returned |
| `take(&mut optional)` | `none` | Returned as optional |
| `swap(&mut a, &mut b)` | Values exchanged | Remains in the two places |

Resource fields and indexed elements may be borrowed, mutated through permitted
access, assigned, replaced, swapped, or taken when optional. They are never
implicitly copied or left partly initialized. `take`, `replace`, and `swap`
also work for data places when their type requirements are met.

## Related topics

- [Ownership and moves](ownership-moves.md)
- [Resource lifecycle](resource-lifecycle.md)
- [Arrays](arrays.md)
- [Slices](slices.md)
- [Bindings and initialization](bindings-initialization.md)
