# Control flow

Arkoi uses simple structured control flow that is easy to read and easy for the compiler to lower.

## If and else

Conditionals use `if`, optional `else if` branches, and a final `else` branch.

```arkoi
if foo1 > 5.0:
    foo1 = 0.0
else if foo1 >= 10.0:
    foo1 = 20.0
else:
    foo1 = 21.0
```

Short single-line branches are also supported.

```arkoi
if foo2 < bar: return bar * foo2
```

## While loops

The language supports `while` loops for repeated execution.

```arkoi
while n != 0:
    result = result * n
    n = n - 1
```

## Return

`return` exits the current function immediately.

```arkoi
return result
```
