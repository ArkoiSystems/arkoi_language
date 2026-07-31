# Indexing

<!-- spec-sections: 12.1–12.3 -->

This page defines custom read-only and mutable indexing, multi-argument selection, place semantics, and indexing failures.

!!! abstract "At a glance"
    `__index__` lends a read-only element place; `__index_mut__` lends a mutable one. Receiver and arguments evaluate left to right, overload selection is exact, and a fallible indexed assignment writes `!` on the indexing expression before `=`.

## Hook forms

Named data and resource aggregates may define either or both indexing modes:

```arkoi
fun Container.__index__(
    self @&Container,
    index @usize,
) !CoreFail @&Element:
    return Container.find(self, index)!

fun Container.__index_mut__(
    self @&mut Container,
    index @usize,
) !CoreFail @&mut Element:
    return Container.find_mut(self, index)!
```

Read-only contexts select `__index__`:

```arkoi
element @&Element = container[index]!
```

Mutable-place contexts select `__index_mut__`:

```arkoi
container[index]!.field = value
element @&mut Element = &mut container[index]!
container[index]! += amount
container[index]!.mutating_method()
```

If the selected mode is unavailable, compilation fails. Mutable indexing requires a stable mutable place; an immutable receiver or temporary cannot supply it.

```arkoi
create_container()[index]! = value  # Compile-time error
```

Built-in arrays and slices retain compiler-defined indexing and do not use these hooks.

## Element places and lifetimes

An indexing hook returns a reference to an actual existing place. It must not return a reference to a hook-local variable, a temporary, storage destroyed on return, or unrelated storage with a shorter lifetime than the receiver.

The result is receiver-tied. Arkoi's limited lexical checks reject obvious movement, replacement, or destruction while a derived reference remains live:

```arkoi
element @&mut Element = &mut container[index]!

container = create_container()  # Compile-time error while element is live
discard(move(container))        # Compile-time error while element is live
```

Arkoi does not guarantee Rust-style exclusivity. Ordinary aliases may exist; the type author must keep exposed storage valid for the permitted lifetime. A type that cannot safely keep mutable elements stable should omit `__index_mut__` and expose named mutations instead.

The mutable result is a true assignable place. For a fallible hook:

```arkoi
container[index()]! = create_element()!
```

Arkoi resolves the receiver and indices, invokes `__index_mut__`, and fixes the destination before constructing the right-hand side. Failed construction leaves the old element unchanged; successful resource replacement uses construct-first, drop-and-replace semantics. Compound assignment resolves the place once and then applies its binary operation. The complete ordering is defined on [Assignment and result use](assignment-and-result-use.md).

## Multiple indices

The grammar accepts one or more comma-separated index expressions:

```arkoi
container[index]
matrix[row, column]
tensor[x, y, z]
```

An empty list is invalid:

```arkoi
container[]  # Compile-time error
```

Hooks remain fixed-arity and statically typed. A type may provide any supported arities and ordered parameter types:

```arkoi
fun Tensor.__index__(
    self @&Tensor,
    row @usize,
    column @usize,
) !CoreFail @&Element:
    return Tensor.find(self, row, column)!

fun Tensor.__index_mut__(
    self @&mut Tensor,
    x @usize,
    y @usize,
    z @usize,
) !CoreFail @&mut Element:
    return Tensor.find_mut(self, x, y, z)!
```

Selection considers:

1. read-only or mutable mode;
2. argument count;
3. ordered argument types;
4. ordinary exact overload rules.

Unsupported arity or type combinations are compile-time errors. Read-only and mutable overload sets are independent, so supporting an arity in one mode does not imply it in the other.

The receiver evaluates first, followed by index arguments from left to right, then the selected hook call. Flexible syntax does not create variadic functions, and indexing adds no arity limit beyond the implementation's ordinary function-parameter limit.

## Failure handling

Fallibility comes from the selected overload. A fallible index expression must propagate:

```arkoi
element @&Element = container[index]!
```

or handle:

```arkoi
element @&Element = container[index] handle failure:
    yield &fallback
```

Omitting both is a compile-time error. An infallible hook requires neither and rejects postfix `!`:

```arkoi
fun Table.__index__(
    self @&Table,
    index @usize,
) @&Element:
    return Table.find(self, index)

element @&Element = table[index]
invalid @&Element = table[index]!  # Compile-time error
```

The same rule applies to mutable indexing. In particular, fallible indexed assignment uses:

```arkoi
container[index]! = replacement
```

whereas an infallible mutable hook uses `table[index] = replacement`. Destination failure is propagated or handled before the right-hand side runs.

`!` has no indexing-specific meaning; it is the ordinary postfix failure-propagation operator.

## Related topics

- [Slicing](slicing.md)
- [Length](length.md)
- [Assignment and result use](assignment-and-result-use.md)
- [Evaluation order](evaluation-order.md)
- [Calls and overloads](calls-overloads.md)
- [References and lifetimes](references-lifetimes.md)
- [Compiler hooks](compiler-hooks.md)
