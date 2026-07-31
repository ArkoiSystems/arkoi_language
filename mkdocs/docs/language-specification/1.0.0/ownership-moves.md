# Ownership and moves

<!-- spec-sections: 5.1 (move, owning parameters, temporaries, returns, whole-value moves) -->

Every resource has one owning context. `move(...)` transfers a named resource
without allocating or copying and makes the source uninitialized.

!!! abstract "At a glance"

    - Data copies normally and cannot be moved.
    - `move(binding)` accepts only a whole named resource binding.
    - A moved mutable binding may be initialized again; an immutable one may not.
    - Ownership-taking resource parameters use `@own`.
    - Named resource returns require `move`; temporaries enter owning contexts
      directly.
    - Fields and indexed elements cannot be moved out individually.

## Moving a resource

```arkoi
first @string = string("Arkoi")!
second @string = move(first)
```

`move(first)` transfers ownership without allocation. `first` is uninitialized
afterward, so any use before valid reinitialization is a compile-time error.
Definite-initialization analysis tracks this state across all control-flow paths
and drops a resource only on paths where it is initialized.

!!! failure "Compile-time error — use after move"

    ```arkoi
    first @string = string("Arkoi")!
    second @string = move(first)
    print_string(string_view(first))
    ```

### Data copies; it does not move

Numbers, Booleans, characters, string views, references, raw pointers, slices,
failures, and data aggregates copy through normal value operations:

```arkoi
second @u32 = first
view_copy @[]u32 = view
```

!!! failure "Compile-time errors — moving data values"

    ```arkoi
    invalid @u32 = move(first)
    invalid_view @[]u32 = move(view)
    ```

## Reinitializing a moved binding

A mutable resource binding can be initialized again after a move:

```arkoi
file @mut File = File.open(first_path)!
consume(move(file))

file = File.open(second_path)!
```

No old value remains, so this is initialization rather than drop-and-replace.
The new value becomes eligible for automatic cleanup. An immutable resource
binding was already initialized once and cannot be initialized again.

!!! failure "Compile-time error — reinitializing an immutable binding"

    ```arkoi
    file @File = File.open(first_path)!
    consume(move(file))
    file = File.open(second_path)!
    ```

## What `move` accepts

`move(...)` accepts a whole named resource binding, including an ownership-taking
resource parameter:

```arkoi
fun forward(file @own File) @File:
    return move(file)
```

It cannot extract a field or indexed element, because that would leave the
containing value partly initialized.

!!! failure "Compile-time errors — partial moves"

    ```arkoi
    other @File = move(user.file)
    first @File = move(files[0])
    ```

Move the complete aggregate or array instead:

```arkoi
backup @User = move(user)
other @[2]File = move(files)
```

Resources may also be moved into a newly constructed aggregate:

```arkoi
user @User = User(
    name = move(name),
    file = move(file),
)
```

To extract while immediately restoring an initialized place, use
[Places and replacement](places-replacement.md).

## Owning parameters

An ownership-taking resource parameter is marked `@own`:

```arkoi
fun consume(file @own File):
    process_file(&file)

consume(move(file))
```

A named resource argument must be moved explicitly.

!!! failure "Compile-time error — implicit ownership transfer"

    ```arkoi
    consume(file)
    ```

A resource parameter without a reference or ownership mode is invalid:

!!! failure "Compile-time error — plain resource parameter"

    ```arkoi
    fun consume(file @File):
        pass
    ```

Data parameters copy normally and never use `own`.

## Temporaries

A fresh resource temporary has no reusable source binding, so it enters an
owning destination or parameter directly:

```arkoi
name @string = load_name()!
consume(load_file()!)
```

Wrapping a temporary in `move(...)` is redundant and invalid.

!!! failure "Compile-time error — moving a temporary"

    ```arkoi
    consume(move(File.open(path)!))
    ```

## Returning resources

Returning a named resource binding transfers ownership and therefore requires
`move(...)`:

```arkoi
fun create_file(path @string_view) !IOFail @File:
    file @File = File.open(path)!
    return move(file)
```

!!! failure "Compile-time error — named resource return without move"

    ```arkoi
    fun invalid(path @string_view) !IOFail @File:
        file @File = File.open(path)!
        return file
    ```

An owning parameter follows the same rule. A temporary resource expression can
be returned directly:

```arkoi
fun create_file(path @string_view) !IOFail @File:
    return File.open(path)!
```

Data values and non-owning references return normally.

`File` in these examples is an illustrative user or library resource type, not
a language built-in.

## Related topics

- [Cloning](cloning.md)
- [Places and replacement](places-replacement.md)
- [Resource lifecycle](resource-lifecycle.md)
- [Aggregates and enums](aggregates-enums.md)
- [Bindings and initialization](bindings-initialization.md)
