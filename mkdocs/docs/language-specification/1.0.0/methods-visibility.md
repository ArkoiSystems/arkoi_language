# Methods and visibility

<!-- spec-sections: 7.1–7.2 -->

Methods and associated functions are qualified external declarations. Arkoi
resolves them statically and grants private-field access only to declarations
that genuinely belong to the type.

!!! abstract "At a glance"

    - Declare `fun Type.name(...)` outside the aggregate.
    - A method receiver is `self @&Type`, `self @&mut Type`, or `self @own Type`.
    - Call methods through the type with the receiver as an explicit first argument.
    - A qualified function without `self` is an associated function.
    - Declarations and fields are private unless marked `pub`.

## Methods

```arkoi
fun Point.distance(self @&Point, other @&Point) @f64:
    return distance_between(self.x, self.y, other.x, other.y)
```

Methods imply neither inheritance nor traits, interface objects, or dynamic
dispatch. Their receiver must use one of these forms:

```arkoi
self @&Point
self @&mut Point
self @own File
```

The ordinary type-category and ownership rules apply. In particular, `@own`
receives a resource and consumes the value explicitly moved by the caller:

```arkoi
fun File.into_buffer(self @own File) @Buffer:
    return buffer_from_file(move(self))

buffer @Buffer = File.into_buffer(move(file))
```

`File` and `Buffer` here are illustrative user or library resource types.

## Explicit receiver calls

Call a method through its declaring type. Its receiver is the first argument and
uses the same explicit borrowing or movement syntax as every other argument:

```arkoi
size @usize = File.size(&file)
File.flush(&mut file)!
buffer @Buffer = File.into_buffer(move(file))
```

An existing reference can be passed directly when it has the required type and
access:

```arkoi
file_ref @&mut File = &mut file
File.flush(file_ref)!
```

All arguments remain explicit:

```arkoi
fun File.write(self @&mut File, buffer @&Buffer) !IOFail:
    write_buffer(self, buffer)!

File.write(&mut file, &buffer)!
```

Fields are selected through a value or reference. Receiver functions are
qualified by their declaring type or a visible interface requirement. Raw
pointers require explicit unsafe dereference before field access.

## Interface receiver calls

A visible interface receiver requirement may be called through the interface
name. The explicit receiver's concrete type selects the implementation entirely
at compile time:

```arkoi
Writer.write(&mut file, data = bytes)
```

This syntax introduces neither interface values nor dynamic dispatch. An
associated-function requirement without `self` must be called through a
concrete implementing type because no receiver is available to select the
implementation.

## Associated functions

A qualified function without `self` belongs to the type as an associated
function:

```arkoi
fun File.open(path @string_view) !IOFail @File:
    return open_native_file(path)!

file @File = File.open(path)!
```

Constructors are ordinary associated functions that return complete values;
there is no initialization hook.

## Public and private declarations

Declarations are private by default. Prefix a type, function, method, associated
function, or field with `pub` to expose it:

```arkoi
pub data Point:
    pub x @f32
    pub y @f32

pub fun parse(text @string_view) !ParseFail @Data:
    return parse_data(text)!

pub fun Point.distance(self @&Point, other @&Point) @f64:
    return distance_between(self.x, self.y, other.x, other.y)
```

Accessibility does not override ordinary ownership and mutability requirements.

## Type-private fields

```arkoi
pub resource File:
    handle @u64
```

A private field is accessible only to:

- methods of its declaring type;
- associated functions of its declaring type; and
- compiler-recognized hooks of its declaring type.

Those declarations must be in the same module as the type:

```arkoi
pub fun File.raw_handle(self @&File) @u64:
    return self.handle

pub fun File.from_handle(handle @u64) @File:
    return File(handle = handle)

fun File.__drop__(self @&mut File):
    close_handle(self.handle)
```

An unrelated same-module function has no private access.

!!! failure "Compile-time error — unrelated private-field access"

    ```arkoi
    fun debug_file(file @&File):
        print_u64(file.handle)
    ```

Another module cannot gain access merely by declaring a qualified function for
the type. Code without private access cannot initialize a private field by name
or position. An aggregate with any inaccessible field must be created through
an accessible constructor.

## Related topics

- [Aggregates and enums](aggregates-enums.md)
- [Calls and overloads](calls-overloads.md)
- [Pipeline expressions](pipelines.md)
- [Compiler hooks](compiler-hooks.md)
- [Resource lifecycle](resource-lifecycle.md)
- [Raw pointers](raw-pointers.md)
