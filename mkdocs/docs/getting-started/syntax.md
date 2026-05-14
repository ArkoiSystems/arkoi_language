# Syntax

Arkoi is still evolving, so this page keeps the syntax overview short and example-driven.

## Functions

Functions currently use `fun`, a typed parameter list, and a typed return annotation:

```arkoi
fun main() @s32:
    return 0
```

Some examples also omit the trailing colon and rely on indentation for the body:

```arkoi
fun main() @u64
    return 1
```

## Types

The current examples use `@`-prefixed types such as:

- `@s32`
- `@u32`
- `@u64`
- `@f32`
- `@f64`
- `@bool`

## Control flow

The language currently shows simple `if`, `else`, `while`, and `return` forms in the examples and tests.

```arkoi
fun ok(foo @s32) @bool
    if foo return 1
    return 0
```

## Try the examples

If you want a small reference while reading, start with the `example/hello_world/hello_world.ark` file and then look at the scanner snapshot example in the tests.