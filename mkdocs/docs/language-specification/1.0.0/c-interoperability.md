# C interoperability

<!-- spec-sections: none -->

Arkoi keeps its native model separate from C's ABI. This overview is a reading
guide; the linked chapters contain the specification rules.

!!! abstract "At a glance"

    Start with declarations and ABI domains, model the foreign data exactly, then
    put failure, ownership, and lifetime policy in an ordinary Arkoi wrapper.

## Choose a topic

| When you need to… | Read… |
| --- | --- |
| Import or export a function, choose boundary types, or understand C names | [Functions, symbols, and ABI domains](c-interoperability/functions-and-abi.md) |
| Transcribe a C struct or enum, declare an opaque type, or map `const` | [Data types and layout](c-interoperability/types-and-layout.md) |
| Bind globals, translate errors, call variadic functions, or use `void*` | [Globals, failures, variadics, and `void`](c-interoperability/globals-errors-variadics-void.md) |
| Model ownership, register callbacks, take addresses, or query layout | [Ownership, callbacks, addresses, and layout queries](c-interoperability/ownership-callbacks-addresses.md) |
| Represent C arrays and unions | [Arrays and unions](c-interoperability/layout-arrays-unions.md) |
| Audit callback execution and whole-program linker names | [Callback execution and symbol uniqueness](c-interoperability/callback-execution-symbols.md) |

## Suggested workflow

1. Write the smallest exact set of C-facing declarations.
2. Keep ABI-facing representations separate from native Arkoi representations.
3. Add an Arkoi wrapper for validation and policy.
4. Audit every retained pointer, callback, failure path, and external symbol.

## Related topics

- [Raw pointers](raw-pointers.md)
- [Ownership and moves](ownership-moves.md)
- [Resource lifecycle](resource-lifecycle.md)
- [Failures](failures.md)
- [Unsafe execution](unsafe.md)
- [Function pointers](function-pointers.md)
