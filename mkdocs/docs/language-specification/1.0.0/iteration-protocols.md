# Iteration protocols

<!-- spec-sections: 11.5–11.8, 11.11–11.13, 11.18 -->

This page defines the canonical source-level interfaces used by custom `for` and `for!` loops.

!!! abstract "At a glance"
    Iteration chooses one creation interface from the source mode and one advancement interface from the concrete iterator type. `Item` must match exactly. `for` requires both calls to be infallible; `for!` is required when either can fail.

## Protocol matrix

The bundled core library provides these compiler-recognized interfaces:

| Receiver mode | Infallible creation | Fallible creation | Hook receiver |
| --- | --- | --- | --- |
| Read-only | `Iterable` | `FallibleIterable` | `__iterate__(self @&Self)` |
| Mutable | `MutableIterable` | `FallibleMutableIterable` | `__iterate_mut__(self @&mut Self)` |
| Consuming | `OwningIterable` | `FallibleOwningIterable` | `__into_iterator__(self @own Self)` |

Advancement uses one of:

| Interface | Associated types | Requirement |
| --- | --- | --- |
| `Iterator` | `Item` | `__next__(self @&mut Self) @?Item` |
| `FallibleIterator` | `Item`, `Failure` | `__next__(self @&mut Self) !Failure @?Item` |

Every creation interface binds `Item` and `Iterator`; fallible creation also binds `Failure`. A `Failure` binding must name a failure type. The returned `Iterator` must be a named aggregate implementing exactly one advancement interface, and its `Item` must exactly equal the creation interface's `Item`.

Receiver-mode syntax is defined once in the [source-selection matrix](for-loops.md#source-selection-matrix).

## Canonical declarations

The essential shapes are:

```arkoi
interface Iterator:
    type Item

    fun __next__(
        self @&mut Self,
    ) @?Item

interface FallibleIterator:
    type Item
    type Failure

    fun __next__(
        self @&mut Self,
    ) !Failure @?Item
```

```arkoi
interface Iterable:
    type Item
    type Iterator

    fun __iterate__(
        self @&Self,
    ) @Iterator

interface MutableIterable:
    type Item
    type Iterator

    fun __iterate_mut__(
        self @&mut Self,
    ) @Iterator

interface OwningIterable:
    type Item
    type Iterator

    fun __into_iterator__(
        self @own Self,
    ) @Iterator
```

Each fallible creation interface has the same shape as its receiver-mode counterpart, adds `type Failure`, and declares `!Failure` on its hook. Thus `FallibleIterable` uses `__iterate__`, `FallibleMutableIterable` uses `__iterate_mut__`, and `FallibleOwningIterable` uses `__into_iterator__`.

Concrete hooks remain ordinary external methods:

```arkoi
implements Iterable for AccountCollection:
    type Item = &Account
    type Iterator = AccountIterator

implements Iterator for AccountIterator:
    type Item = &Account

fun AccountCollection.__iterate__(
    self @&AccountCollection,
) @AccountIterator:
    return AccountIterator.create(self)

fun AccountIterator.__next__(
    self @&mut AccountIterator,
) @?&Account:
    return AccountIterator.take_next(self)
```

`__into_iterator__` uses an owning receiver because consuming iteration takes
ownership of its resource iterable.

## Loop execution

Conceptually, read-only iteration behaves as:

```arkoi
iterator @mut AccountIterator = AccountCollection.__iterate__(&accounts)

loop:
    next @?&Account = AccountIterator.__next__(&mut iterator)

    if next == none:
        break

    account @&Account = next!
    process(account)
```

This fixes observable semantics without requiring that exact compiler output:

- evaluate the iterable expression once;
- call the selected creation hook once;
- own its result in a hidden mutable local;
- call `__next__` before each iteration;
- stop on `none`, or initialize the binding from a present item;
- have `continue` proceed to the next `__next__`;
- clean up the hidden iterator on exhaustion, `break`, `return`, propagated failure, or any other exit.

All protocol calls are direct and statically resolved. They introduce no interface values, dynamic dispatch, function values, or indirect calls. Loop `in` does not invoke membership `__contains__`.

## Fallibility selection

Creation and advancement may be independently fallible, yielding four valid combinations.

The propagation marker belongs to the loop statement:

```arkoi
for! record @&Record in database:
    process(record)

for record @&Record in database!:  # Compile-time error
    process(record)
```

| Creation | Advancement | Loop spelling |
| --- | --- | --- |
| Infallible | Infallible | `for` |
| Fallible | Infallible | `for!` |
| Infallible | Fallible | `for!` |
| Fallible | Fallible | `for!` |

Using `for` when either call is fallible is a compile-time error. Using `for!` when both are infallible is also a compile-time error.

`for!` propagates failures from the single creation call and every fallible advancement call. The enclosing function must declare every possible failure, either directly or through a combined failure set:

```arkoi
failure DatabaseIterationFail = OpenFail | ReadFail
```

If creation fails, the body is never entered. If advancement fails, the current iteration ends immediately, the hidden iterator is cleaned up, and the failure propagates. The body may independently use ordinary handling or propagation. Built-in array and slice iteration is infallible and therefore uses `for`.

## Receiver-specific contracts

### Mutable creation

The mutable iterator may retain receiver-derived access for the loop's duration. Ordinary lexical checks reject obvious movement, replacement, or destruction of a directly known source while it remains live.

Arkoi does not impose Rust-style exclusivity. Aliases may exist when ordinary reference rules allow them, and the type author is responsible for storage stability. A failed mutable creation call never enters the body; after successful creation, ordinary iterator lifetime and cleanup rules apply.

### Consuming creation

Ownership moves from the source before `__into_iterator__` starts. The hidden iterator owns transferred state after success. It may yield copyable data, references subject to the iterator-reference contract, or newly owned resources.

For fallible consuming creation, ownership is **not** rolled back:

- the source binding remains uninitialized;
- the `value` parameter must clean up resources it still owns before propagation;
- state already moved into another owner is cleaned up by that owner during unwinding;
- no partially created iterator becomes visible to the loop.

After a consuming loop, the source is not reconstructed. A mutable source binding may later be reinitialized; an immutable one may not. Built-in arrays and slices provide no consuming protocol.

## Unambiguous conformance

A type may support any combination of read-only, mutable, and consuming modes, with different `Item` and `Iterator` bindings in each mode. Within one mode it may implement at most one creation interface:

| Mode | Mutually exclusive pair |
| --- | --- |
| Read-only | `Iterable` / `FallibleIterable` |
| Mutable | `MutableIterable` / `FallibleMutableIterable` |
| Consuming | `OwningIterable` / `FallibleOwningIterable` |

An iterator type may implement exactly one of `Iterator` and `FallibleIterator`. Competing implementations are compile-time errors. `Item` must match within the selected creation/advancement pair but need not match between receiver modes.

## Canonical identity

The canonical interfaces are ordinary, inspectable Arkoi declarations in the bundled core library and are available through the core prelude or an equivalent implicit mechanism. The exact physical module path and packaging are implementation details.

The compiler recognizes these declarations by **resolved declaration identity**, not by unqualified name, qualified spelling alone, member names, or structural similarity. A user interface named `Iterator` is ordinary and activates no loop behavior.

Every compiler installation must provide one compatible canonical set. Users cannot replace or redefine it; alternative interfaces may coexist under other declaration identities without special semantics.

## Related topics

- [For loops](for-loops.md)
- [Iterator values and lifetimes](iterator-values-and-lifetimes.md)
- [Static interfaces](interfaces-overview.md)
- [Interface implementations](interface-implementations.md)
- [Failure handling](failures.md)
- [Compiler hooks](compiler-hooks.md)
