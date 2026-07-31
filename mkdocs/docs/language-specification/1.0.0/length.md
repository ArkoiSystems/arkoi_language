# Length

<!-- spec-sections: 12.8 -->

This page defines custom support for the built-in `length(...)` operation.

!!! abstract "At a glance"
    A valid, same-module `__length__` hook enables infallible `length(value)` for a named aggregate. It receives a read-only borrow and returns `usize`.

## The `__length__` contract

```arkoi
fun Container.__length__(
    self @&Container,
) @usize:
    return self.element_count
```

The hook:

- belongs to the measured data or resource aggregate;
- is declared in the same module as that type;
- has exactly one parameter, `self @&Type`;
- returns `usize`;
- declares no failure effect;
- neither consumes nor mutates the receiver;
- may be private.

A malformed declaration using the reserved name is a compile-time error. A valid declaration enables:

```arkoi
count @usize = length(container)
```

`length(...)` evaluates its argument exactly once and then invokes the hook.

Built-in arrays, slices, and strings retain compiler-defined infallible length behavior rather than using a user hook. A type with neither built-in behavior nor a valid `__length__` cannot be passed to `length(...)`.

## Related topics

- [Indexing](indexing.md)
- [Slicing](slicing.md)
- [Evaluation order](evaluation-order.md)
- [Compiler hooks](compiler-hooks.md)
