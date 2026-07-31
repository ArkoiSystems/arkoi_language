# Source syntax

<!-- spec-sections: 1.1–1.5 -->

Arkoi uses indentation for blocks, newlines for statement boundaries, and open
parentheses or brackets for multiline expressions.

!!! abstract "At a glance"

    - A colon starts an indented block; every block creates a lexical scope.
    - An intentionally empty block contains `pass`.
    - One trailing comma is allowed in every nonempty comma-separated list.
    - Expressions continue implicitly only inside `(...)` or `[...]`.
    - Comments begin with `#`.

## Blocks and comments

```arkoi
fun greet(name @string_view):
    # The function body is a lexical scope.
    print(name)
```

Every declaration or statement belonging to a block must be indented beneath
the header that ends in `:`. A binding declared in that block is visible only
inside it.

## Empty blocks and `pass`

Every block must contain at least one declaration or statement. Use `pass` when
a grammar permits a block but the block intentionally does nothing:

```arkoi
interface ThreadSafe:
    pass

fun placeholder():
    pass

if condition:
    pass
else:
    perform_work()
```

This rule applies to interface, function, method, associated-function,
conditional, loop, failure-handler, aggregate, and unsafe blocks whenever the
particular construct is allowed to be empty.

`pass` is an ordinary statement and may also appear before, between, or after
other statements, more than once, and at any nested statement level:

```arkoi
fun process():
    pass
    perform_work()
    pass
```

Each `pass`:

- performs no operation and produces no value;
- cannot fail, trap, or have runtime side effects;
- does not affect definite initialization;
- does not satisfy a required value return; and
- does not make unreachable code reachable.

A comment is not a block item, and a same-level declaration does not fill the
preceding block.

!!! failure "Compile-time error — missing block body"

    ```arkoi
    interface ThreadSafe:

    interface PlainData:
        pass
    ```

`implements` without associated-type bindings is a single-line declaration and
therefore needs no block:

```arkoi
implements ThreadSafe for Worker
```

An `implements` block exists only for required associated-type bindings;
`pass` cannot stand in for those bindings.

## Trailing commas

Exactly one optional trailing comma is accepted in any **nonempty**
comma-separated list, whether single-line or multiline:

```arkoi
fun process(
    value @u32,
    enabled @bool,
):
    pass

result @u32 = calculate(left, right,)

point @Point = Point(
    x = 10,
    y = 20,
)

item @T = matrix[
    row,
    column,
]
```

The rule covers parameters, positional and named call or aggregate-construction
arguments, array elements, multiple indices, and parent interfaces in an
`extends` clause. A trailing comma adds no item, does not affect overload
identity or evaluation order, and has no runtime meaning.

!!! failure "Compile-time errors — malformed comma lists"

    ```arkoi
    fun duplicated(value @u32,,):
        pass

    fun empty(,):
        pass
    ```

## Multiline expressions

An expression continues implicitly while a parenthesis or bracket remains
open. Newlines and indentation within that delimiter are formatting only: they
do not end the expression or create a block.

```arkoi
result @u32 = (
    left
    + transform(
        right,
    )
)

item @T = matrix[
    row,
    column,
]
```

Continuation may span comments and blank lines where the expression grammar
allows them. It ends at the matching delimiter and does not change runtime
meaning or evaluation order.

Outside open `(...)` or `[...]`, a newline ends the declaration or statement
unless another explicitly defined multiline grammar construct applies. Arkoi
has no general-purpose continuation character.

!!! failure "Compile-time error — backslash continuation"

    ```arkoi
    result @u32 = left + \
        right
    ```

## Related topics

- [Bindings and initialization](bindings-initialization.md)
- [Functions and returns](functions-returns.md)
- [Aggregates and enums](aggregates-enums.md)
- [Failures](failures.md)
