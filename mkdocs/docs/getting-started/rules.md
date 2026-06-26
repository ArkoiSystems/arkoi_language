# Language rules

This page records the rules that define Arkoi’s behavior.

## Naming

- Identifiers name functions, variables, and parameters.
- Type names are written with the `@` prefix in source code.
- Source files use the `.ark` extension.

## Typing

- Function parameters carry explicit type annotations.
- Variable declarations carry explicit type annotations.
- Function return types carry explicit type annotations.
- The primitive type set includes `@s32`, `@u32`, `@u64`, `@f32`, `@f64`, and `@bool`.

## Scope

- Each function introduces a new scope.
- Variables live within the block where they are declared.
- Parameters are available throughout the function body.

## Control flow

- `if` starts a conditional branch.
- `else if` adds another conditional branch.
- `else` provides the fallback branch.
- `while` repeats its body while the condition remains true.
- `return` ends execution of the current function.

## Expressions and operators

- Arithmetic uses `+`, `-`, `*`, and `/`.
- Comparisons use `<`, `<=`, `>`, `>=`, `==`, and `!=`.
- Boolean logic uses `&&` and `||`.
- Parentheses group expressions and control evaluation order.

## Precedence

From highest to lowest, operator precedence follows this order:

1. Parentheses
2. Multiplication and division
3. Addition and subtraction
4. Comparison
5. Equality
6. Boolean `&&`
7. Boolean `||`

## Conversion

Type conversion uses explicit casts when a value changes representation.

## Style rules

- Use indentation to define blocks.
- Keep function bodies direct and readable.