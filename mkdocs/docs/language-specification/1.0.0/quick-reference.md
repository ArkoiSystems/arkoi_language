# Quick reference

Use this page to recall Arkoi syntax. Each row links to the canonical page that defines the complete rule and its edge cases.

## Declarations

| Form | Syntax | Complete rules |
| --- | --- | --- |
| Immutable binding | `name @T = value` | [Bindings and initialization](bindings-initialization.md) |
| Mutable binding | `name @mut T = value` | [Bindings and initialization](bindings-initialization.md) |
| Constant | `NAME @const T = value` | [Constants and globals](constants-globals.md) |
| Function | `fun name(parameters) @Return:` | [Functions and returns](functions-returns.md) |
| Fallible function | `fun name(parameters) !Failure @Return:` | [Failures](failures.md) |
| Method | `fun Type.name(self @&Type, ...)` | [Methods and visibility](methods-visibility.md) |
| Data aggregate | `data Name:` | [Aggregates and enums](aggregates-enums.md) |
| Resource aggregate | `resource Name:` | [Resource lifecycle](resource-lifecycle.md) |
| Aggregate construction | `Name(positional, field = value)` | [Aggregates and enums](aggregates-enums.md) |
| Module | `module qualified.name` | [Modules and imports](modules-and-imports.md) |
| Interface | `interface Name:` | [Static interfaces](interfaces-overview.md) |

## Type notation

| Notation | Meaning | Complete rules |
| --- | --- | --- |
| `T` | Value of type `T` | [Types and values](types-values.md) |
| `string` | Owned UTF-8 resource | [Types and values](types-values.md#strings-and-string-views) |
| `string_view` | Read-only UTF-8 data view | [Types and values](types-values.md#strings-and-string-views) |
| `?T` | Optional `T` | [Optional values](types-values.md#optional-values) |
| `&T` | Read-only, non-null reference | [References and lifetimes](references-lifetimes.md) |
| `&mut T` | Mutable, non-null reference | [References and lifetimes](references-lifetimes.md) |
| `*T` | Raw pointer with read-only pointee access | [Raw pointers](raw-pointers.md) |
| `*mut T` | Raw pointer with mutable pointee access | [Raw pointers](raw-pointers.md) |
| `[]T` | Read-only slice | [Slices](slices.md) |
| `[]mut T` | Mutable slice | [Slices](slices.md) |
| `[N]T` | Fixed-size array | [Arrays](arrays.md) |
| `fun(...) @R` | Named function type | [Function pointers](function-pointers.md) |

Binding-level `mut`, referent mutability, slice element mutability, and raw-pointer pointee mutability are independent.

String literals have type `string_view`. Convert an owned string with
`string_view(value)` and create an owned copy with `string(view)!`.

## Ownership and access

| Operation | Meaning | Complete rules |
| --- | --- | --- |
| `move(name)` | Transfer a named resource binding | [Moving resources](ownership-moves.md) |
| `clone(value)` | Invoke a resource's clone contract | [Cloning](cloning.md) |
| `take(&mut place)` | Move a resource out and leave an optional empty | [Resource-valued places](places-replacement.md) |
| `replace(&mut place, value)` | Replace a resource and return the old value | [Resource-valued places](places-replacement.md) |
| `swap(&mut left, &mut right)` | Exchange initialized places | [Resource-valued places](places-replacement.md) |
| `readonly(value)` | Remove one layer of mutable access | [Read-only conversion](access-reduction.md) |
| `&place` / `&mut place` | Create a read-only or mutable reference | [References and lifetimes](references-lifetimes.md) |
| `address(place)` | Obtain a raw pointer to stable storage | [Raw pointers](raw-pointers.md) |
| `Type.name(&value, ...)` | Read-only receiver call | [Methods and visibility](methods-visibility.md) |
| `Type.name(&mut value, ...)` | Mutable receiver call | [Methods and visibility](methods-visibility.md) |
| `Type.name(move(value), ...)` | Owning receiver call | [Methods and visibility](methods-visibility.md) |

## Optionals and failures

| Form | Meaning | Complete rules |
| --- | --- | --- |
| `none` | Absent optional value | [Optional values](types-values.md#optional-values) |
| `value?.member` | Safe optional member access | [Optional values](types-values.md#optional-values) |
| `value ?? fallback` | Lazy defaulting | [Optional values](types-values.md#optional-values) |
| `value?` | Propagate absence from an optional-returning function | [Optional values](types-values.md#optional-values) |
| `value!` | Propagate a failure, or convert optional absence to `CoreFail.none_access` | [Failures](failures.md) |
| `expression handle failure:` | Handle a recoverable failure locally | [Failures](failures.md) |
| `fail Failure.member` | Produce a failure | [Failures](failures.md) |

## Control flow and protocols

| Form | Complete rules |
| --- | --- |
| `if` / `elif` / `else` | [Control flow](control-flow.md) |
| `while condition:` | [Control flow](control-flow.md) |
| `loop:` | [Control flow](control-flow.md) |
| `initial |> Function(&_)` | [Pipeline expressions](pipelines.md) |
| `for item in source:` / `for! item in source:` | [For loops](for-loops.md) |
| `container[index]!` | [Indexing](indexing.md) |
| `container[start..end]!` | [Slicing](slicing.md) |
| `length(value)` | [Length](length.md) |
| `implements Interface for Type` | [Interface implementations](interface-implementations.md) |

## C boundary forms

| Form | Purpose | Complete rules |
| --- | --- | --- |
| `import "C" fun ...` | Import a C function | [Functions and ABI](c-interoperability/functions-and-abi.md) |
| `export "C" fun ...` | Export an Arkoi function through the C ABI | [Functions and ABI](c-interoperability/functions-and-abi.md) |
| `import "C" type ...` | Declare an opaque or scalar C ABI type | [C-compatible types](c-interoperability/types-and-layout.md) |
| `export "C" data ...` | Define a C-compatible structure | [C-compatible types](c-interoperability/types-and-layout.md) |
| `import "C" name @T` / `export "C" name @T = value` | Import or export C globals | [Globals and boundary behavior](c-interoperability/globals-errors-variadics-void.md) |

See the [compiler-hook index](compiler-hooks.md) for reserved `__...__` hooks and their canonical contracts.
