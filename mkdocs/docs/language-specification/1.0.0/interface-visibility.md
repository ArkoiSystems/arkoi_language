---
title: Arkoi 1.0 interface visibility
description: Arkoi 1.0 target-language rules for public contracts, visible definitions, associated types, and module boundaries.
---

# Interface visibility

<!-- spec-sections: 13.6 (visibility), 13.10–13.11, 13.21, 13.25, 13.30–13.31, 13.34 (accessibility) -->

This page defines visibility of interface contracts, implementing definitions, parent contracts, public signature types, and associated-type bindings.

!!! abstract "At a glance"
    Conformance itself has no visibility modifier. Interfaces expose or hide their entire contracts, concrete definitions choose `pub` independently, and every type exposed through a public contract or associated binding must be accessible.

## Conformance has no visibility

An implementation declaration is neither public nor private:

```arkoi
implements Sized for Buffer
```

Both `pub implements` and `private implements` are invalid. Once compiled, the relationship is globally recognized wherever the interface and concrete type are accessible. Imports, callers, and lexical contexts do not change it.

The interface and concrete type retain their own visibility. An implementation does not make a private declaration nameable outside its normal boundary.

## Contract visibility

Interface members have no individual visibility modifiers. A public interface exposes its complete associated-type and function contract; a private interface exposes none of that contract outside its module.

```arkoi
pub interface Iterator:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item
```

`pub type Item` and `pub fun next` inside the interface are compile-time errors. The contract is indivisible, including requirements inherited by extension.

### Parent interfaces

A public interface may extend only parents that are public and accessible everywhere the child is exposed. This applies to every direct and transitive parent.

```arkoi
interface InternalCapability:
    pass

pub interface PublicCapability extends InternalCapability:
    pass  # Compile-time error: private parent in a public contract
```

A private interface may extend a public accessible parent or a private parent accessible within its declaring module. A parent that is public in its own module but not exported through the relevant boundary is still inaccessible.

This prevents public contracts from hiding inherited requirements or associated types. It does not alter the separate visibility of concrete definitions.

## Implementing-definition visibility

Every concrete method, associated function, or hook uses ordinary `pub` rules. Either a private or public definition may satisfy a requirement; an interface or implementation declaration never changes that choice.

```arkoi
implements Parseable for Configuration

pub fun Configuration.parse(
    source @string_view,
) !ParseFail @Configuration:
    return parse_configuration(source)!
```

A public definition is directly callable wherever its owner and signature are accessible. A private definition is not directly callable from another module, even though it may establish conformance.

A receiver requirement exposed by a visible interface may be called through the
interface qualifier. That call is checked against the interface contract and
does not make the private concrete definition directly accessible:

```arkoi
Writer.write(&mut file, data = content)
```

A private reserved hook may enable public language syntax without exposing a direct hook call:

```arkoi
implements Sized for Buffer

fun Buffer.__length__(
    self @&Buffer,
) @usize:
    return self.used

size @usize = length(buffer)
```

Outside the module, `length(buffer)` may be valid while a direct
`Buffer.__length__(&buffer)` call fails visibility checking.

### Cross-module implementations

The declaration may live in the concrete type's module or the interface's module. Visibility is checked from the module containing `implements`.

- In the concrete type's own module, private definitions may satisfy requirements.
- In the interface's module, matching definitions on the externally owned concrete type must be publicly accessible.

This applies to methods, associated functions, hooks, every overload, and inherited requirements. It prevents the interface module from establishing conformance through behavior the type's module kept private. Associated-type bindings remain valid declarations within either permitted implementation location, subject to their own accessibility checks.

## Public contract types

Every type and failure anywhere in a public interface's complete flattened contract must be public and externally accessible wherever the interface is accessible. The rule covers parameters, returns, failure effects, and every nested reference, pointer, optional, array, slice, or other composite position.

```arkoi
data InternalBuffer:
    bytes @[]u8

pub interface Encoder:
    fun encode(
        self @&Self,
        output @&mut InternalBuffer,
    )  # Compile-time error
```

The compiler checks after transitive extension is flattened, so a public child cannot inherit a contract type inaccessible at its boundary. A private interface may use private types and failures accessible within its module.

A public transparent alias does not make a private underlying type accessible, because the alias exposes the same type identity.

## Associated-type accessibility

When a public type implements a public interface, every associated binding must be public and externally accessible wherever both declarations are accessible:

```arkoi
data InternalIterator:
    position @usize

pub data Collection:
    count @usize

implements Iterable for Collection:
    type Iterator = InternalIterator  # Compile-time error
```

The check recursively examines the complete bound expression, including references, pointers, optionals, arrays, slices, aliases, and other composites. A public alias cannot mask a private underlying type.

If the concrete type or interface is private, the bound type must be accessible in every context where that conformance is visible and usable. The compiler validates bindings after flattening inherited requirements and substituting types.

Effective binding accessibility follows:

- the interface's visibility and accessibility;
- the implementing type's visibility and accessibility;
- the bound concrete type's visibility and accessibility;
- the contexts where the conformance is usable.

A binding cannot be hidden independently from an accessible conformance or exposed independently from a private interface or type.

## Related topics

- [Static interfaces](interfaces-overview.md)
- [Interface implementations](interface-implementations.md)
- [Interface requirements](interface-requirements.md)
- [Associated types](interface-associated-types.md)
- [Interface extension](interface-extension.md)
- [Access reduction](access-reduction.md)
- [Modules and imports](modules-and-imports.md)
