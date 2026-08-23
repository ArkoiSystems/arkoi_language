---
title: Arkoi 1.0 function pointers
description: Arkoi 1.0 target-language rules for function-pointer types, compatibility, calls, nullability, and C ABI pointers.
---

# Function pointers

<!-- spec-sections: 4.7 -->

Arkoi represents a callable signature as a named function type and its address
as a raw pointer to that type. Indirect calls are explicit and unsafe.

!!! abstract "At a glance"

    - `type Operation = fun(...) @R` names a signature, not a value.
    - `*Operation` is a nullable, copyable raw function pointer.
    - `address(name)` selects a compatible named overload using expected type.
    - `call(pointer, ...)` invokes it with positional arguments in `unsafe`.
    - Function pointers capture no environment and add no dynamic dispatch.

## Function types

Function-type parameters are unnamed and retain the `@Type` notation:

```arkoi
type Operation = fun(
    @s32,
    @s32,
) @s32

type Loader = fun(
    @string_view,
) !LoadFail @File

type RawOperation = unsafe fun(
    @*mut u8,
)
```

A function type describes a signature. It is not itself a storable value and
cannot be a binding, field, parameter, or return type by value. It may be named
through a transparent alias and used as a raw pointer referent.

!!! danger "Compile-time error — named function-type parameter"

    ```arkoi
    type Invalid = fun(
        value @s32,
    ) @s32
    ```

Functions are executable, not writable objects, so `*mut Operation` is invalid.

!!! danger "Compile-time error — mutable function pointer"

    ```arkoi
    operation @*mut Operation
    ```

## Taking an address

```arkoi
fun add(left @s32, right @s32) @s32:
    return left + right

operation @*Operation = address(add)
```

`address(add)` uses the expected `*Operation` type to select exactly one named
overload. Compatibility checks:

- parameter count and ordered parameter types;
- ownership and reference-mutability modes;
- return type and failure effect;
- safety; and
- ABI.

No exact match or multiple matches is a compile-time error. The operation
returns a non-null pointer, although the type `*Operation` remains nullable and
can hold `null` through other operations.

A function name becomes a pointer only through explicit `address(...)`; an
overload set is never a runtime value.

## Indirect calls

```arkoi
unsafe:
    result @s32 = call(operation, 10, 20)
```

`call(...)` is explicit and requires an unsafe context for every indirect raw
function-pointer call. A pointer may be null, invalid, or originate in foreign
or unsafe code; invoking one that is null or invalid is undefined behavior.

Arguments are positional only because function types have no parameter names.
A fallible pointer uses ordinary handling:

```arkoi
unsafe:
    file @File = call(loader, path)!
```

A pointer to an `unsafe fun` retains that safety requirement, though every raw
indirect call is already confined to an unsafe context.

Direct calls remain ordinary:

```arkoi
result @s32 = add(10, 20)
```

## Values and method addresses

Raw function pointers are data. They may be stored, passed, returned, copied,
compared with `null`, and compared with one another using `==` or `!=` for
address identity.

They refer only to named functions and carry no captured environment. Arkoi
does not create closures or bound-method values here. To represent a method,
choose its qualified function explicitly and pass the receiver as a separate
argument to `call`.

Function pointers add indirect calls, not interface objects, runtime overload
lookup, or dynamic dispatch.

## C ABI function types

The declaration modifier selects the C ABI:

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int

compare @*CompareCallback = address(compare_values)
```

`CompareCallback` is the C signature; `*CompareCallback` is its C ABI pointer.
The `import "C"` modifier is not written inside `fun(...)`.

A C ABI function type:

- uses the target C calling convention and representation;
- accepts only C-compatible parameter and return types;
- cannot declare an Arkoi failure effect; and
- supports positional arguments only.

An exactly compatible C-linked named function can be addressed:

```arkoi
export "C" fun compare_values(
    left @*c.void,
    right @*c.void,
) @c.int:
    return 0
```

Calling its pointer remains unsafe:

```arkoi
unsafe:
    result @c.int = call(compare, left, right)
```

Additional ABI restrictions are defined in
[C functions and ABI](c-interoperability/functions-and-abi.md).

## Related topics

- [Calls and overloads](calls-overloads.md)
- [Raw pointers](raw-pointers.md)
- [Unsafe execution](unsafe.md)
- [C functions and ABI](c-interoperability/functions-and-abi.md)
