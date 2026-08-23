---
title: Arkoi 1.0 interface requirements
description: Arkoi 1.0 target-language rules for interface methods, functions, hooks, overloads, and exact requirement matching.
---

# Interface requirements

<!-- spec-sections: 13.2, 13.4–13.5, 13.9, 13.12, 13.17–13.18, 13.24, 13.26–13.27, 13.38 -->

This page defines which declarations an interface may require and the exact rules used to match, merge, overload, and call them.

!!! abstract "At a glance"
    Interface scopes contain declarations, never bodies or storage. After substituting `Self` and associated types, each requirement must match an external concrete definition exactly—except that parameter names do not affect conformance.

## Contract-only bodies

Interface scopes contain declarations only:

```arkoi
interface Comparable:
    fun less(
        self @&Self,
        other @&Self,
    ) @bool

    fun greater(
        self @&Self,
        other @&Self,
    ) @bool
```

A requirement body is a compile-time error. Every implementing type supplies a matching ordinary external definition:

```arkoi
fun Version.greater(
    self @&Version,
    other @&Version,
) @bool:
    return Version.less(other, self)
```

Shared executable logic belongs in ordinary helper functions called by such definitions. Interfaces never contribute code or default implementations.

## Requirement kinds

Interfaces may require instance methods, associated functions, reserved hooks, and valid overload sets.

### `Self`

Inside the contract, `Self` is the unknown implementing concrete type. It may appear wherever a concrete type would otherwise be valid: receiver, parameter, return, reference, pointer, optional, array, slice, ownership-qualified parameter, associated function, or hook.

```arkoi
interface Factory:
    fun create() !CreateFail @Self

interface Consumable:
    fun consume(
        self @own Self,
    )
```

Ordinary type-category and ownership rules still apply. Read-only and mutable references remain distinct. The compiler substitutes every `Self` before matching.

`Self` is not a general alias in ordinary concrete definitions or associated-type bindings.

### Associated functions

An associated-function requirement has no `self` parameter:

```arkoi
interface Parseable:
    fun parse(
        source @string_view,
    ) !ParseFail @Self
```

It belongs to the concrete implementing type and is defined externally:

```arkoi
implements Parseable for Configuration

fun Configuration.parse(
    source @string_view,
) !ParseFail @Configuration:
    return parse_configuration(source)!
```

Associated requirements may use `Self` and associated types where valid, participate in ordinary overloads, be fallible or infallible, and return data or resources under ordinary ownership rules.

Call through the concrete type:

```arkoi
configuration @Configuration = Configuration.parse(text)!
```

`Parseable.parse(...)` is invalid because an associated requirement has no
receiver from which to select a concrete implementation. Resolution through
`Configuration.parse(...)` is direct and static.

### Reserved hooks

An interface may require a reserved language hook. Its declaration must obey the hook's canonical signature, receiver, ownership, return, and failure contract; see [Compiler hooks](compiler-hooks.md). Invalid reserved declarations are compile-time errors even inside an interface.

The matching external hook simultaneously satisfies the requirement and enables its language syntax or built-in operation. The definition may remain private under the [interface visibility rules](interface-visibility.md#implementing-definition-visibility).

Hook requirements otherwise participate in extension, substitution, exact matching, and shared-definition rules exactly like ordinary requirements. An interface supplies no hook behavior itself.

### No storage or associated constants

An interface can require behavior and associated types, but not fields, stored properties, offsets, field visibility, layout, representation, or ABI:

```arkoi
interface Positioned:
    field x @f64  # Compile-time error
```

Expose information through functions so each implementation may choose its own representation:

```arkoi
interface Positioned:
    fun x(
        self @&Self,
    ) @f64
```

Interfaces likewise cannot declare associated constants, and implementation blocks cannot bind them:

```arkoi
interface FixedCapacity:
    CAPACITY @const usize  # Compile-time error
```

Require an associated function such as `capacity() @usize` instead. Ordinary module and local constants remain available inside concrete implementations.

## Exact conformance matrix

The compiler first substitutes `Self` and all associated types, then compares a requirement with the concrete type's ordinary function set.

| Signature property | Distinguishes overload identity | Must match concrete definition | Same-identity inherited declarations may merge | Named-argument role |
| --- | --- | --- | --- | --- |
| Qualified function or method name / member kind | Yes | Exactly | Must be the same | Selects the member |
| Parameter count | Yes | Exactly | Must be the same, otherwise it is a distinct overload | Positional shape |
| Ordered parameter types | Yes | Exactly | Must be the same, otherwise it is a distinct overload | Types checked by position |
| Ownership and reference mutability, including receiver mode | Yes | Exactly | Must be the same, otherwise it is a distinct overload | Types checked by position |
| Fallibility mode: no effect or declared effect | Yes | Exactly | Different modes are distinct overloads | Selects ordinary or failure-aware call syntax |
| Return type, including no return | No | Exactly | Must be the same | None |
| Exact failure-set type within fallible mode | No | Exactly | Must be the same | None |
| Safety (`unsafe` or safe) | No | Exactly | Must be the same | Determines required call context |
| Associated-type substitutions within any type above | Through the substituted parameter identity | Exactly | Substituted results must agree | None |
| Parameter names | No | **Not required to match** | Must match position by position, unless both paths share the same originating declaration | Names accepted depend on the visible signature |
| Visibility | No | Not a signature-match property | Interface member visibility comes from the interface | Controls direct accessibility only |

Arkoi does not adapt, wrap, convert, weaken, strengthen, or otherwise reinterpret a definition to satisfy a requirement.

### Sharing one definition

One external definition may satisfy requirements from several interfaces only when the fully substituted contracts match by the matrix:

```arkoi
interface Resettable:
    fun reset(
        self @&mut Self,
    )

interface Reusable:
    fun reset(
        self @&mut Self,
    )

implements Resettable for Buffer
implements Reusable for Buffer

fun Buffer.reset(
    self @&mut Buffer,
):
    pass
```

A fallible and infallible reset cannot share one definition; they may coexist as
distinct overloads implemented by two definitions. Requirements that disagree
in return, safety, receiver, ownership, type, or arity likewise cannot share a
definition.

If same-named requirements are not exact matches and cannot coexist as an ordinary overload set, the concrete type's combined implementation set is invalid.

## Overload sets

Within one interface or across unrelated implemented interfaces, same-named
declarations may coexist when their identities differ by parameter count,
ordered parameter types, ownership, reference mutability, or fallibility mode:

```arkoi
interface Formatter:
    fun format(
        self @&Self,
        value @u64,
    ) @string

    fun format(
        self @&Self,
        value @f64,
    ) @string
```

Each overload is an independent requirement, and the concrete type must provide all of them. Providing only a subset is a compile-time error.

Return type, exact failure-set type within the fallible mode, visibility,
safety, and parameter names do not distinguish overloads. Same-identity
requirements that differ only in one of those properties are conflicting rather
than overloads, unless they are exact compatible requirements allowed to share
one definition.

Calls use ordinary exact overload rules. Interfaces add no best-match ranking, conversion-based selection, return-based selection, dynamic dispatch, or runtime overload metadata.

## Parameter names

Concrete parameter names may differ from the requirement:

```arkoi
interface Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
    )

implements Writer for File

fun File.write(
    self @&mut File,
    bytes @[]u8,
):
    pass
```

Names are excluded from function identity, overload identity, concrete conformance, and shared-definition matching.

They still define source-level named-argument APIs:

- a direct concrete call validates the concrete definition's names;
- an interface-qualified receiver call validates the requirement's names, then
  maps arguments by position to the statically selected concrete definition.

```arkoi
File.write(&mut file, bytes = content)
Writer.write(&mut file, data = content)
```

Using `data =` in a direct concrete call above is invalid. This rule requires no runtime name metadata, adapter, wrapper, or dynamic dispatch.

When extension merges unrelated same-identity requirements, their parameter names must agree; see [Interface extension](interface-extension.md#merging-inherited-requirements).

## Unsafe requirements

An interface may require an unsafe method, function, associated function, or hook:

```arkoi
interface RawReadable:
    unsafe fun read_raw(
        self @&Self,
        destination @*c.unsigned_char,
        length @usize,
    )
```

The concrete external definition must also be `unsafe`. A safe definition cannot satisfy an unsafe requirement or vice versa. Requirements differing only in safety conflict because safety does not distinguish overloads.

Calling the concrete operation still requires an unsafe context:

```arkoi
unsafe:
    RawReadable.read_raw(&device, pointer, length)
```

Conformance neither weakens the safety contract nor adds runtime checks.

## Related topics

- [Static interfaces](interfaces-overview.md)
- [Associated types](interface-associated-types.md)
- [Interface extension](interface-extension.md)
- [Interface implementations](interface-implementations.md)
- [Interface visibility](interface-visibility.md)
- [Calls and overloads](calls-overloads.md)
- [Unsafe execution](unsafe.md)
- [Compiler hooks](compiler-hooks.md)
