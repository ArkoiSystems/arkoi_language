# Evaluation order

<!-- spec-sections: 10.1–10.2 -->

This page defines when subexpressions run. Assignment has additional destination-first rules on its own page.

!!! abstract "At a glance"
    Arkoi evaluates written lists and binary operands from left to right. Method receivers come first, boolean operators short-circuit, and evaluation stops immediately when an earlier operation fails or leaves control flow.

## Lists, calls, and initializers

Function arguments are evaluated left to right, followed by the call:

```arkoi
process(
    create_first(),
    create_second(),
    create_third(),
)
```

Named arguments follow **textual order**, not parameter-declaration order.

```arkoi
copy_file(
    overwrite = check_overwrite(),
    source = load_source(),
    destination = load_destination(),
)!
```

Here `check_overwrite()` runs before `load_source()`, which runs before `load_destination()`.

For a method call, Arkoi evaluates the receiver before explicit arguments:

```arkoi
get_file().write(
    load_first_buffer(),
    load_second_buffer(),
)!
```

The left-to-right written-order rule also covers named aggregate field initializers, array elements, and built-ins including `replace(...)`, `swap(...)`, `convert(...)`, `truncate(...)`, and `bitcast(...)`.

## Operators and postfix chains

Binary operands evaluate left to right, then Arkoi applies the operator:

```arkoi
result @u32 = create_left() + create_right()
```

This rule covers arithmetic, comparison, bitwise, shift, range-bound, and other binary operators unless their own rule says otherwise.

Boolean operations short-circuit:

- `left() and right()` evaluates `right()` only when `left()` is `true`;
- `left() or right()` evaluates `right()` only when `left()` is `false`.

Postfix chains evaluate their base first and proceed outward in source order:

```arkoi
field @Field = load_items()![index]!.field
```

## Early exit and cleanup

If an earlier expression fails, traps, returns, or otherwise leaves control flow, no later expression in that sequence is evaluated. Temporary resources already constructed are cleaned up under the ordinary deterministic cleanup rules.

Assignment first resolves and validates its destination; see [Assignment and result use](assignment-and-result-use.md). Index and slice arguments inherit these left-to-right rules after their receiver.

## Related topics

- [Assignment and result use](assignment-and-result-use.md)
- [Calls and overloads](calls-overloads.md)
- [Failure handling](failures.md)
- [Ownership and moves](ownership-moves.md)
- [Indexing](indexing.md)
- [Slicing](slicing.md)
