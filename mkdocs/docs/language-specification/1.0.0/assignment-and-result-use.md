# Assignment and result use

<!-- spec-sections: 10.3–10.7 -->

This page defines destination evaluation, replacement, compound assignment, and the requirement to use or explicitly discard expression results.

!!! abstract "At a glance"
    Assignment is a statement with one destination. Arkoi resolves that destination before evaluating the replacement, resolves compound destinations once, constructs resources before dropping old values, and rejects silently unused results.

## Destination-first assignment

For a fallible computed place, Arkoi evaluates and validates the destination
before the right-hand side:

```arkoi
get_items()[calculate_index()]! = create_value()
```

1. Evaluate `get_items()`.
2. Evaluate `calculate_index()`.
3. Resolve and validate the destination place, including bounds checks.
4. Evaluate `create_value()`.
5. Perform the assignment.

If destination evaluation fails, traps, or leaves control flow, the right-hand side is not evaluated. Once resolved, the destination remains fixed and is not evaluated again after the right-hand side.

### Resource replacement

For a resource place, assignment preserves construct-first, drop-and-replace behavior:

```arkoi
files[index()]! = create_file()!
```

1. Resolve and validate the destination.
2. Fully evaluate and construct the replacement.
3. If construction fails, keep the old destination value unchanged.
4. Drop the old resource.
5. Install the replacement.

Destination-side effects and validation therefore happen before replacement construction, while failed construction never destroys the old resource.

## Compound assignment

A compound destination is evaluated exactly once:

```arkoi
values[index()]! += amount()
```

1. Evaluate the receiver and index expressions.
2. Resolve and validate the element place.
3. Read its current value.
4. Evaluate `amount()`.
5. Apply the corresponding binary operation.
6. Store the result in the already-resolved place.

This has the value behavior of
`values[index()]! = values[index()]! + amount()`, but does not reevaluate
`index()`.

Arithmetic mode remains explicit:

| Mode | Add | Subtract | Multiply |
| --- | --- | --- | --- |
| Trapping | `+=` | `-=` | `*=` |
| Recoverable | `+!=` | `-!=` | `*!=` |
| Wrapping | `+%=` | `-%=` | `*%=` |

Division, remainder, shifts, and bitwise operations have compound forms whenever their binary operation exists:

```arkoi
value /= divisor
value /!= divisor
value %= divisor
value %!= divisor

value <<= count
value <<!= count
value >>= count
value >>!= count

value &= mask
value |= mask
value ^= mask
```

A recoverable compound operation follows the selected operation's ordinary propagation or handling rules. The destination remains unchanged when the operation fails before producing a result. Compound assignment requires a mutable, assignable place.

There are no prefix or postfix increment/decrement expressions:

```arkoi
count++  # Compile-time error
--count  # Compile-time error
```

Use `count += 1`, `count -= 1`, or the checked/wrapping equivalents.

## Assignment is a statement

Assignment and compound assignment produce no value. One statement has exactly one destination, so chaining is invalid:

```arkoi
first = second = 0  # Compile-time error
```

Write separate statements:

```arkoi
second = 0
first = second
```

Assignment cannot occur in conditions, arguments, arithmetic or boolean expressions, initializers, returns, or another assignment.

```arkoi
if value = read():       # Compile-time error
    pass

result @u32 = value = 3  # Compile-time error
return value = 3         # Compile-time error
```

Named arguments use `name = expression`, but the name denotes a parameter, not a mutable destination:

```arkoi
copy_file(source = &source, destination = &destination)!
```

## Using expression results

A value-producing expression cannot stand alone:

```arkoi
4 + 5           # Compile-time error
value           # Compile-time error
create_file()!  # Compile-time error: resource result is ignored
```

A value must be bound, assigned, returned, passed as an argument, used in another expression, or explicitly discarded. A call that produces no value may stand alone:

```arkoi
log_message(&message)
file.flush()!
```

Use `discard(expression)` to state intentional disposal:

```arkoi
discard(calculate_value())
discard(File.open(&path)!)
```

`discard(...)` evaluates its argument exactly once and produces no value. A data result is ignored; a successfully constructed resource is cleaned up immediately through its normal deterministic cleanup. If evaluation fails, traps, or leaves control flow, no value is discarded and ordinary propagation and cleanup apply.

## Related topics

- [Evaluation order](evaluation-order.md)
- [Places and replacement](places-replacement.md)
- [Operator hooks](operator-hooks.md)
- [Ownership and moves](ownership-moves.md)
- [Failure handling](failures.md)
- [Indexing](indexing.md)
