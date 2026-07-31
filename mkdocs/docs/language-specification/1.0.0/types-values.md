# Types and values

<!-- spec-sections: 3.1–3.6, 3.8 -->

Arkoi separates freely copied **data** from ownership-controlled **resources**.
Type constructors describe structure; explicit verbs describe ownership changes.

!!! abstract "At a glance"

    - `mut` changes a binding, `own` marks ownership-taking parameters, and
      `const` declares an addressless compile-time value.
    - `?`, `&`, `*`, `[]`, and `[N]` construct compound types.
    - Data copies normally; resources require explicit ownership operations.
    - `usize` and `ssize` match the target pointer width.
    - `string` owns UTF-8 storage; `string_view` provides read-only access.
    - A type alias is transparent and creates no new type identity.

## Type and ownership notation

| Notation | Meaning |
| --- | --- |
| `value @mut T` | Reassignable binding of `T` |
| `value @own T` | Ownership-taking resource parameter |
| `value @const T = expression` | Addressless compile-time value declaration |
| `?T` | Optional `T` |
| `&T`, `&mut T` | Read-only or mutable reference |
| `*T`, `*mut T` | Read-only or mutable-pointee raw pointer |
| `[]T`, `[]mut T` | Read-only or mutable-element slice |
| `[N]T` | Fixed-size array |

Symbols construct types, while `move(value)` and `clone(value)` explicitly
perform ownership-changing operations. Binding mutability, referent or element
access, and ownership transfer are independent concepts.

## Data and resource categories

| Data | Resources |
| --- | --- |
| Safely copied by value | Never copied implicitly |
| Includes numbers, `bool`, `char`, `string_view`, references, failure values, simple enums, and permitted raw pointers | Includes built-in `string` and user/library resource types |
| User aggregate declared with `data` | User aggregate declared with `resource` |
| May contain data fields only | May contain data and resource fields |

```arkoi
data Point:
    x @f32
    y @f32

resource User:
    id @u32
    name @string
```

A `resource` declaration may express semantic ownership even when all visible
fields are data. Arkoi never silently recategorizes a declaration.

!!! failure "Compile-time error — resource field in data"

    ```arkoi
    data Invalid:
        name @string
    ```

`File`, `Socket`, and `Buffer` are illustrative user or library resource types,
not built-in language types.

### Composite categories

- `?T` is a resource exactly when `T` is a resource.
- `[N]T` is a resource exactly when `T` is a resource.
- References, slices, raw pointers, and `string_view` are non-owning data values.

See [Ownership and moves](ownership-moves.md) for resource value semantics and
[Aggregates and enums](aggregates-enums.md) for declarations.

## Numeric, character, and Boolean types

| Family | Types | Definition |
| --- | --- | --- |
| Unsigned integer | `u8`, `u16`, `u32`, `u64`, `usize` | `usize` is target pointer-width unsigned |
| Signed integer | `s8`, `s16`, `s32`, `s64`, `ssize` | `ssize` is target pointer-width signed |
| Floating point | `f32`, `f64` | IEEE 754 |
| Character | `char` | One Unicode scalar value |
| Byte | `u8` | Raw byte |
| Boolean | `bool` | `true` or `false` |

## Literals

### Integers

Integer literals are arbitrary-precision compile-time integers until context
converts them to a concrete type. A context-free integer literal expression is
invalid. Compile-time arithmetic is performed at arbitrary precision and then
checked against the destination.

```arkoi
decimal @u32 = 123
hexadecimal @u32 = 0xff
binary @u32 = 0b1010
octal @u32 = 0o755
grouped @u32 = 1_000_000
```

Underscores may occur only between valid digits.

### Floating point

Floating literals remain compile-time values until assigned to `f32` or `f64`.
Decimal, scientific, and hexadecimal notation are supported.

```arkoi
ratio @f64 = 1.5
large @f64 = 1e10
small @f32 = 2.5e-4
grouped @f64 = 1_000.25
hexadecimal @f64 = 0x1.8p1
hexadecimal_small @f32 = 0x1p-4
```

A hexadecimal floating literal starts with `0x` or `0X`. Its significand uses
hexadecimal digits and may contain a radix point, with at least one hexadecimal
digit in total. Its mandatory `p` or `P` exponent uses decimal digits and
specifies a power of two.

Underscores may separate digits within the significand or exponent, but cannot
touch the radix prefix, radix point, exponent marker, or exponent sign.

### Characters

A character literal uses single quotes and must decode to exactly one Unicode
scalar value:

```arkoi
letter @char = 'a'
newline @char = '\n'
ascii_a @char = '\x41'
face @char = '\u{1f600}'
```

Supported escapes are `\n`, `\r`, `\t`, `\0`, `\\`, `\'`, `\xNN`, and
`\u{...}`. The byte escape requires exactly two hexadecimal digits. A Unicode
escape cannot encode a surrogate or an out-of-range value.

## Strings and string views

| Type | Category | Purpose |
| --- | --- | --- |
| `string` | Resource | Owns valid UTF-8 text storage |
| `string_view` | Data | Read-only, non-owning view of valid UTF-8 storage |

String literals have type `string_view` and use immutable compiler-managed
storage with program lifetime. This makes literal-backed constants
allocation-free:

```arkoi
TITLE @const string_view = "Arkoi"
```

The language does not expose either type's memory representation.
Making a `string_view` binding `mut` permits rebinding the view, not modifying
the viewed bytes.

### Conversions

```arkoi
owned @string = load_text()!
view @string_view = string_view(owned)
owned_copy @string = string(TITLE)!
```

`string_view(owned)` borrows the owned string without allocating or copying
bytes. The view cannot outlive the string, and the string cannot be moved,
replaced, or dropped while the view remains live.

`string(view)!` copies the UTF-8 bytes into independent owned storage and may
produce `CoreFail.out_of_memory`. Neither conversion is implicit. Read-only text
parameters normally take `string_view` by value; ownership-taking parameters use
`@own string`.

Only views whose storage derives entirely from string literals may be used in
constant expressions.

### `length` and `bytes`

Both text types support `length(...)` and `bytes(...)`:

```arkoi
text @const string_view = "Hé"
byte_count @usize = length(text)  # 3
encoded @[]u8 = bytes(text)       # [0x48, 0xC3, 0xA9]
```

`length` returns the UTF-8 byte count. `bytes` returns a zero-copy read-only
`[]u8` with the source storage's lifetime. Neither operation allocates, moves an
owned string, or permits mutable access to encoded bytes. Direct indexing is not
defined for either text type.

!!! failure "Compile-time error — direct text indexing"

    ```arkoi
    value @u8 = text[index]
    ```

## Optional values

`?T` contains either a `T` or `none`. Optionals are distinct from nullable raw
pointers and are the ordinary way to represent absence.

| Operation | Meaning |
| --- | --- |
| `value?.member` | Safe member access, producing an optional |
| `value ?? fallback` | Use `fallback` only when absent; evaluation is lazy |
| `value?` | Unwrap or return `none` from an optional-returning function |
| `value!` | Unwrap or propagate `CoreFail.none_access` |

```arkoi
profile @?Profile = find_profile()
name @?string_view = profile?.display_name
display @string_view = name ?? "anonymous"
```

Postfix `?` is valid only when the enclosing function's successful return type
is optional. Postfix `!` on an optional requires the enclosing failure effect
to permit `CoreFail.none_access`.

When an expression is both fallible and returns an optional, each postfix
operator resolves one layer: the first `!` propagates the call failure and the
second `!` unwraps `none`. Thus `lookup()!!` means “propagate its declared
failure, then propagate `CoreFail.none_access` if its successful result is
absent.”

## Transparent type aliases

```arkoi
type UserId = u64
pub type FileDescriptor = u64
type OptionalFile = ?File
type Header = [16]u8
type FileView = &File
```

An alias is exactly the aliased type. It may name data or resource types,
optionals, arrays, slices, references, or raw pointers, but it:

- creates no representation or distinct type identity;
- changes no ownership, copying, cleanup, clone, visibility, or failure rule;
- cannot define hooks; and
- cannot be directly or indirectly recursive.

Consequently, an alias and its target cannot distinguish overloads.

## Related topics

- [Aggregates and enums](aggregates-enums.md)
- [Ownership and moves](ownership-moves.md)
- [References and lifetimes](references-lifetimes.md)
- [Slices](slices.md)
- [Arrays](arrays.md)
