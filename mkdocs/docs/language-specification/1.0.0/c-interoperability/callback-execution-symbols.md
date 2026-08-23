---
title: Arkoi 1.0 C callback execution and symbols
description: Arkoi 1.0 target-language rules for foreign callback execution, concurrency, reentrancy, symbol uniqueness, and linking.
---

# C callback execution and symbol uniqueness

<!-- spec-sections: 15.19, 15.20 -->

An exported Arkoi callback executes under the foreign API's threading and lifetime
contract. Separately, every C-linked symbol must be unique across the complete linked
program.

!!! abstract "At a glance"

    - Assume arbitrary-threaded, concurrent, and reentrant callback entry unless C
      documents stronger guarantees.
    - Arkoi adds no runtime attachment, synchronization, failure catcher, or lifetime
      extension at callback entry.
    - Unsupported unwinding across the boundary is undefined behavior.
    - C-linked names share the target linker's namespaces and cannot overload.

## Callback entry and execution

Foreign code may enter an `export "C"` function under whatever conditions its API
defines. Arkoi supplies none of these assumptions automatically:

| Area | No implicit guarantee |
| --- | --- |
| Thread | Main thread; an Arkoi-created or already attached thread |
| Scheduling | Serialized callbacks; one invocation at a time; non-reentrant entry |
| Program state | Other Arkoi execution is idle; an implicit lock is held |
| Thread-local state | Any particular TLS or runtime context is initialized |

Unless the C API says otherwise, a callback can run on an arbitrary native thread,
concurrently with other callbacks and ordinary Arkoi execution, or reentrantly before
an earlier invocation returns.

The callback implementation must follow the API's threading, synchronization, and
reentrancy contract. Shared mutable state needs program-provided synchronization:
the C ABI inserts no locks, serialization, or single-threaded guarantee. A function
pointer type does not let the compiler infer any of these execution properties.

```arkoi
import "C" type Callback = fun(
    @c.int,
)

export "C" fun receive_value(
    value @c.int,
):
    pass
```

### Failures and foreign control transfer

Callbacks use the same canonical
[C failure boundary](globals-errors-variadics-void.md#failure-boundary): no Arkoi
failure effect crosses back to C, and all failures are handled before return.

Foreign or C++ exceptions, platform unwinding, `longjmp` across Arkoi frames, and
other foreign control transfers through a callback boundary are unsupported. If one
crosses, behavior is undefined.

An Arkoi panic, trap, or uncaught failure likewise cannot unwind through foreign
frames. Before returning to C, a callback that can encounter one must translate it
to a C-compatible result, record state, terminate the process, or use another
explicitly supported mechanism.

### Runtime services are not implied

Callback entry does not automatically:

- attach a foreign thread to the Arkoi runtime;
- create Arkoi thread-local state or scheduler context;
- install a failure boundary or catch foreign exceptions;
- block reentrant entry;
- pin referenced objects or extend borrowed lifetimes; or
- retain callback state.

If an Arkoi implementation requires runtime thread attachment before ordinary Arkoi
execution, implementation-provided support or a manually written C wrapper must
attach the thread before entering the callback.

### Registration lifetime

Passing an exported function address to C says nothing about when or how often C
will call it. The programmer must keep referenced global or external state valid,
keep externally retained data pointers alive for the whole callback lifetime, and
keep the callback code available while C can invoke it. Shutdown or unloading must
not invalidate a registered callback.

The program must also honor API thread affinity, provide synchronization, and
preserve invariants under reentrant invocation. Violating any of these lifetime or
execution requirements is undefined behavior.

## C symbol uniqueness

The [function symbol mapping](functions-and-abi.md#function-symbol-names) defines the
external name. That name must occupy a unique slot in the target linker's relevant
namespace across the complete linked Arkoi program.

Consequently, a C-linked function, global, exported object, or other symbol cannot
reuse `process` where the target considers those categories to share a namespace:

```arkoi
import "C" fun process(
    value @c.int,
)

export "C" fun process(
    value @c.int,
):
    pass
# Compile-time error: duplicate C symbol `process`
```

C-linked functions cannot overload, because different Arkoi signatures still claim
one external name:

```arkoi
import "C" fun convert(
    value @c.int,
) @c.int

import "C" fun convert(
    value @c.double,
) @c.double
# Compile-time error: duplicate C symbol `convert`
```

The uniqueness check is unchanged when declarations:

- are in different Arkoi modules;
- have identical signatures;
- have different visibility;
- differ only between import and export; or
- are functions versus globals on a target that puts both in one linker namespace.

Every foreign declaration is distinct. Repeating even an identical declaration in
another module is a compile-time error. The compiler diagnoses all duplicate
C-linked symbols visible in the complete Arkoi program.

A collision involving only a symbol supplied by an external native object or library
may instead be reported by the platform linker when Arkoi compilation cannot see it.
The target linker defines which symbol categories conflict, and Arkoi rejects every
conflict required by that target.

### Types do not create linker symbols

`import "C" type` participates in type checking, not symbol uniqueness:

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int
```

C ABI type declarations and manually transcribed C type names create no linker
symbols, even if a runtime symbol happens to use the same spelling.

## Related topics

- [Callback parameters and addresses](ownership-callbacks-addresses.md)
- [C functions and symbol names](functions-and-abi.md)
- [C globals and failure boundary](globals-errors-variadics-void.md)
- [Function pointers](../function-pointers.md)
- [Unsafe execution](../unsafe.md)
