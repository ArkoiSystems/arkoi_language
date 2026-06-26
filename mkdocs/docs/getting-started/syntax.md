# Syntax reference

The syntax is centered on a few explicit rules: typed functions, typed variables, simple expressions, and structured control flow.

## Lexical shape

- Source files are plain text files with the `.ark` extension.
- Comments start with `#` and continue to the end of the line.
- Whitespace is used for readability and indentation in block-oriented code.

## Declarations

### Functions

Functions use `fun`, a name, a typed parameter list, and a return type annotation.

```arkoi
fun main() @u64:
    return 0
```

Parameters are written with a name followed by `@type`.

```arkoi
fun add(lhs @s32, rhs @s32) @s32:
    return lhs + rhs
```

### Variables

Variables are declared with an explicit type annotation.

```arkoi
result @f32 = 0.0
flag @bool = true
count @s32 = 10
```

Assignments reuse the variable name without repeating the type.

```arkoi
result = result + 1.0
```

## Types

The language uses a compact primitive type set:

- `@s32`
- `@u32`
- `@u64`
- `@f32`
- `@f64`
- `@bool`
- [See more about types](types.md)

Types are explicit in declarations, parameters, and return annotations.

## Expressions

The language supports arithmetic, comparisons, logical operators, function calls, and explicit grouping with parentheses.

```arkoi
return (bar * foo2) + (foo2 < foo2)
```

Operators shown in the examples include:

- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `<`, `<=`, `>`, `>=`, `==`, `!=`
- Boolean: `&&`, `||`

## Control flow

Conditionals and loops are block-oriented.

```arkoi
if foo1 > 5.0:
    foo1 = 0.0
else if foo1 >= 10.0:
    foo1 = 20.0
else:
    foo1 = 21.0
```

```arkoi
while n != 0:
    result = result * n
    n = n - 1
```

`return` exits the current function.

## Literals

The language includes integer, floating-point, and boolean literals.

```arkoi
0
10.5
true
false
```

## Example shape

This is a representative minimal program:

```arkoi
fun main() @s32:
    return 0
```

See `example/test/test.ark` for a broader sample that exercises functions, branches, loops, arithmetic, comparisons, logical expressions, and assignments.