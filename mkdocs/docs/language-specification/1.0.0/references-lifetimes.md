# References and lifetimes

<!-- spec-sections: 6.1 (references, borrowing, stable storage, limited lifetime checking) -->

References are non-null, non-owning aliases to stable storage. Arkoi checks basic
mutability and obvious lexical lifetime relationships without imposing
Rust-style exclusive borrowing.

!!! abstract "At a glance"

    - `&T` is read-only; `&mut T` permits mutation of the referent.
    - A reference is initialized once and cannot be rebound.
    - Ordinary function arguments are never borrowed implicitly.
    - References can target only stable addressable storage, not temporaries.
    - A reference transparently uses the value and place syntax of its referent.
    - Arkoi rejects obvious escapes and uses limited obvious-last-use checking.
    - Mutable references may alias; they are not an exclusivity promise.

## Reference types

```arkoi
&T       # Read-only reference
&mut T   # Mutable reference
```

A reference binding is fixed after its one initialization. It may use ordinary
delayed initialization, but it cannot use binding-level `mut`. Access is
selected by the reference type:

| Declaration | Referent access |
| --- | --- |
| `view @&Item = &item` | Read-only |
| `editable @&mut Item = &mut item` | Mutable |

References are data values. Copying a reference does not copy or take ownership
of its referent, and a reference never cleans that referent up. Such copying
occurs in a reference-typed initializer, argument, or return context and creates
another fixed alias to the same place.

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

Receiver arguments follow the same explicit borrowing and movement rules as
ordinary function arguments. See [Methods and visibility](methods-visibility.md).

## Transparent referent access

A reference name designates its referent in value and place expressions:

```arkoi
fun increment(value @&mut u32):
    value = value + 1

fun clear(file @&mut File):
    file.handle = 0
    File.flush(file)!
```

Reading uses the referent value. Assignment through `&mut T` writes the
referent; it never changes the reference binding. Assignment through `&T` is
invalid. Field, indexing, and operator syntax is identical to direct access on
`T`; an existing reference can be the explicit receiver argument of a
type-qualified method call.

An initialized reference destination always writes through, even when the
source is another reference:

```arkoi
destination @&mut u32 = &mut first
source @&u32 = &second
destination = source  # Copies second into first.
```

A reference has no dereference operator. Unary `*` is reserved for raw pointers
and requires `unsafe`.

Borrowing a reference name reborrows its referent. The reference binding is not
itself an addressable place:

```arkoi
alias @&Item = reference
reborrow @&Item = &reference
mutable_reborrow @&mut Item = &mut mutable_reference
```

The last form requires an existing `&mut Item`. A reference expression may also
be passed or returned directly when the surrounding type is a compatible
reference type.

## Stable addressable storage

A reference may be formed from stable storage such as:

- a local binding, parameter, or module variable;
- an aggregate field;
- an array or slice element; or
- allocated storage.

A compile-time constant has no storage and cannot be borrowed. Selecting a
field or element from a composite constant does not make it addressable.

Views created with `string_view(owned_string)` or `bytes(owned_string)` borrow
the owned string and follow these lifetime checks. Literal-backed string views
instead use program-lifetime storage.

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
    file @File = File.open(path)!
    reference @&File = &file

    consume(move(file))
    inspect(reference)
    ```

Arkoi consistently recognizes obvious last use:

```arkoi
file @File = File.open(path)!
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
