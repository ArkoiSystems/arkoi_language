# Comparisons and membership

<!-- spec-sections: 9.6–9.18, 9.21 -->

This page defines equality, ordering, aggregate comparison hooks, and membership expressions.

!!! abstract "At a glance"
    Comparison is static and type-directed. Built-in composite types compare their contents, resources require explicit hooks, references and pointers compare addresses, and `in` delegates to the container's `__contains__` hook.

## Comparison expressions

Arkoi supports `==`, `!=`, `<`, `<=`, `>`, and `>=`. One comparison expression contains exactly one comparison operator; chained comparison syntax is invalid.

```arkoi
within_range @bool = 0 < value and value < 10
invalid @bool = 0 < value < 10  # Compile-time error
```

Numeric comparison first applies the same lossless common-type conversion principles as ordinary implicit numeric conversion.

## Comparison audit matrix

Every operator is enabled independently: support for one comparison never implies support for another.

| Operand kind | Equality and inequality | Ordering | Customizable |
| --- | --- | --- | --- |
| `data` aggregate | Structural when every field supports the corresponding operator | Lexicographic in field declaration order when every field supports the operations required by that operator | Each operator may be replaced independently |
| `resource` aggregate | Only through the corresponding explicit hook | Only through the corresponding explicit hook | Yes; no operation is generated |
| Fixed array | Element-wise when the element type supports the corresponding operator | Lexicographic from index `0`; only equal-length, compatible array types compare | No |
| Slice | Viewed contents; length is checked before element equality | Lexicographic contents, then shorter shared prefix first | No |
| `?T` | Presence first, then `T`'s corresponding operator | `none` precedes every present value, then uses `T`'s corresponding operator | No |
| `&T`, `&mut T` | Address identity; reference mutability may differ | Unsupported | No |
| Raw pointer | Address identity, including comparison with `null` | Unsupported | No |
| Enum | Same enum type and logical member discriminant | Unsupported | No |

The availability condition is checked per operator. For example, `?Resource` supports `==` only when the resource explicitly supplies `__eq__`, and supports `!=` only when it supplies `__ne__`.

### Structural equality

Generated data-aggregate equality visits fields in declaration order. Fixed-array equality visits indices from `0` upward.

- `==` uses each component's normal `==`, stops at the first unequal component, and is `true` only when all components compare equal.
- `!=` uses each component's normal `!=`, stops at the first unequal component, and is `false` only when all components compare equal.
- A zero-length array has `==` result `true` and `!=` result `false`.

Slice equality first compares lengths. Unequal lengths produce `false` for `==` and `true` for `!=`; equal-length slices then follow the corresponding element algorithm from index `0`. Slices compare viewed contents, so different backing allocations may compare equal. Read-only and mutable slices use the same read-only, non-mutating comparison.

Optional equality has this truth table:

| Left | Right | `==` | `!=` |
| --- | --- | --- | --- |
| `none` | `none` | `true` | `false` |
| `none` | present | `false` | `true` |
| present | `none` | `false` | `true` |
| present | present | Compare contained values with the corresponding operator | Compare contained values with the corresponding operator |

These optional rules apply to both data and resource optionals.

Reference equality ignores referent values and never calls their hooks. Two references compare equal only when they designate the same address; `&T` and `&mut T` may compare when their referent types match after ignoring mutability. The operation is safe and infallible.

Raw-pointer equality compares stored address values without dereferencing. It is safe, does not require live or related pointees, may compare a pointer with `null`, and never calls pointee hooks. To compare designated values, access them explicitly under the applicable reference or pointer rules.

Enum equality is independent of physical storage. Values must have the same enum type and logical member discriminant; equal numeric discriminants from different enum types cannot be compared.

### Lexicographic ordering

For data aggregates and fixed arrays, Arkoi compares fields or elements in order. For `<`, the first less component yields `true`, the first greater component yields `false`, and complete equivalence yields `false`. `>` reverses the relation; `<=` and `>=` use the same order and include complete equivalence. Each generated operator exists only when all components support the comparisons that its algorithm requires.

Fixed arrays compare only arrays of the same length and compatible element type because length is part of the type. Zero-length arrays produce `false`, `true`, `false`, `true` for `<`, `<=`, `>`, and `>=`, respectively.

Slice ordering compares shared indices from `0`. At the first unequal element it uses the requested relation; if the shared prefix is equal, the shorter slice is less. Equal lengths and equal contents are equivalent. Read-only and mutable slices use the same non-mutating ordering.

Optional ordering fixes `none < present`:

| Left | Right | `<` | `<=` | `>` | `>=` |
| --- | --- | --- | --- | --- | --- |
| `none` | `none` | `false` | `true` | `false` | `true` |
| `none` | present | `true` | `true` | `false` | `false` |
| present | `none` | `false` | `false` | `true` | `true` |
| present | present | Use `T`'s `<` | Use `T`'s `<=` | Use `T`'s `>` | Use `T`'s `>=` |

These rules also apply to resource optionals, provided the contained resource explicitly supplies the requested ordering hook.

## Aggregate comparison hooks

Data aggregates may replace generated comparisons; resources gain comparisons only through these hooks:

| Operator | Hook |
| --- | --- |
| `==` | `__eq__` |
| `!=` | `__ne__` |
| `<` | `__lt__` |
| `<=` | `__le__` |
| `>` | `__gt__` |
| `>=` | `__ge__` |

```arkoi
fun Version.__lt__(
    self @&Version,
    other @&Version,
) @bool:
    if self.major != other.major:
        return self.major < other.major

    if self.minor != other.minor:
        return self.minor < other.minor

    return self.patch < other.patch
```

Every comparison hook:

- belongs to the compared aggregate and is declared in the same module;
- has exactly `self @&Type` and `other @&Type`;
- returns `bool` and declares no failure effect;
- may be private;
- replaces or enables only its corresponding operator.

A malformed reserved-hook declaration is a compile-time error. Hooks are independent: Arkoi neither derives missing operations nor verifies that `__ne__` negates `__eq__` or that ordering hooks form a mathematically consistent ordering.

When a data aggregate replaces only one comparison, every other comparison remains compiler-generated whenever its fields meet that operator's availability requirements.

Resources receive no structural comparisons even when all stored fields are comparable, because ownership, external state, or identity may not be represented by those fields. Fixed arrays, slices, optionals, references, raw pointers, and enums cannot define custom comparison hooks.

## Membership

Membership uses `in` and `not in`; it is separate from the `in` token in a `for` loop.

```arkoi
present @bool = item in container
absent @bool = item not in container
```

The container type owns an overloadable `__contains__` hook:

```arkoi
fun Container.__contains__(
    self @&Container,
    item @&Element,
) @bool:
    return self.contains_element(item)
```

Overloads are selected by the exact item-parameter signature under ordinary overload resolution. The receiver is `self @&Container`; aggregate and resource items use read-only references, while freely copyable data may be passed by value. Membership parameters cannot use `@own` or `&mut`, so neither operand is consumed or mutated.

An infallible hook enables `in` and `not in`. `not in` negates the `in` result; there is no negative hook.

A fallible hook enables only `in!` and `not in!`:

```arkoi
fun RemoteSet.__contains__(
    self @&RemoteSet,
    item @&Item,
) !LookupFail @bool:
    return self.lookup(item)!

present @bool = item in! container
absent @bool = item not in! container
```

The `!` belongs to the membership operator, not to a postfix expression. `in` with a fallible hook, `in!` with an infallible hook, and their corresponding `not in` forms are compile-time errors.

```arkoi
invalid @bool = (item in container)!  # Compile-time error
```

A fallible membership expression may instead handle failure:

```arkoi
present @bool = item in! container handle failure:
    yield false
```

The item is evaluated first and the container second, following binary left-to-right evaluation.

## Related topics

- [Numeric operations](expressions-and-numeric-operations.md)
- [Operator hooks](operator-hooks.md)
- [Evaluation order](evaluation-order.md)
- [Calls and overloads](calls-overloads.md)
- [Compiler hooks](compiler-hooks.md)
- [For loops](for-loops.md)
