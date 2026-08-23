---
title: Arkoi 1.0 cloning
description: Arkoi 1.0 target-language rules for explicit cloning, clone hooks, resource copies, and clone failures.
---

# Cloning

<!-- spec-sections: 5.1 (clone hooks, sources, and composite cloning) -->

`clone(value)` asks a resource type for an independent owned value without
consuming the source. Cloneability and failure behavior come from the type's
`__clone__` contract.

!!! abstract "At a glance"

    - A resource is cloneable only through a valid `__clone__` hook.
    - `clone` accepts any readable, non-temporary resource place.
    - Its failure effect is exactly the selected hook or contained clone effect.
    - Optionals and arrays derive cloning from their contained resource.
    - Data—including slices—copies normally and rejects `clone`.
    - User resource aggregates receive no synthesized clone hook.

## Clone hooks and effects

An infallible hook declares no failure effect:

```arkoi
fun SharedHandle.__clone__(
    self @&SharedHandle,
) @SharedHandle:
    return duplicate_shared_handle(self)

copy @SharedHandle = clone(original)
```

A fallible hook declares its effect normally:

```arkoi
fun File.__clone__(self @&File) !IOFail @File:
    return duplicate_file(self)!

copy @File = clone(original)!
```

The caller may instead handle the failure:

```arkoi
copy @File = clone(original) handle failure:
    log_failure(failure)
    yield File.open(fallback_path)!
```

`clone(...)` is a language built-in with defined operand access, not an ordinary
call that inserts an implicit borrow. It reads the source place and invokes the
type's hook without moving or uninitializing that source.

The expression exposes **exactly** the hook's declared failure effect. Allocation
does not silently add an undeclared effect. A hook declared infallible must
handle any allocation failure internally; a fallible hook exposes allocation
failure only through its declared failure set (which may include `CoreFail`
directly or through Arkoi's failure-set inclusion rules).

Every resource is movable, but only a resource with a valid hook is cloneable.

## Valid source expressions

Unlike `move`, clone may read any accessible, non-temporary resource place:

```arkoi
binding_copy @File = clone(file)!
field_copy @File = clone(user.file)!
element_copy @File = clone(files[0])!
dynamic_copy @File = clone(files[index]!)!
```

For the dynamic example, the inner `!` propagates bounds failure before the hook
runs; the outer `!` propagates clone failure. The original binding, field, or
element remains initialized and owned in its existing location.

Cloning a fresh temporary is redundant because the temporary can enter the new
owning context directly.

!!! danger "Compile-time error — cloning a temporary"

    ```arkoi
    copy @File = clone(File.open(path)!)!
    ```

```arkoi
copy @File = File.open(path)!
```

## Data is copied, not cloned

`clone(...)` accepts resources only. Data uses assignment, parameter passing,
and return:

```arkoi
second @u32 = first
point_copy @Point = point
view_copy @[]u32 = view
```

!!! danger "Compile-time errors — cloning data"

    ```arkoi
    second @u32 = clone(first)
    point_copy @Point = clone(point)
    view_copy @[]u32 = clone(view)
    ```

A slice copy duplicates only its pointer-and-length descriptor, never its
elements.

## Optional resource cloning

When `T` is a resource, `?T` is cloneable exactly when `T` is cloneable:

```arkoi
copied_optional @?File = clone(optional)!
```

- `none` clones to `none`.
- A present value is cloned through `T.__clone__`.
- The expression has exactly `T`'s clone failure effect.

## Resource-array cloning

When `T` is a resource, `[N]T` is cloneable exactly when `T` is cloneable:

```arkoi
copied_files @[2]File = clone(files)!
```

Elements clone from first to last. If an element clone fails, the newly cloned
prefix is dropped in reverse order before propagation; the original array is
unchanged.

The array exposes exactly `T`'s clone effect. It does not wrap it in an
`ArrayCloneFail`, and nested built-in composites preserve that same effect
transitively.

Data-only optionals and arrays remain data and copy normally; `clone(...)` is
invalid for them.

## User resource aggregates

Arkoi does not synthesize cloning for a user-declared resource aggregate. Its
author must define `__clone__` and can choose the resource's independence and
failure semantics explicitly.

## Related topics

- [Ownership and moves](ownership-moves.md)
- [Places and replacement](places-replacement.md)
- [Resource lifecycle](resource-lifecycle.md)
- [Arrays](arrays.md)
- [Failures](failures.md)
