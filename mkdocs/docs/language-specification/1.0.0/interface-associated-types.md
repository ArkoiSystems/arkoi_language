# Interface associated types

<!-- spec-sections: 13.8, 13.13, 13.23, 13.32–13.34 -->

This page defines associated-type declaration, binding, identity, scope, independence, and syntax.

!!! abstract "At a glance"
    Every required associated type is bound explicitly and exactly once. Bindings are independent concrete type expressions: Arkoi has no inference, defaults, projections, sibling references, or `Self` in an `implements` block.

## Declaring and binding

```arkoi
interface Iterator:
    type Item
    type Position

implements Iterator for BufferIterator:
    type Item = &Byte
    type Position = usize
```

Every directly declared or inherited associated type must be assigned exactly once in the corresponding implementation. Arkoi does not infer a binding from method signatures and provides no default associated types.

These are compile-time errors:

```arkoi
implements Iterator for MissingPosition:
    type Item = &Byte
    # Missing Position

implements Iterator for DuplicateItem:
    type Item = &Byte
    type Item = Byte
    type Position = usize

implements Iterator for UnknownBinding:
    type Item = &Byte
    type Position = usize
    type Unknown = u32
```

Inherited bindings are written in the single child implementation:

```arkoi
interface BaseIterator:
    type Item

interface IndexedIterator extends BaseIterator:
    type Index

implements IndexedIterator for BufferIterator:
    type Item = &Byte
    type Index = usize
```

After all bindings exist, the compiler substitutes them into every inherited and direct requirement before matching concrete definitions.

## Valid bound types

A binding may use any otherwise valid **concrete** Arkoi type, including:

- primitives, references, raw pointers, optionals, fixed arrays, and slices;
- transparent aliases;
- named data and resource aggregates;
- enums and failure types.

Some of these categories cannot implement interfaces themselves, but they remain valid associated-type values. An interface name is not a concrete type and cannot be bound.

## Lexical scope

An associated-type name is in scope within its declaring interface and child interfaces that inherit it:

```arkoi
interface Iterator:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item

interface DoubleEndedIterator extends Iterator:
    fun next_back(
        self @&mut Self,
    ) @?Item
```

Outside interface contracts, a bare associated-type name is not in scope. An `implements` block uses it only on the left of `type Name = ...`; concrete definitions write the substituted concrete type:

```arkoi
fun BufferIterator.next(
    self @&mut BufferIterator,
) @?&Byte:
    return BufferIterator.take_next(self)
```

`Item`, `Self.Item`, and interface-qualified associated-type projection syntax are all invalid in concrete definitions. Arkoi provides no source-level projection syntax.

## Identity across interfaces

Associated-type identity includes its declaring interface. Unrelated interfaces may reuse a spelling and one type may bind each independently:

```arkoi
interface Iterator:
    type Item

interface Container:
    type Item

implements Iterator for Buffer:
    type Item = &Byte

implements Container for Buffer:
    type Item = Byte
```

Internally these are distinct requirements. Independent implementations do not conflict merely because the names match.

Extension cannot combine unrelated associated types with the same name into one child contract. The child cannot disambiguate them; the declarations must be renamed. If the same originating associated type arrives through multiple inheritance paths, it remains one merged requirement.

## Concrete and independent bindings

`Self` is valid only inside an interface declaration, not in a binding:

```arkoi
implements Linked for Node:
    type Next = ?&Node

implements Linked for InvalidNode:
    type Next = ?&Self  # Compile-time error
```

The restriction applies at every nesting level. Bindings also cannot refer to another associated-type binding—earlier, later, inherited, or from another implementation:

```arkoi
implements Collection for Buffer:
    type Item = Byte
    type OptionalItem = ?Item  # Compile-time error
```

Write the complete type independently:

```arkoi
implements Collection for Buffer:
    type Item = Byte
    type OptionalItem = ?Byte
```

This keeps bindings order-independent and cycle-free. An ordinary visible module-level alias remains valid:

```arkoi
type TableKey = string

implements Mapping for Table:
    type Key = TableKey
```

## Visibility syntax

Neither an associated-type declaration nor a binding has an individual visibility modifier:

```arkoi
pub interface Iterable:
    type Item

implements Iterable for Collection:
    type Item = Element
```

`pub type Item` is invalid in either location. The interface determines declaration visibility; the interface, implementing type, bound type, and usable conformance determine effective accessibility. Those checks are defined on [Interface visibility](interface-visibility.md#associated-type-accessibility).

## Related topics

- [Static interfaces](interfaces-overview.md)
- [Interface requirements](interface-requirements.md)
- [Interface extension](interface-extension.md)
- [Interface implementations](interface-implementations.md)
- [Interface visibility](interface-visibility.md)
- [Types and values](types-values.md)
