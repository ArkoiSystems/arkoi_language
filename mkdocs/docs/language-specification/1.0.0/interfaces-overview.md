# Static interfaces

<!-- spec-sections: 13.1 (model and declaration syntax), 13.14, 13.20 -->

This page introduces Arkoi's nominal, compile-time interface model and the source positions in which an interface name is valid.

!!! abstract "At a glance"
    Interfaces describe compile-time contracts, not values. They contain associated-type and function requirements, use `Self` for the unknown implementing type, and add no runtime representation or dispatch.

## Declaration model

An interface may declare associated types and required functions:

```arkoi
pub interface Iterator:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item
```

`Self` denotes the concrete implementing type. Requirements remain declarations inside the interface; implementations are ordinary external functions on a concrete type.

Interfaces are private by default. `pub interface` exposes the complete contract. Conformance is explicit and nominal: matching names and functions alone never establish it.

Interfaces may require:

- associated types;
- instance methods;
- associated functions;
- reserved language hooks;
- overload sets under ordinary Arkoi rules.

The detailed contract language is on [Interface requirements](interface-requirements.md); implementation syntax and eligibility are on [Interface implementations](interface-implementations.md).

## No runtime interface values

An interface is not a runtime type:

```arkoi
iterator @Iterator  # Compile-time error
```

Only concrete values can be stored, passed, or returned:

```arkoi
iterator @AccountIterator = collection.iterate()
```

Interfaces introduce no interface-valued variables, trait objects, virtual tables, runtime casts, implementation lookup, dynamic dispatch, or indirect calls. Requirement calls resolve statically to concrete functions during compilation.

They likewise cannot be embedded in aggregates or composed into references, pointers, optionals, arrays, or slices. Heterogeneous collections of implementations are not available through interfaces.

## Where interface names are valid

An interface name may appear only where the grammar expects an interface declaration identity:

```arkoi
implements Interface for Type
```

```arkoi
interface Child extends Parent:
    pass
```

It is not a general type expression and cannot be used as a variable, constant, field, parameter, return, associated-type binding, element, optional-content, reference, pointer, or slice type.

```arkoi
value @Iterator                    # Compile-time error
parameter @&Iterator               # Compile-time error
result @?Iterator                  # Compile-time error
items @[]Iterator                  # Compile-time error
```

Associated-type bindings must name concrete Arkoi types:

```arkoi
implements Container for Buffer:
    type Item = Iterator  # Compile-time error
```

Compiler and core mechanisms may identify canonical interfaces internally by declaration identity; that does not turn their names into source-level value types.

## Marker interfaces

An interface may have no associated types and no required functions:

```arkoi
interface ThreadSafe:
    pass
```

This is a marker interface. A named aggregate opts in through ordinary explicit conformance:

```arkoi
implements ThreadSafe for MessageQueue
```

A marker has no representation, storage, methods, hooks, executable code, dynamic dispatch, layout effect, or ABI effect. Naming and structure never imply it. It follows ordinary ownership, uniqueness, visibility, module-scope, and extension rules.

Markers may extend or be extended:

```arkoi
interface Sendable:
    pass

interface ConcurrentResource extends Sendable:
    pass
```

Implementing the child also establishes its inherited parent conformance. A marker remains a valid nominal classification even when no additional compile-time feature currently consumes it.

## Related topics

- [Interface requirements](interface-requirements.md)
- [Associated types](interface-associated-types.md)
- [Interface extension](interface-extension.md)
- [Interface implementations](interface-implementations.md)
- [Interface visibility](interface-visibility.md)
- [Types and values](types-values.md)
