# Resource lifecycle

<!-- spec-sections: 5.3–5.4 -->

Resources are constructed as complete values and cleaned up deterministically
when their lexical lifetime ends. Cleanup itself cannot produce a recoverable
failure.

!!! abstract "At a glance"

    - Constructors are ordinary associated functions returning complete values.
    - An optional `__drop__` hook is infallible.
    - Every initialized resource is cleaned up on every scope-exit path.
    - Cleanup runs in reverse successful-initialization order.
    - Moving suppresses cleanup at the source; reinitialization restores it.

## Construction

Arkoi has no initialization hook. A resource type exposes ordinary, possibly
fallible associated functions:

```arkoi
pub fun File.open(path @&string) !IOFail @File:
    handle @u64 = open_handle(path)!
    return File(handle = handle)
```

A type may define several named constructors. Each returns a fully initialized
value. Constructor-local resources are cleaned up automatically if construction
fails; locals transferred into result fields must be moved explicitly.
Calling code can never observe a partially initialized resource.

`File` is illustrative user or library code, not a built-in type.

## Cleanup hooks

A resource may define `__drop__`:

```arkoi
fun File.__drop__(self @&mut File):
    close_handle(self.handle)
```

The hook is infallible. It cannot:

- declare a failure effect;
- let `fail` escape; or
- propagate with postfix `!`.

It may call a fallible operation only if it handles every possible failure
internally. Automatic cleanup therefore cannot replace an already propagating
failure or otherwise change normal control flow. Ordinary unrecoverable traps
remain possible.

When cleanup success matters to the program, expose an explicit fallible method
instead:

```arkoi
pub fun File.close(self @&mut File) !IOFail:
    close_native_file(self.handle)!
```

## When cleanup runs

An initialized resource is cleaned up when its lexical lifetime ends through:

- normal block completion;
- `return`;
- `fail`;
- postfix `!` or `?` propagation;
- `break`; or
- `continue`.

A moved or otherwise uninitialized binding is not dropped. If a mutable binding
is later reinitialized, its new value becomes eligible for cleanup.

## Cleanup order

| Owner | Order |
| --- | --- |
| Local resources | Reverse order of successful initialization |
| Resource aggregate | Its `__drop__`, then initialized resource fields in reverse declaration order |
| `[N]Resource` | Index `N - 1` down to `0` |
| Nested arrays | Reverse order recursively at each level |
| Partially constructed array | Successfully initialized prefix in reverse order |

The language prevents double cleanup of fields and elements.

Data values do not have resource cleanup. References, slices, and raw pointers
do not clean up the storage or resource they observe.

## Failure during construction

If construction stops by failure, every resource successfully initialized
inside the constructor or partial composite is dropped according to the same
reverse-order rules. No incomplete value escapes.

This also governs partial array cloning; see [Cloning](cloning.md).

## Related topics

- [Ownership and moves](ownership-moves.md)
- [Cloning](cloning.md)
- [Places and replacement](places-replacement.md)
- [Methods and visibility](methods-visibility.md)
- [Failures](failures.md)
