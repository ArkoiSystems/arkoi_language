---
title: Arkoi 1.0 interface extension
description: Arkoi 1.0 target-language rules for extending interfaces, inherited requirements, diamonds, and conformance paths.
---

# Interface extension

<!-- spec-sections: 13.3, 13.22, 13.28–13.29, 13.37 -->

This page defines contract inheritance, flattening, inherited conformance, merge conflicts, and overlapping conformance paths.

!!! abstract "At a glance"
    Extension flattens an acyclic parent graph into one static contract. Identical diamonds merge; inherited declarations cannot be overridden; and every route to one parent conformance must agree exactly.

## Extending contracts

An interface may extend one or more parents:

```arkoi
interface Readable:
    fun read(
        self @&Self,
    ) @u8

interface Writable:
    fun write(
        self @&mut Self,
        value @u8,
    )

interface Stream extends Readable, Writable:
    fun flush(
        self @&mut Self,
    )
```

`Stream` combines every inherited requirement with its direct `flush` requirement. An implementing type must satisfy the complete flattened contract.

Extension provides contract composition only—no default code, runtime inheritance, value conversion, dynamic dispatch, fields, or storage. The graph must be acyclic; direct and indirect cycles are compile-time errors.

Associated-type requirements are inherited too. A child may use an inherited name in its own declarations, and an implementing type binds inherited and direct associated types together in the child's single implementation.

When generic constraints are introduced, child conformance will satisfy constraints requiring any transitive parent.

## Inherited conformance

Implementing a child automatically implements every transitive parent:

```arkoi
interface Readable:
    fun read(
        self @&Self,
    )

interface Stream extends Readable:
    fun flush(
        self @&mut Self,
    )

implements Stream for FileStream
```

A separate `implements Readable for FileStream` is a compile-time error, even if it repeats identical bindings and requirements. This applies through any extension depth and keeps one canonical conformance path per interface.

If a type needs an independent parent implementation, it cannot simultaneously implement a child that already supplies that conformance.

## Merging inherited requirements

The same originating declaration inherited through several paths merges once:

```arkoi
interface Base:
    fun reset(
        self @&mut Self,
    )

interface Left extends Base:
    pass

interface Right extends Base:
    pass

interface Combined extends Left, Right:
    pass
```

`Combined` has one `Base.reset` requirement, with its original parameter names. The same rule applies to a shared originating associated type.

Unrelated inherited functions with the same identity merge only when:

- every complete contract property matches;
- parameter names match position by position.

Different parameter names conflict because the child cannot choose one named-argument API:

```arkoi
interface First:
    fun write(
        self @&mut Self,
        data @[]u8,
    )

interface Second:
    fun write(
        self @&mut Self,
        bytes @[]u8,
    )

interface Combined extends First, Second:
    pass  # Compile-time error: inherited named-argument APIs conflict
```

The merge rule covers instance methods, associated functions, hooks, and every
overload. Same-identity declarations also conflict when return type, exact
failure-set type within the fallible mode, safety, or another
non-overload-distinguishing property differs. Infallible and fallible
declarations have different identities and remain separate requirements.

Unrelated inherited associated types with the same name conflict and must be renamed in their declaring interfaces. A child has no projection or disambiguation syntax.

## No inherited redeclaration

A child cannot redeclare an inherited requirement, even identically:

```arkoi
interface Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
    )

interface BufferedWriter extends Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
    )  # Compile-time error
```

It cannot override, refine, rename, duplicate, or change any parameter, receiver, return, failure, safety, or other contract property. Inherited associated types likewise cannot be redeclared.

A genuinely distinct overload is valid:

```arkoi
interface BufferedWriter extends Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
        flush @bool,
    )
```

## Overlapping child paths

A concrete type may independently implement several children that share a parent:

```arkoi
interface Identifiable:
    type Id

    fun id(
        self @&Self,
    ) @Id

interface Storable extends Identifiable:
    fun store(
        self @&Self,
    )

interface DisplayableRecord extends Identifiable:
    fun display(
        self @&Self,
    )

implements Storable for Record:
    type Id = u64

implements DisplayableRecord for Record:
    type Id = u64
```

Every route to each shared parent must establish exactly the same conformance. All associated-type bindings and concrete definitions for methods, associated functions, and hooks must agree, including receiver/parameter modes and types, returns, failures, safety, and every other exact contract property.

When all routes agree, the compiler merges them into one canonical parent conformance. Conflicting bindings or definitions are compile-time errors. The rule applies through deep, multiple, and diamond-shaped acyclic graphs.

An explicit implementation of the shared parent remains redundant and invalid.

## Related topics

- [Static interfaces](interfaces-overview.md)
- [Interface requirements](interface-requirements.md)
- [Associated types](interface-associated-types.md)
- [Interface implementations](interface-implementations.md)
- [Interface visibility](interface-visibility.md)
