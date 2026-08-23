---
title: Arkoi 1.0 iterator values and lifetimes
description: Arkoi 1.0 target-language rules for iterator result ownership, yielded references, invalidation, and escape lifetimes.
---

# Iterator values and lifetimes

<!-- spec-sections: 11.9–11.10, 11.16–11.17 -->

This page defines ownership and reference validity for yielded items, loop-binding mutability, and exhaustion.

!!! abstract "At a glance"
    A yielded resource is owned by the current iteration. Yielded references may escape, but Arkoi does not prove their general validity. `@mut` changes the local loop binding, not the source element, and a loop stops at its first `none`.

## Owned resource items

A custom iterator may return an owned resource as `Item`:

```arkoi
implements FallibleIterator for FileLoader:
    type Item = File
    type Failure = IOFail

fun FileLoader.__next__(
    self @&mut FileLoader,
) !IOFail @?File:
    return FileLoader.load_next(self)!
```

This example assumes that `loader` has an applicable creation-interface
implementation whose `Item` is `File` and whose `Iterator` is `FileLoader`.
Only the advancement implementation is shown here.

For `for! file @File in loader`, each present value transfers ownership to the fresh loop binding. The item is cleaned up at the end of that iteration unless the body transfers it:

```arkoi
for! file @File in loader:
    archive(move(file))
```

`continue` cleans up a still-owned item before advancement. `break`, `return`, propagation, and other exits clean it up before leaving its scope. After `move(file)`, the binding is uninitialized and the loop does not clean that item twice; using it again requires a valid mutable-binding reinitialization.

The iterator implementation must produce a valid owned resource by construction, transfer from its internal state, or another explicit ownership operation. A possibly failing production step uses `FallibleIterator` and `for!`.

The possible associated-type bindings below are alternatives, not declarations
that appear together in one implementation:

- `type Item = &File` for a borrowed read-only item;
- `type Item = &mut File` for a borrowed mutable item; or
- `type Item = File` for a newly owned item.

Built-in arrays and slices never implicitly yield owned resource elements. This rule applies only when a custom iterator returns an owned resource.

## Yielded references

Custom iterators may yield `&T` or `&mut T`. Arkoi does not determine whether such a reference points into the iterable, iterator storage, reusable temporary storage, or another owner.

The author must ensure that every yielded reference remains valid for every ordinary permitted use, including storing it beyond the iteration:

```arkoi
matches @mut ReferenceCollection = create_collection()

for item @&Item in source:
    if matches_filter(item):
        ReferenceCollection.append(&mut matches, item)
```

The compiler applies only its limited lexical checks and may reject obvious invalidation, such as moving a directly known source while a visibly derived reference is live. It does not automatically expire a yielded reference at iteration end or before the next `__next__`.

An iterator using reusable internal storage must document and enforce its validity restrictions. Advancing while an earlier stored reference becomes invalid is programmer error and may cause undefined behavior.

For mutable references, the author must additionally ensure:

- storage remains valid and supports the mutation;
- advancement or iterator destruction does not invalidate a still-live reference unless the program stops using it;
- aliasing and overlap obey Arkoi's permissive ordinary reference model.

Arkoi deliberately does not perform Rust-style borrow or lifetime analysis for iterator results.

## Mutable loop bindings

`@mut` makes each iteration's fresh local binding mutable when its type permits
binding mutability:

```arkoi
for value @mut u32 in numbers:
    value += 1
```

For a copyable data item, this changes only the local copy, not the collection. To mutate an element, iterate a mutable reference:

```arkoi
for value @&mut u32 in &mut numbers:
    value += 1
```

The reference remains bound to the yielded element for that iteration.
`&mut u32` permits writing the element directly; reference loop bindings cannot
use binding-level `mut`.

An owned resource item may also use a mutable owning binding:

```arkoi
for file @mut File in move(files):
    file = open_replacement()!
```

Ordinary resource-binding rules apply: replacement is constructed before the old value is dropped; `move(file)` uninitializes the binding; a mutable binding may be reinitialized after a move; an immutable one may not; any still-owned value is cleaned up when the iteration scope exits.

The declared binding type must exactly equal the iterator's `Item`; binding-level `mut` is not part of the value type. A new binding and scope are created for every iteration.

## Exhaustion

A loop stops immediately at the first `none` returned by `__next__` and makes no further advancement call.

Arkoi does **not** require an iterator to remain exhausted. A later manual call may return another `none`, a new item, or—when fallible—a failure. The compiler does not cache `none`, add permanent-exhaustion state, or require fused behavior.

An iterator may document and implement permanent exhaustion itself. The loop contract remains deterministic: that loop ends on its first observed `none`.

## Related topics

- [Iteration protocols](iteration-protocols.md)
- [For loops](for-loops.md)
- [Ownership and moves](ownership-moves.md)
- [References and lifetimes](references-lifetimes.md)
- [Places and replacement](places-replacement.md)
- [Failure handling](failures.md)
