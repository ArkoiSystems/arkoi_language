# Glossary

These terms have one meaning throughout the Arkoi 1.0.0 specification. Follow each link for the complete normative rules.

## Values and storage

### Binding

A name associated with a typed value or storage location. Binding mutability controls reassignment; it is independent from access through references, pointers, and slices. See [bindings and initialization](bindings-initialization.md).

### Compile-time constant

An addressless typed value declared as `NAME @const T = expression` and fully
evaluated during compilation. It is not a place and has no storage identity. See
[constants and module globals](constants-globals.md).

### Module variable

An immutable or mutable module-level binding with stable program-lifetime
storage. Even an immutable module variable is an addressable place rather than a
compile-time constant. See [constants and module globals](constants-globals.md).

### String view

A copyable, read-only `string_view` over valid UTF-8 storage. Literal-backed
views have static lifetime; views of owned strings borrow their source. See
[strings and string views](types-values.md#strings-and-string-views).

### Place

An expression that designates storage rather than producing only a temporary value. Bindings, fields, array elements, and supported indexed results can be places. See [resource-valued places](places-replacement.md).

### Temporary

A value produced by an expression without a reusable named binding. Its lifetime normally ends after the containing full expression, subject to the specific loop-lifetime rule. See [references and lifetimes](references-lifetimes.md) and [iterator values and lifetimes](iterator-values-and-lifetimes.md).

### Pipeline carrier

The single current value of a [pipeline expression](pipelines.md), designated
inside each stage by `_`. A returned value replaces it; a no-value stage
preserves it.

### Stable addressable place

Storage whose address and lifetime are sufficiently evident for Arkoi's limited lexical checks to create a reference or raw pointer. See [references and lifetimes](references-lifetimes.md).

## Types and ownership

### Data type

A freely copied value type containing no resource-owned state. Composite categories are defined by their contained types. See [data and resource types](types-values.md).

### Resource type

An ownership-controlled value that is moved by default, cleaned up deterministically, and cloned only through an explicit clone contract. See [moving resources](ownership-moves.md) and [resource lifecycle](resource-lifecycle.md).

### Move

An ownership transfer that leaves its source uninitialized. Named resource bindings require `move(name)` in consuming contexts. See [moving resources](ownership-moves.md).

### Clone

Explicit creation of an independent resource value through `__clone__`; it is not ordinary data copying. See [cloning](cloning.md).

### Ownership-taking parameter

A parameter declared with `@own T` that receives a resource by value and owns cleanup unless ownership is transferred again. See [moving resources](ownership-moves.md).

## References and unsafe access

### Reference

A fixed, non-null, non-owning alias to stable storage. Its name transparently
designates the referent, and Arkoi applies limited lexical lifetime analysis.
See [references and lifetimes](references-lifetimes.md).

### Raw pointer

A nullable address value. Storing or comparing it is safe; dereferencing, arithmetic, and pointer-to-reference conversion require an unsafe context. See [raw pointers](raw-pointers.md).

### Slice

A copied pointer-and-length descriptor that provides bounded, non-owning access to contiguous elements. See [slices](slices.md).

### Unsafe context

An `unsafe` block or function in which operations with programmer-verified memory or foreign-code obligations are permitted. It does not disable unrelated type checking. See [unsafe execution](unsafe.md).

## Execution outcomes

### Failure

A typed, recoverable control-flow effect that must be propagated explicitly or handled locally. See [failures](failures.md).

### Trap

An unrecoverable execution stop defined by the language, such as an operation whose contract explicitly traps. Traps cannot be handled by the failure-effect system.

### Undefined behavior

Behavior for which Arkoi gives no guarantees because unsafe or foreign code violated an assigned obligation. Safe language operations do not produce undefined behavior by themselves.

### Compile-time error

A source program that violates an Arkoi syntax, typing, ownership, effect, or semantic constraint and must be rejected before execution.

### Build-time error

A failure to resolve files, declared module identities, target configuration, or external artifacts before or around language compilation. See [modules and imports](modules-and-imports.md).

## Abstraction and integration

### Compiler hook

A reserved `__name__` function recognized by Arkoi syntax or lifecycle behavior. Hook resolution is static. See the [compiler-hook index](compiler-hooks.md).

### Interface

A static, nominal compile-time contract with no runtime value representation or dynamic dispatch. See [static interfaces](interfaces-overview.md).

### Conformance

An explicit, whole-program relationship established by `implements` after the compiler verifies the required associated types and external definitions. See [interface implementations](interface-implementations.md).

### Module

The qualified identity declared exactly once by an Arkoi source file and used by imports and qualified names. See [modules and imports](modules-and-imports.md).

### ABI domain

The representation and calling-convention domain of a type or declaration. Native Arkoi ABI types and C ABI types are distinct even when a target gives them identical machine representations. See [C functions and ABI](c-interoperability/functions-and-abi.md).

### Complete C type

A C ABI type whose size and alignment are known at the use site. Opaque imported data remains incomplete and is usable only through permitted indirection. See [C-compatible types](c-interoperability/types-and-layout.md).
