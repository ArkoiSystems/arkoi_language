# Types

Arkoi uses explicit types in declarations, parameters, and return annotations. The type system is focused on primitives that map cleanly to compiler output.

## Primitive types

The language defines:

- `@s8`
- `@u8`
- `@s16`
- `@u16`
- `@s32`
- `@u32`
- `@s64`
- `@u64`
- `@usize`
- `@ssize`
- `@f32`
- `@f64`
- `@bool`

## Typed variables

Variables declare their type next to the name.

```arkoi
count @s32 = 10
ratio @f64 = 2.5
ready @bool = true
```

## Typed parameters

Function parameters also carry type annotations.

```arkoi
fun calling_convention(a @u32, b @u32) @u32:
    return a + b
```

## Type conversion

Examples show explicit type-cast behavior in the language pipeline. When using conversions, prefer making the intent visible in the source rather than relying on implicit coercion.

## Practical rule

When writing Arkoi code, treat types as mandatory unless the compiler and docs explicitly say otherwise. That keeps the language predictable and easier to reason about.