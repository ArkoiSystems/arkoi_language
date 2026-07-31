# Methods and visibility

<!-- spec-sections: 7.1–7.2 -->

Methods and associated functions are qualified external declarations. Arkoi
resolves them statically and grants private-field access only to declarations
that genuinely belong to the type.

!!! abstract "At a glance"

    - Declare `fun Type.name(...)` outside the aggregate.
    - A method receiver is exactly `self @&Type` or `self @&mut Type`.
    - Dot syntax may borrow only the receiver; other arguments stay explicit.
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
```

Ownership-consuming receivers are not supported.

!!! failure "Compile-time error — owning receiver"

    ```arkoi
    fun File.consume(self @own File):
        pass
    ```

A consuming operation is an associated function with an explicit owning
parameter:

```arkoi
fun File.into_buffer(file @own File) @Buffer:
    return buffer_from_file(move(file))

buffer @Buffer = File.into_buffer(move(file))
```

`File` and `Buffer` here are illustrative user or library resource types.

## Receiver borrowing

Dot syntax can insert a borrow for the receiver only:

```arkoi
size @usize = file.size()
file.flush()!
```

These correspond to:

```arkoi
size @usize = File.size(&file)
File.flush(&mut file)!
```

Other arguments must be borrowed explicitly:

```arkoi
fun File.write(self @&mut File, buffer @&Buffer) !IOFail:
    write_buffer(self, buffer)!

file.write(&buffer)!
```

References themselves use ordinary field and method access. Only raw pointers
need explicit unsafe dereference before dot access.

## Associated functions

A qualified function without `self` belongs to the type as an associated
function:

```arkoi
fun File.open(path @&string) !IOFail @File:
    return open_native_file(path)!

file @File = File.open(&path)!
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

pub fun parse(text @&string) !ParseFail @Data:
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
the type. Code without private access also cannot use named-field construction
for a private field; it must call an accessible constructor.

## Related topics

- [Aggregates and enums](aggregates-enums.md)
- [Calls and overloads](calls-overloads.md)
- [Compiler hooks](compiler-hooks.md)
- [Resource lifecycle](resource-lifecycle.md)
- [Raw pointers](raw-pointers.md)
