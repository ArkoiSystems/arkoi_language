# Bindings and initialization

<!-- spec-sections: 2.1; lexical scope from 1.1 -->

A binding gives a typed value a name. Binding mutability controls reassignment;
it is separate from ownership and from access through references or slices.

!!! abstract "At a glance"

    - Declare a binding as `name @Type`; add `mut` to permit reassignment.
    - Immutable and mutable bindings may both be initialized later.
    - Arkoi never supplies implicit zero or default initialization.
    - Every use must be definitely initialized on that path.
    - Local names are unique throughout a function: shadowing is forbidden.

## Declaration forms

| Form | Meaning |
| --- | --- |
| `count @u32 = 10` | Immutable binding, initialized now |
| `count @mut u32 = 10` | Reassignable binding, initialized now |
| `result @u32` | Immutable binding awaiting its one assignment |
| `counter @mut u32` | Mutable binding awaiting initialization |

The `@` introduces the type annotation. `mut` applies to the binding, so an
immutable binding may be initialized or assigned exactly once, while a mutable
binding may later be reassigned.

Reference bindings are fixed aliases and cannot use binding-level `mut`;
`&mut T` instead grants mutable access to the referent.

The related `NAME @const T = expression` form declares an addressless
compile-time value rather than a runtime storage binding. Its complete rules are
under [Constants and module globals](constants-globals.md).

```arkoi
result @u32

if ready:
    result = 10
else:
    result = 20

print_u32(result)
```

## Definite initialization

Arkoi checks initialization across branches, loops, handlers, early exits, and
postfix propagation paths. A value may be read or cleaned up only where it is
definitely initialized.

!!! failure "Compile-time error — possibly uninitialized"

    ```arkoi
    result @u32

    if ready:
        result = 10

    print_u32(result)
    ```

There is no implicit zero initialization and no default constructor. Moving a
resource also changes its initialization state; see [Ownership and
moves](ownership-moves.md).

## Lexical scope and unique names

Every indentation block creates a lexical scope, and a declaration is visible
only in its scope. Nevertheless, a local name may not be redeclared anywhere
in the same function—including nested blocks and the parameter scope.

!!! failure "Compile-time error — shadowing"

    ```arkoi
    fun process(value @u32):
        if value > 0:
            value @u32 = 20
    ```

Constants participate in the same no-shadowing rule. This restriction makes a
name identify one binding throughout a function even when control flow is
nested.

## Related topics

- [Source syntax](source-syntax.md)
- [Constants and module globals](constants-globals.md)
- [Ownership and moves](ownership-moves.md)
- [References and lifetimes](references-lifetimes.md)
