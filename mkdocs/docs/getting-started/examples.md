# Current Compiler Examples

These repository programs demonstrate the subset implemented by the current compiler. For target-language examples and rules, use the [Arkoi 1.0.0 specification](../language-specification/1.0.0/index.md).

## Hello world

The simplest example is `example/hello_world/hello_world.ark`, which shows a minimal `main` function.

```arkoi
fun main() @s32:
    return 0
```

## Recursion and loops

`example/factorial/factorial.ark` demonstrates recursion, loops, and multiple functions.

## Operators and branching

`example/test/test.ark` is the broadest sample. It exercises:

- Function calls
- Arithmetic expressions
- Comparisons
- Boolean logic
- `if` / `else if` / `else`
- `while`
- Assignment and returns
