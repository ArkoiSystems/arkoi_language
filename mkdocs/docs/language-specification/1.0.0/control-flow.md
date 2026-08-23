---
title: Arkoi 1.0 control flow
description: Arkoi 1.0 target-language rules for conditions, loops, break, continue, return, and unreachable control paths.
---

# Control flow

<!-- spec-sections: 11.1, 11.2, 11.4 -->

This page defines conditional statements, condition-controlled loops, intentional infinite loops, and named loop targets.

!!! abstract "At a glance"
    `if` and loops are statements with lexical scopes. Conditions are strict `bool` values. Use `loop` for intentional indefinite iteration, and name a loop when `break` or `continue` must target an outer level.

## Conditional statements

```arkoi
if condition:
    perform_first_action()
elif other_condition:
    perform_second_action()
else:
    perform_fallback()
```

`if` is statement-only and produces no value. Its conditions must have type `bool`; there is no truthiness conversion. Parentheses are optional and normally omitted. `elif` and `else` are optional, and each branch has its own lexical scope.

Definite-initialization analysis considers every branch that can continue after the conditional:

```arkoi
result @u32

if condition:
    result = 10
else:
    result = 20

print(result)
```

Without the `else`, `result` could remain uninitialized and its later use would be a compile-time error.

## While loops

```arkoi
while condition:
    perform_action()
```

The condition must be `bool`, is evaluated before every iteration, and may cause the body to run zero times. Parentheses are optional and normally omitted. The body creates a lexical scope.

`break` exits the nearest enclosing loop. `continue` ends the current iteration and begins the next condition check.

```arkoi
index @mut usize = 0

while index < length(items):
    process(items[index]!)
    index += 1
```

Python-style `while ... else` is unsupported. Definite-initialization analysis cannot assume that a `while` body executes, even when it assigns a value and then breaks.

## Intentional infinite loops

Use `loop` when iteration has no condition:

```arkoi
loop:
    event @Event = next_event()!

    if event.should_stop:
        break

    if event.should_skip:
        continue

    process(&event)
```

The body has a lexical scope and is considered to execute at least once. Reaching its end begins another iteration. A `loop` completes normally only through a reachable `break`; `return`, `fail`, propagation, or a trap may also leave or terminate it.

`while true:` remains valid because `true` is a `bool`, but `loop` is the canonical intentional-infinite form. This distinction participates in definite-initialization and reachability analysis.

## Named loops

Any `loop`, `while`, or `for` may add `named name`:

```arkoi
loop named outer:
    for value @u32 in values named scan:
        if should_skip(value):
            continue scan

        if should_stop(value):
            break outer
```

An unnamed `break` or `continue` targets the nearest loop. A named form targets the matching **enclosing** loop:

```arkoi
while should_retry() named retry:
    if finished:
        break retry
```

The target must be an enclosing loop, and simultaneously enclosing named loops must have unique names.

## Related topics

- [For loops](for-loops.md)
- [Assignment and result use](assignment-and-result-use.md)
- [Types and values](types-values.md)
- [Failure handling](failures.md)
