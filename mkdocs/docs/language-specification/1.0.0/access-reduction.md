---
title: Arkoi 1.0 access reduction
description: Arkoi 1.0 target-language rules for reducing mutable access to read-only references, pointers, slices, and arrays.
---

# Access reduction

<!-- spec-sections: 6.2–6.3 -->

`readonly(...)` explicitly removes one level of mutation permission from a
reference, slice, or raw pointer. Arkoi never performs these conversions
implicitly.

!!! abstract "At a glance"

    - Supported forms are `&mut T → &T`, `[]mut T → []T`, and `*mut T → *T`.
    - The operation is infallible, non-consuming, and not overloadable.
    - It preserves address and, for slices, length.
    - It changes only the outer access view; it does not recursively freeze.
    - The original mutable-access value remains usable under Arkoi's permissive
      aliasing rules.

## Defined conversions

| Input | Output | Preserved |
| --- | --- | --- |
| `&mut T` | `&T` | Same referent address |
| `[]mut T` | `[]T` | Same start address and length |
| `*mut T` | `*T` | Same pointer address |

```arkoi
mutable_reference @&mut Data = get_mutable_data()
reference_view @&Data = readonly(mutable_reference)

mutable_slice @[]mut Item = get_mutable_items()
slice_view @[]Item = readonly(mutable_slice)

mutable_pointer @*mut Item = get_mutable_pointer()
pointer_view @*Item = readonly(mutable_pointer)
```

For all supported types, `readonly`:

- takes exactly one operand;
- is infallible and not overloadable;
- copies, moves, and mutates no underlying value;
- neither modifies nor consumes the input; and
- removes only mutation permission from the returned view.

For a slice, no elements are copied.

## Explicit call and overload selection

There is no implicit `&mut T` to `&T` conversion:

```arkoi
fun inspect(value @&Collection):
    print_collection(value)

collection @&mut Collection = get_mutable_collection()
inspect(readonly(collection))
```

!!! danger "Compile-time error — implicit access reduction"

    ```arkoi
    inspect(collection)
    ```

This keeps exact overload resolution predictable:

```arkoi
fun inspect(value @&Collection):
    inspect_read_only(value)

fun inspect(value @&mut Collection):
    inspect_mutable(value)

inspect(collection)            # Selects &mut Collection.
inspect(readonly(collection))  # Selects &Collection.
```

The same explicit rule applies to slices, including mutable subslices:

```arkoi
mutable_part @[]mut Item = items[start..end]!
read_only_part @[]Item = readonly(items[start..end]!)
```

## Input remains available

Arkoi permits aliases, so the original access value can be used again:

```arkoi
items @[]mut Item = get_mutable_items()
inspect_all(readonly(items))
modify_first(items)
```

The name describes the returned view. It neither freezes the referent nor makes
the underlying object permanently immutable.

## One-level conversion only

`readonly` does **not**:

- turn an owned value into a reference or implicitly borrow it;
- turn a raw pointer into a reference;
- turn read-only access back into mutable access;
- recursively alter nested mutability in a composite;
- copy, move, or freeze the underlying object; or
- serve as a general numeric or representation conversion.

!!! danger "Compile-time error — unsupported operand"

    ```arkoi
    value @Data = create_data()
    readonly(value)
    ```

Use the checked `convert(...)!` family for general conversion.

## Iteration receiver selection

`readonly(reference)` can deliberately select read-only iteration through an
existing mutable reference; the complete receiver and loop-binding rules are in
the [for-loop source-selection matrix](for-loops.md#source-selection-matrix).

## Related topics

- [References and lifetimes](references-lifetimes.md)
- [Slices](slices.md)
- [Raw pointers](raw-pointers.md)
- [Calls and overloads](calls-overloads.md)
- [For loops](for-loops.md#source-selection-matrix)
