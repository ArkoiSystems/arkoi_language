# Functions

Functions are the main abstraction. They define executable behavior, take typed parameters, and return a typed value.

## Basic form

```arkoi
fun main() @u64:
    return 0
```

## Parameters

Parameters are declared with a name and a type.

```arkoi
fun factorial_recursive(n @u32) @u32:
    if n <= 1:
        return 1
    return n * factorial_recursive(n - 1)
```

Multiple parameters are separated by commas.

```arkoi
fun add(lhs @s32, rhs @s32) @s32:
    return lhs + rhs
```

## Return values

Every function has an explicit return type annotation.

```arkoi
fun ok(foo1 @f64) @bool:
    return foo1 > 0.0
```

## Calling functions

Functions are called by name with parentheses and comma-separated arguments.

```arkoi
return test1(4, 5.0)
```
