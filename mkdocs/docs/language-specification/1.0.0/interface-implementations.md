# Interface implementations

<!-- spec-sections: 13.1 (implementation syntax), 13.6 (coherence), 13.7, 13.15–13.16, 13.19, 13.35–13.36 -->

This page defines explicit conformance syntax, eligible types, ownership and uniqueness, declaration placement, and transparent aliases.

!!! abstract "At a glance"
    `implements Interface for Type` establishes one global nominal relationship. Only the interface's module or the named aggregate's module may declare it, and only named user-defined `data` and `resource` types are eligible.

## Declaring conformance

Use one line when the interface has no associated types:

```arkoi
implements hashing.Hashable for Account
```

Use a block containing only associated-type bindings when required:

```arkoi
implements iteration.Iterator for AccountIterator:
    type Item = &Account
```

Function bodies never appear inside the implementation block. The compiler substitutes `Self` and the associated bindings into every requirement, then verifies that matching ordinary external definitions exist. Missing or mismatched definitions are compile-time errors; unrelated extra functions on the concrete type are allowed.

Conformance is explicit and nominal. Merely providing matching functions does not implement an interface.

Each declaration names exactly one interface and one concrete type:

```arkoi
implements Hashable for Account
implements Displayable for Account
```

Comma-separated combined conformance is invalid. Bindings and all validation belong solely to the one named interface. Extension is the mechanism for a contract that combines parents.

## Ownership, uniqueness, and coherence

An implementation may be declared only by:

- the module defining the interface; or
- the module defining the concrete type.

A third module that owns neither side cannot establish conformance. Only one implementation of a given interface for a given concrete type may exist in the program.

Once valid, conformance is globally coherent: every context that can access both declarations observes the same relationship. It cannot vary by caller, import path, lexical scope, or visibility context. The declaration itself has no public/private state; accessibility rules are on [Interface visibility](interface-visibility.md).

Inherited parent conformance and overlapping child paths have additional one-canonical-path rules on [Interface extension](interface-extension.md).

## Eligible concrete types

Only named user-defined `data` and `resource` aggregates with their own nominal identity may implement interfaces:

```arkoi
data Buffer:
    bytes @[]u8

resource File:
    handle @u64

implements Sized for Buffer
implements Closable for File
```

These categories are ineligible:

- transparent aliases;
- built-in primitives;
- references and raw pointers;
- optionals, fixed arrays, slices, and other compiler-defined composites;
- function types, which are not value types in Arkoi;
- failure and enum types.

```arkoi
type ByteCount = usize

implements Sized for ByteCount  # Compile-time error
implements Sized for []u8       # Compile-time error
implements Sized for [16]u8     # Compile-time error
implements Sized for ?Buffer    # Compile-time error
```

Compiler-defined composites retain only built-in or element-derived behavior. This restriction gives each implementation a distinct concrete identity, owning module, and natural home for external methods and hooks.

## Module-level, order-independent declarations

Interfaces and `implements` declarations are allowed only at module level—not inside functions, methods, associated functions, aggregates, interfaces, control-flow blocks, handlers, or another local or nested scope.

```arkoi
fun configure():
    implements LocalCapability for LocalType  # Compile-time error
```

This keeps conformance global, independent of runtime control flow and lexical scope, and makes whole-program duplicate checking possible.

Module-level order does not matter. The compiler collects the complete module before checking conformance:

```arkoi
implements Hashable for Account

fun Account.hash(
    self @&Account,
) @u64:
    return self.id
```

The function may equivalently precede the declaration. Associated-type bindings
are part of the module-level `implements` declaration and obey the same
order-independent collection rule: they need not appear textually next to or
before the concrete external definitions. Placing the declaration first is a
readability convention, not a semantic requirement.

## Ordinary concrete definitions

Implementing methods, associated functions, and hooks remain in the concrete type's ordinary external function namespace. Arkoi adds no implementation block, namespace, dispatch table, interface-qualified definition, or grouping syntax.

```arkoi
implements Hashable for Account
implements Displayable for Account

fun Account.hash(
    self @&Account,
) @u64:
    return self.id

fun Account.display(
    self @&Account,
) @string:
    return format_account(self)

fun Account.internal_helper(
    self @&Account,
):
    pass
```

When visibility permits, an implementing function can be called through its
concrete type:

```arkoi
hash @u64 = Account.hash(&account)
```

A receiver requirement can instead be selected through its declaring interface:

```arkoi
hash @u64 = Hashable.hash(&account)
```

The receiver's concrete type selects the implementation statically; the
interface never becomes a runtime value.

## Transparent aliases

A transparent alias has exactly its underlying type's conformances because it has the same type identity:

```arkoi
implements Iterable for Buffer:
    type Item = Byte

type ByteBuffer = Buffer
```

Where the alias is visible, `ByteBuffer` is recognized as implementing `Iterable`, using the underlying bindings, functions, and hooks. It creates no new implementation identity.

An implementation cannot target an alias:

```arkoi
implements Displayable for ByteBuffer  # Compile-time error
```

This also prevents duplicate implementations through multiple aliases. Aliases cannot add, remove, replace, or specialize conformance; only the underlying named user aggregate may appear after `for` in an `implements` declaration.

## Related topics

- [Static interfaces](interfaces-overview.md)
- [Interface requirements](interface-requirements.md)
- [Associated types](interface-associated-types.md)
- [Interface extension](interface-extension.md)
- [Interface visibility](interface-visibility.md)
- [Modules and imports](modules-and-imports.md)
