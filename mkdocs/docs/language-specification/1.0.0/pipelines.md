# Pipeline expressions

<!-- spec-sections: 9.22, 10.1 -->

A pipeline carries one current value through stage expressions. The `_`
placeholder identifies that value and keeps borrowing, mutation, and ownership
transfer visible at every stage.

!!! abstract "At a glance"

    - Write `initial |> stage-with-_`.
    - Every stage contains exactly one `_`.
    - A returned value becomes the next `_`.
    - A no-value stage preserves the existing `_`.
    - `&_`, `&mut _`, and `move(_)` use the ordinary access rules.

## Building a pipeline

```arkoi
packet @Packet = (
    Buffer.create()
    |> Buffer.reserve(&mut _, 4096)!
    |> Buffer.append(&mut _, &header)!
    |> Buffer.append(&mut _, &payload)!
    |> Buffer.freeze(move(_))
    |> Packet.from_buffer(move(_))!
)
```

The initial expression establishes the pipeline carrier. A stage may place its
single placeholder in any argument position:

```arkoi
encoded @Buffer = (
    payload
    |> Encoder.encode(&mut encoder, &_)!
)
```

`_` exists only as a pipeline placeholder. It does not declare a binding,
discard a result, or act as a wildcard.

## Selecting the next value

The stage's result alone selects the next carrier:

| Stage result | Next `_` |
| --- | --- |
| Produces a value | The returned value |
| Produces no value | The existing carrier |

A no-value mutation preserves the carrier for the next stage and for the final
pipeline result:

```arkoi
buffer @Buffer = (
    Buffer.create()
    |> Buffer.reserve(&mut _, 4096)!
    |> Buffer.append(&mut _, &header)!
)
```

When an operation both mutates the carrier and returns a value, the return wins:

```arkoi
bytes_read @usize = (
    file
    |> File.read(&mut _, &mut buffer)!
)
```

`file` and `buffer` may be modified, while the complete pipeline evaluates to
the returned `usize`.

## Access and ownership

The placeholder uses ordinary explicit access syntax:

```arkoi
size @usize = file |> File.size(&_)
result @Buffer = file |> File.into_buffer(move(_))!
```

`&mut _` requires mutable access to the current carrier. `move(_)` consumes it,
so that stage must return the replacement carrier. A no-value stage must leave
the existing carrier initialized.

The pipeline gives temporary carriers stable storage for the duration of the
expression. This permits a newly returned resource to be borrowed, mutated, or
moved by a later stage. A named resource enters by ownership only through an
explicit `move(name)` or a later `move(_)`:

```arkoi
updated @Buffer = (
    move(buffer)
    |> Buffer.reserve(&mut _, 4096)!
    |> Buffer.compact(move(_))
)
```

The final carrier is the pipeline's result and follows the ordinary movement,
lifetime, result-use, and cleanup rules.

## Evaluation and failure

Arkoi evaluates the initial expression exactly once and then evaluates each
stage once from left to right. Expressions inside a stage retain their ordinary
evaluation order.

Postfix propagation and local handling apply to the stage expression normally.
When a stage fails, traps, or leaves control flow, later stages do not run and
live resources are cleaned up normally.

`|>` has lower precedence than every other expression operator and groups from
left to right. Multiline pipelines use parentheses because Arkoi continues
expressions across lines only inside parentheses or brackets.

## Related topics

- [Methods and visibility](methods-visibility.md)
- [Moving resources](ownership-moves.md)
- [References and lifetimes](references-lifetimes.md)
- [Failures](failures.md)
- [Evaluation order](evaluation-order.md)
- [Assignment and result use](assignment-and-result-use.md)
