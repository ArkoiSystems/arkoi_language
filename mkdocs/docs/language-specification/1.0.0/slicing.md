# Slicing

<!-- spec-sections: 12.4–12.7 -->

This page defines custom half-open slices, optional bound representation, return types, and explicit mutable-slice selection.

!!! abstract "At a glance"
    Slicing uses dedicated `__slice__` and `__slice_mut__` hooks with two `?usize` bounds. Ordinary syntax is always read-only; only an explicit mutable borrow of the complete slicing expression selects `__slice_mut__`.

## Hook forms

Named data and resource aggregates may define either or both modes:

```arkoi
fun Container.__slice__(
    self @&Container,
    start @?usize,
    end @?usize,
) !CoreFail @&ContainerView:
    return Container.view_ref(self, start, end)!

fun Container.__slice_mut__(
    self @&mut Container,
    start @?usize,
    end @?usize,
) !CoreFail @&mut MutableContainerView:
    return Container.mutable_view_ref(self, start, end)!
```

Slicing is distinct from indexing:

```arkoi
container[index]
container[start..end]
container[..end]
container[start..]
container[..]
```

Range syntax never resolves through `__index__` or `__index_mut__`. Slice hooks
may be overloaded under ordinary exact resolution and may be fallible or
infallible. A fallible call requires postfix propagation or explicit handling.
Selecting a mode for which the type has no matching hook is a compile-time
error.

The receiver evaluates first, then present bounds from left to right. Mutable slicing requires a stable mutable receiver. Receiver-derived results follow the same stability and lifetime rules as indexed results.

Built-in arrays and slices keep compiler-defined slicing and do not use these hooks.

## Bounds

Both hook parameters are always `?usize`:

| Source form | `start` argument | `end` argument |
| --- | --- | --- |
| `container[start..end]` | evaluated `start` | evaluated `end` |
| `container[..end]` | `none` | evaluated `end` |
| `container[start..]` | evaluated `start` | `none` |
| `container[..]` | `none` | `none` |

An omitted bound performs no evaluation. The hook assigns the custom type's meaning to `none`; conventionally it means the beginning or logical end.

One two-bound signature handles all four forms. Bounds are half-open, so a present `end` is the first excluded position.

## Return types and lifetimes

Either hook may return any otherwise valid Arkoi type:

- a receiver-tied non-owning view, reference, or built-in slice;
- a copied data value;
- a newly constructed independent resource;
- another type permitted by ordinary type, ownership, and lifetime rules.

A receiver-derived result cannot outlive the receiver:

```arkoi
fun Container.__slice__(
    self @&Container,
    start @?usize,
    end @?usize,
) !CoreFail @[]Element:
    start_index @usize = start ?? 0
    end_index @usize = end ?? length(self.elements)
    return self.elements[start_index..end_index]!
```

An independently owned result may outlive the receiver. A named helper can construct one:

```arkoi
fun Container.slice_copy(
    self @&Container,
    start @?usize,
    end @?usize,
) !CoreFail @Buffer:
    return Buffer.copy_from(self, start, end)!
```

`__slice_mut__` is selected by context but need not return a mutable view. Its
result may be mutable, read-only, copied, or independently owned. The hook
name—not the return type—selects the mode. Borrowing a returned value remains
subject to the ordinary stable-address and temporary-lifetime rules.

## Selecting mutable slicing

Ordinary slicing chooses `__slice__` even for a mutable binding:

```arkoi
container @mut Container = create_container()
view @&ContainerView = container[start..end]!
```

Read-only borrowing also chooses `__slice__`:

```arkoi
view @&ContainerView = &container[start..end]!
```

Only an explicit `&mut` borrow of the **complete slicing expression** chooses `__slice_mut__`:

```arkoi
view @&mut MutableContainerView = &mut (container[start..end]!)
```

The grouping makes the order explicit: evaluate and, when needed, propagate the
complete slicing expression, then form the borrow. The hooks in this example
lend receiver-owned places through reference return types, so the borrow does
not retain a temporary result. A by-value hook result cannot be retained behind
`&` or `&mut`; ordinary temporary rules still apply. Merely having a mutable
receiver does not select the mutable hook.

The receiver must be a stable mutable place:

```arkoi
invalid_a @&mut MutableContainerView =
    &mut (immutable_container[start..end]!)  # Compile-time error

invalid_b @&mut MutableContainerView =
    &mut (create_container()[start..end]!)  # Compile-time error
```

This explicit selection prevents mutable-capable slicing from being chosen accidentally.

## Related topics

- [Indexing](indexing.md)
- [Length](length.md)
- [Evaluation order](evaluation-order.md)
- [Failure handling](failures.md)
- [References and lifetimes](references-lifetimes.md)
- [Compiler hooks](compiler-hooks.md)
