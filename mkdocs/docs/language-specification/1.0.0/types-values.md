# Types and values

<!-- spec-sections: 3.1–3.6, 3.8 -->

Arkoi separates freely copied **data** from ownership-controlled **resources**.
Type constructors describe structure; explicit verbs describe ownership changes.

!!! abstract "At a glance"

    - `mut` changes a binding; `own` marks ownership-taking parameters.
    - `?`, `&`, `*`, `[]`, and `[N]` construct compound types.
    - Data copies normally; resources require explicit ownership operations.
    - `usize` and `ssize` match the target pointer width.
    - `string` is a UTF-8 resource; `length` and `bytes` are language built-ins.
    - A type alias is transparent and creates no new type identity.

## Type and ownership notation

| Notation | Meaning |
| --- | --- |
| `value @mut T` | Reassignable binding of `T` |
| `value @own T` | Ownership-taking resource parameter |
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
| Includes numbers, `bool`, `char`, references, failure values, simple enums, and permitted raw pointers | Includes built-in `string` and user/library resource types |
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
- References, slices, and raw pointers are non-owning data values even when
  they point to resources.

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
Decimal and scientific notation are supported; hexadecimal floating literals
are not.

```arkoi
ratio @f64 = 1.5
large @f64 = 1e10
small @f32 = 2.5e-4
grouped @f64 = 1_000.25
```

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

## Strings and UTF-8

Arkoi has one built-in string type, `string`; there is no separate `str`.
String literals produce `string` values, every string contains valid UTF-8, and
`string` is a resource.

The in-memory representation is unspecified. Implementations may use static or
heap storage, small-string optimization, or another representation. Borrow
strings through ordinary `&string` and `&mut string` references.

### `length` and `bytes`

`length(text)` is an infallible language built-in and returns a `usize` byte
count—not a scalar or grapheme count:

```arkoi
text @string = "é"
byte_count @usize = length(text)  # 2
```

`bytes(text)` is an infallible language built-in that produces a zero-copy
read-only `[]u8` view of the same UTF-8 storage:

```arkoi
text @string = "Hé"
encoded @[]u8 = bytes(text)  # [0x48, 0xC3, 0xA9]
```

These are language operations with defined operand access, not ordinary calls
that implicitly borrow an argument. `length` observes the operand without
moving it. `bytes` borrows its storage; the slice cannot outlive, or remain live
while safe code moves, replaces, or invalidates, the source string.

`bytes` neither allocates nor copies. It cannot return `[]mut u8`, because
arbitrary byte mutation could break UTF-8 validity. Direct string indexing is
not defined.

!!! failure "Compile-time error — direct string indexing"

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
user @?User = none
name @?string = user?.name
display @string = name ?? "anonymous"
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
