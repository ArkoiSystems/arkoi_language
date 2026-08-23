---
title: Arkoi 1.0 compiler hooks
description: Arkoi 1.0 target-language reference for reserved compiler hooks and the language operations that select them.
---

# Compiler hooks

<!-- spec-sections: 7.3 -->

Reserved double-underscore names connect ordinary Arkoi declarations to
language syntax and built-in protocols.

!!! abstract "At a glance"

    - Only hook names defined by the language have special behavior.
    - Hooks are external functions or methods belonging to their owning type.
    - The compiler checks each hook's complete contract.
    - Programs normally invoke a hook through its corresponding operation or
      syntax, not through runtime lookup.
    - Hook overloads use exact compile-time overload resolution.

## Hook families

| Family | Examples of language behavior |
| --- | --- |
| Lifecycle | Cloning and deterministic cleanup |
| Comparison and ordering | `==`, `!=`, `<`, `<=`, `>`, `>=` |
| Arithmetic and bitwise | User-defined operator behavior |
| Indexing and slicing | `value[...]` and range slicing |
| Collection queries | Length and membership |
| Iteration | Canonical creation and advancement protocols |

The detailed signature and behavior of each hook lives with the corresponding
feature. For example, [Cloning](cloning.md) defines `__clone__`, [Resource
lifecycle](resource-lifecycle.md) defines `__drop__`, [Comparisons and
membership](comparisons-and-membership.md) defines comparison and membership
hooks, and the operator, indexing, and iteration chapters define their own
families.

## Declaration and validation

Hooks use the same qualified external form as methods:

```arkoi
fun File.__drop__(self @&mut File):
    close_handle(self.handle)
```

For every recognized hook, the compiler validates the required:

- parameter and return signature;
- receiver and ownership modes;
- safety contract;
- failure effect; and
- overload rules.

Hooks participate in normal compile-time overload selection when their feature
allows multiple operand signatures. They do not create runtime method tables or
dynamic dispatch.

Only language-defined reserved names receive special treatment. Inventing an
otherwise similar double-underscore name does not define new syntax or a new
compiler protocol.

## Visibility and ownership

A hook belongs to its qualified owning type and must follow the same module
placement and private-field access rules as the type's methods. The language
operation that selects a hook still applies the hook's declared access,
ownership, safety, and failure contract.

## Related topics

- [Methods and visibility](methods-visibility.md)
- [Cloning](cloning.md)
- [Resource lifecycle](resource-lifecycle.md)
- [Comparisons and membership](comparisons-and-membership.md)
- [Operator hooks](operator-hooks.md)
- [Indexing](indexing.md)
- [Slicing](slicing.md)
- [Length](length.md)
- [Iteration protocols](iteration-protocols.md)
