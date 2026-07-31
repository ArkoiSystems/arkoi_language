# References and lifetimes

<!-- spec-sections: 6.1 (references, borrowing, stable storage, limited lifetime checking) -->

References are non-null, non-owning views of stable storage. Arkoi checks basic
mutability and obvious lexical lifetime relationships without imposing
Rust-style exclusive borrowing.

!!! abstract "At a glance"

    - `&T` is read-only; `&mut T` permits mutation of the referent.
    - Ordinary function arguments are never borrowed implicitly.
    - References can target only stable addressable storage, not temporaries.
    - Safe unary `*` dereferences a reference; field and method access use `.`.
    - Arkoi rejects obvious escapes and uses limited obvious-last-use checking.
    - Mutable references may alias; they are not an exclusivity promise.

## Reference types

```arkoi
&T       # Read-only reference
&mut T   # Mutable reference
```

Binding mutability and referent access are separate:

| Declaration | Binding | Referent |
| --- | --- | --- |
| `a @&Item` | Fixed | Read-only |
| `b @&mut Item` | Fixed | Mutable |
| `c @mut &Item` | Reassignable | Read-only |
| `d @mut &mut Item` | Reassignable | Mutable |

References are data values. Copying a reference does not copy or take ownership
of its referent, and a reference never cleans that referent up.

## Borrowing

Ordinary function calls require explicit `&` or `&mut`:

```arkoi
fun inspect(file @&File):
    print_file(file)

fun modify(file @&mut File):
    update_file(file)

inspect(&file)
modify(&mut file)
```

The target of `&mut` must itself permit mutable access.

!!! failure "Compile-time errors — missing explicit borrow"

    ```arkoi
    inspect(file)
    modify(file)
    ```

Dot method syntax has a narrow receiver-only borrowing rule; it does not change
ordinary argument behavior. See [Methods and
visibility](methods-visibility.md).

## Dereference and member access

Safe unary `*` dereferences a valid reference:

```arkoi
fun increment(value @&mut u32):
    *value = *value + 1
```

Reference-based fields and methods use ordinary dot access:

```arkoi
handle @u64 = file_reference.handle
file_reference.flush()!
```

This differs from a raw pointer: unary `*` on a raw pointer is unsafe, and its
field access is written `(*pointer).field`.

## Stable addressable storage

A reference may be formed from stable storage such as:

- a local binding or parameter;
- an aggregate field;
- an array or slice element; or
- allocated storage.

Taking a reference does not extend a temporary's lifetime.

!!! failure "Compile-time error — reference to a temporary"

    ```arkoi
    reference @&Item = &make_item()
    ```

Store the value in a stable binding first when its lifetime must continue.

## Limited lifetime checking

Arkoi rejects relationships that are obviously invalid, including returning a
reference to a local binding:

!!! failure "Compile-time error — local reference escape"

    ```arkoi
    fun invalid() @&Item:
        item @Item = make_item()
        return &item
    ```

A reference derived directly from a parameter may be returned when the
relationship is lexically evident:

```arkoi
fun identity(item @&Item) @&Item:
    return item
```

An owned value cannot be moved, replaced, or dropped while an obviously live
reference or slice derived from it can still be used.

!!! failure "Compile-time error — moving an obviously borrowed value"

    ```arkoi
    file @File = File.open(&path)!
    reference @&File = &file

    consume(move(file))
    inspect(reference)
    ```

Arkoi consistently recognizes obvious last use:

```arkoi
file @File = File.open(&path)!
reference @&File = &file
inspect(reference)

consume(move(file))
```

The same rule governs construct-first assignment, `replace(...)`, and lexical
cleanup. This is limited lexical and obvious-last-use analysis, not a complete
borrow checker. When a complex safe relationship cannot be proven, restructure
the code or use an explicitly available unsafe operation while maintaining its
invariants.

References—including mutable references—may alias. `&mut T` grants mutation
permission; it does not promise unique or `noalias` access.

## Related topics

- [Access reduction](access-reduction.md)
- [Raw pointers](raw-pointers.md)
- [Slices](slices.md)
- [Ownership and moves](ownership-moves.md)
- [Places and replacement](places-replacement.md)
