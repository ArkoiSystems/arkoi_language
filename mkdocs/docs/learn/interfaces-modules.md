---
title: Arkoi interfaces and modules
description: Learn how Arkoi 1.0 organizes files into modules and models nominal compile-time contracts with static interfaces.
---

# Interfaces and modules

Modules give declarations stable qualified names. Static interfaces describe
behavior that named user aggregates opt into explicitly, with all selection
resolved at compile time.

!!! info "Arkoi 1.0 target-language illustration"

    The files on this page illustrate the **target language**. They are not
    promised to compile with the current executable. See
    [Current compiler vs. Arkoi 1.0](../getting-started/compatibility.md) before
    trying them with compiler 0.1.0.

## What you will learn

By the end of this chapter, you will be able to:

- give every source file a stable module identity;
- import another module without flattening its names;
- distinguish a static interface contract from a runtime value;
- implement requirements nominally for a named aggregate; and
- bind an associated type to a concrete type.

This chapter builds on [Functions, calls, and methods](calls-methods.md) and
[Collections and iteration](collections-iteration.md).

## One file is one named module

Every Arkoi source file begins with exactly one `module` declaration. Imports
come immediately afterward and name complete absolute modules:

```text
src/
├── contracts/counted.ark
└── inventory/batch.ark
```

The first file can define a public contract:

```arkoi title="src/contracts/counted.ark — Arkoi 1.0 target"
module contracts.counted

pub interface Counted:
    fun count(
        self @&Self,
    ) @usize
```

`Self` stands for the concrete implementing type. An interface body contains
requirements, not executable function bodies or storage.

## A concrete module implements the contract

The aggregate's module imports the contract and declares one nominal
relationship:

```arkoi title="src/inventory/batch.ark — Arkoi 1.0 target"
module inventory.batch

import contracts.counted as counted

pub data Batch:
    pub size @usize

implements counted.Counted for Batch

pub fun Batch.count(
    self @&Batch,
) @usize:
    return self.size

pub fun report(batch @&Batch) @usize:
    return counted.Counted.count(batch)
```

The implementing function is an ordinary external function on `Batch`. The
explicit receiver's concrete type selects it when the interface-qualified call
is compiled. The interface introduces no runtime representation, virtual table,
dynamic dispatch, or interface-valued variable.

Conformance is nominal: a matching `Batch.count` function alone is not enough;
the `implements` declaration is required. Only named user-defined `data` and
`resource` aggregates can implement interfaces.

## Associated types bind concrete details

An interface may leave a related type open:

```arkoi
pub interface Source:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item
```

An implementation with an associated type uses a small block that contains
only the binding. The concrete type and matching function remain ordinary
declarations:

```arkoi title="fragment — Arkoi 1.0 target"
data Counter:
    current @u32
    stop @u32

implements Source for Counter:
    type Item = u32

fun Counter.next(
    self @&mut Counter,
) @?u32:
    if self.current >= self.stop:
        return none

    value @u32 = self.current
    self.current = self.current + 1
    return value
```

The matching `Counter.next` body remains an ordinary external function. After
substituting `Self` and `Item`, its contract must match the requirement exactly.
Interfaces may also extend other interfaces; inherited requirements are
flattened and checked without creating runtime inheritance.

## Keep module and interface roles separate

| Concern | Rule |
| --- | --- |
| File identity | The declared module name matches the source file's project path. |
| Imports | Absolute, module-wide, qualified, and placed before other declarations. |
| Dependency direction | The module import graph is acyclic. |
| Contract selection | Interface conformance is explicit, nominal, and compile-time only. |
| Implementation location | Only the interface's module or the concrete type's module may declare it. |
| Executable behavior | Concrete external functions provide every required body. |

Imports do not copy declarations into the current namespace and cannot be
re-exported. A local `as` alias shortens qualification only inside the importing
module.

## Check your understanding

1. Does importing `contracts.counted as counted` make `Counted` an unqualified
   name in `inventory.batch`?
2. Is a matching `Batch.count` function enough to establish conformance?
3. Where does the executable body for an interface requirement live?
4. Can a primitive such as `u32` implement a user interface?

<details markdown="1">
<summary>Show the answers</summary>

1. No. The alias only shortens the qualifier to `counted.Counted`.
2. No. The module must also declare `implements counted.Counted for Batch`.
3. In an ordinary external function on the concrete type, not inside the
   interface or the `implements` block.
4. No. Only named user-defined `data` and `resource` aggregates are eligible.

</details>

## Continue

Next, cross the boundary between compiler-checked Arkoi and programmer-verified
operations in [Unsafe code, hooks, and C interoperability](systems-interop.md).

Canonical rules:

- [Modules and imports](../language-specification/1.0.0/modules-and-imports.md)
- [Static interfaces](../language-specification/1.0.0/interfaces-overview.md)
- [Interface requirements](../language-specification/1.0.0/interface-requirements.md)
- [Associated types](../language-specification/1.0.0/interface-associated-types.md)
- [Interface implementations](../language-specification/1.0.0/interface-implementations.md)
- [Interface visibility](../language-specification/1.0.0/interface-visibility.md)
