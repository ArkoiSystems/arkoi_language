# Arkoi Programming Language Specification

**Language version:** 1  
**Document status:** Current language specification

Arkoi is a statically typed, compiled systems programming language with indentation-defined blocks, explicit ownership and failure handling, deterministic resource cleanup, statically resolved interfaces, and direct C interoperability.

This document defines the accepted syntax and semantics of Arkoi. Statements marked as compile-time errors describe constraints enforced before execution. A *failure* is recoverable through the language's failure-effect system. A *trap* is unrecoverable. Undefined behavior can arise only where this specification assigns responsibility to unsafe or foreign code.

## Contents
- [1. Language and Source Model](#1-language-and-source-model)
- [2. Declarations, Bindings, and Constants](#2-declarations-bindings-and-constants)
- [3. Types and Values](#3-types-and-values)
- [4. Functions and Calls](#4-functions-and-calls)
- [5. Ownership, Aggregates, and Resources](#5-ownership-aggregates-and-resources)
- [6. References, Pointers, Slices, and Arrays](#6-references-pointers-slices-and-arrays)
- [7. Methods, Visibility, and Compiler Hooks](#7-methods-visibility-and-compiler-hooks)
- [8. Failures and Unsafe Execution](#8-failures-and-unsafe-execution)
- [9. Expressions, Conversions, and Operators](#9-expressions-conversions-and-operators)
- [10. Evaluation, Assignment, and Result Use](#10-evaluation-assignment-and-result-use)
- [11. Control Flow and Iteration](#11-control-flow-and-iteration)
- [12. Indexing, Slicing, and Length](#12-indexing-slicing-and-length)
- [13. Static Interfaces](#13-static-interfaces)
- [14. Modules and Imports](#14-modules-and-imports)
- [15. C Interoperability](#15-c-interoperability)
- [16. Representative Program](#16-representative-program)

## 1. Language and Source Model

### 1.1 Source Structure and Blocks

Arkoi uses indentation to define blocks. A colon begins a block.

```arkoi
fun greet(name @string_view):
    print(name)
```

Comments begin with `#`.

```arkoi
# This is a comment.
```

Every indentation block creates a lexical scope. A binding declared in a block exists only inside that block.

---

### 1.2 Empty Blocks and `pass`

Every indentation block must contain at least one declaration or statement.

Whenever a construct introduces a block but intentionally contains no declarations or statements, the block must contain `pass`.

```arkoi
interface ThreadSafe:
    pass
```

This is the required syntax for an empty marker interface.

```arkoi
interface PlainData:
    pass
```

The rule applies generally to every Arkoi construct whose grammar permits an otherwise empty block, including:

- Interface declarations.
- Function, method, and associated-function bodies.
- `if`, `elif`, and `else` branches.
- `while`, `for`, and `loop` bodies.
- Failure handlers.
- Aggregate declarations when an empty aggregate is otherwise permitted.
- Unsafe blocks.

```arkoi
fun placeholder():
    pass

if condition:
    pass
else:
    perform_work()
```

A colon followed immediately by another declaration at the same indentation level does not form a valid empty block.

```arkoi
interface ThreadSafe:

interface PlainData:
    pass
```

The first interface is a compile-time syntax error because its block contains no indented content.

`pass`:

- Is a statement.
- Performs no operation.
- Produces no value.
- Cannot fail.
- Cannot trap.
- Has no runtime side effects.
- Exists solely to make an intentionally empty block explicit.

A block containing declarations or executable statements does not require `pass`.

```arkoi
interface Formatter:
    fun format(self @&Self, value @u64) @string
```

An `implements` declaration without associated-type bindings continues to use its single-line form and therefore does not create an empty block.

```arkoi
implements ThreadSafe for Worker
```

An `implements` block is used only when it contains required associated-type bindings, so `pass` is not valid as a substitute for missing bindings.

### 1.3 `pass` in Nonempty Blocks

`pass` is permitted anywhere an ordinary statement is syntactically valid.

```arkoi
fun process():
    pass
    perform_work()
```

It may therefore appear:

- In an otherwise empty block.
- Before, between, or after other statements.
- In any nested statement block.
- More than once in the same block.

```arkoi
if condition:
    pass
    update_state()
    pass
```

Every `pass` statement:

- Performs no operation.
- Produces no value.
- Cannot fail.
- Cannot trap.
- Has no side effects.
- Does not alter definite-initialization analysis.
- Does not satisfy return requirements for value-returning functions.
- Does not make unreachable code reachable.

Using `pass` in a nonempty block is unnecessary but valid. The compiler does not reject it merely because other statements or declarations are present.

The empty-block rule therefore requires at least one syntactic block item, and `pass` is the canonical explicit item when no real work or declaration is intended.

### 1.4 Optional Trailing Commas

Arkoi permits one optional trailing comma in any nonempty comma-separated list.

```arkoi
fun process(
    value @u32,
    enabled @bool,
):
    pass
```

The trailing comma is valid in both multiline and single-line forms.

```arkoi
fun process(value @u32, enabled @bool,):
    pass
```

This applies wherever Arkoi uses a comma-separated list, including:

- Function, method, hook, and associated-function parameters.
- Positional and named call arguments.
- Array literal elements.
- Positional and named aggregate-construction arguments.
- Multiple index expressions.
- Parent interfaces in an `extends` clause.

```arkoi
result @u32 = calculate(
    left,
    right,
)

point @Point = Point(
    x = 10,
    y = 20,
)

value @T = matrix[
    row,
    column,
]
```

Only one trailing comma is permitted.

```arkoi
fun process(value @u32,,): # Compile-time error
    pass  
```

A comma cannot appear by itself in an otherwise empty list.

```arkoi
fun empty(,): # Compile-time error
    pass  
```

A trailing comma:

- Does not add an element or parameter.
- Does not change overload identity.
- Does not affect evaluation order.
- Has no runtime meaning.
- Exists to support consistent formatting and simpler source edits.

### 1.5 Multiline Expressions and Line Continuation

Arkoi provides implicit line continuation while an expression is inside an open delimiter.

This applies inside:

- Parentheses: `(...)`
- Brackets: `[...]`

```arkoi
result @u32 = calculate(
    left,
    right,
)

item @T = matrix[
    row,
    column,
]
```

Newlines inside an open delimiter do not terminate the expression and do not create indentation-based blocks.

Indentation within the continued expression is used for formatting only.

```arkoi
result @u32 = calculate(
        left,
        transform(
            right,
        ),
    )
```

The compiler does not assign semantic meaning to the chosen continuation indentation, provided the tokens form a valid expression and any nested block syntax remains valid.

Line continuation is implicit only inside open parentheses or brackets. A backslash at line end is a syntax error.

```arkoi
result @u32 = left + \
    right  # Compile-time syntax error
```

A backslash at the end of a line never joins that line with the next source line.

Outside open parentheses or brackets, a newline ends the current statement or declaration unless the grammar is already inside an indentation-based block header or another explicitly defined multiline construct.

To split a long expression across lines, the programmer must place the continued portion inside parentheses or brackets.

```arkoi
result @u32 = (
    left
    + right
    + additional
)
```

Implicit continuation:

- Has no runtime meaning.
- Does not change evaluation order.
- Does not create a lexical scope.
- Does not require a continuation marker.
- May span blank lines and comments where otherwise syntactically valid.
- Ends when the matching delimiter closes.

Arkoi therefore has no general-purpose explicit line-continuation character.

## 2. Declarations, Bindings, and Constants

### 2.1 Declarations and Mutability

Bindings are declared directly with a name, type annotation, and optional initializer.

The `@` symbol introduces a declaration's type annotation.

#### Immutable binding

```arkoi
count @u32 = 10
```

An immutable binding may be initialized or assigned exactly once.

#### Mutable binding

```arkoi
count @mut u32 = 10
count = 20
```

The `mut` keyword applies to the binding, meaning that the binding may be reassigned.

Reference bindings are fixed aliases and do not permit binding-level `mut`.
Mutability inside `&mut T` controls access to the referent instead.

#### Delayed initialization

Both immutable and mutable bindings may be declared without an initializer.

```arkoi
result @u32
counter @mut u32
```

Arkoi performs definite-initialization checking across branches, loops, handlers, early exits, and propagation paths. There is no implicit zero initialization or default initialization.

#### Name Uniqueness and Shadowing

A local name may not be redeclared anywhere in the same function, including nested blocks and parameter scopes.

```arkoi
value @u32 = 10

if condition:
    value @u32 = 20  # Compile-time error: shadowing is not allowed.
```

---

### 2.2 Compile-Time Constants

Compile-time constants use the `const` binding modifier after `@` and require
an explicit type and initializer.

```arkoi
name @const Type = expression
```

```arkoi
WIDTH @const usize = 4
MAX_RETRIES @const u32 = 5
PI @const f64 = 3.141592653589793
```

A constant initializer is evaluated completely at compile time. A constant is
an addressless value declaration: it has no storage location or runtime object
identity.

Constants:

- Require an explicit type.
- Cannot be reassigned.
- Cannot be shadowed.
- May use other constants and permitted compile-time operations.
- May be used in array lengths, global initializers, and ordinary expressions.
- Must not depend on runtime values or operations.
- Cannot contain resources, references, raw pointers, slices, or other values whose meaning depends on runtime storage or lifetime. A `string_view` backed entirely by string-literal storage is permitted.
- Is not a place and therefore cannot be assigned, borrowed with `&` or `&mut`,
  or passed to `address(...)`.

```arkoi
BUFFER_SIZE @const usize = 1024
TOTAL_SIZE @const usize = BUFFER_SIZE * 4

buffer @[TOTAL_SIZE]u8
```

A runtime-dependent initializer is a compile-time error.

```arkoi
size @usize = read_size()!
BUFFER_SIZE @const usize = size  # Compile-time error
```

The result of a constant expression must be representable by its declared type.

Using a constant in an ordinary runtime expression produces its value. It does
not materialize addressable constant storage. This also applies to composite
constant values: an element such as `VALUES[0]` is a value, not a place backed
by the constant declaration.

### 2.3 Constant Scope and Visibility

Constants may be declared at module scope or inside a lexical block.

Module constants are private by default.

```arkoi
DEFAULT_SIZE @const usize = 1024
```

A public module constant places `pub` before the binding name.

```arkoi
pub DEFAULT_SIZE @const usize = 1024
```

Local constants are visible only within their lexical block.

```arkoi
fun process():
    CHUNK_SIZE @const usize = 64
    # ...
```

The `const` modifier is mutually exclusive with `mut` and `own`. Constants
cannot use delayed initialization.

All constants, including local constants, require compile-time-only initializers.

Constants obey Arkoi's existing no-shadowing rule. A constant cannot shadow a local binding, parameter, or another constant, and no later declaration may shadow it within the same function.

### 2.4 Constant Expressions

Arkoi uses a restricted constant-expression model.

Constant expressions may contain:

- Literals.
- References to other constants.
- Arithmetic, bitwise, comparison, and boolean operators.
- Explicit numeric operations such as `convert(...)`, `truncate(...)`, and safe numeric `bitcast(...)`.
- Optional, fixed-size array, and data-aggregate construction when every contained value is constant.
- `string_view` values backed entirely by string literals.
- Field access and compile-time-valid array indexing.

```arkoi
WIDTH @const usize = 4
AREA @const usize = WIDTH * 8
ENABLED @const bool = WIDTH > 2
MASK @const u32 = 0xff00 | 0x00ff
```

```arkoi
ORIGIN @const Point = Point(
    x = 0.0,
    y = 0.0,
)

VALUES @const [3]u32 = [10, 20, 30]
SECOND @const u32 = VALUES[1]
```

Constant expressions cannot contain:

- Calls to user-defined functions.
- Resource construction or resource values.
- Runtime state.
- Allocation or I/O.
- References, raw pointers, slices, or non-static `string_view` values.
- Unsafe operations.
- Failure propagation or failure handlers.

Arkoi does not define `const fun`.

Compile-time arithmetic follows the same semantic rules as runtime arithmetic. A trap or invalid operation encountered during constant evaluation is a compile-time error.

### 2.5 Module-Level Variables

Arkoi supports module-level variables only when their types are data types.

```arkoi
counter @mut u32 = 0

default_header @[16]u8 = [
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
]
```

Module-level variables are private by default. A public module-level variable uses `pub`.

```arkoi
pub counter @mut u32 = 0
```

Module-level variable initializers must be static initializer expressions. A
static initializer is a constant expression, extended only with references to
the initial values of other module-level data variables as described in
Section 2.6.

```arkoi
INITIAL_COUNT @const u32 = 10
counter @mut u32 = INITIAL_COUNT
```

A module-level variable may be immutable or mutable according to the normal binding syntax.

```arkoi
version @u32 = 1
counter @mut u32 = 0
```

Unlike a constant, a module-level variable has stable program-lifetime storage
and is an addressable place. This remains true for an immutable module variable:
immutability prevents reassignment, but does not remove its address or storage
identity. Reading a module variable in ordinary code loads its stored value; its
name is not a constant expression merely because its initializer was evaluated
at compile time.

A module-level variable must have a data type; a resource-typed module variable is a compile-time error.

```arkoi
global_file @File = File.open(path)!  # Compile-time error
```

This avoids runtime initialization order, startup failure, global cleanup order, and long-lived borrowing problems.

Resources must be created inside functions and passed explicitly through the program.

Local variables are not subject to the constant-initializer restriction.

### 2.6 Acyclic Initializer Dependencies

Dependencies among compile-time constants and module-level static initializers must be acyclic.

Order-independent module declarations may refer to declarations that appear later in the source file.

```arkoi
WIDTH @const usize = HEIGHT * 2
HEIGHT @const usize = 4
```

The compiler evaluates constants and module-level static initializers in
dependency order rather than textual order. A reference to a module variable
within another static initializer denotes that declaration's initial value; it
does not perform a runtime load.

A direct or indirect dependency cycle is a compile-time error.

```arkoi
FIRST @const usize = SECOND + 1
SECOND @const usize = FIRST + 1
```

```arkoi
first @u32 = second
second @u32 = first
```

The compiler reports the declarations participating in the cycle.

Function calls cannot occur in constant or module-level static initializers, so function-call recursion is unrelated to this initializer dependency graph.

## 3. Types and Values

### 3.1 Mixed Type and Ownership Syntax

Arkoi deliberately combines keywords, symbols, and explicit operations.

#### Keywords describe binding or ownership behavior

```arkoi
value @mut u32
file @own File
limit @const usize = 100
```

- `mut` means a binding may be reassigned.
- `own` means a resource parameter receives ownership.
- `const` declares an addressless value evaluated at compile time.

#### Symbols construct compound types

```arkoi
?T       # Optional T
&T       # Read-only reference to T
&mut T   # Mutable reference to T
*T       # Raw pointer to T
```

#### Verbs perform ownership-changing operations

```arkoi
move(value)
clone(value)!
```

This separation keeps structural types compact while making ownership transfer explicit and readable.

---

### 3.2 Data Types and Resource Types

Arkoi divides types into two fundamental categories.

#### Data types

A data type is safely and normally copyable by value.

Built-in data types include:

- Numeric types.
- `bool`.
- `char`.
- `string_view`.
- Raw pointers to C ABI types.
- References.
- Failure values.
- Simple enumerations.

A user-defined data aggregate uses `data`.

```arkoi
data Point:
    x @f32
    y @f32
```

A `data` aggregate may contain only data fields. Adding a resource field is a compile-time error. Arkoi never silently changes a type's category.

#### Resource types

A resource type has controlled ownership and is not implicitly copied.

Built-in or expected resource types include:

- `string`.
- `File`.
- `Socket`.
- `Buffer`.

A user-defined resource aggregate uses `resource`.

```arkoi
resource User:
    id @u32
    name @string
```

A resource may contain both data and resource fields.

```arkoi
resource File:
    handle @u64
```

A resource declaration may represent semantic ownership even when its fields appear data-like.

#### Composite category rules

- `?T` is a resource when `T` is a resource; otherwise it is data.
- An array of `T` is a resource when `T` is a resource; otherwise it is data.
- References, raw pointers, and `string_view` are non-owning data values.

---

### 3.3 Numeric Types

#### Unsigned integers

```arkoi
u8
u16
u32
u64
usize
```

#### Signed integers

```arkoi
s8
s16
s32
s64
ssize
```

Arkoi uses `s8` rather than `i8`.

#### Floating-point types

```arkoi
f32
f64
```

These follow IEEE 754 behavior.

#### Character and byte types

```arkoi
char  # Unicode scalar value
u8    # Raw byte
```

---

### 3.4 Literals

#### Integer literals

Integer literals are context-dependent and behave as arbitrary-precision compile-time integers until converted to a concrete type.

```arkoi
value @u32 = 100
```

A context-free integer literal expression is invalid.

Compile-time integer arithmetic uses arbitrary precision and is checked against the destination type afterward.

Supported forms:

```arkoi
123
0xff
0b1010
0o755
1_000_000
```

Underscores may appear only between valid digits.

#### Floating-point literals

Floating literals remain compile-time values until assigned to `f32` or `f64`.

Supported forms include decimal, scientific, and hexadecimal notation.

```arkoi
1.5
1e10
2.5e-4
1_000.25
0x1p0
0x1.8p1
0x1p-4
```

A hexadecimal floating literal starts with `0x` or `0X`. Its significand uses
hexadecimal digits and may contain a radix point, with at least one hexadecimal
digit in total. A `p` or `P` exponent is mandatory; its decimal exponent digits
specify a power of two.

Underscores may separate digits within the significand or exponent. They cannot
touch the radix prefix, radix point, exponent marker, or exponent sign.

#### Character literals

Character literals use single quotes and must represent exactly one Unicode scalar value after escape processing.

```arkoi
'a'
'\n'
'\x41'
'\u{1f600}'
```

Supported escapes:

- `\n`
- `\r`
- `\t`
- `\0`
- `\\`
- `\'`
- `\xNN`
- `\u{...}`

`\xNN` requires exactly two hexadecimal digits. Unicode escapes must represent valid scalar values and may not encode surrogates or out-of-range values.

---

### 3.5 Strings and String Views

Arkoi has two built-in UTF-8 text types:

```arkoi
string       # Owning resource.
string_view  # Read-only, non-owning data view.
```

Both types contain valid UTF-8. Their in-memory representations are
unspecified. A `string` owns its text storage and follows the ordinary resource
rules. A `string_view` copies normally and conceptually identifies immutable
UTF-8 storage together with its byte length.

String literals have type `string_view`. Their encoded bytes reside in
immutable compiler-managed storage for the complete program lifetime, so they
require no runtime allocation or cleanup.

```arkoi
TITLE @const string_view = "Arkoi"
```

A `string_view` constant is valid only when all of its backing storage derives
from string literals. Copying such a constant copies the view value; the
constant declaration itself remains addressless.

`@mut string_view` makes the view binding reassignable; it does not make the
viewed UTF-8 bytes mutable.

#### Converting Between Text Types

The built-in `string_view(...)` conversion creates a zero-copy view over an
initialized `string` place.

```arkoi
owned @string = load_text()!
view @string_view = string_view(owned)
```

The view borrows the string's storage. It cannot outlive the string, and safe
code cannot move, replace, or drop that string while the view remains live.
The conversion is infallible and does not copy encoded bytes.

The built-in `string(...)` conversion copies a view's bytes into an independent
owned string.

```arkoi
owned_copy @string = string(TITLE)!
```

This conversion may allocate and therefore has the `CoreFail.out_of_memory`
effect. The resulting string has no lifetime dependency on the source view.
There is no implicit conversion in either direction.

Read-only text parameters normally accept `string_view` by value. A caller with
an owned string passes `string_view(owned)` explicitly. An operation that takes
ownership accepts `@own string` under the ordinary resource rules.

#### Text Length and Bytes

The infallible built-ins `length(...)` and `bytes(...)` accept both text types.
`length(...)` returns the number of UTF-8 bytes, not the number of Unicode scalar
values or user-perceived characters. `bytes(...)` returns a zero-copy,
read-only `[]u8` over the same encoded storage.

```arkoi
text @const string_view = "Hé"
byte_count @usize = length(text)       # 3
encoded @[]u8 = bytes(text)            # [0x48, 0xC3, 0xA9]
```

The byte slice inherits the source view's lifetime. For an owned string, it
borrows that string under the same rules as `string_view(...)`. Safe code cannot
obtain mutable byte access because arbitrary mutation could violate UTF-8.

Neither text type supports direct indexing.

```arkoi
value = text[index]  # Compile-time error
```

---

### 3.6 Optional Values

Optional types use `?T`. The absent value is `none`.

```arkoi
profile @?Profile = find_profile()
```

Optionals are distinct from nullable raw pointers and are the ordinary representation for optional data.

Optional values are narrowed only through explicit optional operations.

#### Safe member access

```arkoi
name @?string_view = profile?.display_name
```

#### Defaulting

```arkoi
name @string_view = optional_name ?? "anonymous"
```

The fallback expression is evaluated lazily.

#### Absence propagation

```arkoi
value?
```

Postfix `?` unwraps an optional value or returns `none` from the enclosing function. It is valid only when the function's successful return type is optional.

#### Absence-to-failure conversion

```arkoi
value!
```

Postfix `!` on an optional unwraps it or propagates `CoreFail.none_access`. The enclosing function must permit that failure.

The two operators have deliberately different meanings:

- `?` preserves absence as `none`.
- `!` converts absence into recoverable failure.

---

### 3.7 Enumerations

Arkoi supports flat, payload-free enumerations.

```arkoi
pub enum Status:
    ready
    waiting
    failed
```

Enumeration values are qualified by their declaring type.

```arkoi
status @Status = Status.ready
```

Enums are data types and copy normally.

```arkoi
first @Status = Status.ready
second @Status = first
```

Enum members may declare explicit integer discriminants.

```arkoi
enum HttpMethod:
    get = 1
    post = 2
    delete = 4
```

Discriminant rules:

- Discriminants are compile-time integer constant expressions.
- Discriminant values must be unique within the enum.
- The first member without an explicit value receives `0`.
- A later implicit member receives the previous member's discriminant plus one.
- A discriminant must be representable by Arkoi's compile-time integer model.
- Explicit discriminants define logical numeric values but do not determine storage size or ABI representation.

Enum visibility applies to the enum as a whole.

```arkoi
pub enum Status:
    ready
    waiting
    failed
```

When the enum type is accessible, all of its members are accessible through qualified names such as `Status.ready`.

Individual enum members cannot be declared `pub` or private separately.

A private enum and all of its members remain inaccessible outside the enum's visibility boundary.

```arkoi
enum Code:
    first = 5
    second      # 6
    third = 10
    fourth      # 11
```

Arkoi enum rules:

- Enum members do not carry payloads.
- Members must be accessed using the enum type name.
- Equality and inequality comparisons are supported.
- Relational comparisons (`<`, `<=`, `>`, and `>=`) are not supported directly on enums.
- Enums do not implicitly convert to or from integer types.
- Exact storage size and ABI representation are unspecified.

Explicit conversion may expose or validate discriminants.

```arkoi
number @u32 = convert(HttpMethod.get, u32)!

method @HttpMethod = convert(number, HttpMethod)!
```

Converting an integer to an enum produces `CoreFail.invalid_conversion` when no member has that discriminant. Numeric range failure during enum-to-integer conversion produces `CoreFail.out_of_range`.

Enum ordering requires explicit conversion to a numeric type.

```arkoi
left @s32 = convert(first, s32)!
right @s32 = convert(second, s32)!

if left < right:
    # ...
```

This makes numeric ordering an explicit interpretation rather than an inherent enum operation.

```arkoi
if status == Status.failed:
    report_failure()
elif status == Status.waiting:
    wait()
else:
    start()
```

### 3.8 Type Aliases

Arkoi supports transparent type aliases.

```arkoi
type UserId = u64
type ByteView = []u8
```

A type alias is another name for exactly the same type.

```arkoi
id @UserId = 42
raw @u64 = id
```

No conversion is required because `UserId` and `u64` are identical types.

Type aliases are private by default. A public alias uses `pub type`.

```arkoi
pub type FileDescriptor = u64
```

An alias may refer to data types, resource types, arrays, slices, optionals, references, and raw pointers.

```arkoi
type OptionalFile = ?File
type Header = [16]u8
type FileView = &File
```

Aliases:

- Do not create a new runtime representation.
- Do not create a distinct type identity.
- Do not change copying, ownership, cleanup, cloning, visibility, or failure behavior.
- Cannot define hooks.
- Cannot be directly or indirectly recursive.

### 3.9 Empty Aggregates

Arkoi permits named `data` and `resource` aggregates with no fields.

```arkoi
data Marker:
    pass

resource HandleToken:
    pass
```

Because empty blocks are otherwise invalid, an empty aggregate body must contain `pass`.

An empty aggregate:

- Is still a distinct named type.
- Retains its declared `data` or `resource` category.
- May have external methods.
- May have associated functions.
- May define permitted reserved hooks.
- May implement interfaces.
- May be used in optionals, arrays, references, pointers, and other otherwise-valid type expressions.
- May participate in overload resolution as its own exact type.

```arkoi
data StartSignal:
    pass

implements Displayable for StartSignal

fun StartSignal.display(
    self @&StartSignal,
) @string:
    return "start"
```

An empty `data` aggregate remains freely copyable because it contains no resource fields.

An empty `resource` aggregate remains ownership-controlled despite containing no fields. It is not implicitly copied and follows ordinary move, cleanup, and resource-hook rules.

```arkoi
resource CapabilityToken:
    pass
```

Such a type may represent nominal identity, an external capability, a synchronization token, or another concept whose meaning does not require Arkoi-visible fields.

Allowing the type does not by itself guarantee zero size or any particular binary layout.

### 3.10 Enumeration Membership

Every enum declaration must contain at least one enum member.

```arkoi
enum Status:
    ready
```

An empty enum is invalid.

```arkoi
enum Never:
    pass  # Compile-time error
```

Although `pass` may make other intentionally empty blocks syntactically nonempty, it does not satisfy an enum's semantic requirement to declare at least one member.

The following are therefore invalid:

```arkoi
enum Empty:
    pass
```

```arkoi
enum Empty:
    # comment only
```

Comments do not count as members, and `pass` is not an enum member.

An enum may contain one member.

```arkoi
enum Presence:
    present
```

A concept requiring nominal identity but no enumerable cases should use an empty aggregate or marker interface instead.

```arkoi
data Marker:
    pass
```

```arkoi
interface MarkerCapability:
    pass
```

## 4. Functions and Calls

### 4.1 Functions and Parameters

Functions are declared with `fun`.

```arkoi
fun add(left @u32, right @u32) @u32:
    return left + right
```

The return type follows the parameter list.

#### Parameter binding mutability

Parameters are immutable local bindings by default.

```arkoi
fun process(value @u32):
    # value cannot be reassigned.
```

A mutable local copy uses `@mut`.

```arkoi
fun increment(value @mut u32) @u32:
    value = value + 1
    return value
```

This changes only the local parameter binding. Caller-visible mutation requires a mutable reference.

```arkoi
fun increment(value @&mut u32):
    value = value + 1
```

---

### 4.2 Function Return Forms

A function without a declared return type may complete by reaching the end of its body or may exit early using a bare `return`.

```arkoi
fun log_value(value @u32):
    if value == 0:
        return

    print(value)
```

Returning a value from a function without a declared return type is a compile-time error.

A function with a declared return type must use `return value` when returning normally.

```arkoi
fun calculate() @u32:
    return 42
```

A bare `return` is invalid in a function with a declared return type.

```arkoi
fun calculate() @u32:
    return  # Compile-time error
```

### 4.3 Complete Return-Path Checking

A function with a declared return type must not reach the end of its body on any reachable control-flow path.

Every reachable path must do one of the following:

- Return a value compatible with the declared return type.
- Leave through `fail` or failure propagation.
- Enter a loop that cannot complete normally.
- Terminate through an explicit unrecoverable operation.

```arkoi
fun classify(value @s32) @u32:
    if value < 0:
        return 0
    else:
        return 1
```

The compiler rejects a function when any reachable path may complete without producing a value.

```arkoi
fun classify(value @s32) @u32:
    if value < 0:
        return 0

    # Compile-time error: this path reaches the end.
```

A failure path does not need to return a value.

```arkoi
fun load(id @u32) !LoadFail @Item:
    if id == 0:
        fail LoadFail.invalid_id

    return find_item(id)!
```

A path that enters a loop with no reachable normal exit also satisfies the rule because it never reaches the function end.

```arkoi
fun run_forever() @u32:
    loop:
        process_events()
```

### 4.4 Function, Method, and Hook Overloading

Arkoi supports compile-time overloading for functions, methods, associated functions, and reserved hooks.

Multiple declarations may share the same complete name when their overload identities differ.

```arkoi
fun parse(value @u32) @Node:
    # ...

fun parse(value @string_view) @Node:
    # ...
```

Calls select an overload from the statically known argument types and, when
needed, the call's explicit fallibility syntax.

```arkoi
number @u32 = 10
text @const string_view = "10"

number_node @Node = parse(number)
text_node @Node = parse(text)
```

The overload identity consists of:

- The complete qualified function name.
- The number of parameters.
- The ordered parameter types.
- Parameter ownership and reference mutability modes.
- The fallibility mode: no failure effect or a declared failure effect.

For methods, the receiver parameter is part of the overload identity.

The following do not distinguish overloads:

- Parameter names.
- Return type.
- The exact failure-set type within the fallible mode.
- Visibility.
- Function-body behavior.

Therefore, these declarations conflict because their parameter signatures are identical:

```arkoi
fun convert_value(value @u32) @u64:
    # ...

fun convert_value(value @u32) @f64:
    # Compile-time error: return type does not distinguish an overload
```

One infallible and one fallible definition may share the same parameter
signature:

```arkoi
fun load(path @string_view) @File:
    # ...

fun load(path @string_view) !IOFail @File:
    # ...

file @File = load(path)
fallible_file @File = load(path)!
```

Two fallible definitions with the same parameter signature conflict even when
their declared failure sets differ:

```arkoi
fun load(path @string_view) !IOFail @File:
    # ...

fun load(path @string_view) !ParseFail @File:
    # Compile-time error: exact failure-set types do not distinguish overloads.
```

Overload resolution follows these steps:

1. Resolve the complete function or member name.
2. Collect only declarations visible at the call site.
3. Match positional and named arguments to each candidate's parameters.
4. Reject candidates with incompatible argument counts, argument names, or argument types.
5. When both fallibility modes remain, select the fallible mode for a call
   directly followed by postfix `!` or `handle failure`; otherwise select the
   infallible mode.
6. Select the single remaining candidate.
7. Report a compile-time error if no candidate or more than one candidate remains.

Fallibility selection is local to the call expression. The enclosing
function's failure effect, an expected return type, or a handler around a larger
containing expression does not select an overload.

When both modes exist, a directly attached postfix `!` selects the fallible
overload and consumes its failure layer before any optional layer. For a
fallible overload returning `?T`, one `!` produces `?T` and two produce `T`.
Calling the infallible overload and unwrapping its optional result requires the
unmarked result to be stored first.

```arkoi
maybe @?Item = find(key)    # Infallible overload.
item @Item = maybe!         # Unwraps the optional.

fallible_maybe @?Item = find(key)!  # Fallible overload; propagates failure.
fallible_item @Item = find(key)!!   # Also unwraps the optional.
```

If argument matching leaves only one fallibility mode, that overload is
selected and its result must still satisfy the ordinary propagation, handling,
or optional-unwrapping rules.

Parameter names participate in matching named call arguments, but do not make otherwise identical declarations distinct.

```arkoi
fun resize(width @u32, height @u32):
    # ...

fun resize(size @Size):
    # ...

resize(width = 100, height = 80)
resize(size = requested_size)
```

Arkoi does not apply implicit numeric conversions merely to make an overload applicable. An explicit conversion may be used to select the intended overload.

```arkoi
fun process(value @u32):
    # ...

fun process(value @u64):
    # ...

small @u16 = 10

process(small)                 # Compile-time error: no exact overload
process(convert(small, u32)!)  # Selects process(u32)
```

Context-dependent literals may make a call ambiguous.

```arkoi
fun store(value @u32):
    # ...

fun store(value @u64):
    # ...

store(10)  # Compile-time error: the literal fits more than one overload
```

The caller resolves the ambiguity by providing a typed value or explicit conversion.

```arkoi
value @u32 = 10
store(value)
```

Arkoi has no preferred-type ranking, conversion-cost ranking, or “best match” rule.

Transparent type aliases preserve type identity and therefore do not create distinct overload signatures.

```arkoi
type UserId = u64

fun find(value @u64):
    # ...

fun find(value @UserId):
    # Compile-time error: UserId and u64 are the same type
```

Methods and associated functions use the same rules.

```arkoi
fun Vector.scale(self @&Vector, scalar @f32) @Vector:
    # ...

fun Vector.scale(self @&Vector, scalar @f64) @Vector:
    # ...
```

Reserved operator hooks also use this overload mechanism. A type may define
several hooks in one operator family when their overload identities differ.

```arkoi
fun Vector.__mul__(self @&Vector, scalar @f64) @Vector:
    # ...

fun Vector.__mul__(self @&Vector, other @&Vector) @f64:
    # ...
```

Operator syntax first chooses the normal or reverse hook family according to the operator-resolution rules, then applies the same exact overload selection described here.

For a compiler-generated hook call, the corresponding language operation
defines how its infallible or fallible spelling selects the hook's fallibility
mode. Hook-specific contracts may restrict a hook to one mode.

Overload sets remain compile-time name-resolution structures rather than values. A specific overload may be selected and converted into a function pointer only through the explicit `address(...)` operation with sufficient function-pointer type context.

### 4.5 Named Function Arguments

Function calls may use positional arguments, named arguments, or a mixture of both.

```arkoi
fun copy_file(
    source @string_view,
    destination @string_view,
    overwrite @bool,
) !IOFail:
    # ...
```

A call may be entirely positional:

```arkoi
copy_file(source, destination, true)!
```

A call may use named arguments:

```arkoi
copy_file(
    source = source,
    destination = destination,
    overwrite = true,
)!
```

When positional and named arguments are mixed, all positional arguments must appear before the first named argument.

```arkoi
copy_file(
    source,
    destination = destination,
    overwrite = true,
)!
```

A positional argument after a named argument is a compile-time error.

Named arguments follow the same ownership and borrowing rules as positional arguments.

```arkoi
store(
    file = move(file),
)
```

Unknown argument names, duplicate arguments, missing required arguments, and extra arguments are compile-time errors.

Every declared parameter must be supplied exactly once at a call site.

### 4.6 Parameter Names and API Stability

Parameter names are part of a function's source-level API because callers may use named arguments.

```arkoi
copy_file(
    source = source,
    destination = destination,
    overwrite = true,
)!
```

Named arguments must use the parameter names declared by the function.

Renaming a parameter of a public function is therefore a breaking source-level change for callers that use named arguments.

Parameter names are not part of:

- The function's runtime type.
- ABI identity.
- Calling convention.
- Function-value compatibility.

Functions with the same parameter and return types remain type-compatible regardless of parameter names.

Private function parameters may be renamed when all affected call sites are updated.

### 4.7 Function Types and Function Pointers

Arkoi supports named function types and raw pointers to named functions.

A function type reuses the `fun` keyword.

Function-type parameter types are unnamed and always use Arkoi's `@Type` notation.

```arkoi
type Operation = fun(
    @s32,
    @s32,
) @s32
```

`Operation` is the function signature type itself. It is not an address and cannot exist as a stored value by itself.

A pointer to a function of that type is written with Arkoi's ordinary raw-pointer syntax:

```arkoi
*Operation
```

For example:

```arkoi
operation @*Operation = address(add)
```

A function type may include the same signature properties as an ordinary Arkoi function where applicable:

```arkoi
type Loader = fun(
    @string_view,
) !LoadFail @File

type RawOperation = unsafe fun(
    @*mut u8,
)
```

Parameter names never appear in a function type.

```arkoi
type Invalid = fun(
    value @s32,
) @s32
# Compile-time error
```

A function type:

- Describes a callable signature.
- Cannot be constructed as a value.
- Cannot be used as a binding type by value.
- Cannot be stored as an aggregate field by value.
- Cannot be passed or returned by value.
- May be used as the referent of a raw pointer.
- May be named through a transparent type alias.

The mutable raw-pointer form is invalid for function types because executable functions are not writable objects.

```arkoi
operation @*mut Operation
# Compile-time error
```

A compatible named function is converted to a raw function pointer explicitly with `address(...)`.

```arkoi
fun add(
    left @s32,
    right @s32,
) @s32:
    return left + right

operation @*Operation = address(add)
```

`address(add)` produces a non-null raw pointer value whose type is `*Operation`, but the raw-pointer type itself remains nullable under Arkoi's ordinary pointer rules.

The expected pointed-to function type selects the exact overload and verifies:

- Parameter count.
- Ordered parameter types.
- Ownership and reference-mutability modes.
- Return type.
- Failure effect.
- Safety.
- ABI.

If the expected type does not identify exactly one compatible overload, `address(...)` is a compile-time error.

A raw function pointer is invoked with the explicit `call(...)` operation.

```arkoi
unsafe:
    result @s32 = call(
        operation,
        10,
        20,
    )
```

Every indirect call through a raw function pointer requires an unsafe context because the pointer may be null, invalid, or obtained through foreign or unsafe operations.

Calling a null or invalid function pointer causes undefined behavior.

Indirect calls accept positional arguments only because function types contain no parameter names.

A fallible native function pointer uses ordinary failure handling and propagation inside the required unsafe context.

```arkoi
unsafe:
    file @File = call(
        loader,
        path,
    )!
```

An unsafe function type retains its safety requirement. Calling a pointer to an unsafe function also requires an unsafe context, as does every other indirect function-pointer call.

Raw pointers to function types are data values. They may be:

- Stored in bindings.
- Stored in data aggregates when otherwise permitted.
- Passed as arguments.
- Returned from functions.
- Copied.
- Compared for address identity with `==` and `!=`.
- Compared with `null`.

Function-pointer values refer only to named functions and carry no captured environment. A function name becomes a pointer only through an explicit `address(...)` operation with sufficient function-pointer type context. Methods must be represented by a separately chosen function pointer and explicit receiver argument rather than as bound-method values.

Ordinary direct calls continue to use normal function-call syntax.

```arkoi
result @s32 = add(10, 20)
```

Indirect calls remain explicit through `call(...)`.

C ABI function types use `import "C" type`.

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int
```

The `import "C"` modifier applies the C ABI to the declared function type. The ABI is not written inside the `fun(...)` signature.

`CompareCallback` is the C function signature type. A C function pointer using that signature has type:

```arkoi
*CompareCallback
```

Because `CompareCallback` belongs to the C ABI, `*CompareCallback` is also a C ABI pointer type.

A C ABI function type:

- Uses the target C calling convention.
- Uses C function representation and call rules.
- May contain only C-compatible parameter and return types.
- Cannot declare an Arkoi failure effect.
- Uses positional arguments only.

A compatible C-linked named function may be addressed when its signature matches exactly.

```arkoi
export "C" fun compare_values(
    left @*c.void,
    right @*c.void,
) @c.int:
    # ...

compare @*CompareCallback = address(compare_values)
```

Calling a C ABI function pointer requires an unsafe context under the general raw-function-pointer rule.

```arkoi
unsafe:
    result @c.int = call(
        compare,
        left,
        right,
    )
```

Function pointers introduce indirect calls, but they do not introduce dynamic dispatch, interface objects, closures, or runtime overload lookup.

### 4.8 Recursive Functions

Arkoi permits direct and mutual recursion.

Direct recursion:

```arkoi
fun factorial(value @u32) @u32:
    if value == 0:
        return 1

    return value * factorial(value - 1)
```

Mutual recursion:

```arkoi
fun is_even(value @u32) @bool:
    if value == 0:
        return true

    return is_odd(value - 1)

fun is_odd(value @u32) @bool:
    if value == 0:
        return false

    return is_even(value - 1)
```

Module-level declaration order does not restrict recursive call relationships.

Arkoi does not guarantee tail-call optimization.

Excessive recursion may exhaust the call stack and cause an unrecoverable trap.

Resource cleanup follows the ordinary lexical cleanup rules whenever a recursive call frame exits.

### 4.9 Function Parameter Arity

Every Arkoi-defined function has a fixed parameter list.

```arkoi
fun log(values @...):  # Compile-time error
```

Every ordinary Arkoi function has a fixed, statically typed parameter list.

Functions that need to process a variable number of homogeneous values use arrays or slices.

```arkoi
fun print_values(values @[]u32):
    for value @u32 in values:
        print_u32(value)
```

Heterogeneous variadic parameters are invalid for Arkoi-defined functions. Imported C functions may use a C variadic tail under the rules in the C interoperability chapter.

### 4.10 Program Entry Point

An executable Arkoi program defines exactly one module-level `main` function.

```arkoi
fun main():
    # ...
```

The entry point:

- Is named exactly `main`.
- Is declared at module scope.
- Is not a method or associated function.
- Has no parameters.
- Has no return value.
- Does not use `pub`; the compiler discovers it as the executable entry point.
- May declare a failure effect.

```arkoi
fun main() !AppFail:
    run_application()!
```

When a failure propagates out of `main`, the runtime terminates the program unsuccessfully.

## 5. Ownership, Aggregates, and Resources

### 5.1 Ownership Transfer and Cloning

Arkoi requires ownership-changing operations to be explicit at the point of use.

#### Move

```arkoi
second @string = move(first)
```

`move(value)` transfers ownership without allocation. The source binding becomes uninitialized.

`move(...)` is valid only for resource values. Data values—including numbers, booleans, characters, `string_view`, references, raw pointers, slices, failures, and data aggregates—are copied normally and cannot be passed to `move(...)`.

```arkoi
second @u32 = first
view_copy @[]u32 = view

invalid @u32 = move(first)      # Compile-time error.
invalid_view @[]u32 = move(view) # Compile-time error.
```

Using a moved resource binding before reinitialization is a compile-time error.

A mutable resource binding may be reinitialized after it has been moved:

```arkoi
file @mut File = File.open(first_path)!
consume(move(file))

# file is uninitialized here.
file = File.open(second_path)!
```

Because no old value remains after the move, this assignment performs initialization rather than drop-and-replace. The newly initialized value becomes eligible for automatic cleanup again.

An immutable resource binding cannot be reinitialized after it has been moved. It was already initialized once, so another assignment would violate immutable-binding semantics.

```arkoi
file @File = File.open(first_path)!
consume(move(file))

file = File.open(second_path)!  # Compile-time error.
```

Definite-initialization analysis tracks mutable resource bindings across moves and reinitializations. A resource may be used or dropped only on paths where it is definitely initialized.

`move(...)` accepts only a whole named resource binding, including an ownership-taking resource parameter.

```arkoi
file @File = File.open(path)!
other @File = move(file)

fun forward(file @own File) @File:
    return move(file)
```

Moving from a field or indexed element is invalid because it would leave the containing aggregate or array partially initialized.

```arkoi
other @File = move(user.file)  # Compile-time error.
first @File = move(files[0])   # Compile-time error.
```

A temporary resource enters an owning context directly and must not be wrapped in `move(...)`.

```arkoi
consume(File.open(path)!)
return File.open(path)!

consume(move(File.open(path)!))  # Compile-time error: redundant move.
```

#### Clone

```arkoi
backup @string = clone(original)!
```

`clone(value)` creates an independent resource by invoking the type's clone hook. The hook may be infallible or may declare a failure effect. Postfix `!` is required only when the selected clone operation is fallible and the caller chooses to propagate that failure.

#### Passing resources by value

An ownership-taking resource parameter uses `@own`.

```arkoi
fun consume(file @own File):
    # This function owns file.
```

The caller must explicitly transfer ownership.

```arkoi
consume(move(file))
```

Passing a named resource directly is invalid.

```arkoi
consume(file)  # Compile-time error.
```

A plain resource parameter without an ownership mode is also invalid.

```arkoi
fun consume(file @File):  # Compile-time error.
```

Data parameters are copied normally and do not use `own`.

```arkoi
fun process(count @u32):
    # count is copied.
```

#### Temporaries

A temporary resource that has no reusable source binding may initialize or enter an owning parameter directly.

```arkoi
name @string = load_name()!
consume(load_file()!)
```

#### Returning owned resources

Returning a named resource binding transfers ownership and therefore requires an explicit `move(...)`.

```arkoi
fun create_file() !IOFail @File:
    file @File = File.open(path)!
    return move(file)
```

Returning the named binding directly is invalid.

```arkoi
return file  # Compile-time error: named resource return requires move(...).
```

A resource received through an ownership-taking parameter follows the same rule.

```arkoi
fun forward(file @own File) @File:
    return move(file)
```

A temporary resource expression may be returned directly because there is no reusable source binding.

```arkoi
fun create_file() !IOFail @File:
    return File.open(path)!
```

Data values and non-owning references are returned normally.

#### Clone hooks

A resource is cloneable only when it defines `__clone__`. The resource author decides whether cloning is infallible or exposes a recoverable failure effect.

An infallible clone hook declares no failure effect:

```arkoi
fun SharedHandle.__clone__(self @&SharedHandle) @SharedHandle:
    # Return an independent SharedHandle.
```

It is called without postfix `!`:

```arkoi
copy @SharedHandle = clone(original)
```

A fallible clone hook declares its failure effect normally:

```arkoi
fun File.__clone__(self @&File) !IOFail @File:
    # Return an independent File.
```

The caller may propagate or handle that failure:

```arkoi
copy @File = clone(original)!

copy @File = clone(original) handle failure:
    log_failure(failure)
    yield File.open(fallback_path)!
```

Every resource is movable. Only resources with a valid clone hook are cloneable. The compiler does not assume that cloning is fallible merely because the value is a resource.

`clone(...)` is valid only for resource values. Data values are copied through ordinary assignment, parameter passing, and return. Applying `clone(...)` to a data value is a compile-time error:

```arkoi
second @u32 = clone(first)       # Compile-time error.
point_copy @Point = clone(point) # Compile-time error when Point is data.
```

#### Clone source expressions

Unlike `move(...)`, cloning does not remove or uninitialize its source. `clone(...)` may therefore read from any non-temporary resource place that is accessible to the expression.

Valid clone sources include whole named bindings, resource fields, and resource array or slice elements:

```arkoi
binding_copy @File = clone(file)!
field_copy @File = clone(user.file)!
element_copy @File = clone(files[0])!
dynamic_copy @File = clone(files[index]!)!
```

The original resource remains initialized and owned by its existing binding or containing aggregate. Any bounds check needed to locate an indexed place occurs before its clone hook is called.

Cloning a temporary resource expression is rejected as redundant:

```arkoi
copy @File = clone(File.open(path)!)!  # Compile-time error.
```

The temporary can enter the destination owning context directly:

```arkoi
copy @File = File.open(path)!
```

Therefore:

- `move(...)` accepts only whole named resource bindings.
- `clone(...)` accepts any readable, non-temporary resource place whose type is cloneable.
- Neither operation accepts data values.

#### Composite cloning

Built-in composite types derive resource cloning from their contained type. A composite whose contained values are data remains a data type and is copied normally; `clone(...)` is rejected for it.

An optional `?T` that is a resource because `T` is a resource is cloneable when `T` is cloneable:

```arkoi
copied_optional @?File = clone(optional)!
```

- `none` clones to `none`.
- A present value is cloned using the contained resource's `__clone__` hook.

A fixed-size array `[N]T` that is a resource because `T` is a resource is cloneable when `T` is cloneable:

```arkoi
copied_files @[2]File = clone(files)!
```

Array elements are cloned from first to last. If cloning an element fails, every element already cloned for the new array is dropped in reverse order before the failure propagates. The original array remains unchanged.

Composite cloning exposes exactly the clone failure effect of the contained type. Arkoi does not introduce wrapper failures such as `ArrayCloneFail` or `OptionalCloneFail`.

```arkoi
fun File.__clone__(self @&File) !IOFail @File:
    # ...

optional_copy @?File = clone(optional)!
array_copy @[4]File = clone(files)!
```

Both composite clone expressions have the `IOFail` effect inherited from `File`. Nested built-in composites preserve the same effect transitively. Data-only composites remain infallibly copyable through ordinary value semantics, and using `clone(...)` on them is a compile-time error.

User-declared resource aggregates do not receive an automatically synthesized clone operation. They remain cloneable only when their author defines an explicit `__clone__` hook, allowing the type to choose its own cloning semantics.

Slices are non-owning data descriptors. Copying or passing a slice copies only its pointer-and-length descriptor and never clones the underlying elements. Because slices are data values, `clone(slice)` is a compile-time error.

#### Whole-Value Moves

Arkoi permits moving entire resource bindings but not moving individual fields out of an existing resource aggregate.

```arkoi
name @string = move(user.name)  # Compile-time error.
```

Moving an entire aggregate is valid.

```arkoi
backup @User = move(user)
```

Moving resource bindings into a newly constructed aggregate is also valid.

```arkoi
user @User = User(
    name = move(name),
    file = move(file),
)
```

This keeps resource aggregates either fully initialized or fully uninitialized.

#### Taking optional values

The built-in `take(...)` operation applies to mutable optional places of type `?T`, regardless of whether `T` is data or resource. It returns the previous optional value and replaces the source with `none`.

```arkoi
current @mut ?File = File.open(path)!
taken @?File = take(&mut current)
```

After the operation, `current` contains `none`. When `T` is a resource, ownership of the contained resource transfers into the returned optional. When `T` is data, the previous data value is returned normally.

The result can be required to contain a value by using postfix `!`:

```arkoi
file @File = take(&mut current)!
```

If the optional is `none`, postfix `!` propagates `CoreFail.none_access`. The `take(...)` operation itself is infallible once its mutable place has been resolved.

`take(...)` is equivalent in meaning to replacing the optional with `none`:

```arkoi
taken @?T = replace(&mut optional, none)
```

#### Replacing initialized values

The built-in `replace(...)` operation swaps the value stored in a mutable place and returns the previous value. It works for both data and resource types.

```arkoi
old_count @u32 = replace(&mut count, 42)
old_file @File = replace(&mut file, File.open(new_path)!)
```

A named replacement resource still requires explicit ownership transfer:

```arkoi
old_file @File = replace(&mut file, move(replacement))
```

`replace(...)` may target mutable local bindings, mutable fields, mutable array or slice elements, or other addressable mutable places. It never leaves the destination uninitialized. The replacement value is fully evaluated before the old value is removed. If construction of the replacement fails, the original value remains unchanged.

For resources, the returned old value is owned by the caller. If that returned resource is not stored or moved elsewhere, it is cleaned up as a temporary under the normal deterministic-cleanup rules.

Directly moving an individual resource field or array element remains prohibited. `replace(...)` is permitted because it installs a new initialized value while extracting the old one, so no partially initialized aggregate or array is observable.

#### Swapping initialized values

The built-in `swap(...)` operation exchanges the values stored in two mutable, fully initialized places. It is infallible and works for both data and resource types.

```arkoi
swap(&mut first, &mut second)
swap(&mut files[0], &mut files[1])
```

Both places must have the same type and must permit mutation. `swap(...)` may target mutable local bindings, mutable fields, mutable array or slice elements, or other addressable mutable places.

For resources, ownership of the two values is exchanged without cloning, dropping, or leaving either place uninitialized. No `move(...)` marker is required because neither value leaves the two supplied places.

```arkoi
first @mut File = File.open(first_path)!
second @mut File = File.open(second_path)!

swap(&mut first, &mut second)
```

`swap(...)` performs no allocation and declares no failure effect.

If both arguments identify the exact same typed place, `swap(...)` is valid and has no effect:

```arkoi
swap(&mut value, &mut value)
swap(&mut items[0], &mut items[0])
```

Distinct places may be swapped normally, including places reached through aliasing mutable references or slices. If unsafe operations create two differently described places that partially overlap without being the exact same typed place, passing them to `swap(...)` is invalid unsafe behavior.

---

### 5.2 Aggregate Construction

Aggregate construction accepts positional arguments, named arguments, or
positional arguments followed by named arguments.

```arkoi
data Point:
    x @f32
    y @f32

positional @Point = Point(10.0, 20.0)

mixed @Point = Point(
    10.0,
    y = 20.0,
)

named @Point = Point(
    y = 20.0,
    x = 10.0,
)
```

Construction rules:

- Positional arguments initialize fields in declaration order.
- After the first named argument, every remaining argument must be named.
- A named argument may initialize any remaining accessible field.
- Every field must be initialized exactly once.
- Missing fields are errors.
- Unknown fields are errors.
- Duplicate fields are errors.
- Field values must match declared types.
- Argument expressions are evaluated from left to right in written order.
- Declaration order is part of the source API for positional construction.

```arkoi
duplicate @Point = Point(10.0, x = 20.0)  # Compile-time error.
invalid @Point = Point(x = 10.0, 20.0)    # Compile-time error.
```

#### Resource fields

Named resource bindings require explicit ownership transfer or cloning.

```arkoi
user @User = User(
    name = move(name),
    file = move(file),
)
```

Temporary resources may initialize fields directly.

```arkoi
user @User = User(
    name = load_name()!,
    file = File.open(path)!,
)
```

Partially initialized aggregate values are never observable.

#### Aggregate Field Access

Aggregate fields are accessed individually, subject to normal visibility, mutability, and ownership rules. Aggregate destructuring syntax is a compile-time error.

```arkoi
point @Point = get_point()

x @f32 = point.x
y @f32 = point.y
```

Resource aggregates cannot be destructured to move out individual resource fields. This preserves the whole-value-move rule and prevents partially moved aggregates.

---

### 5.3 Resource Cleanup

Resource cleanup is deterministic and follows lexical scope.

A resource may define an infallible `__drop__` hook.

```arkoi
fun File.__drop__(self @&mut File):
    close_handle(self.handle)
```

A `__drop__` hook cannot declare a failure effect, propagate a failure with postfix `!`, or allow `fail` to leave the hook. It may call fallible operations only when every possible failure is handled internally.

Automatic cleanup therefore cannot change ordinary control flow or replace a failure that is already propagating. Unrecoverable traps remain possible under the language's normal trap rules.

Operations whose cleanup result matters must be exposed as explicit fallible functions or methods rather than relying on `__drop__`.

```arkoi
pub fun File.close(self @&mut File) !IOFail:
    close_native_file(self.handle)!
```

An initialized resource is cleaned up when its lexical lifetime ends, including through:

- Normal block completion.
- `return`.
- `fail`.
- Postfix `!` propagation.
- Postfix `?` propagation.
- `break`.
- `continue`.

A moved or otherwise uninitialized binding is not dropped. If it is later reinitialized, it becomes droppable again.

#### Cleanup order

Local resources are dropped in reverse order of successful initialization.

For resource fields:

1. The aggregate's custom `__drop__` hook runs first.
2. Initialized resource fields are dropped in reverse declaration order.

For fixed-size arrays containing resources, initialized elements are dropped in reverse index order. For an array `[N]T`, cleanup proceeds from index `N - 1` down to index `0`. Nested arrays apply this rule recursively. The same reverse-order rule is used when cleaning up an array that was only partially constructed before a failure.

The language must prevent accidental double cleanup of fields and array elements.

Data values, references, and raw pointers do not clean up referenced or pointed-to resources.

---

### 5.4 Resource Construction

Resource construction uses ordinary associated functions rather than an initialization hook.

Constructors are ordinary associated functions that return fully initialized values.

```arkoi
pub fun File.open(path @string_view) !IOFail @File:
    # Construct and return a complete File.
```

A type may define multiple named constructors.

Constructor-local resources are automatically cleaned up if construction fails. Resource locals that become fields must be explicitly moved into the returned aggregate.

No partially initialized resource is exposed to calling code.

---

### 5.5 Resource Reassignment

Mutable resource bindings use construct-first, then drop-and-replace semantics.

```arkoi
file @mut File = File.open(first_path)!
file = File.open(second_path)!
```

Replacement occurs in this order:

1. Fully evaluate and construct the right-hand side.
2. If construction fails, preserve the existing destination unchanged.
3. After successful construction, drop the old destination value.
4. Install the new value.

This prevents an observably partially initialized destination.

#### Replacement from a moved value

```arkoi
first = move(second)
```

The conceptual order is:

1. Prepare the move from `second`.
2. Drop the old value in `first`.
3. Install the moved value into `first`.
4. Mark `second` uninitialized.

Self-move assignment is a compile-time error.

```arkoi
file = move(file)  # Compile-time error.
```

---

## 6. References, Pointers, Slices, and Arrays

### 6.1 References and Raw Pointers

#### References

References are non-null, non-owning aliases to stable storage.

```arkoi
&T       # Read-only reference
&mut T   # Mutable reference
```

Arkoi enforces basic mutability and lexical lifetime rules but does not provide a complete Rust-style exclusivity or borrow analysis.

A reference binding is initialized once and remains bound to the same referent.
It may use Arkoi's ordinary delayed initialization, but binding-level `mut` is
not permitted for a reference type. Referent access is determined by the
reference type:

```arkoi
view @&Item = &item
editable @&mut Item = &mut item
```

A reference name transparently designates its referent in value and place
expressions. Reading through `&T` or `&mut T` reads the referent. Assignment
through `&mut T` writes the referent and never changes the reference binding.
Assignment through `&T` is invalid.
Fields, methods, indexing, and operators use the same direct syntax as the
referenced value. References have no dereference operator; unary `*` applies
only to raw pointers.

```arkoi
fun increment(value @&mut u32):
    value = value + 1

fun reset(file @&mut File):
    file.handle = 0
```

An assignment whose reference destination is already initialized always writes
the referent, including when the source is another reference:

```arkoi
destination @&mut u32 = &mut first
source @&u32 = &second
destination = source  # Copies second into first.
```

In a reference-typed initializer, argument, or return context, an existing
reference is copied as a reference to the same referent. Borrowing a reference
name reborrows its referent rather than creating a reference to the reference
binding. The reference binding is not itself an addressable place. The mutable
form requires existing mutable access.

```arkoi
alias @&Item = reference
inspect(reference)
another_alias @&Item = &reference
```

#### Explicit borrowing at ordinary call sites

Arkoi never inserts implicit borrowing for ordinary function arguments.

```arkoi
fun inspect(file @&File):
    # ...

fun modify(file @&mut File):
    # ...

inspect(&file)
modify(&mut file)
```

These forms are invalid:

```arkoi
inspect(file)
modify(file)
```

The target of `&mut` must provide mutable access.

#### Stable addressable values

References may be created only from stable addressable storage, such as:

- Local bindings.
- Parameters.
- Module-level variables.
- Aggregate fields.
- Array elements.
- Allocated storage.

Compile-time constants are addressless values rather than storage. A constant,
or an element or field selected from a constant, cannot be borrowed.

`string_view(owned_string)` and `bytes(owned_string)` create read-only borrows
of the string's storage and participate in the same lifetime and invalidation
checks as other derived views.

Arkoi does not extend the lifetime of temporary values merely because a reference is taken.

```arkoi
reference @&Item = &make_item()  # Compile-time error.
```

#### Limited lifetime checking

Arkoi rejects obvious reference escapes, such as returning a reference to a local binding.

References derived directly from parameters may be returned when the relationship is lexically evident.

Arkoi also prevents an owned value from being moved, replaced, or dropped while an obviously live reference or slice derived from that value may still be used.

```arkoi
file @File = File.open(path)!
reference @&File = &file

consume(move(file))  # Compile-time error: file is still borrowed.
inspect(reference)
```

The operation is valid after the compiler can see that the derived borrow is no longer live:

```arkoi
file @File = File.open(path)!
reference @&File = &file
inspect(reference)

consume(move(file))  # Valid: reference is not used afterward.
```

The same rule applies to construct-first replacement, explicit `replace(...)`, and lexical cleanup. Arkoi uses limited lexical and obvious last-use analysis for these cases; it does not provide a complete Rust-style borrow checker.

Arkoi does not guarantee complete alias or lifetime safety. Complex relationships that cannot be established by the limited analysis require safer restructuring or an explicit unsafe operation where one exists.

#### Raw pointers

Raw pointers are nullable. `*T` provides read-only pointee access, while `*mut T` provides mutable pointee access. Pointer-binding mutability remains independent from pointee mutability.

Safe pointer-value operations include:

- Declaration.
- Copying.
- Storage.
- Passing and returning.
- Equality comparison.
- Comparison with null.

Unsafe pointer operations include:

- Dereferencing.
- Writing through a `*mut T` pointer.
- Pointer arithmetic and pointer indexing.
- Integer-to-pointer conversion.
- Reinterpretation between unrelated pointer types.
- Pointer-to-reference conversion.

Raw pointers must be dereferenced explicitly with unary `*` inside an unsafe context:

```arkoi
pointer @*u32 = get_pointer()
mutable_pointer @*mut u32 = get_mutable_pointer()

unsafe:
    value @u32 = *pointer
    *mutable_pointer = 42
```

Field access through a raw pointer also requires an explicit dereference:

```arkoi
file_pointer @*File = get_file_pointer()
mutable_file_pointer @*mut File = get_mutable_file_pointer()

unsafe:
    handle @u64 = (*file_pointer).handle
    (*mutable_file_pointer).handle = 42
```

Arkoi does not provide implicit raw-pointer dereferencing through ordinary dot access and does not include a separate `->` operator in Arkoi. Values and references may use ordinary dot access, while raw-pointer field access remains visibly unsafe:

```arkoi
reference.field
(*raw_pointer).field
```

#### Function-based pointer arithmetic

Raw-pointer arithmetic uses dedicated functions rather than the numeric `+` and `-` operators.

```arkoi
unsafe:
    next @*Item = offset(pointer, 1)
    previous @*Item = offset(pointer, -1)
```

`offset(pointer, amount)` adjusts the address by a signed number of elements. An offset of `1` advances by the size of the pointed-to type rather than by one byte. The result has the same pointer type as the input.

The distance between two pointers is computed explicitly:

```arkoi
unsafe:
    count @ssize = distance(begin, end)
```

`distance(begin, end)` returns the signed number of elements from `begin` to `end`. Both pointers must have the same pointed-to type.

Pointer indexing is permitted as unsafe shorthand:

```arkoi
unsafe:
    value @Item = pointer[index]
    pointer[index] = replacement
```

It is equivalent to offsetting and then dereferencing:

```arkoi
*offset(pointer, index)
```

Pointer arithmetic, pointer distance, and pointer indexing are valid only in an unsafe context. Arkoi does not support numeric pointer expressions such as `pointer + 1`, `pointer - 1`, or `end - begin`.

#### Object-bounded pointer arithmetic

Raw-pointer arithmetic retains the provenance of the allocated object from which the pointer originated.

`offset(pointer, amount)` may produce a pointer to an element within the same allocated object or a one-past pointer immediately after it.

```arkoi
unsafe:
    end @*Item = offset(begin, length)
```

A one-past pointer may be compared, passed around, or moved back into the allocation, but it may not be dereferenced.

Producing a pointer before the beginning or beyond the one-past position violates the unsafe pointer contract. `distance(begin, end)` is valid only when both pointers refer into the same allocated object, including its one-past position.

Raw pointers do not store allocation bounds. Arkoi may reject obvious violations when provenance and bounds are statically evident, but unsafe code is generally responsible for retaining and respecting the associated length or end pointer. Violating these requirements results in undefined behavior.

Machine-address overflow during `offset` traps.

Code that intentionally manipulates flat machine addresses must make that operation explicit:

```arkoi
unsafe:
    address @usize = pointer_address(pointer)
    adjusted @usize = address + byte_offset
    result @*Item = pointer_from_address(adjusted)
```

Converting through an integer does not preserve ordinary object-bounded guarantees. The programmer is responsible for the resulting pointer's validity, alignment, provenance, lifetime, and accessible range.

A non-null pointer does not prove validity.

#### Slices

Arkoi provides first-class slices as bounded, non-owning views over contiguous elements.

```arkoi
items @[]Item
mutable_items @[]mut Item
```

A slice conceptually contains:

- A pointer to its first element.
- A length measured in elements.

Slices do not own or clean up the underlying storage. Their lifetime must not exceed the storage they reference. Arkoi applies the same limited lexical lifetime checking used for references.

`[]T` provides read-only access to its elements. `[]mut T` provides mutable access to its elements.

```arkoi
fun inspect(items @[]Item):
    value @Item = items[0]!

fun modify(items @[]mut Item):
    items[0]! = replacement
```

Binding mutability and element mutability are independent:

```arkoi
items @mut []Item       # Reassignable slice; read-only elements.
items @[]mut Item       # Fixed slice binding; mutable elements.
items @mut []mut Item   # Reassignable slice; mutable elements.
```

Slices are small, non-owning data values. Passing or assigning a slice copies only its pointer-and-length descriptor; it does not copy or transfer ownership of the underlying elements.

```arkoi
inspect(items)
modify(mutable_items)
```

Slice length is available through the infallible built-in `length(...)` function:

```arkoi
count @usize = length(items)
```

Slice indexing is bounds-checked and recoverable:

```arkoi
value @Item = items[index]!
mutable_items[index]! = replacement
```

An out-of-range index produces `CoreFail.out_of_range`. A program may propagate or handle that failure normally. Writing through a slice additionally requires `[]mut T`.

The unsafe built-in `pointer(slice)` returns a raw pointer to the slice's first
element and preserves its element-access mode:

```arkoi
unsafe:
    pointer @*Item = pointer(items)
```

Constructing a slice from a raw pointer and length is also unsafe because the programmer must guarantee that the range is alive, correctly aligned, contiguous, and contains valid elements.

Mutable slices may alias. Two `[]mut T` values may refer to overlapping or identical memory, and the compiler does not treat mutable slices as exclusive or `noalias`.

```arkoi
first @[]mut Item = mutable_items
second @[]mut Item = mutable_items

first[0]! = a
second[0]! = b  # May modify the same element.
```

`[]mut T` expresses permission to modify elements through the slice. It does not promise unique access to those elements. This matches Arkoi's `&mut T` model and avoids requiring a Rust-style borrow checker.

##### Slice Lifetimes and Global Slices

Slices obey the same limited lexical lifetime rules as references. A slice derived from local storage cannot escape the lifetime of that storage.

```arkoi
fun invalid_view() @[]u32:
    values @[4]u32 = [10, 20, 30, 40]
    return values[..]  # Compile-time error: values is local.
```

A slice derived from a slice parameter may be returned because it cannot outlive the caller-provided storage under Arkoi's lexical checks.

```arkoi
fun prefix(items @[]u32, end @usize) !CoreFail @[]u32:
    return items[..end]!
```

Global slices are permitted when their backing storage has a provable static lifetime.

```arkoi
values @[4]u32 = [10, 20, 30, 40]
view @[]u32 = values[..]

mutable_values @mut [4]u32 = [10, 20, 30, 40]
mutable_view @[]mut u32 = mutable_values[..]
```

Safe code cannot store a slice of local, parameter, temporary, or otherwise shorter-lived storage in a global binding.

```arkoi
view @mut []u32

fun initialize():
    values @[4]u32 = [10, 20, 30, 40]
    view = values[..]  # Compile-time error: values does not have static lifetime.
```

A slice into a movable, replaceable, or droppable global resource is not accepted as statically valid merely because the owner is global. Replacing, moving, or dropping that owner could invalidate the slice. More complex global slices may be constructed in unsafe code when the programmer guarantees the backing storage remains valid.

Arkoi rejects obvious dangling slices but does not perform complete lifetime or alias analysis.

##### Subslices

A subslice is formed with a half-open range. The starting index is included and the ending index is excluded.

```arkoi
middle @[]Item = items[start..end]!
prefix @[]Item = items[..end]!
suffix @[]Item = items[start..]!
all @[]Item = items[..]
```

The bounds must satisfy:

```text
0 <= start <= end <= length(items)
```

An invalid bound produces `CoreFail.out_of_range`. Negative indices are not supported. Taking the complete slice with `items[..]` cannot be out of range and does not require postfix `!`.

A subslice preserves the access permission of its source:

```arkoi
read_only @[]Item = mutable_items[start..end]!
mutable @[]mut Item = mutable_items[start..end]!
```

A mutable slice may be viewed as read-only. A read-only slice cannot be converted into a mutable slice. As with mutable slices generally, independently created mutable subslices may overlap and do not imply exclusive access.

#### Fixed-Size Arrays

Arkoi uses `[N]T` for a fixed-size array containing exactly `N` elements of type `T`. The length is part of the array's type.

```arkoi
values @[4]u32 = [10, 20, 30, 40]
```

Arrays with different lengths are different types:

```arkoi
first @[4]u32
second @[8]u32
```

The relationship between arrays and slices is explicit:

```arkoi
[4]u32    # Owns exactly four u32 values.
[]u32     # Non-owning read-only view.
[]mut u32 # Non-owning mutable view.
```

A full read-only slice is created with the complete-range expression:

```arkoi
view @[]u32 = values[..]
```

Creating a mutable slice requires a mutable array binding:

```arkoi
values @mut [4]u32 = [10, 20, 30, 40]
view @[]mut u32 = values[..]
```

A fixed-size array owns its elements. Its data-or-resource category follows its element type: `[N]T` is a resource when `T` is a resource, and otherwise it is data.

##### Collection Length

Arkoi provides the infallible built-in function `length(...)` for fixed-size arrays, slices, `string`, and `string_view`. The result type is `usize`.

```arkoi
array_count @usize = length(values)
slice_count @usize = length(view)
byte_count @usize = length(text)
```

For a fixed-size array `[N]T`, `length(array)` evaluates to the compile-time value `N`. For a slice, it returns the stored element count. For `string` and `string_view`, it returns the UTF-8 byte count.

```arkoi
text @const string_view = "é"
bytes @usize = length(text)  # 2 in UTF-8.
```

`length(...)` never fails. Determining Unicode scalar or grapheme counts requires separate iteration or library functionality.

##### Array Length Expressions

The length `N` in `[N]T` must be a compile-time integer expression. Every fixed-size array therefore has a statically known size and layout.

```arkoi
WIDTH @const usize = 4
HEIGHT @const usize = 3

pixels @[WIDTH * HEIGHT]u32
header @[2 + 6]u8
```

A runtime binding cannot be used as an array length, even when its value does not change after initialization:

```arkoi
size @usize = 4
values @[size]u32  # Compile-time error: array length is not a compile-time expression.
```

The evaluated length must be nonnegative and representable as `usize`. Any arithmetic used to form the length is evaluated using Arkoi's compile-time integer rules before the result is checked.

##### Zero-Length Arrays

`[0]T` is a valid fixed-size array type. It contains no elements and requires no element initialization or cleanup.

```arkoi
empty @[0]u8 = []
view @[]u8 = empty[..]
```

Indexing a zero-length array with any compile-time index is a compile-time error. Dynamic indexing always produces `CoreFail.out_of_range`.

```arkoi
value @u8 = empty[0]  # Compile-time error.
```

A zero-length array preserves the element type and alignment requirements but contains no element storage. When `T` is a resource, `[0]T` owns no resource instances and performs no element cleanup.

Taking `empty[..]` produces an ordinary empty slice.

##### Array Indexing

When an array index is known at compile time, the compiler checks it statically.

A constant index within the array bounds is infallible and does not require postfix `!`:

```arkoi
first @u32 = values[0]
values[2] = 42
```

A constant index outside the array bounds is a compile-time error:

```arkoi
value @u32 = values[4]  # Compile-time error for [4]u32.
```

When the index is not known at compile time, indexing is recoverably bounds-checked:

```arkoi
value @u32 = values[index]!
values[index]! = 42
```

An invalid dynamic index produces `CoreFail.out_of_range`. Writing through an array index additionally requires mutable access to the array.

This differs from slice indexing only where the compiler can prove a fixed-array index valid. Slice lengths are runtime values, so slice indexing generally remains fallible:

```arkoi
array[0]       # Statically proven valid.
array[index]!  # Runtime bounds check.
slice[0]!      # Runtime bounds check.
```

##### Resource Elements as Addressable Places

A fixed-size array owns each of its elements. When the element type is a resource, indexing identifies the existing storage location—the element *place*—rather than producing an implicitly copied or moved resource value.

For data elements, reading an indexed element may copy the value normally:

```arkoi
number @u32 = numbers[0]
```

For resource elements, an ordinary value read is invalid because it would imply an ownership copy:

```arkoi
file @File = files[0]  # Compile-time error: resources are not copied implicitly.
```

The resource place may be borrowed without changing ownership:

```arkoi
inspect(&files[0])
modify(&mut files[0])
```

An individual element of `[N]Resource` cannot be moved out directly because that would leave the owning array partially initialized:

```arkoi
first @File = move(files[0])  # Compile-time error.
```

Moving the complete array remains valid because the array transitions as one whole resource value:

```arkoi
other @[2]File = move(files)
```

A resource element may be extracted only through an operation that immediately installs another initialized value. `replace(...)` returns the old value while installing the replacement:

```arkoi
old @File = replace(&mut files[0], move(replacement))
```

For `[N]?Resource`, `take(...)` may extract the optional resource while replacing the element with `none`:

```arkoi
taken @?File = take(&mut files[0])
```

The same place rules apply to dynamically indexed array and slice elements after their bounds check succeeds:

```arkoi
inspect(&files[index]!)
old @File = replace(&mut files[index]!, move(replacement))
```

##### Direct Assignment to Resource Places

A mutable resource-valued place may be assigned directly. Direct assignment uses Arkoi's construct-first, drop-and-replace semantics.

```arkoi
files @mut [2]File = [
    File.open(first_path)!,
    File.open(second_path)!,
]

files[0] = File.open(replacement_path)!
```

A named replacement resource requires explicit ownership transfer:

```arkoi
replacement @File = File.open(replacement_path)!
files[0] = move(replacement)
```

The same rule applies to mutable resource fields and dynamically indexed array or slice elements:

```arkoi
user.file = File.open(path)!
files[index]! = move(replacement)
mutable_files[index]! = File.open(path)!
```

The operation proceeds in this order:

1. Resolve the destination place, including any required bounds check.
2. Fully evaluate and construct the replacement value.
3. If replacement construction fails, leave the existing resource unchanged.
4. Drop the existing resource value.
5. Install the replacement value.

The destination is never observably uninitialized. Direct assignment drops the previous resource. `replace(...)` remains distinct because it returns ownership of the previous value instead of dropping it.

```arkoi
files[0] = move(replacement)
old @File = replace(&mut files[0], move(replacement))
```

More generally, resource-valued fields and indexed elements are addressable owning places. They may be borrowed, mutated through permitted access, replaced, swapped, or taken when optional, but they are never copied or directly moved out in a way that leaves their containing aggregate partially initialized.

`take(...)`, `replace(...)`, and `swap(...)` also apply to data places where their type requirements are satisfied.

##### Array Repetition

Every fixed-array element must be written explicitly in an array literal or produced through explicit construction code. The form `[value; N]` is a compile-time error.

---

### 6.2 Explicit Read-Only Reference Conversion

Arkoi does not implicitly convert a mutable reference into a read-only reference.

To remove mutation permission explicitly, Arkoi provides the built-in `readonly(...)` operation.

```arkoi
mutable @&mut Collection = get_mutable_collection()
view @&Collection = readonly(mutable)
```

`readonly(...)`:

- Accepts exactly one expression of type `&mut T`.
- Returns `&T` referring to the same object.
- Does not copy, move, or mutate the referent.
- Does not modify or consume the original mutable reference.
- Is infallible.
- Is not overloadable.
- Cannot convert `&T` into another type.
- Cannot create mutable access from read-only access.

```arkoi
fun inspect(
    value @&Collection,
):
    # ...

collection @&mut Collection = get_mutable_collection()

inspect(readonly(collection))
```

Passing `&mut T` directly where `&T` is required is a compile-time error.

```arkoi
inspect(collection)  # Compile-time error
```

The explicit operation preserves Arkoi's exact overload-resolution model.

```arkoi
fun inspect(
    value @&Collection,
):
    # ...

fun inspect(
    value @&mut Collection,
):
    # ...

inspect(collection)            # Selects the &mut Collection overload
inspect(readonly(collection))  # Selects the &Collection overload
```

Because Arkoi permits reference aliasing, the original mutable reference remains usable according to the ordinary reference rules.

```arkoi
collection @&mut Collection = get_mutable_collection()

inspect(readonly(collection))
modify(collection)
```

For `for` and `for!`, the iterable expression alone selects the receiver mode. The loop-binding type does not participate in protocol selection.

An existing mutable reference selects mutable iteration.

```arkoi
collection @&mut Collection = get_mutable_collection()

for item @&mut Item in collection:
    modify(item)
```

To request read-only iteration through the same mutable reference, the programmer uses `readonly(...)`.

```arkoi
for item @&Item in readonly(collection):
    inspect(item)
```

The selection rules are:

- An expression of type `T` follows the ordinary direct-value iteration rules.
- An expression of type `&T` selects the read-only iteration mode of `T`.
- An expression of type `&mut T` selects the mutable iteration mode of `T`.
- `readonly(reference)` converts `&mut T` to `&T` and therefore selects read-only iteration.
- `move(binding)` selects consuming iteration for a named resource binding.
- The declared loop-binding type must exactly match the selected iterator's `Item` type, ignoring only binding-level `@mut`.

Arkoi never tries multiple iteration modes based on the loop-binding type.

```arkoi
collection @&mut Collection = get_mutable_collection()

for item @&Item in collection:
    # Compile-time error when MutableIterable.Item is not &Item
```

The programmer must explicitly choose the required receiver access before protocol selection.

The name `readonly` describes the permission of the returned reference. It does not freeze the referent or permanently change its mutability.

### 6.3 `readonly(...)` as an Access-Reducing Conversion

`readonly(...)` is generally an explicit conversion from a mutable-access view to the corresponding read-only view.

It does not freeze, copy, move, or otherwise transform the underlying value. It converts only the access permissions represented by the input type.

For references:

```arkoi
mutable @&mut Data = get_mutable_data()
view @&Data = readonly(mutable)
```

The conversion is:

```text
&mut T -> &T
```

For slices:

```arkoi
mutable @[]mut Item = get_mutable_items()
view @[]Item = readonly(mutable)
```

The conversion is:

```text
[]mut T -> []T
```

For slices and raw pointers, `readonly(...)` preserves the same address. For a mutable slice, it also preserves the same length. In each case it removes mutation permission without consuming the original value.

For a mutable slice, `readonly(...)`:

- Preserves the same starting address.
- Preserves the same length.
- Does not copy any elements.
- Removes element-mutation permission from the returned slice.
- Does not modify or consume the original mutable slice value.
- Is infallible.

The original mutable reference or slice remains available according to Arkoi's ordinary permissive aliasing rules.

```arkoi
items @[]mut Item = get_mutable_items()

inspect_all(readonly(items))
modify_first(items)
```

`readonly(...)` is a built-in conversion operation rather than an overloadable function.

It supports the built-in access-reducing conversions:

```text
&mut T  -> &T
[]mut T -> []T
*mut T  -> *T
```

No other conversion is implied merely by the name.

In particular, `readonly(...)` does not:

- Convert an owned aggregate into a reference.
- Borrow an ordinary value implicitly.
- Convert a raw pointer into a reference.
- Convert a read-only view back into a mutable view.
- Recursively alter mutability inside arbitrary composite types.
- Make the underlying object permanently immutable.

Applying `readonly(...)` to a type without a defined mutable-to-read-only view conversion is a compile-time error.

```arkoi
value @Data = create_data()
readonly(value)  # Compile-time error
```

This operation belongs conceptually to Arkoi's conversion system, but it has a dedicated name because it expresses a specific permission-removing conversion and cannot fail. General checked representation or numeric conversions continue to use `convert(...)!`.

## 7. Methods, Visibility, and Compiler Hooks

### 7.1 Methods and Associated Functions

Methods are declared outside the aggregate, using a qualified function name.

```arkoi
fun Point.distance(self @&Point, other @&Point) @f64:
    # ...
```

They use static resolution in Arkoi. Method declarations do not imply inheritance, traits, or dynamic dispatch.

#### Allowed method receivers

Methods use one of these receiver forms:

```arkoi
self @&Type
self @&mut Type
self @own Type
```

The ordinary ownership rules apply to the receiver. An owning receiver is valid
when the received type is a resource and consumes the explicitly moved value.

```arkoi
fun File.into_buffer(self @own File) @Buffer:
    # ...

buffer @Buffer = File.into_buffer(move(file))
```

#### Explicit receiver calls

Methods are called through their declaring type. The receiver is the first
argument and follows the same explicit borrowing and movement rules as every
other argument.

```arkoi
size @usize = File.size(&file)
File.flush(&mut file)!
buffer @Buffer = File.into_buffer(move(file))
```

An existing reference may be passed directly when its type and access match the
receiver parameter.

```arkoi
file_ref @&mut File = &mut file
File.flush(file_ref)!
```

Fields are selected through a value or reference. Receiver functions are
qualified by their declaring type or a visible interface requirement.

```arkoi
fun File.write(self @&mut File, buffer @&Buffer) !IOFail:
    # ...

File.write(&mut file, &buffer)!
```

Every receiver call therefore exposes whether the operation receives read-only
access, mutable access, or ownership.

An interface receiver requirement may be called through the interface name.
The concrete implementation is selected statically from the explicit receiver's
concrete type.

```arkoi
Writer.write(&mut file, data = bytes)
```

Interface qualification does not create an interface value or dynamic dispatch.
An associated-function requirement without `self` is called through a concrete
implementing type because there is no receiver from which to select an
implementation.

#### Associated functions

Qualified functions without a `self` parameter are associated functions.

```arkoi
fun File.open(path @string_view) !IOFail @File:
    # ...
```

They are called through the type name.

```arkoi
file @File = File.open(path)!
```

---

### 7.2 Visibility and Encapsulation

Declarations are private by default. The `pub` keyword exposes them.

#### Public types

```arkoi
pub data Point:
    pub x @f32
    pub y @f32
```

#### Public functions

```arkoi
pub fun parse(text @string_view) !ParseFail @Data:
    # ...
```

#### Public methods and associated functions

```arkoi
pub fun Point.distance(self @&Point, other @&Point) @f64:
    # ...

pub fun File.open(path @string_view) !IOFail @File:
    # ...
```

#### Type-private fields

Fields are private to their declaring type by default.

```arkoi
pub resource File:
    handle @u64
```

A private field may be accessed only by:

- Methods belonging to the type.
- Associated functions belonging to the type.
- Compiler-recognized hooks belonging to the type.

These functions must be declared in the same module as the type.

```arkoi
pub fun File.raw_handle(self @&File) @u64:
    return self.handle

pub fun File.from_handle(handle @u64) @File:
    return File(
        handle = handle,
    )

fun File.__drop__(self @&mut File):
    close_handle(self.handle)
```

An unrelated function cannot access the field, even in the same module.

```arkoi
fun debug_file(file @&File):
    print(file.handle)  # Compile-time error.
```

Another module cannot gain access merely by declaring a qualified function for the type.

Public fields remain directly accessible according to normal reference and mutability rules.

#### Construction with private fields

Code without private access cannot initialize a private field by name or
position. Because construction initializes every field, an aggregate with any
inaccessible field must be created through an accessible constructor function.

---

### 7.3 Compiler Hooks

Compiler-recognized hooks use reserved double-underscore names. Arkoi defines lifecycle hooks, comparison and ordering hooks, arithmetic and bitwise operator hooks, indexing and slicing hooks, length and membership hooks, and the canonical iteration hooks described by this specification.

Only language-defined hook names receive special behavior. The compiler validates each hook against its required signature, ownership modes, safety, failure effect, and overload rules. Hooks are declared as ordinary external functions or methods of their owning type and are used through the corresponding language operation or syntax.

---

## 8. Failures and Unsafe Execution

### 8.1 Failure Effects

Arkoi uses failure effects rather than exceptions or a first-class `Result` wrapper.

#### Failure declarations

```arkoi
failure IOFail:
    not_found
    permission_denied
```

Failures are compact, flat, enum-like discriminants. Arkoi failures do not carry messages, payloads, source errors, or stack traces.

#### Built-in failures

```arkoi
failure CoreFail:
    none_access
    out_of_range
    invalid_conversion
    division_by_zero
    out_of_memory
```

Every user-defined failure set includes `CoreFail` as a failure-set inclusion rule, not through object-oriented inheritance.

`CoreFail.out_of_memory` represents recoverable memory-allocation failure. Operations that allocate, including `string(view)!` and resource cloning such as `clone(value)!`, may produce this failure. Allocation failure is not an automatic trap; callers may propagate or handle it through Arkoi's normal failure-effect rules.

#### Function failure effects

A function's failure effect appears after the parameter list and before the return type.

```arkoi
fun read_file(path @string_view) !IOFail @File:
    # ...
```

#### Producing a failure

```arkoi
fail IOFail.not_found
```

#### Explicit propagation

Failure propagation is never automatic.

```arkoi
file @File = read_file(path)!
```

The call must use postfix `!` or handle the failure locally.

#### Combined failure sets

```arkoi
failure LoadFail = IOFail | ParseFail
```

Combined failure sets are transitive and do not create runtime wrapper values. Failure values remain qualified by their defining failure type.

#### Expression-level handling

```arkoi
profile @Profile = read_profile(id) handle failure:
    log_failure(failure)
    yield default_profile()
```

`yield` produces the value of the handler expression.

Each handler path must either:

- Yield a compatible value.
- Return from the function.
- Produce or propagate a failure.
- Otherwise leave control flow through a valid terminating construct.

Failures are recoverable. Traps are unrecoverable.

---

### 8.2 Unsafe Code

Unsafe operations must appear inside an unsafe context.

#### Unsafe block

```arkoi
unsafe:
    # Low-level operations.
```

#### Unsafe function

```arkoi
unsafe fun read_raw(pointer @*c.unsigned_char) @u8:
    # Entire body is an unsafe context.
```

An unsafe function may be called only from an unsafe block or another unsafe function.

Unsafe code remains statically typed. It permits operations the compiler cannot fully verify; it does not disable type checking.

A safe function may internally use unsafe code and expose a safe interface when it correctly enforces its invariants.

---

## 9. Expressions, Conversions, and Operators

### 9.1 Boolean Values and Operators

Arkoi uses strict booleans.

```arkoi
bool
true
false
```

There is no general truthiness conversion. Numbers, pointers, text values, and optionals do not automatically become booleans.

Boolean operators are keywords:

```arkoi
and
or
not
```

`and` and `or` short-circuit.

Arkoi does not use `&&`, `||`, or logical `!`.

---

### 9.2 Numeric Conversion

Implicit numeric conversion is permitted only when every value of the source type is exactly representable in the destination type.

Allowed implicit conversions include:

- Same-sign integer widening.
- Unsigned-to-signed conversion when the signed destination covers the entire unsigned source range.
- `f32` to `f64`.

Other conversions must be explicit.

Arkoi does not implicitly convert numbers to booleans or booleans to numbers.

#### Checked conversion

```arkoi
converted @u32 = convert(value, u32)!
```

`convert` is safe and fallible. It may produce:

- `CoreFail.out_of_range`.
- `CoreFail.invalid_conversion`.

#### Truncating conversion

```arkoi
small @u8 = truncate(value, u8)
```

`truncate` intentionally discards information according to defined language rules. Integer narrowing keeps the low-order bits.

#### Bit reinterpretation

```arkoi
bits @u32 = bitcast(value, u32)
```

`bitcast` preserves the exact bit pattern and never performs a numeric conversion. The source and destination must have the same size.

In safe code, `bitcast` is limited to equal-sized fixed-width integer and floating-point types for which every source bit pattern is a valid destination value.

```arkoi
bits @u32 = bitcast(number, u32)
number @f32 = bitcast(bits, f32)
signed @s32 = bitcast(bits, s32)
```

Bitcasts involving types with additional validity requirements require an unsafe context. This includes booleans, characters, enumerations with restricted discriminants, references, raw pointers, and aggregates.

```arkoi
unsafe:
    flag @bool = bitcast(byte, bool)
    character @char = bitcast(bits, char)
    pointer @*Item = bitcast(address, *Item)
    reference @&Item = bitcast(pointer, &Item)
    item @Item = bitcast(raw, Item)
```

An unsafe bitcast still requires equal sizes and a compile-time-known layout. It performs no runtime validation. The programmer must ensure that the resulting bit pattern satisfies every validity, alignment, lifetime, initialization, and ownership requirement of the destination type. Violating those requirements produces undefined behavior.

Bitcasting a named resource value requires an explicit ownership transfer:

```arkoi
unsafe:
    converted @OtherResource = bitcast(move(resource), OtherResource)
```

The source becomes uninitialized and the destination becomes the sole owner of the transferred bits. Unsafe `bitcast` never implicitly copies a resource.

---

### 9.3 Integer Arithmetic

Arithmetic behavior is stable across debug and release builds.

#### Ordinary arithmetic

```arkoi
+
-
*
```

These operators trap on overflow.

#### Recoverable arithmetic

```arkoi
+!
-!
*!
```

These operators produce `CoreFail.out_of_range` and propagate it using the operator's built-in `!` behavior. They may be followed by a local `handle failure` expression.

#### Wrapping arithmetic

```arkoi
+%
-%
*%
```

These operators use modular wrapping arithmetic.

The arithmetic variants apply to integers, not floating-point values.

---

### 9.4 Division and Remainder

#### Division

```arkoi
/
```

Integer division traps on:

- Division by zero.
- Signed minimum divided by `-1` when that result is not representable.

Signed division truncates toward zero.

#### Recoverable division

```arkoi
/!
```

This produces:

- `CoreFail.division_by_zero`.
- `CoreFail.out_of_range`.

#### Remainder

```arkoi
%
```

Remainder traps on division by zero and follows truncating division semantics.

For signed minimum remainder `-1`, the result is `0` and does not trap.

#### Recoverable remainder

```arkoi
%!
```

This produces `CoreFail.division_by_zero`.

There are no wrapping division or remainder operators.

---

### 9.5 Shift Operators

```arkoi
<<
>>
```

Shift counts must be nonnegative and smaller than the bit width of the left operand.

Ordinary shifts trap on invalid counts.

Left shift also traps when significant bits would be discarded or the mathematical result is not representable.

Recoverable variants are:

```arkoi
<<!
>>!
```

They produce `CoreFail.out_of_range`.

There are no `<<%` or `>>%` operators.

Signed right shift is arithmetic. Unsigned right shift is logical.

To logically shift a signed integer, the value must first be bitcast to the corresponding unsigned type.

---

### 9.6 Comparisons

Arkoi supports:

```arkoi
==
!=
<
<=
>
>=
```

Each comparison expression contains one comparison operator; chained comparison syntax is invalid.

```arkoi
0 < value < 10  # Invalid.
```

Use explicit boolean composition.

```arkoi
0 < value and value < 10
```

Numeric comparisons use the same lossless common-type conversion principles as ordinary implicit numeric conversion.

---

### 9.7 Equality for Data Aggregates

A `data` aggregate automatically supports `==` and `!=` when every field supports the corresponding operation.

```arkoi
data Point:
    x @s32
    y @s32

first @Point = Point(x = 10, y = 20)
second @Point = Point(x = 10, y = 20)

same @bool = first == second
different @bool = first != second
```

When no custom hook exists, the compiler generates structural comparison.

Generated `==`:

- Compares fields in declaration order.
- Uses each field type's normal `==` operation.
- Stops at the first unequal field.
- Produces `true` only when every field compares equal.
- Is available only when every field supports `==`.

Generated `!=`:

- Compares fields in declaration order.
- Uses each field type's normal `!=` operation.
- Stops at the first field that compares unequal.
- Produces `false` only when every field compares equal.
- Is available only when every field supports `!=`.

A data aggregate may replace either generated operation independently with reserved hooks.

```arkoi
fun Point.__eq__(
    self @&Point,
    other @&Point,
) @bool:
    return self.x == other.x

fun Point.__ne__(
    self @&Point,
    other @&Point,
) @bool:
    return self.x != other.x
```

The `__eq__` and `__ne__` hooks:

- Must belong to the compared data type.
- Must be declared in the same module as the type.
- Must have exactly two parameters: `self @&Type` and `other @&Type`.
- Must return `bool`.
- Must not declare a failure effect.
- Replace only their corresponding generated operation.
- Need not be public.

If only one hook is defined, the other operation remains compiler-generated when its fields support that operation.

Arkoi does not require `__ne__` to be the logical negation of `__eq__`. Maintaining that relationship is the type author's responsibility.

A malformed declaration using either reserved hook name is a compile-time error.

### 9.8 Equality for Resource Aggregates

Resource aggregates receive no automatically generated equality or inequality operations.

Even when every stored field supports comparison, a resource type does not support `==` or `!=` unless the type author explicitly defines the corresponding hook.

```arkoi
resource Buffer:
    # fields

fun Buffer.__eq__(
    self @&Buffer,
    other @&Buffer,
) @bool:
    # Explicit semantic equality
```

```arkoi
fun Buffer.__ne__(
    self @&Buffer,
    other @&Buffer,
) @bool:
    # Explicit semantic inequality
```

The resource hooks follow the same signature, same-module, visibility, and infallibility requirements as data-aggregate comparison hooks.

The hooks are independent:

- `__eq__` enables `==`.
- `__ne__` enables `!=`.
- Defining one does not automatically define the other.
- Arkoi does not infer one operation by negating the other.

Without the corresponding hook, the operator is a compile-time error.

```arkoi
same @bool = first_buffer == second_buffer
# Compile-time error when Buffer has no __eq__

different @bool = first_buffer != second_buffer
# Compile-time error when Buffer has no __ne__
```

This prevents Arkoi from assigning structural comparison semantics to resource values whose meaning may depend on ownership, external state, or identity not represented by their stored fields.

### 9.9 Equality for Fixed Arrays

A fixed array supports `==` and `!=` when its element type supports the corresponding operator.

```arkoi
first @[3]u32 = [1, 2, 3]
second @[3]u32 = [1, 2, 3]

same @bool = first == second
different @bool = first != second
```

Generated fixed-array `==`:

- Compares elements from index `0` upward.
- Uses the element type's normal `==` operation.
- Stops at the first unequal element.
- Produces `true` only when every element compares equal.
- Is available only when the element type supports `==`.

Generated fixed-array `!=`:

- Compares elements from index `0` upward.
- Uses the element type's normal `!=` operation.
- Stops at the first element that compares unequal.
- Produces `false` only when every element compares equal.
- Is available only when the element type supports `!=`.

Zero-length arrays compare equal with `==` and unequal with `!=` producing `false`.

Fixed arrays are built-in composite types and cannot define custom `__eq__` or `__ne__` hooks. Their comparison behavior is derived entirely from the element type.

### 9.10 Equality for Slices

Slices compare by their viewed contents rather than by pointer-and-length identity.

A slice supports `==` and `!=` when its element type supports the corresponding operator.

```arkoi
first @[]u32 = values_a[..]
second @[]u32 = values_b[..]

same @bool = first == second
different @bool = first != second
```

Slice `==`:

1. Compares the slice lengths.
2. Produces `false` immediately when the lengths differ.
3. Otherwise compares elements from index `0` upward.
4. Uses the element type's normal `==` operation.
5. Stops at the first unequal element.
6. Produces `true` only when all corresponding elements compare equal.

Slice `!=`:

1. Compares the slice lengths.
2. Produces `true` immediately when the lengths differ.
3. Otherwise compares elements from index `0` upward.
4. Uses the element type's normal `!=` operation.
5. Stops at the first element that compares unequal.
6. Produces `false` only when all corresponding elements compare equal.

Two slices may compare equal even when they refer to different storage.

```arkoi
first == second  # May be true even when the backing allocations differ
```

Mutable and read-only slices use the same read-only comparison behavior. Comparison does not modify either slice or its backing storage.

Slice types are built-in composite types and cannot define custom `__eq__` or `__ne__` hooks.

### 9.11 Equality for Optionals

An optional `?T` supports `==` and `!=` when `T` supports the corresponding operator.

Optional `==` follows these rules:

```arkoi
none == none        # true
none == some_value  # false
some_value == none  # false
left == right       # Compares the contained values
```

When both optionals contain values, comparison uses `T`'s normal `==` operation.

Optional `!=` follows these rules:

```arkoi
none != none        # false
none != some_value  # true
some_value != none  # true
left != right       # Compares the contained values
```

When both optionals contain values, comparison uses `T`'s normal `!=` operation.

```arkoi
first @?u32 = 10
second @?u32 = 10
empty @?u32 = none

same @bool = first == second
different @bool = first != empty
```

These rules apply to both data and resource optionals.

A resource optional supports `==` only when the contained resource type explicitly supports `==`, and supports `!=` only when the contained resource type explicitly supports `!=`.

Optional types are built-in composite types and cannot define custom `__eq__` or `__ne__` hooks.

### 9.12 Comparisons Through References and Raw-Pointer Equality

A reference expression transparently designates its referent for comparison.
The referent type determines whether each comparison operator is available and
which built-in rule or comparison hook it uses.

```arkoi
same_value @bool = first_ref == second_ref
ordered @bool = first_ref < second_ref
```

Reference mutability does not change the referent's read-only comparison
behavior. To compare whether two references designate the same place, compare
raw pointers obtained from their referents:

```arkoi
same_place @bool = address(first_ref) == address(second_ref)
```

Raw-pointer equality:

```arkoi
same @bool = first_pointer == second_pointer
different @bool = first_pointer != second_pointer
is_null @bool = pointer == null
```

Two raw pointers compare equal when they contain the same address value. A raw pointer may be compared with `null`.

Raw-pointer comparison:

- Is safe because it does not dereference either pointer.
- Does not require the addresses to designate live or related objects.
- Does not call comparison hooks on the pointed-to type.

Raw-pointer types cannot define custom `__eq__` or `__ne__` hooks.

Ordering operators such as `<`, `<=`, `>`, and `>=` are not supported for raw pointers.

Raw-pointer pointees are compared only after explicit unsafe dereferencing.

### 9.13 Equality for Enums

Every enum automatically supports `==` and `!=`.

```arkoi
status == Status.ready
status != Status.failed
```

Enum comparison uses the enum member's logical discriminant.

Two enum values compare equal only when:

- They have the same enum type.
- They represent the same enum member.

Different enum types cannot be compared, even when their numeric discriminants are equal.

```arkoi
enum First:
    value = 1

enum Second:
    value = 1

first @First = First.value
second @Second = Second.value

same @bool = first == second  # Compile-time error
```

Enum equality is independent of the enum's physical storage representation.

Enums cannot define custom `__eq__` or `__ne__` hooks.

### 9.14 Ordering for Data Aggregates

A `data` aggregate automatically supports `<`, `<=`, `>`, and `>=` when every field supports the corresponding ordering operation.

Generated ordering is lexicographic in field declaration order.

```arkoi
data Version:
    major @u32
    minor @u32
    patch @u32

older @bool = first < second
```

For generated `<`, Arkoi compares fields from first to last:

1. If the current fields compare with `<`, the result is `true`.
2. If the current fields compare with `>`, the result is `false`.
3. Otherwise, comparison proceeds to the next field.
4. If all fields compare equal, the result is `false`.

Generated `>` follows the corresponding reversed ordering.

Generated `<=` and `>=` use the same lexicographic field order and include equality when every field compares equivalent.

Each generated operator is available only when every field supports the operations required for that operator's lexicographic comparison.

A data aggregate may independently replace any generated ordering operation using a reserved hook.

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

The reserved ordering hooks are:

```text
__lt__  for <
__le__  for <=
__gt__  for >
__ge__  for >=
```

Each ordering hook:

- Must belong to the compared data type.
- Must be declared in the same module as the type.
- Must have exactly two parameters: `self @&Type` and `other @&Type`.
- Must return `bool`.
- Must not declare a failure effect.
- Replaces only its corresponding generated operator.
- Need not be public.

Hooks are independent. Arkoi does not derive one ordering operator from another and does not verify that the four operators define a mathematically consistent ordering.

A malformed declaration using a reserved ordering-hook name is a compile-time error.

### 9.15 Ordering for Resource Aggregates

Resource aggregates receive no automatically generated ordering operations.

Even when every stored field supports ordering, a resource type does not support `<`, `<=`, `>`, or `>=` unless the type author explicitly defines the corresponding hook.

```arkoi
fun FilePosition.__lt__(
    self @&FilePosition,
    other @&FilePosition,
) @bool:
    return self.offset < other.offset
```

The available hooks are:

```text
__lt__  for <
__le__  for <=
__gt__  for >
__ge__  for >=
```

The resource hooks follow the same signature, same-module, visibility, and infallibility requirements as data-aggregate ordering hooks.

The hooks are independent:

- `__lt__` enables `<`.
- `__le__` enables `<=`.
- `__gt__` enables `>`.
- `__ge__` enables `>=`.
- Defining one does not define any other operator.
- Arkoi does not infer or generate missing resource-ordering operators.

Without the corresponding hook, use of an ordering operator is a compile-time error.

This prevents Arkoi from assigning structural ordering semantics to resource values whose meaning may depend on ownership, external state, or identity not represented by their stored fields.

### 9.16 Ordering for Fixed Arrays

A fixed array supports `<`, `<=`, `>`, and `>=` when its element type supports the comparisons required by the corresponding operator.

Fixed-array ordering is lexicographic from index `0` upward.

```arkoi
first @[3]u32 = [1, 4, 2]
second @[3]u32 = [1, 5, 0]

less @bool = first < second  # true
```

For `<`, Arkoi compares corresponding elements in increasing index order:

1. If the current left element is less than the current right element, the result is `true`.
2. If the current left element is greater than the current right element, the result is `false`.
3. Otherwise, comparison proceeds to the next index.
4. If every element compares equal, the result is `false`.

The other ordering operators use the corresponding lexicographic relation.

```arkoi
[1, 2, 3] <  [1, 2, 3]  # false
[1, 2, 3] <= [1, 2, 3]  # true
[1, 2, 3] >  [1, 2, 3]  # false
[1, 2, 3] >= [1, 2, 3]  # true
```

Because array length is part of the type, fixed-array ordering compares only arrays of the same length and compatible element type. No separate length comparison occurs.

Zero-length arrays are equal for ordering purposes:

```arkoi
empty_a @[0]u8 = []
empty_b @[0]u8 = []

empty_a < empty_b   # false
empty_a <= empty_b  # true
empty_a > empty_b   # false
empty_a >= empty_b  # true
```

Fixed arrays are built-in composite types and cannot define custom `__lt__`, `__le__`, `__gt__`, or `__ge__` hooks.

### 9.17 Ordering for Slices

A slice supports `<`, `<=`, `>`, and `>=` when its element type supports the comparisons required by the corresponding operator.

Slice ordering is lexicographic by viewed contents.

```arkoi
first @[]u32 = [1, 2][..]
second @[]u32 = [1, 2, 3][..]

less @bool = first < second  # true
```

Arkoi compares corresponding elements from index `0` upward.

For `<`:

1. If the current left element is less than the current right element, the result is `true`.
2. If the current left element is greater than the current right element, the result is `false`.
3. Otherwise, comparison proceeds to the next shared index.
4. If all shared elements compare equal, the shorter slice is less.
5. If the slices have equal length and all elements compare equal, the result is `false`.

The other ordering operators use the corresponding lexicographic relation.

```arkoi
[1, 4][..] < [1, 5][..]     # true
[1, 2][..] < [1, 2, 3][..]  # true
[1, 3][..] < [1, 2, 9][..]  # false
```

Mutable and read-only slices use the same read-only ordering behavior. Comparison does not modify either slice or its backing storage.

Slice types are built-in composite types and cannot define custom `__lt__`, `__le__`, `__gt__`, or `__ge__` hooks.

### 9.18 Ordering for Optionals

An optional `?T` supports `<`, `<=`, `>`, and `>=` when `T` supports the comparisons required by the corresponding operator.

Optional ordering uses a fixed presence order:

```text
none < present value
```

An empty optional is always less than an optional containing a value, regardless of the contained value.

```arkoi
none < none         # false
none <= none        # true
none > none         # false
none >= none        # true

none < value        # true
none <= value       # true
none > value        # false
none >= value       # false

value < none        # false
value <= none       # false
value > none        # true
value >= none       # true
```

When both optionals contain values, Arkoi compares the contained values using `T`'s corresponding ordering operation.

```arkoi
first @?u32 = 10
second @?u32 = 20

less @bool = first < second  # true
```

These rules apply to both data and resource optionals.

A resource optional supports an ordering operator only when the contained resource type explicitly supports that operator through its corresponding hook.

Optional types are built-in composite types and cannot define custom `__lt__`, `__le__`, `__gt__`, or `__ge__` hooks.

### 9.19 User-Defined Operator Hooks

Data and resource aggregates may define reserved hooks for arithmetic, bitwise, shift, and unary operators.

A normal binary hook is owned by the left operand's type.

```arkoi
data Vector:
    x @f64
    y @f64

fun Vector.__mul__(
    self @&Vector,
    scalar @f64,
) @Vector:
    return Vector(
        x = self.x * scalar,
        y = self.y * scalar,
    )
```

This enables:

```arkoi
scaled @Vector = vector * 2.0
```

A reverse binary hook is owned by the right operand's type and is used only when the left operand provides no applicable operation.

```arkoi
fun Vector.__r_mul__(
    self @&Vector,
    scalar @f64,
) @Vector:
    return Vector(
        x = scalar * self.x,
        y = scalar * self.y,
    )
```

This enables:

```arkoi
scaled @Vector = 2.0 * vector
```

For a binary expression:

```arkoi
left * right
```

Arkoi resolves the operator in this order:

1. Try the built-in operation or the left operand type's normal hook.
2. If no applicable operation exists, try the right operand type's reverse hook.
3. If neither operation exists, report a compile-time error.

The left-side operation always takes priority when both sides could provide an implementation.

Operator resolution:

- Is static at compile time.
- Searches only the statically identified normal or reverse hook overload set; it does not search unrelated functions.
- Does not apply implicit conversions merely to make a hook applicable.
- Cannot add hooks to a built-in type from an unrelated module.
- Requires every aggregate hook to be declared in the same module as its owning type.
- Uses ordinary parameter and return-type checking.

Supported normal and reverse binary hooks include:

```text
__add__       __r_add__       +
__sub__       __r_sub__       -
__mul__       __r_mul__       *
__div__       __r_div__       /
__rem__       __r_rem__       %

__bit_and__   __r_bit_and__   &
__bit_or__    __r_bit_or__    |
__bit_xor__   __r_bit_xor__   ^
__shl__       __r_shl__       <<
__shr__       __r_shr__       >>
```

Supported unary hooks include:

```text
__neg__       unary -
__pos__       unary +
__bit_not__   ~
```

Trapping, recoverable, and wrapping arithmetic variants are independent operations with independent hooks.

```text
__add__               +
__add_checked__       +!
__add_wrapping__      +%

__sub__               -
__sub_checked__       -!
__sub_wrapping__      -%

__mul__               *
__mul_checked__       *!
__mul_wrapping__      *%
```

Their reverse forms use the same `__r_...__` naming pattern.

```text
__r_add__
__r_add_checked__
__r_add_wrapping__
```

Division, remainder, and shift checked variants likewise use distinct hooks corresponding to their recoverable operator forms.

Arkoi does not derive one operator variant from another. A type may implement any subset.

An operator hook may declare a failure effect. A fallible operator expression must be explicitly propagated or handled like any other fallible expression.

```arkoi
result @Matrix = (matrix / divisor)!
```

Ordinary binary operator hooks do not silently consume resource operands. Resource operands are borrowed through their declared parameters, and any consuming operation must remain explicit through a named function using `move(...)`.

Compound assignment reuses the corresponding binary operator hook.

```arkoi
value += other
```

resolves through `__add__`, while:

```arkoi
value +!= other
value +%= other
```

resolve through `__add_checked__` and `__add_wrapping__` respectively.

The compound-assignment destination is still resolved exactly once, and resource replacement retains construct-first, drop-and-replace behavior.

Malformed declarations using reserved operator-hook names are compile-time errors.

### 9.20 Operator Operand Ownership and Mutability

User-defined operator hooks never consume or mutate their operands.

The owning aggregate operand is always received through a read-only reference.

```arkoi
fun Vector.__add__(
    self @&Vector,
    other @&Vector,
) @Vector:
```

For a normal binary hook, `self` refers to the left operand.

For a reverse binary hook, `self` refers to the right operand.

```arkoi
fun Vector.__r_mul__(
    self @&Vector,
    scalar @f64,
) @Vector:
```

Operator-hook parameters:

- Cannot use `@own`.
- Cannot use `&mut`.
- Cannot implicitly move from either operand.
- Cannot mutate either operand through the hook parameters.
- May receive freely copyable data operands by value, such as `scalar @f64`.
- Must receive aggregate or resource operands through read-only references.
- Follow the ordinary rule that by-value parameters are immutable local bindings.

Operator expressions therefore never silently consume or modify either source operand.

```arkoi
result @Vector = first + second
```

After the expression, both `first` and `second` remain initialized and unchanged.

An operator hook may return a newly constructed data or resource value. Any resources created by the hook follow the ordinary ownership and cleanup rules.

Operations that intentionally mutate or consume an operand must use explicitly named functions or methods.

```arkoi
Vector.scale_in_place(&mut vector, scalar)
Buffer.combine(move(first), move(second))
```

Such consuming or mutating behavior cannot be attached to operator syntax.

Compound assignment may mutate its destination because compound assignment itself is an explicit assignment statement. It still invokes a read-only binary operator hook to produce the replacement value, then installs that result using the normal assignment rules.

### 9.21 Membership Operators

Arkoi supports membership expressions using `in` and `not in`.

```arkoi
present @bool = item in container
absent @bool = item not in container
```

Custom membership is provided by the reserved `__contains__` hook owned by the container type.

```arkoi
fun Container.__contains__(
    self @&Container,
    item @&Element,
) @bool:
    # ...
```

The hook may be overloaded by exact item-parameter signature under the ordinary overload-resolution rules.

Membership hooks are read-only and non-consuming:

- The container receiver must be `self @&Type`.
- Aggregate and resource item operands must be passed through read-only references.
- Freely copyable data operands may be passed by value.
- `@own` and `&mut` are not permitted for membership operands.
- Membership never mutates or consumes the item or container.

An infallible hook enables `in` and `not in`.

```arkoi
present @bool = item in container
absent @bool = item not in container
```

`not in` is the boolean negation of the corresponding `in` result. There is no separate negative-membership hook.

A fallible `__contains__` hook enables the recoverable membership operators `in!` and `not in!`.

```arkoi
fun RemoteSet.__contains__(
    self @&RemoteSet,
    item @&Item,
) !LookupFail @bool:
    # ...
```

Propagation uses the operator form itself:

```arkoi
present @bool = item in! container
absent @bool = item not in! container
```

The `!` belongs to the membership operator. Postfix propagation is not used for membership.

```arkoi
present @bool = (item in container)!  # Compile-time error
```

Using `in` with a fallible hook or `in!` with an infallible hook is a compile-time error. The same rule applies to `not in` and `not in!`.

A fallible membership expression may also be handled explicitly using `handle failure`.

```arkoi
present @bool = item in! container handle failure:
    yield false
```

Membership evaluates the item first and the container second, following Arkoi's left-to-right binary-operand evaluation rule.

Membership expressions are distinct from `for item in collection` loop syntax. The loop form does not invoke `__contains__`.

### 9.22 Pipeline Expressions

A pipeline expression carries one current value through an ordered sequence of
stage expressions.

```arkoi
packet @Packet = (
    Buffer.create()
    |> Buffer.reserve(&mut _, 4096)!
    |> Buffer.append(&mut _, &header)!
    |> Buffer.freeze(move(_))
    |> Packet.from_buffer(move(_))!
)
```

The expression before the first `|>` establishes the initial pipeline carrier.
Each stage contains exactly one unqualified `_` placeholder designating the
current carrier. The placeholder is valid only within a pipeline stage and is
not an ordinary identifier, binding, wildcard, or discard operation.

A stage accesses the carrier explicitly:

```arkoi
File.size(&_)
File.flush(&mut _)!
File.into_buffer(move(_))!
```

The selected function's parameter type must accept that exact access mode.
Ordinary borrowing, mutability, ownership, overload, visibility, safety, and
failure-effect rules remain in force.

The result of each stage determines the next carrier:

- When the stage produces a value, that returned value becomes `_`.
- When the stage produces no value, the existing `_` continues, including any
  mutation performed through `&mut _`.

A returned value always becomes the next carrier even when the stage also
mutates the previous carrier.

```arkoi
bytes_read @usize = (
    file
    |> File.read(&mut _, &mut buffer)!
)
```

Here `File.read` may modify `file` and `buffer`, but its returned `usize` is the
pipeline result.

Every stage must leave an initialized carrier. A stage that uses `move(_)` must
therefore return a replacement value. The pipeline placeholder gives its
carrier a single stable identity for the pipeline's duration, so `move(_)` may
consume a resource temporary produced by the initial expression or an earlier
stage. Beginning with `move(name)` remains necessary when ownership of a named
resource enters the pipeline itself.

The final carrier is the value of the complete pipeline expression and follows
the ordinary result-use, movement, reference-lifetime, and cleanup rules.

`|>` has lower precedence than every other expression operator and groups from
left to right. A multiline pipeline is enclosed in parentheses under Arkoi's
ordinary line-continuation rule.

## 10. Evaluation, Assignment, and Result Use

### 10.1 Evaluation Order

Arkoi evaluates expressions in argument and initializer lists from left to right in their written order.

```arkoi
process(
    create_first(),
    create_second(),
    create_third(),
)
```

The evaluation order is:

1. `create_first()`
2. `create_second()`
3. `create_third()`
4. The call to `process(...)`

Named arguments are also evaluated in textual order rather than parameter-declaration order.

```arkoi
copy_file(
    overwrite = check_overwrite(),
    source = load_source(),
    destination = load_destination(),
)!
```

Here, `check_overwrite()` is evaluated first, followed by `load_source()` and then `load_destination()`.

The receiver of a method call is its explicit first argument and therefore uses
the same left-to-right written order as every other call argument.

```arkoi
File.write(
    get_file_reference(),
    load_first_buffer(),
    load_second_buffer(),
)!
```

Evaluation order is:

1. `get_file_reference()`
2. `load_first_buffer()`
3. `load_second_buffer()`
4. The method call

The same left-to-right written-order rule applies to:

- Function and method arguments.
- Aggregate-construction arguments.
- Array literal elements.
- Built-in operations such as `replace(...)`, `swap(...)`, `convert(...)`, `truncate(...)`, and `bitcast(...)`.

A pipeline evaluates its initial expression exactly once, then evaluates its
stages once each from left to right. The current carrier already exists when a
stage begins; expressions within that stage follow their ordinary evaluation
order. If a stage fails or otherwise leaves control flow, later stages are not
evaluated and every live resource is cleaned up normally.

If evaluation fails or leaves control flow, later expressions are not evaluated. Already constructed temporary resources are cleaned up according to the ordinary cleanup rules.

### 10.2 Operator Operand Evaluation Order

Arkoi evaluates the operands of binary operators from left to right.

```arkoi
result @u32 = create_left() + create_right()
```

The evaluation order is:

1. `create_left()`
2. `create_right()`
3. Application of `+`

This left-to-right rule applies to:

- Arithmetic operators.
- Comparison operators.
- Bitwise operators.
- Shift operators.
- Range-bound expressions.
- Other binary operators unless a more specific rule states otherwise.

Boolean operators retain their short-circuit behavior.

```arkoi
left() and right()
```

`left()` is evaluated first. `right()` is evaluated only if `left()` produces `true`.

```arkoi
left() or right()
```

`left()` is evaluated first. `right()` is evaluated only if `left()` produces `false`.

Postfix operations evaluate the base expression first and then proceed outward in source order.

```arkoi
load_items()![index]!.field
```

If an earlier operand or postfix operation fails, traps, or otherwise leaves control flow, later operations are not evaluated. Already constructed temporary resources are cleaned up according to the ordinary cleanup rules.

### 10.3 Assignment Evaluation Order

For assignment to a computed place, Arkoi evaluates and validates the destination before evaluating the right-hand side.

```arkoi
get_items()[calculate_index()] = create_value()
```

The evaluation order is:

1. Evaluate `get_items()`.
2. Evaluate `calculate_index()`.
3. Resolve and validate the destination place, including bounds checks.
4. Evaluate `create_value()`.
5. Perform the assignment.

If destination evaluation fails, traps, or otherwise leaves control flow, the right-hand side is not evaluated.

For assignment to a resource place:

```arkoi
files[index()] = create_file()!
```

Arkoi applies this order:

1. Resolve and validate the destination place.
2. Fully evaluate and construct the replacement resource.
3. If replacement construction fails, leave the old destination value unchanged.
4. Drop the old destination resource.
5. Install the replacement resource.

This preserves the construct-first, drop-and-replace guarantee while ensuring destination-side effects and destination-validation failures occur before right-hand-side evaluation.

The destination place remains fixed after it has been resolved; it is not reevaluated after the right-hand side completes.

### 10.4 Compound Assignments

Compound assignment evaluates its destination place exactly once.

```arkoi
values[index()] += amount()
```

The evaluation order is:

1. Evaluate `values`.
2. Evaluate `index()`.
3. Resolve and validate the destination element place.
4. Read its current value.
5. Evaluate `amount()`.
6. Apply the corresponding binary operator.
7. Store the result into the resolved destination place.

The destination expression is not reevaluated.

Compound assignment has the same value operation as expanding it to an ordinary assignment, but not the same evaluation behavior.

```arkoi
values[index()] += amount()
```

has the value behavior of:

```arkoi
values[index()] = values[index()] + amount()
```

while evaluating `index()` only once.

Arithmetic compound assignments preserve the arithmetic mode of their operator.

```arkoi
value += amount    # Trapping overflow
value +!= amount   # Recoverable overflow
value +%= amount   # Wrapping arithmetic

value -= amount
value -!= amount
value -%= amount

value *= amount
value *!= amount
value *%= amount
```

Division, remainder, shifts, and bitwise operators also have corresponding compound-assignment forms whenever the underlying binary operator exists.

Examples include:

```arkoi
value /= divisor
value /!= divisor
value %= divisor
value %!= divisor

value <<= count
value <<!= count
value >>= count
value >>!= count

value &= mask
value |= mask
value ^= mask
```

A recoverable compound assignment propagates or handles failure using the same rules as the underlying recoverable operator. The destination remains unchanged if the operation fails before producing a result.

Compound assignment is valid only for mutable, assignable places.

### 10.5 Incrementing and Decrementing

Incrementing and decrementing use compound assignment. Prefix and postfix `++` and `--` are compile-time errors.

```arkoi
count++   # Compile-time error
count--   # Compile-time error
++count   # Compile-time error
--count   # Compile-time error
```

Incrementing and decrementing use compound assignment.

```arkoi
count += 1
count -= 1
```

The arithmetic mode remains explicit.

```arkoi
count += 1    # Trapping overflow
count +!= 1   # Recoverable overflow
count +%= 1   # Wrapping arithmetic
```

Assignment and compound assignment are statements and do not produce values.

### 10.6 Assignment Statements

Assignment and compound assignment are statements and do not produce values.

Each assignment statement has one destination. Chained assignment is a compile-time error.

```arkoi
first = second = 0  # Compile-time error
```

Each assignment must be written as a separate statement.

```arkoi
second = 0
first = second
```

Assignment cannot appear inside:

- Conditions.
- Function arguments.
- Arithmetic or boolean expressions.
- Initializers.
- Return expressions.
- Other assignments.

```arkoi
if value = read():       # Compile-time error
result @u32 = value = 3  # Compile-time error
return value = 3         # Compile-time error
```

Named call arguments use `name = expression` syntax but are not assignments.

```arkoi
copy_file(source = source, destination = destination)!
```

The left side of a named argument names a parameter and does not designate a mutable place.

### 10.7 Unused Expression Results

An expression that produces a value cannot appear as a standalone statement.

```arkoi
4 + 5           # Compile-time error
value           # Compile-time error
create_file()!  # Compile-time error if the returned resource is ignored
```

A function or method call that produces no value may appear as a standalone statement.

```arkoi
log_message(&message)
File.flush(&mut file)!
```

A produced value must otherwise be:

- Bound to a declaration.
- Assigned to an existing mutable place.
- Returned.
- Passed as an argument.
- Used as part of another expression.
- Explicitly discarded.

Arkoi provides the built-in operation `discard(expression)` for intentional disposal.

```arkoi
discard(calculate_value())
discard(File.open(path)!)
```

`discard(...)` evaluates its argument exactly once.

For a data value, the resulting value is ignored.

For a resource value, the resource is immediately cleaned up after successful construction according to its ordinary deterministic cleanup rules.

If evaluation of the argument fails, traps, or otherwise leaves control flow, no value is discarded and normal propagation or cleanup rules apply.

`discard(...)` produces no value and may appear as a standalone statement.

## 11. Control Flow and Iteration

### 11.1 Conditional Control Flow

Arkoi uses statement-only conditional control flow with indentation-based blocks.

```arkoi
if condition:
    perform_first_action()
elif other_condition:
    perform_second_action()
else:
    perform_fallback()
```

Conditions must have type `bool`. Arkoi does not apply truthiness conversions. Parentheses around the condition are optional and normally omitted.

`elif` and `else` are optional. Every branch creates its own lexical scope.

In Arkoi, `if` is a statement and does not directly produce a value.

Definite-initialization analysis considers every continuing branch. A delayed binding may be used after the conditional only when every continuing path initializes it.

```arkoi
result @u32

if condition:
    result = 10
else:
    result = 20

print(result)
```

This is invalid because `result` remains uninitialized when the condition is false:

```arkoi
result @u32

if condition:
    result = 10

print(result)  # Compile-time error.
```

---

### 11.2 While Loops

Arkoi uses indentation-based `while` loops.

```arkoi
while condition:
    perform_action()
```

The condition must have type `bool`. Arkoi does not apply truthiness conversions. Parentheses around the condition are optional and normally omitted.

The condition is evaluated before every iteration, so the body may execute zero times. The loop body creates a lexical scope.

`break` exits the nearest enclosing loop. `continue` ends the current iteration and begins the next condition check.

```arkoi
index @mut usize = 0

while index < length(items):
    process(items[index]!)
    index = index + 1
```

Python-style `while ... else` is not supported in Arkoi.

Definite-initialization analysis cannot assume that a `while` body executes, even when the body contains `break`.

```arkoi
result @u32

while condition:
    result = 10
    break

print(result)  # Compile-time error: result may be uninitialized.
```

---

### 11.3 For Loops

Arkoi uses indentation-based `for` loops over fixed arrays, slices, and named aggregate types that provide a canonical iteration mode.

```arkoi
for value @u32 in values:
    process(value)
```

The iteration binding always has an explicit type and is scoped to the loop body. `break` exits the nearest enclosing loop, and `continue` begins the next iteration. Python-style `for ... else` is not supported in Arkoi.

For data elements, iteration may copy each element by value:

```arkoi
for value @u32 in values:
    process(value)
```

Data elements may also be borrowed explicitly:

```arkoi
for value @&u32 in values:
    inspect(value)
```

Resource elements cannot be copied or moved out implicitly. They must be borrowed:

```arkoi
for file @&File in files:
    inspect(file)
```

Mutable iteration requires mutable element access:

```arkoi
for file @&mut File in mutable_files:
    File.flush(file)!
```

The source must permit mutable element access, such as a mutable fixed-size array binding or a `[]mut T` slice.

For built-in arrays and slices, iteration by owned resource value is invalid:

```arkoi
for file @File in files:  # Compile-time error.
    consume(move(file))
```

Fixed-size arrays and slices use compiler-defined iteration; named aggregates use the canonical static iteration interfaces.

---

### 11.4 Infinite Loops

Arkoi provides a dedicated `loop` statement for intentional indefinite iteration.

```arkoi
loop:
    process_events()
```

A `loop` body creates a lexical scope and begins another iteration after reaching its end.

`break` exits the nearest enclosing loop. `continue` immediately begins the next iteration.

```arkoi
loop:
    event @Event = next_event()!

    if event.should_stop:
        break

    if event.should_skip:
        continue

    process(&event)
```

Unlike `while`, a `loop` has no condition and is considered to execute at least once.

A `loop` does not complete normally unless a reachable `break` exits it. Other operations such as `return`, `fail`, postfix propagation, or a trap may also leave or terminate the loop.

This allows definite-initialization and reachability analysis to distinguish intentional infinite loops from condition-controlled `while` loops.

Arkoi does not use `while true:` as the canonical form for intentional infinite iteration, although it remains a valid `while` loop because `true` is a valid `bool` condition.

#### Named Loops

Any `loop`, `while`, or `for` statement may be given a name using the `named` clause.

```arkoi
loop named outer:
    for value @u32 in values named scan:
        if should_stop(value):
            break outer
```

```arkoi
while should_retry() named retry:
    if finished:
        break retry
```

An unlabeled `break` or `continue` targets the nearest enclosing loop.

```arkoi
break
continue
```

A named `break` or `continue` targets the matching enclosing loop.

```arkoi
break outer
continue scan
```

The named target must refer to an enclosing loop. Loop names must be unique among simultaneously enclosing named loops.

---

### 11.5 Custom `for` Iteration Protocol

Custom `for` iteration uses the compiler-recognized static `Iterable` and `Iterator` interfaces.

```arkoi
interface Iterator:
    type Item

    fun __next__(
        self @&mut Self,
    ) @?Item
```

```arkoi
interface Iterable:
    type Item
    type Iterator

    fun __iterate__(
        self @&Self,
    ) @Iterator
```

The `Iterator` associated type bound by an `Iterable` implementation must be a named aggregate type that implements `Iterator`.

The two interfaces must agree on the yielded item type.

```arkoi
implements Iterable for AccountCollection:
    type Item = &Account
    type Iterator = AccountIterator

implements Iterator for AccountIterator:
    type Item = &Account
```

A mismatch is a compile-time error.

```arkoi
implements Iterable for AccountCollection:
    type Item = &Account
    type Iterator = AccountIterator

implements Iterator for AccountIterator:
    type Item = Account  # Compile-time error: item types do not match
```

The concrete hook definitions remain ordinary external methods.

```arkoi
fun AccountCollection.__iterate__(
    self @&AccountCollection,
) @AccountIterator:
    # ...
```

```arkoi
fun AccountIterator.__next__(
    self @&mut AccountIterator,
) @?&Account:
    # ...
```

A custom value may appear after `in` in a `for` statement when its concrete type implements `Iterable`.

```arkoi
for account @&Account in accounts:
    process(account)
```

The loop binding type must exactly match the `Item` associated type of the selected `Iterable` implementation.

Conceptually, the compiler lowers the loop to direct statically resolved calls:

```arkoi
iterator @mut AccountIterator = AccountCollection.__iterate__(&accounts)

loop:
    next @?&Account = AccountIterator.__next__(&mut iterator)

    if next == none:
        break

    account @&Account = next!
    process(account)
```

This conceptual lowering describes evaluation and control-flow semantics. It does not require the compiler to emit this exact source-level structure.

The iterable expression is evaluated exactly once before iteration begins.

`__iterate__` is called exactly once. The resulting concrete iterator is stored in a hidden mutable local owned by the loop.

`__next__` is called before each iteration. Returning `none` terminates the loop. Returning a present value initializes the loop binding for that iteration.

The iterator value is cleaned up according to the ordinary lexical resource-cleanup rules when the loop ends through exhaustion, `break`, `return`, failure propagation, or another control-flow exit.

`continue` proceeds to the next `__next__` call.

Calls to `__iterate__` and `__next__` are direct static calls. The protocol introduces no interface values, dynamic dispatch, function values, or indirect calls.

Built-in arrays and slices retain their compiler-defined iteration behavior and do not need explicit `Iterable` implementations.

The `in` token in a `for` statement denotes iteration and does not invoke the membership `__contains__` hook.

### 11.6 Fallible Custom Iteration

Fallible custom iteration uses the dedicated `for!` statement.

```arkoi
for! record @&Record in database:
    process(record)
```

The `!` belongs to the `for` construct. It is not postfix propagation applied to the iterable expression.

```arkoi
for record @&Record in database!:
    # Compile-time error
```

Because interface failure effects must match exactly, Arkoi provides separate compiler-recognized interfaces for fallible iterator creation and advancement.

An infallible iterator implements `Iterator`.

```arkoi
interface Iterator:
    type Item

    fun __next__(
        self @&mut Self,
    ) @?Item
```

A fallible iterator implements `FallibleIterator` and binds its failure type explicitly.

```arkoi
interface FallibleIterator:
    type Item
    type Failure

    fun __next__(
        self @&mut Self,
    ) !Failure @?Item
```

The `Failure` binding must name a failure type.

An infallible iterable implements `Iterable`.

```arkoi
interface Iterable:
    type Item
    type Iterator

    fun __iterate__(
        self @&Self,
    ) @Iterator
```

A fallible iterable implements `FallibleIterable` and binds the failure type of iterator creation.

```arkoi
interface FallibleIterable:
    type Item
    type Iterator
    type Failure

    fun __iterate__(
        self @&Self,
    ) !Failure @Iterator
```

The iterator type returned by either iterable interface must implement exactly one of:

- `Iterator`
- `FallibleIterator`

Its `Item` binding must exactly match the iterable's `Item` binding.

This permits all four combinations:

- Infallible `__iterate__` and infallible `__next__`.
- Fallible `__iterate__` and infallible `__next__`.
- Infallible `__iterate__` and fallible `__next__`.
- Fallible `__iterate__` and fallible `__next__`.

Example with infallible iterator creation and fallible advancement:

```arkoi
implements Iterable for Database:
    type Item = &Record
    type Iterator = DatabaseIterator

implements FallibleIterator for DatabaseIterator:
    type Item = &Record
    type Failure = DatabaseFail
```

```arkoi
fun Database.__iterate__(
    self @&Database,
) @DatabaseIterator:
    # ...
```

```arkoi
fun DatabaseIterator.__next__(
    self @&mut DatabaseIterator,
) !DatabaseFail @?&Record:
    # ...
```

A normal `for` loop is valid only when both selected operations are infallible.

```arkoi
for item @Item in collection:
    process(item)
```

Using `for` when either `__iterate__` or `__next__` is fallible is a compile-time error.

A `for!` loop is required when either selected operation is fallible.

Using `for!` when both operations are infallible is a compile-time error, consistent with Arkoi's rule that propagation syntax cannot be applied to infallible operations.

A `for!` loop propagates failures produced by:

- The single initial call to `__iterate__`, when that call is fallible.
- Any call to `__next__`, when that call is fallible.

The enclosing function must declare a failure effect containing every failure that the selected iteration operations may produce.

If the two operations use different failure types, the enclosing effect must include both, directly or through a declared combined failure set.

```arkoi
failure DatabaseIterationFail = OpenFail | ReadFail
```

If `__iterate__` fails, the loop body is never entered.

If `__next__` fails, the current iteration stops immediately, the hidden iterator is cleaned up according to ordinary lexical cleanup rules, and the failure propagates.

The body may independently use Arkoi's ordinary explicit propagation and failure-handling mechanisms.

Built-in array and slice iteration remains infallible and therefore uses ordinary `for`, not `for!`.

### 11.7 Mutable Custom Iteration

Mutable custom iteration uses the compiler-recognized static `MutableIterable` interface.

```arkoi
interface MutableIterable:
    type Item
    type Iterator

    fun __iterate_mut__(
        self @&mut Self,
    ) @Iterator
```

Mutable iteration is selected only when the complete iterable expression is explicitly borrowed with `&mut`.

```arkoi
for item @&mut Element in &mut collection:
    modify(item)
```

Ordinary iteration continues to select `Iterable`, even when the collection binding itself is mutable.

```arkoi
collection @mut Collection = create_collection()

for item @&Element in collection:
    inspect(item)
```

A mutable binding alone does not select `MutableIterable`.

Read-only borrowing also selects ordinary `Iterable`.

```arkoi
for item @&Element in &collection:
    inspect(item)
```

Therefore:

- `for item @Item in collection` selects `Iterable`.
- `for item @Item in &collection` selects `Iterable`.
- `for item @Item in &mut collection` selects `MutableIterable`.

Selecting `MutableIterable` requires a stable mutable receiver. An immutable receiver or temporary receiver is a compile-time error.

```arkoi
for item @&mut Element in &mut immutable_collection:
    # Compile-time error

for item @&mut Element in &mut create_collection():
    # Compile-time error
```

The `Iterator` associated type bound by `MutableIterable` must be a named aggregate implementing either `Iterator` or `FallibleIterator`.

Its `Item` associated type must exactly match `MutableIterable.Item`.

A typical mutable iterator yields mutable references.

```arkoi
implements MutableIterable for Buffer:
    type Item = &mut Byte
    type Iterator = BufferMutIterator

implements Iterator for BufferMutIterator:
    type Item = &mut Byte
```

The concrete hook definition remains external.

```arkoi
fun Buffer.__iterate_mut__(
    self @&mut Buffer,
) @BufferMutIterator:
    # ...
```

The iterator may retain a mutable receiver-derived view for the duration of the loop. Ordinary limited lexical lifetime rules reject obvious movement, replacement, or destruction of the collection while the iterator remains live.

Arkoi does not impose Rust-style exclusivity. Mutable aliases may exist where the ordinary reference rules permit them, and the type author remains responsible for storage stability.

Conceptually, mutable iteration lowers to a direct call to `__iterate_mut__`, followed by direct calls to the concrete iterator's `__next__`.

Built-in mutable arrays and mutable slices retain compiler-defined mutable iteration behavior and do not need explicit `MutableIterable` implementations.

### 11.8 Fallible Mutable Iterator Creation

Fallible mutable iterator creation uses the compiler-recognized static `FallibleMutableIterable` interface.

```arkoi
interface FallibleMutableIterable:
    type Item
    type Iterator
    type Failure

    fun __iterate_mut__(
        self @&mut Self,
    ) !Failure @Iterator
```

The `Failure` associated type must name a failure type.

`FallibleMutableIterable` is selected only when mutable iteration is explicitly requested with `&mut` and the loop uses `for!`.

```arkoi
for! item @&mut Element in &mut collection:
    modify(item)
```

The returned concrete iterator type must be a named aggregate implementing exactly one of:

- `Iterator`
- `FallibleIterator`

Its `Item` associated type must exactly match `FallibleMutableIterable.Item`.

This permits both combinations:

- Fallible mutable iterator creation with infallible advancement.
- Fallible mutable iterator creation with fallible advancement.

Example with fallible creation and infallible advancement:

```arkoi
implements FallibleMutableIterable for Buffer:
    type Item = &mut Byte
    type Iterator = BufferMutIterator
    type Failure = LockFail

implements Iterator for BufferMutIterator:
    type Item = &mut Byte
```

```arkoi
fun Buffer.__iterate_mut__(
    self @&mut Buffer,
) !LockFail @BufferMutIterator:
    # ...
```

If the returned iterator instead implements `FallibleIterator`, failures from both `__iterate_mut__` and `__next__` propagate through the same `for!` loop.

The enclosing function must declare a failure effect containing every failure that mutable iterator creation or advancement may produce.

Using ordinary `for` with `FallibleMutableIterable` is a compile-time error.

Using `for!` with `MutableIterable` and an infallible `Iterator` is a compile-time error because no selected iteration operation is fallible.

If `__iterate_mut__` fails, the loop body is never entered.

If the mutable iterator is created successfully, its lifetime, cleanup, receiver-stability requirements, and direct static `__next__` calls follow the ordinary mutable custom-iteration rules.

Built-in mutable array and slice iteration remains infallible and does not use `FallibleMutableIterable`.

### 11.9 Iterators Yielding Owned Resources

A custom iterator may yield newly owned resource values.

```arkoi
implements FallibleIterator for FileLoader:
    type Item = File
    type Failure = IOFail
```

```arkoi
fun FileLoader.__next__(
    self @&mut FileLoader,
) !IOFail @?File:
    # ...
```

The iterator returns ownership of each present resource item to the loop.

```arkoi
for! file @File in loader:
    process(&file)
```

For each iteration:

- The loop binding owns the yielded resource.
- The binding is initialized from the present value returned by `__next__`.
- The resource is cleaned up at the end of that iteration unless ownership has been transferred.
- `continue` cleans up the current item before requesting the next item.
- `break`, `return`, failure propagation, and other control-flow exits clean up the current item before leaving its scope.
- Cleanup follows Arkoi's ordinary deterministic lexical cleanup rules.

The loop body may transfer ownership from the binding explicitly.

```arkoi
for! file @File in loader:
    archive(move(file))
```

After `move(file)`, the loop binding is uninitialized and the loop does not clean up that item again.

The ordinary rules for moved bindings continue to apply. Using the loop binding after moving it is a compile-time error unless a mutable binding is validly reinitialized under the ordinary binding rules.

A yielded owned resource is distinct from a borrowed resource item.

```arkoi
type Item = &File      # Borrowed read-only item
type Item = &mut File  # Borrowed mutable item
type Item = File       # Newly owned item
```

The iterator implementation is responsible for producing a valid owned resource value. It may construct a new resource, transfer one from its own internal state, or obtain ownership through another explicit Arkoi ownership operation.

Built-in arrays and slices do not implicitly yield owned resource elements. Their resource elements remain places and must be iterated by reference. The owned-item rule applies only when a custom iterator's `Item` associated type is itself a resource value returned by `__next__`.

If constructing or obtaining the next resource may fail, that failure is expressed through `FallibleIterator` and propagated by `for!`.

### 11.10 Validity of References Yielded by Iterators

Custom iterators may yield read-only or mutable references.

```arkoi
implements Iterator for BufferIterator:
    type Item = &Byte
```

```arkoi
implements Iterator for BufferMutIterator:
    type Item = &mut Byte
```

Arkoi does not perform advanced lifetime analysis to determine whether a yielded reference points into:

- The original iterable.
- The iterator's own internal storage.
- Reusable temporary storage.
- Another externally owned object.

The iterator author is responsible for ensuring that every yielded reference remains valid for every way in which ordinary Arkoi code may use or store that reference.

A yielded reference may be copied into an outer binding or stored in another collection when its type otherwise permits it.

```arkoi
matches @mut ReferenceCollection = create_collection()

for item @&Item in source:
    if matches_filter(item):
        ReferenceCollection.append(&mut matches, item)
```

The references may remain in `matches` after the loop. The programmer and iterator implementation are responsible for ensuring that their referents remain alive and stable.

Arkoi applies only its ordinary limited lexical reference checks. It may reject obvious invalidation, such as moving or destroying a directly known source while a visibly derived reference remains live, but it does not prove general iterator-reference safety.

In particular, the compiler does not automatically expire a yielded reference at the end of an iteration or before the next `__next__` call.

An iterator that yields a reference to reusable internal storage must document and enforce any restrictions required for correct use. Returning such a reference when later iterator advancement can invalidate already stored references is programmer error and may cause undefined behavior.

The same responsibility applies to mutable yielded references. The iterator author must ensure:

- The referenced storage remains valid.
- Mutation through the reference is supported by the underlying storage.
- Advancing or destroying the iterator does not invalidate live references unless the program itself ensures they are no longer used.
- Aliasing and overlap remain within Arkoi's ordinary permissive reference model.

This design deliberately favors a simpler compiler and explicit low-level programmer responsibility over Rust-style borrow and lifetime analysis.

### 11.11 Consuming Custom Iteration

Consuming custom iteration uses the compiler-recognized static `OwningIterable` interface.

```arkoi
interface OwningIterable:
    type Item
    type Iterator

    fun __into_iterator__(
        self @own Self,
    ) @Iterator
```

Consuming iteration is selected only when the complete iterable expression explicitly transfers ownership with `move(...)`.

```arkoi
for file @File in move(files):
    process(&file)
```

The source binding becomes uninitialized when ownership is transferred into the loop.

```arkoi
files @mut FileCollection = load_files()!

for file @File in move(files):
    process(&file)

inspect(&files)  # Compile-time error until files is validly reinitialized
```

Ordinary iteration does not consume the collection.

```arkoi
for file @&File in files:
    inspect(file)
```

Mutable iteration borrows rather than consumes it.

```arkoi
for file @&mut File in &mut files:
    modify(file)
```

Therefore:

- `for item @Item in collection` selects `Iterable`.
- `for item @Item in &collection` selects `Iterable`.
- `for item @Item in &mut collection` selects `MutableIterable`.
- `for item @Item in move(collection)` selects `OwningIterable`.

`__into_iterator__` uses an owning receiver because consuming iteration takes
ownership of its resource iterable.

The `Iterator` associated type must be a named aggregate implementing either `Iterator` or `FallibleIterator`.

Its `Item` associated type must exactly match `OwningIterable.Item`.

```arkoi
implements OwningIterable for FileCollection:
    type Item = File
    type Iterator = FileCollectionIterator

implements Iterator for FileCollectionIterator:
    type Item = File
```

The concrete definition remains an ordinary external method.

```arkoi
fun FileCollection.__into_iterator__(
    self @own FileCollection,
) @FileCollectionIterator:
    # Transfer the collection's owned state into the iterator
```

The hidden iterator owns whatever state was transferred from the consumed collection and is cleaned up according to ordinary lexical resource-cleanup rules when the loop ends.

The iterator may yield:

- Freely copyable data values.
- Read-only or mutable references, subject to the programmer-enforced iterator-reference validity contract.
- Newly owned resource values.

When an owned resource item is yielded, the loop binding owns that item and cleans it up at the end of the iteration unless ownership is explicitly transferred with `move(item)`.

```arkoi
for file @File in move(files):
    archive(move(file))
```

The consumed source is not automatically reconstructed after the loop. A mutable source binding may be reinitialized later according to Arkoi's ordinary moved-binding rules; an immutable source binding may not.

Built-in arrays and slices do not provide consuming iteration.

### 11.12 Fallible Consuming Iterator Creation

Fallible consuming iterator creation uses the compiler-recognized static `FallibleOwningIterable` interface.

```arkoi
interface FallibleOwningIterable:
    type Item
    type Iterator
    type Failure

    fun __into_iterator__(
        self @own Self,
    ) !Failure @Iterator
```

The `Failure` associated type must name a failure type.

`FallibleOwningIterable` is selected only when ownership is explicitly transferred with `move(...)` and the loop uses `for!`.

```arkoi
for! file @File in move(files):
    process(&file)
```

The returned concrete iterator type must be a named aggregate implementing exactly one of:

- `Iterator`
- `FallibleIterator`

Its `Item` associated type must exactly match `FallibleOwningIterable.Item`.

This permits both combinations:

- Fallible consuming iterator creation with infallible advancement.
- Fallible consuming iterator creation with fallible advancement.

Example:

```arkoi
implements FallibleOwningIterable for Archive:
    type Item = File
    type Iterator = ArchiveIterator
    type Failure = ArchiveFail

implements FallibleIterator for ArchiveIterator:
    type Item = File
    type Failure = IOFail
```

```arkoi
fun Archive.__into_iterator__(
    self @own Archive,
) !ArchiveFail @ArchiveIterator:
    # ...
```

A `for!` loop propagates failures from:

- The initial `__into_iterator__` call.
- Any fallible `__next__` call on the returned iterator.

The enclosing function must declare a failure effect containing every failure that creation or advancement may produce, directly or through a combined failure set.

Ownership transfers from the source binding before `__into_iterator__` begins executing.

Therefore, if `__into_iterator__` fails:

- The source binding remains uninitialized.
- Ownership is not restored automatically.
- The function must clean up every resource still owned by its `value` parameter before propagating the failure.
- Any resource state already transferred into another owning value must be cleaned up by that value under ordinary unwinding rules.
- No partially created iterator becomes visible to the loop.

This follows Arkoi's general rule that ownership transfer is not rolled back by failure.

Using ordinary `for` with `FallibleOwningIterable` is a compile-time error.

Using `for!` with `OwningIterable` and an infallible `Iterator` is a compile-time error because no selected iteration operation is fallible.

If iterator creation succeeds, the hidden iterator owns the transferred state and follows the ordinary consuming-iteration cleanup rules.

Built-in arrays and slices do not use `FallibleOwningIterable` in Arkoi.

### 11.13 Multiple Iteration Modes and Unambiguous Selection

A named aggregate type may support any combination of the three iteration receiver modes:

- Read-only iteration.
- Mutable iteration.
- Consuming iteration.

The loop syntax selects the receiver mode unambiguously.

```arkoi
for item @&Item in collection:
    # Read-only iteration

for item @&mut Item in &mut collection:
    # Mutable iteration

for item @Item in move(collection):
    # Consuming iteration
```

The three modes may bind different `Item` and `Iterator` associated types.

```arkoi
implements Iterable for FileCollection:
    type Item = &File
    type Iterator = FileIterator

implements MutableIterable for FileCollection:
    type Item = &mut File
    type Iterator = FileMutIterator

implements OwningIterable for FileCollection:
    type Item = File
    type Iterator = FileOwningIterator
```

This permits one collection to yield:

- Read-only references when borrowed normally.
- Mutable references when explicitly borrowed with `&mut`.
- Owned values when explicitly consumed with `move(...)`.

For each receiver mode, a concrete type may implement at most one iterator-creation interface.

Read-only mode permits exactly one of:

- `Iterable`
- `FallibleIterable`

Mutable mode permits exactly one of:

- `MutableIterable`
- `FallibleMutableIterable`

Consuming mode permits exactly one of:

- `OwningIterable`
- `FallibleOwningIterable`

Implementing both creation interfaces for the same mode is a compile-time error.

```arkoi
implements Iterable for Collection
implements FallibleIterable for Collection:
    type Failure = IterationFail
# Compile-time error: competing read-only creation protocols
```

Likewise, each concrete iterator type may implement exactly one advancement interface:

- `Iterator`
- `FallibleIterator`

Implementing both for the same iterator type is a compile-time error.

```arkoi
implements Iterator for CollectionIterator:
    type Item = &Item

implements FallibleIterator for CollectionIterator:
    type Item = &Item
    type Failure = IterationFail
# Compile-time error: competing advancement protocols
```

Loop fallibility is determined from the single selected creation interface and the single selected iterator advancement interface:

- `for` is required when both creation and advancement are infallible.
- `for!` is required when either creation or advancement is fallible.
- `for!` is invalid when both are infallible.
- `for` is invalid when either is fallible.

The associated `Item` type must match exactly between the selected creation interface and the selected iterator interface.

Associated `Item` types do not need to match across different receiver modes.

This preserves flexible collection behavior while preventing competing protocols from making loop selection ambiguous.

### 11.14 Consuming Iteration over Resource Temporaries

A newly produced resource temporary may enter consuming iteration directly.

```arkoi
for item @Item in create_collection():
    process(item)
```

The temporary's ownership transfers directly into the selected `OwningIterable` implementation.

A named resource binding still requires explicit `move(...)`.

```arkoi
collection @Collection = create_collection()

for item @Item in move(collection):
    process(item)
```

Omitting `move(...)` from a named resource binding selects ordinary read-only iteration rather than consuming iteration, when `Iterable` is available.

```arkoi
for item @&Item in collection:
    inspect(item)
```

If the named resource type does not support the selected non-consuming mode, the loop is a compile-time error. Arkoi never inserts an implicit move from a named binding.

The selection rules are:

- A named resource binding selects `Iterable` unless explicitly borrowed with `&mut` or consumed with `move(...)`.
- `&mut binding` selects `MutableIterable`.
- `move(binding)` selects `OwningIterable`.
- A resource temporary selects `OwningIterable` directly.
- A data temporary selects ordinary `Iterable`, because data values may be copied normally.

```arkoi
for item @DataItem in create_data_collection():
    process(item)
```

A resource temporary used for consuming iteration is evaluated exactly once. If iterator creation succeeds, the hidden iterator owns the transferred resource state. If iterator creation is fallible, `FallibleOwningIterable` is selected and the loop must use `for!`.

```arkoi
for! item @Item in create_fallible_collection():
    process(item)
```

If producing the temporary expression itself is fallible, that expression still uses Arkoi's ordinary postfix propagation or explicit failure handling.

```arkoi
for! item @Item in load_collection()!:
    process(item)
```

In this form:

- The postfix `!` propagates failure from `load_collection()`.
- The `for!` propagates failure from `__into_iterator__` or `__next__`.
- The temporary is transferred into consuming iteration only after its expression has completed successfully.

Applying `move(...)` to a resource temporary remains invalid because `move(...)` is restricted to whole named resource bindings.

```arkoi
for item @Item in move(create_collection()):
    # Compile-time error
```

If a resource temporary supports only non-consuming iteration, it cannot be used directly in a `for` loop unless it is first stored in a named binding. Direct temporary iteration always selects the consuming mode for resources.

### 11.15 Lifetime of Data Temporaries During Iteration

When the iterable expression of a `for` loop produces a data temporary, Arkoi keeps that temporary alive for the entire loop.

```arkoi
for item @&Item in create_data_collection():
    inspect(item)
```

Conceptually, the compiler stores the temporary in a hidden immutable binding before creating the iterator.

```arkoi
hidden_collection @DataCollection = create_data_collection()
hidden_iterator @mut DataIterator =
    DataCollection.__iterate__(&hidden_collection)

loop:
    next @?&Item = DataIterator.__next__(&mut hidden_iterator)

    if next == none:
        break

    item @&Item = next!
    inspect(item)
```

The exact lowering is implementation-defined, but the observable lifetime behavior is fixed.

The hidden data temporary:

- Is evaluated exactly once.
- Is immutable.
- Remains alive while the hidden iterator exists.
- Is destroyed when the loop exits.
- Is cleaned up through ordinary lexical control-flow rules.

Its lifetime ends when the loop exits through:

- Normal exhaustion.
- `break`.
- `return`.
- Failure propagation.
- Any other control-flow exit.

This rule allows an iterator to borrow the data temporary or yield references into it for use during the loop.

The rule is specific to the iterable expression of a `for` or `for!` statement. It does not introduce general temporary lifetime extension.

```arkoi
item @&Item = &create_data_collection().first
# Compile-time error under the ordinary temporary-reference rules
```

References yielded from the iterator may still escape the loop under Arkoi's programmer-enforced iterator-reference validity rules. If such a reference outlives the hidden data temporary, using it is programmer error and may cause undefined behavior.

Resource temporaries follow the separate consuming-iteration rule. Their ownership transfers into the hidden iterator, so they do not use this hidden borrowed-source binding model.

### 11.16 Mutable Loop Bindings

A `for` or `for!` loop binding may use Arkoi's ordinary `@mut` binding modifier
when its type permits binding mutability.

```arkoi
for value @mut u32 in numbers:
    value += 1
```

The loop binding is a fresh local binding initialized for each iteration.

When the iterator yields a freely copyable data value, mutating the loop binding changes only that local copy. It does not modify the source collection or iterator state.

```arkoi
for value @mut u32 in numbers:
    value += 1
    # The corresponding element in numbers is unchanged
```

To mutate the actual underlying element, the selected iterator must yield a mutable reference.

```arkoi
for value @&mut u32 in &mut numbers:
    value += 1
```

The reference remains bound to the yielded element for that iteration, while
`&mut u32` permits writing the element directly.

For an owned resource item, `@mut Resource` creates a mutable local owning binding.

```arkoi
for file @mut File in move(files):
    file = open_replacement()!
```

Reassignment, movement, reinitialization, and cleanup follow Arkoi's ordinary resource-binding rules:

- Reassignment constructs the replacement before dropping the old resource.
- `move(file)` leaves the loop binding uninitialized.
- A mutable loop binding may be validly reinitialized after a move.
- An immutable loop binding may not be reinitialized after a move.
- Any still-owned resource is cleaned up when the iteration scope exits.

The loop binding's declared type must still exactly match the selected iterator's `Item` associated type except for the binding-level `mut` modifier, which does not change the value type.

A new loop binding is created for each iteration, and its scope is the loop body for that iteration.

### 11.17 Iterator Exhaustion

A `for` or `for!` loop stops immediately when the selected iterator's `__next__` operation returns `none`.

```arkoi
next @?Item = Iterator.__next__(&mut iterator)

if next == none:
    break
```

After observing `none`, the loop does not call `__next__` again.

Arkoi does not require an iterator to remain permanently exhausted after returning `none`.

Manual calls made outside the completed loop may return:

- `none` again.
- Another item.
- A failure, when using `FallibleIterator`.

The result is determined by the iterator type's own documented contract.

```arkoi
first @?Item = Iterator.__next__(&mut iterator)

if first == none:
    later @?Item = Iterator.__next__(&mut iterator)
    # The language does not require later to be none
```

The compiler does not insert permanent-exhaustion state, cache the first `none`, or enforce fused-iterator behavior.

An iterator that promises permanent exhaustion may implement that behavior itself, but it is not part of the core `Iterator` or `FallibleIterator` contract.

The `for` loop's behavior remains deterministic: it stops at the first `none` produced during that loop and performs no further advancement calls.

### 11.18 Canonical Source-Defined Iteration Interfaces

The compiler-recognized iteration interfaces are declared as ordinary Arkoi source code in the bundled core library.

For example, the core library may contain a module such as:

```arkoi
module core.iteration

pub interface Iterator:
    type Item

    fun __next__(
        self @&mut Self,
    ) @?Item

pub interface Iterable:
    type Item
    type Iterator

    fun __iterate__(
        self @&Self,
    ) @Iterator
```

The remaining iteration interfaces are declared in the same way:

- `FallibleIterator`
- `MutableIterable`
- `FallibleIterable`
- `FallibleMutableIterable`
- `OwningIterable`
- `FallibleOwningIterable`

These interfaces use normal Arkoi interface syntax and may be inspected as source.

The compiler recognizes the canonical declarations by their resolved declaration identity in the bundled core library, not merely by:

- Their unqualified names.
- Their qualified spelling alone.
- Their member names.
- Structural similarity.

A user-defined interface with the same name or shape is an ordinary interface and does not activate `for` behavior.

```arkoi
module application.custom

interface Iterator:
    type Item

    fun __next__(
        self @&mut Self,
    ) @?Item
```

`application.custom.Iterator` is distinct from the canonical `core.iteration.Iterator`.

The canonical iteration interfaces are automatically available through Arkoi's core prelude or equivalent implicit core-module mechanism. User modules do not need to import them solely to write standard implementations.

```arkoi
implements Iterable for Collection:
    type Item = &Element
    type Iterator = CollectionIterator
```

The exact physical module path and compiler packaging mechanism are implementation details, but the language guarantees one canonical set of iteration-interface declarations.

The bundled declarations are required language-support definitions rather than an optional library feature. A compiler installation must provide compatible canonical declarations for `for` and `for!` semantics to function.

Users cannot replace or redefine the canonical declarations used by the compiler. Alternative interfaces may coexist under other module identities but have no special loop behavior.

This design keeps the protocol visible and source-defined while ensuring that compiler behavior depends on stable declaration identity rather than accidental names or structure.

### 11.19 Built-In Array and Slice Iteration

Built-in fixed arrays and slices have compiler-defined iteration behavior.

The compiler implementation may internally reuse the same lowering, contracts, helper machinery, or semantic model used for the canonical core-library iteration interfaces.

However, arrays and slices do not expose user-visible core-library implementation declarations.

There are no source-level declarations such as:

```arkoi
implements Iterable for []Element
implements MutableIterable for []mut Element
implements Iterable for [N]Element
```

Likewise, the core library does not expose synthetic array- or slice-iterator aggregate types solely to support built-in `for` behavior.

The exact internal representation and lowering are implementation-specific.

The language guarantees the observable built-in behavior:

- Read-only arrays and slices support ordinary iteration.
- Mutable arrays and mutable slices support explicit mutable iteration.
- Their iteration is infallible.
- Their element-binding and ownership behavior follows the existing array and slice rules.
- Resource elements are yielded by reference and are never implicitly moved.
- Bounds and traversal order follow the built-in array and slice semantics.
- No explicit interface implementation is required or visible.

A compiler may internally model built-in iteration as though the types satisfied the canonical iteration contracts, but this is not a source-level conformance relationship.

Therefore:

- Users cannot inspect, override, replace, or conflict with built-in array or slice iteration implementations.
- User-defined interfaces with similar declarations do not affect built-in iteration.

The canonical iteration interfaces remain part of the required core library. Built-in array and slice support remains part of the language/compiler and may depend on equivalent internal machinery without exposing core-library implementations.

### 11.20 Iteration Through Existing References

Reference types do not implement iteration interfaces themselves.

Instead, `for` protocol selection transparently uses the referenced named aggregate type.

A read-only reference selects the referent type's read-only iteration mode.

```arkoi
collection @&Collection = get_collection()

for item @&Item in collection:
    inspect(item)
```

This selects `Iterable for Collection`. Arkoi does not require or permit a separate implementation for `&Collection`.

```arkoi
implements Iterable for &Collection  # Compile-time error
```

A mutable reference selects the referent type's mutable iteration mode.

```arkoi
collection @&mut Collection = get_mutable_collection()

for item @&mut Item in collection:
    modify(item)
```

This selects `MutableIterable for Collection` directly. `&mut collection` is a
mutable reborrow of the same referent and selects the same mode when a distinct
reborrow is needed.

The selection rules are:

- An expression of type `T` selects the ordinary rules for a direct value of `T`.
- An expression of type `&T` selects the read-only iteration mode of `T`.
- An expression of type `&mut T` selects the mutable iteration mode of `T`.
- A reference expression never selects consuming iteration.

The referenced type must be a named aggregate with the required canonical iteration implementation, or a built-in array or slice with compiler-defined iteration behavior.

The iterable reference expression is evaluated exactly once.

The hidden iterator receives the existing reference-derived access to the referent. Arkoi does not create an independent copy of the referenced aggregate.

Reference mutability determines the available mode:

```arkoi
readonly @&Collection = get_collection()
mutable @&mut Collection = get_mutable_collection()

for item @&Item in readonly:
    inspect(item)

for item @&mut Item in mutable:
    modify(item)
```

A read-only reference cannot select mutable iteration.

```arkoi
for item @&mut Item in readonly:
    # Compile-time error
```

This rule also applies to references to built-in arrays and slices.

```arkoi
values @&[16]u32 = get_array()

for value @u32 in values:
    process(value)
```

Reference-transparent iteration is a special `for` protocol-selection rule. It does not make reference types nominal interface implementations and does not change the general restriction that references cannot implement interfaces.

## 12. Indexing, Slicing, and Length

### 12.1 User-Defined Indexing

Data and resource aggregates may define read-only and mutable indexing hooks.

Read-only indexing uses `__index__`.

```arkoi
fun Container.__index__(
    self @&Container,
    index @usize,
) !CoreFail @&Element:
```

Mutable indexing uses `__index_mut__`.

```arkoi
fun Container.__index_mut__(
    self @&mut Container,
    index @usize,
) !CoreFail @&mut Element:
```

The hooks may be overloaded by exact index-parameter signature under the ordinary function-overloading rules.

Read-only contexts select `__index__`.

```arkoi
element @&Element = container[index]!
```

Mutable-place contexts select `__index_mut__`.

```arkoi
container[index]!.field = value
element @&mut Element = &mut container[index]!
container[index]! += amount
container[index]!.mutating_method()
```

A type may define only `__index__`, only `__index_mut__`, or both. An unavailable indexing mode is a compile-time error.

The mutable indexing receiver must be a stable mutable place. A temporary or immutable receiver cannot be used for mutable indexing.

```arkoi
create_container()[index]! = value  # Compile-time error
```

A custom indexing hook returns a reference to an actual existing place. It must not return a reference to:

- A local variable inside the hook.
- A temporary value.
- Storage whose lifetime ends when the hook returns.
- Unrelated storage with a shorter lifetime than the receiver.

The returned reference lifetime is tied to the receiver.

```arkoi
element @&mut Element = &mut container[index]!
```

The compiler applies the same limited lexical lifetime checks used for other receiver-derived references. It rejects obvious moves, replacement, or destruction of the receiver while a derived reference remains live.

```arkoi
element @&mut Element = &mut container[index]!

container = create_container()  # Compile-time error while element is live
discard(move(container))        # Compile-time error while element is live
```

Arkoi does not provide Rust-style exclusivity guarantees. Multiple references may alias according to Arkoi's ordinary reference rules.

The type author is responsible for ensuring that storage returned by an indexing hook remains valid for the permitted lifetime. A type whose elements may be relocated or invalidated in ways it cannot safely expose should omit `__index_mut__` and provide named mutation operations instead.

A mutable indexing result is a true assignable place.

```arkoi
container[index()] = create_element()!
```

Assignment evaluation follows the ordinary destination-first rule:

1. Evaluate the receiver.
2. Evaluate index arguments from left to right.
3. Invoke `__index_mut__` and resolve the destination place.
4. Evaluate and fully construct the right-hand side.
5. If construction fails, leave the old element unchanged.
6. Otherwise replace the destination using the normal assignment rules.

For resource elements, replacement retains construct-first, drop-and-replace behavior.

Compound assignment resolves the indexed place exactly once and then applies the corresponding binary operator hook.

Built-in arrays and slices continue to use their compiler-defined indexing semantics rather than these user-defined hooks.

### 12.2 Multi-Argument Indexing

Indexing syntax accepts one or more comma-separated index expressions.

```arkoi
container[index]
matrix[row, column]
tensor[x, y, z]
custom[a, b, c, d]
```

The parser accepts an index argument list without knowing which arities the receiver type supports.

Conceptually:

```text
index expression:
    receiver "[" argument ("," argument)* "]"
```

An empty index argument list is invalid.

```arkoi
container[]  # Compile-time error
```

Each `__index__` or `__index_mut__` declaration remains fixed-arity and statically typed.

```arkoi
fun Tensor.__index__(
    self @&Tensor,
    index @usize,
) !CoreFail @&Element:
```

```arkoi
fun Tensor.__index__(
    self @&Tensor,
    row @usize,
    column @usize,
) !CoreFail @&Element:
```

```arkoi
fun Tensor.__index__(
    self @&Tensor,
    x @usize,
    y @usize,
    z @usize,
) !CoreFail @&Element:
```

A type may define any set of supported fixed arities and ordered index-argument types.

```arkoi
tensor[index]!
tensor[row, column]!
tensor[x, y, z]!
```

Index-hook overload selection uses:

- The indexing mode: read-only or mutable.
- The number of index arguments.
- The ordered index-argument types.
- The ordinary exact overload-resolution rules.

An unsupported argument count or type combination is a compile-time error.

```arkoi
tensor[x, y]!  # Compile-time error when no matching two-index overload exists
```

Mutable indexing resolves independently through `__index_mut__`.

```arkoi
tensor[x, y, z]! = value
```

requires a matching mutable overload:

```arkoi
fun Tensor.__index_mut__(
    self @&mut Tensor,
    x @usize,
    y @usize,
    z @usize,
) !CoreFail @&mut Element:
```

A type may support an arity for read-only indexing without supporting the same arity for mutable indexing, or vice versa.

Index arguments evaluate from left to right in written order after the receiver and before the hook call.

Flexible index-list syntax does not introduce variadic functions. Every indexing hook has a fixed parameter list, and the compiler imposes no indexing-specific arity limit beyond its general implementation limit for function parameters.

### 12.3 Failure Handling for Indexing

Indexing follows Arkoi's universal failure-expression rules.

Whether an indexing expression is fallible depends on the selected `__index__` or `__index_mut__` overload.

A fallible indexing hook declares a failure effect.

```arkoi
fun Container.__index__(
    self @&Container,
    index @usize,
) !CoreFail @&Element:
```

Calling that hook through indexing produces a fallible expression.

The caller must either propagate the failure with postfix `!`:

```arkoi
element @&Element = container[index]!
```

or handle the failure explicitly:

```arkoi
element @&Element = container[index] handle failure:
    yield &fallback
```

Omitting propagation or handling is a compile-time error.

```arkoi
element @&Element = container[index]  # Compile-time error
```

An infallible indexing hook declares no failure effect.

```arkoi
fun Table.__index__(
    self @&Table,
    index @usize,
) @&Element:
```

Its indexing expression requires no postfix `!`.

```arkoi
element @&Element = table[index]
```

Applying postfix `!` to an infallible indexing expression is a compile-time error.

```arkoi
element @&Element = table[index]!  # Compile-time error
```

The same rule applies to mutable indexing.

```arkoi
container[index]! = replacement  # Fallible __index_mut__
table[index] = replacement       # Infallible __index_mut__
```

For fallible mutable indexing, destination resolution must propagate or handle the failure before the right-hand side is evaluated. If destination resolution fails, the right-hand side is not evaluated.

There is no indexing-specific meaning for `!`. It is the same postfix failure-propagation operator used by every fallible Arkoi expression or operation.

### 12.4 User-Defined Slicing

Data and resource aggregates may define dedicated read-only and mutable slicing hooks.

Read-only slicing uses `__slice__`.

```arkoi
fun Container.__slice__(
    self @&Container,
    start @usize,
    end @usize,
) !CoreFail @ContainerView:
```

Mutable slicing uses `__slice_mut__`.

```arkoi
fun Container.__slice_mut__(
    self @&mut Container,
    start @usize,
    end @usize,
) !CoreFail @MutableContainerView:
```

Slice syntax is distinct from ordinary indexing syntax.

```arkoi
container[index]       # Indexing
container[start..end]  # Slicing
```

The supported slice forms are:

```arkoi
container[start..end]
container[..end]
container[start..]
container[..]
```

Read-only contexts select `__slice__`. Mutable contexts select `__slice_mut__`.

A type may define only read-only slicing, only mutable slicing, or both. An unavailable slicing mode is a compile-time error.

Slicing hooks:

- May be overloaded under the ordinary exact overload-resolution rules.
- Follow the same receiver-stability and lifetime rules as indexing hooks.
- May be fallible or infallible.
- Use postfix `!` only when the selected hook is fallible and failure is propagated.
- Evaluate the receiver first and then bound expressions from left to right.
- Are separate from `__index__` and `__index_mut__`; range syntax never resolves through ordinary indexing hooks.

Mutable slicing requires a stable mutable receiver and may return a mutable view or another receiver-tied mutable reference type.

Built-in arrays and slices continue to use their compiler-defined slicing behavior rather than user-defined hooks.

The exact parameter representation used for omitted bounds is specified separately.

### 12.5 Slice Bound Representation

User-defined slicing hooks receive both slice bounds as optionals.

```arkoi
fun Container.__slice__(
    self @&Container,
    start @?usize,
    end @?usize,
) !CoreFail @ContainerView:
```

Mutable slicing uses the same bound representation.

```arkoi
fun Container.__slice_mut__(
    self @&mut Container,
    start @?usize,
    end @?usize,
) !CoreFail @MutableContainerView:
```

The four slice forms map to hook arguments as follows:

```text
container[start..end]  -> start = start, end = end
container[..end]       -> start = none,  end = end
container[start..]     -> start = start, end = none
container[..]          -> start = none,  end = none
```

Present bounds are evaluated from left to right in written order. An omitted bound performs no evaluation and supplies `none`.

The hook determines the semantic meaning of omitted bounds for the custom type. Conventionally, `none` for `start` means the beginning and `none` for `end` means the logical end.

A slicing hook therefore uses one consistent two-bound signature rather than separate overloads for each open-bound form.

The bounds remain half-open: a present `end` identifies the first excluded position.

### 12.6 Slicing Hook Return Types

User-defined `__slice__` and `__slice_mut__` hooks may return any valid Arkoi type.

```arkoi
fun Container.__slice__(
    self @&Container,
    start @?usize,
    end @?usize,
) !CoreFail @ContainerView:
```

A slicing hook may return:

- A non-owning view tied to the receiver.
- A reference tied to the receiver.
- A built-in slice tied to the receiver.
- A copied data value.
- A newly constructed independent resource.
- Any other type permitted by the ordinary type, ownership, and lifetime rules.

A result derived from the receiver must not outlive the receiver.

```arkoi
fun Container.__slice__(
    self @&Container,
    start @?usize,
    end @?usize,
) @[]Element:
```

The returned slice's lifetime is tied to `self`.

A slicing hook may instead construct and return an independently owned resource.

```arkoi
fun Container.__slice_copy__(
    self @&Container,
    start @?usize,
    end @?usize,
) !CoreFail @Buffer:
```

Such an independently owned result may outlive the receiver because it does not borrow receiver storage.

`__slice_mut__` is selected by mutable slicing context but is not required to return a mutable reference or mutable view.

```arkoi
fun Container.__slice_mut__(
    self @&mut Container,
    start @?usize,
    end @?usize,
) !CoreFail @MutableContainerView:
```

Its return type may be mutable, read-only, copied, or independently owned, provided it satisfies the ordinary type, ownership, and lifetime rules.

The hook name determines which slicing mode is selected; the return type does not participate in overload resolution.

### 12.7 Selecting Read-Only or Mutable Slicing

Ordinary slicing syntax selects the read-only `__slice__` hook, even when the receiver is mutable.

```arkoi
view @ContainerView = container[start..end]!
```

Mutable slicing is selected only when the complete slicing expression is explicitly borrowed with `&mut`.

```arkoi
view @&mut MutableContainerView = &mut container[start..end]!
```

The mutable receiver alone does not select `__slice_mut__`.

```arkoi
container @mut Container = create_container()

view @ContainerView = container[start..end]!
# Selects __slice__, not __slice_mut__
```

Read-only borrowing also selects `__slice__`.

```arkoi
view @&ContainerView = &container[start..end]!
```

Therefore:

- `container[start..end]` selects `__slice__`.
- `&container[start..end]` selects `__slice__`.
- `&mut container[start..end]` selects `__slice_mut__`.

Selecting `__slice_mut__` requires a stable mutable receiver. An immutable receiver or temporary receiver is a compile-time error.

```arkoi
&mut immutable_container[start..end]!  # Compile-time error
&mut create_container()[start..end]!   # Compile-time error
```

If the selected hook is fallible, postfix `!` applies to the complete slicing expression before the borrow result is used.

Explicit mutable borrowing prevents a mutable-capable slicing hook from being selected accidentally merely because the receiver happens to be mutable.

### 12.8 User-Defined Length

Data and resource aggregates may define the reserved `__length__` hook.

```arkoi
fun Container.__length__(
    self @&Container,
) @usize:
    return self.element_count
```

A valid `__length__` hook enables the built-in `length(...)` operation for that type.

```arkoi
count @usize = length(container)
```

The `__length__` hook:

- Must belong to the measured aggregate type.
- Must be declared in the same module as the type.
- Must have exactly one parameter: `self @&Type`.
- Must return `usize`.
- Must not declare a failure effect.
- Must not consume or mutate the receiver.
- Need not be public.

A malformed declaration using the reserved `__length__` name is a compile-time error.

`length(value)` evaluates its argument exactly once and then invokes the hook.

Built-in arrays, slices, `string`, and `string_view` retain their compiler-defined infallible `length(...)` behavior and do not use user-defined hooks.

Types without built-in length behavior or a valid `__length__` hook cannot be passed to `length(...)`.

## 13. Static Interfaces

### 13.1 Static Interfaces

Arkoi interfaces are compile-time contracts. They are not runtime value types and do not introduce dynamic dispatch.

An interface declaration contains its associated-type requirements and required function declarations.

```arkoi
pub interface Iterator:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item
```

`Self` denotes the concrete type that implements the interface.

Required function declarations remain inside the interface scope. Their concrete definitions remain ordinary external Arkoi functions or methods.

```arkoi
fun AccountIterator.next(
    self @&mut AccountIterator,
) @?&Account:
    # ...
```

An implementation declaration states that a concrete type intentionally implements an interface.

When no associated-type assignments are required, the declaration is written on one line.

```arkoi
implements hashing.Hashable for Account
```

When associated types must be assigned, the declaration uses a block.

```arkoi
implements iteration.Iterator for AccountIterator:
    type Item = &Account
```

The implementation block contains only interface-related bindings such as associated-type assignments. Function bodies are not defined inside the implementation block.

The compiler substitutes the concrete type and associated-type assignments into the interface requirements and verifies that matching external definitions exist.

```text
Self = AccountIterator
Item = &Account
```

For example, this interface requirement:

```arkoi
fun next(
    self @&mut Self,
) @?Item
```

requires this concrete definition:

```arkoi
fun AccountIterator.next(
    self @&mut AccountIterator,
) @?&Account:
    # ...
```

A missing or mismatched required definition is a compile-time error.

Interfaces may require:

- Associated types.
- Instance methods.
- Associated functions.
- Overloaded functions under Arkoi's ordinary overload rules.

Concrete types may define additional functions not mentioned by the interface.

Conformance is explicit and nominal. Merely having functions with matching names and signatures does not make a type implement an interface.

```arkoi
implements Interface for Type
```

must be present for conformance to exist.

An implementation declaration is permitted only in:

- The module that defines the interface, or
- The module that defines the concrete type.

A third module that owns neither side cannot define the implementation.

```arkoi
# Compile-time error when this module owns neither Hashable nor Account
implements hashing.Hashable for account.Account
```

This prevents unrelated modules from introducing competing implementations for the same interface and concrete type.

Only one implementation of a given interface for a given concrete type may exist in the program.

Interfaces are private by default and may be declared `pub`.

An interface is not a runtime type.

```arkoi
iterator @Iterator  # Compile-time error
```

Only concrete values may be stored, passed, or returned.

```arkoi
iterator @AccountIterator = AccountCollection.iterate(&collection)
```

Interfaces therefore provide no:

- Interface-valued variables.
- Trait objects.
- Virtual method tables.
- Runtime interface casts.
- Runtime implementation lookup.
- Dynamic dispatch.
- Indirect function calls.

Calls through interface requirements are resolved statically to concrete functions during compilation.

### 13.2 Interface Requirement Bodies

Interface scopes contain declarations only.

An interface cannot provide function or method bodies.

```arkoi
interface Comparable:
    fun less(
        self @&Self,
        other @&Self,
    ) @bool

    fun greater(
        self @&Self,
        other @&Self,
    ) @bool
```

A function body inside an interface is a compile-time error.

```arkoi
interface Comparable:
    fun greater(
        self @&Self,
        other @&Self,
    ) @bool:
        return Comparable.less(other, self)  # Compile-time error
```

Every required interface function must have a matching concrete external definition for each implementing type.

```arkoi
fun Version.greater(
    self @&Version,
    other @&Version,
) @bool:
    return Version.less(other, self)
```

Shared implementation logic may be placed in ordinary helper functions and called by the external definitions.

Interfaces therefore define contracts only and never contribute executable code.

### 13.3 Interface Extension

An interface may extend one or more other interfaces.

```arkoi
interface Readable:
    fun read(
        self @&Self,
    ) @u8

interface Writable:
    fun write(
        self @&mut Self,
        value @u8,
    )

interface Stream extends Readable, Writable:
    fun flush(
        self @&mut Self,
    )
```

Extension combines the parent requirements with the declarations written directly in the child interface.

Conceptually, `Stream` above behaves like the following flattened contract:

```arkoi
interface Stream:
    fun read(
        self @&Self,
    ) @u8

    fun write(
        self @&mut Self,
        value @u8,
    )

    fun flush(
        self @&mut Self,
    )
```

A concrete type implementing the child interface must satisfy every inherited and directly declared requirement.

```arkoi
implements Stream for FileStream

fun FileStream.read(
    self @&FileStream,
) @u8:
    # ...

fun FileStream.write(
    self @&mut FileStream,
    value @u8,
):
    # ...

fun FileStream.flush(
    self @&mut FileStream,
):
    # ...
```

Implementing a child interface also makes the type an implementation of every transitive parent interface. A separate `implements` declaration for an inherited parent is unnecessary and is not permitted.

Interface extension is compile-time contract composition only. It provides no:

- Default implementations.
- Runtime inheritance.
- Interface-valued conversions.
- Dynamic dispatch.
- Inherited storage or fields.

The interface-extension graph must be acyclic. Direct or indirect cycles are compile-time errors.

```arkoi
interface First extends Second:
interface Second extends First:  # Compile-time error
```

Multiple inheritance is permitted.

If the same requirement is inherited through more than one path, identical declarations merge into one requirement.

```arkoi
interface Base:
    fun reset(
        self @&mut Self,
    )

interface Left extends Base:
interface Right extends Base:

interface Combined extends Left, Right:
```

`Combined` contains one `reset` requirement.

Inherited declarations conflict when they use the same function identity but
require incompatible return types, exact failure-set types within the fallible
mode, or other non-overload-distinguishing properties. Such a child interface
is invalid.

Associated-type requirements are inherited in the same way.

```arkoi
interface Iterator:
    type Item

interface DoubleEndedIterator extends Iterator:
    fun next_back(
        self @&mut Self,
    ) @?Item
```

The child may use inherited associated types in its declarations.

An implementing type binds inherited and directly declared associated types in its single child implementation declaration.

```arkoi
implements DoubleEndedIterator for BufferIterator:
    type Item = &Byte
```

The same associated-type name inherited through multiple paths merges when it refers to the same originating requirement. Unrelated inherited associated types with the same name conflict and must be renamed at their declaring interfaces; the child cannot disambiguate them.

When generic constraints are introduced, a type implementing a child interface will satisfy constraints requiring any of its parent interfaces.

### 13.4 Sharing One Definition Across Interfaces

A single concrete external function or method may satisfy requirements from multiple implemented interfaces only when the fully substituted requirements match exactly.

```arkoi
interface Resettable:
    fun reset(
        self @&mut Self,
    )

interface Reusable:
    fun reset(
        self @&mut Self,
    )

implements Resettable for Buffer
implements Reusable for Buffer

fun Buffer.reset(
    self @&mut Buffer,
):
    # ...
```

The one `Buffer.reset` definition satisfies both requirements because, after substituting `Self = Buffer`, the required declarations are identical.

Exact matching includes:

- Qualified function or method name.
- Parameter count.
- Ordered parameter types.
- Receiver type and receiver mutability.
- Ownership and reference modes.
- Return type, including the absence of a return type.
- Failure effect, including the absence of a failure effect.
- Any associated-type substitutions appearing in the declaration.

Parameter names do not affect matching, consistent with Arkoi's ordinary function identity rules.

These requirements do not match:

```arkoi
interface Resettable:
    fun reset(
        self @&mut Self,
    )

interface RecoverableReset:
    fun reset(
        self @&mut Self,
    ) !ResetFail
```

A non-fallible definition cannot satisfy the fallible requirement, and a fallible definition cannot satisfy the non-fallible requirement.

Likewise, requirements with different return types, ownership modes, reference mutability, parameter types, or arity cannot share one definition.

Arkoi does not adapt, wrap, convert, weaken, strengthen, or otherwise reinterpret a concrete definition to make interface requirements compatible.

If a concrete type explicitly implements multiple interfaces whose same-named requirements are not exact matches but also cannot coexist under the ordinary overload rules, the implementation set is a compile-time error.

### 13.5 Reserved Hooks in Interfaces

Interfaces may require reserved language hooks.

```arkoi
interface Sized:
    fun __length__(
        self @&Self,
    ) @usize
```

```arkoi
interface Searchable:
    type Item

    fun __contains__(
        self @&Self,
        item @&Item,
    ) @bool
```

A reserved hook declared in an interface must obey the same declaration rules, signature restrictions, ownership rules, mutability rules, return-type requirements, and failure rules as the corresponding hook on a concrete type.

For example, `__length__` must remain read-only, infallible, and return `usize`.

```arkoi
interface Sized:
    fun __length__(
        self @&Self,
    ) @usize
```

An invalid hook declaration is a compile-time error even when it appears inside an interface.

```arkoi
interface InvalidSized:
    fun __length__(
        self @&mut Self,
    ) !LengthFail @u64  # Compile-time error
```

When a concrete type implements such an interface, the matching external hook definition both:

- Satisfies the interface requirement.
- Enables the corresponding language syntax or built-in operation for that concrete type.

```arkoi
implements Sized for Buffer

fun Buffer.__length__(
    self @&Buffer,
) @usize:
    return self.used

count @usize = length(buffer)
```

Likewise, an interface-required `__contains__` hook enables `in`, `not in`, or their fallible forms according to the hook's declared failure effect.

Reserved hook requirements participate in interface extension, associated-type substitution, exact requirement matching, and shared-definition rules in the same way as ordinary interface function requirements.

Interfaces do not themselves provide hook behavior. They only require that implementing concrete types provide matching external hook definitions.

### 13.6 Interface Implementation Visibility

An `implements` declaration has no visibility modifier.

```arkoi
implements Sized for Buffer
```

The following forms are invalid:

```arkoi
pub implements Sized for Buffer      # Compile-time error
private implements Sized for Buffer  # Compile-time error
```

Once a valid implementation declaration is part of the compiled program, the conformance is globally recognized wherever both the interface and concrete type are accessible.

Conformance does not vary by calling module, import path, lexical scope, or visibility context.

This means that when a module can refer to both `Sized` and `Buffer`, it observes the same answer to whether `Buffer` implements `Sized` as every other module in the program.

The interface and concrete type retain their own ordinary visibility rules. An inaccessible private interface or type cannot be named from another module merely because an implementation exists.

The implementation declaration itself is neither public nor private. It is a global compile-time relationship between one interface and one concrete type.

This rule preserves:

- Globally coherent conformance.
- The prohibition on duplicate implementations.
- Static method and hook resolution.
- Import-independent program meaning.

### 13.7 Types Eligible for Interface Implementation

Only named user-defined `data` and `resource` aggregate types may implement interfaces.

```arkoi
data Buffer:
    bytes @[]u8

resource File:
    handle @u64

implements Sized for Buffer
implements Closable for File
```

The concrete implementing type must have its own distinct nominal aggregate identity.

The following kinds of types cannot implement interfaces:

- Transparent type aliases.
- Built-in primitive types.
- References.
- Raw pointers to C ABI types.
- Optional types.
- Fixed arrays.
- Slices.
- Function types, which Arkoi does not support as values.
- Failure types.
- Enum types.
- Other compiler-defined composite types.

```arkoi
type ByteCount = usize

implements Sized for ByteCount  # Compile-time error
implements Sized for []u8       # Compile-time error
implements Sized for [16]u8     # Compile-time error
implements Sized for ?Buffer    # Compile-time error
```

A transparent alias cannot implement an interface because it has no type identity distinct from its aliased type.

Compiler-defined composite types retain only their built-in or element-derived behavior. Users cannot attach interface conformance or external methods to a particular composite instantiation.

Restricting implementations to named aggregates gives every conformance relationship:

- A distinct concrete type identity.
- A clear owning module.
- A natural location for external method and hook definitions.
- Compatibility with the implementation ownership rule.

### 13.8 Associated-Type Bindings

Every associated type required by an interface must be assigned exactly once in the corresponding `implements` declaration.

```arkoi
interface Iterator:
    type Item
    type Position

implements Iterator for BufferIterator:
    type Item = &Byte
    type Position = usize
```

Associated-type bindings are explicit. Arkoi does not infer them from method signatures or other declarations.

There are no default associated types.

A missing binding is a compile-time error.

```arkoi
implements Iterator for BufferIterator:
    type Item = &Byte
    # Compile-time error: Position is not assigned
```

A duplicate binding is a compile-time error.

```arkoi
implements Iterator for BufferIterator:
    type Item = &Byte
    type Item = Byte  # Compile-time error
    type Position = usize
```

Binding a name that is not an associated type required by the interface or one of its transitive parents is a compile-time error.

```arkoi
implements Iterator for BufferIterator:
    type Item = &Byte
    type Position = usize
    type Unknown = u32  # Compile-time error
```

Inherited associated types are assigned in the same child implementation block.

```arkoi
interface Iterator:
    type Item

interface IndexedIterator extends Iterator:
    type Index

implements IndexedIterator for BufferIterator:
    type Item = &Byte
    type Index = usize
```

Each associated-type binding may name any otherwise valid Arkoi type, including:

- Primitive types.
- References and raw pointers.
- Optional types.
- Fixed arrays and slices.
- Transparent aliases.
- Named data aggregates.
- Named resource aggregates.
- Enum and failure types.

The fact that some of these types cannot themselves implement interfaces does not prevent them from being used as associated-type values.

After all associated types are bound, the compiler substitutes them into every inherited and directly declared requirement before checking for matching concrete external definitions.

### 13.9 Associated Functions in Interfaces

Interfaces may require associated functions that do not take a `self` parameter.

```arkoi
interface Parseable:
    fun parse(
        source @string_view,
    ) !ParseFail @Self
```

A concrete implementation remains an ordinary external associated-function definition on the implementing type.

```arkoi
implements Parseable for Configuration

fun Configuration.parse(
    source @string_view,
) !ParseFail @Configuration:
    # ...
```

The compiler substitutes `Self` with the implementing concrete type and verifies that a matching external definition exists.

Associated-function requirements:

- Do not take a `self` receiver.
- Belong to the implementing concrete type.
- May use `Self` and associated types in parameters, return types, and failure-related declarations where otherwise valid.
- May be overloaded under Arkoi's ordinary exact-signature overload rules.
- May be fallible or infallible.
- May return data or resource values according to ordinary ownership rules.
- Must be declared inside the interface scope.
- Must be defined externally on each implementing concrete type.
- Cannot provide default bodies.

Calls are made through the concrete type.

```arkoi
configuration @Configuration = Configuration.parse(text)!
```

Associated functions cannot be called through the interface name because they
provide no receiver from which to select a concrete implementation.

```arkoi
configuration @Configuration = Parseable.parse(text)!  # Compile-time error
```

Interface-associated functions use direct static resolution and introduce no dynamic dispatch, implementation lookup, function values, or indirect calls.

### 13.10 Visibility of Interface-Implementing Definitions

A concrete function, method, associated function, or reserved hook that satisfies an interface requirement uses Arkoi's ordinary visibility rules.

The programmer explicitly chooses whether the concrete definition is public by writing `pub`.

```arkoi
implements Parseable for Configuration

pub fun Configuration.parse(
    source @string_view,
) !ParseFail @Configuration:
    # ...
```

The public definition may be called directly from other modules when the type and function are otherwise accessible.

```arkoi
configuration @Configuration = Configuration.parse(text)!
```

Without `pub`, the concrete definition is private to its defining module.

```arkoi
implements Parseable for Configuration

fun Configuration.parse(
    source @string_view,
) !ParseFail @Configuration:
    # ...
```

Other modules cannot call the private concrete function directly.

```arkoi
configuration @Configuration =
    Configuration.parse(text)!  # Visibility error outside the defining module
```

A receiver requirement exposed by a visible interface may be called through the
interface qualifier. That call is checked against the interface contract and
does not make the private concrete definition directly accessible.

```arkoi
Writer.write(&mut file, data = content)
```

Concrete-definition visibility is independent from interface conformance:

- A private definition may satisfy an interface requirement.
- A public definition may satisfy an interface requirement.
- The `implements` declaration does not automatically make a definition public.
- A public interface does not automatically make its concrete implementations public.
- Visibility does not affect exact signature matching or conformance validation.

This does not make the concrete definition directly callable by unrelated external code.

For example, a private reserved hook may enable public language syntax while remaining unavailable as a direct concrete hook call.

```arkoi
implements Sized for Buffer

fun Buffer.__length__(
    self @&Buffer,
) @usize:
    return self.used

size @usize = length(buffer)          # Valid
size @usize = Buffer.__length__(&buffer)  # Visibility error outside the module
```

Thus, `pub` controls the concrete callable API, while the interface declaration controls interface-mediated capability.

### 13.11 Visibility of Interface Members

Declarations inside an interface do not have individual visibility modifiers.

The visibility of every associated type and required function is determined entirely by the visibility of the interface itself.

```arkoi
pub interface Iterator:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item
```

Because `Iterator` is public, its complete contract is public.

Individual `pub` modifiers inside an interface are invalid.

```arkoi
pub interface Iterator:
    pub type Item  # Compile-time error

    pub fun next(  # Compile-time error
        self @&mut Self,
    ) @?Item
```

A private interface has a private contract.

```arkoi
interface InternalIterator:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item
```

Code outside the defining module cannot name the private interface or rely on its contract.

An interface contract is therefore indivisible:

- A public interface exposes all of its associated-type and function requirements.
- A private interface exposes none of them outside its module.
- Individual requirements cannot be hidden or exported separately.
- Interface extension inherits requirements together with the visibility of the referenced parent interface.

Concrete definitions that satisfy interface requirements retain their own independent visibility, chosen with the ordinary `pub` keyword.

### 13.12 Use of `Self` in Interfaces

Inside an interface declaration, `Self` denotes the concrete type that implements the interface.

`Self` may appear anywhere an ordinary concrete type could otherwise appear in an interface requirement, including:

- Receiver types.
- Parameter types.
- Return types.
- Reference and raw-pointer types.
- Optional, array, and slice compositions.
- Ownership-qualified parameters.
- Associated-function declarations.
- Reserved-hook declarations.

```arkoi
interface Comparable:
    fun compare(
        self @&Self,
        other @&Self,
    ) @s32
```

```arkoi
interface Cloneable:
    fun clone_value(
        self @&Self,
    ) !CloneFail @Self
```

```arkoi
interface Factory:
    fun create() !CreateFail @Self
```

Ordinary ownership and type-category rules continue to apply.

For example, consuming a resource value requires `@own Self`.

```arkoi
interface Consumable:
    fun consume(
        self @own Self,
    )
```

Mutable and read-only references remain distinct.

```arkoi
interface Editable:
    fun inspect(
        self @&Self,
    )

    fun modify(
        self @&mut Self,
    )
```

When checking an implementation, the compiler replaces every occurrence of `Self` with the implementing concrete type before validating associated-type bindings and matching external definitions.

```text
Self = Configuration
```

For example:

```arkoi
interface Parseable:
    fun parse(
        source @string_view,
    ) !ParseFail @Self

implements Parseable for Configuration

fun Configuration.parse(
    source @string_view,
) !ParseFail @Configuration:
    # ...
```

`Self` is valid only within interface declarations and interface-related type checking. It is not a general alias available in ordinary concrete function definitions.

### 13.13 Scope of Associated-Type Names

An associated-type name is lexically scoped to the interface that declares it and to child interfaces that inherit it.

```arkoi
interface Iterator:
    type Item

    fun next(
        self @&mut Self,
    ) @?Item
```

A child interface may refer directly to an inherited associated type by its declared name.

```arkoi
interface DoubleEndedIterator extends Iterator:
    fun next_back(
        self @&mut Self,
    ) @?Item
```

Within the interface contract, the bare associated-type name refers to that interface requirement.

Outside interface declarations, the bare associated-type name is not in scope.

```arkoi
value @Item  # Compile-time error outside the interface contract
```

The `implements` block uses the declared associated-type name only on the left side of a binding.

```arkoi
implements Iterator for BufferIterator:
    type Item = &Byte
```

Concrete external definitions use the bound concrete type directly.

```arkoi
fun BufferIterator.next(
    self @&mut BufferIterator,
) @?&Byte:
    # ...
```

Concrete definitions do not use `Item`, `Self.Item`, or interface-qualified associated-type projection syntax.

```arkoi
fun BufferIterator.next(
    self @&mut BufferIterator,
) @?Item:  # Compile-time error
    # ...
```

Associated-type projection syntax is a compile-time error.

If multiple inherited interfaces introduce unrelated associated types with the same name, the child interface is invalid under the existing inherited-associated-type conflict rule.

### 13.14 Interface-Name Positions

In Arkoi, interface names are not general type expressions. They may identify an
interface declaration relationship or qualify a receiver-requirement call.

The supported uses are:

```arkoi
implements Interface for Type
```

```arkoi
interface Child extends Parent:
```

```arkoi
item @?Item = Iterator.next(&mut iterator)
```

The receiver's concrete type selects the implementation at compile time.

Interface names cannot be used as variable, constant, field, parameter, return, associated-type binding, array-element, optional-content, reference, pointer, or slice types.

```arkoi
value @Iterator                    # Compile-time error
field @Iterator                    # Compile-time error
parameter @&Iterator               # Compile-time error
result @?Iterator                  # Compile-time error
items @[]Iterator                  # Compile-time error
```

Interfaces therefore cannot be:

- Stored in bindings.
- Passed as values.
- Returned from functions.
- Embedded in aggregates.
- Referenced through `&Interface` or `&mut Interface`.
- Used behind raw pointers.
- Used as optional, array, or slice element types.
- Used to create heterogeneous collections of implementations.

This restriction follows from Arkoi's static-interface model:

- Interfaces are compile-time contracts only.
- They have no runtime representation.
- They do not erase or hide the concrete implementing type.
- They introduce no dynamic dispatch or interface objects.

Associated-type bindings must name concrete Arkoi types, not interfaces.

```arkoi
implements Container for Buffer:
    type Item = Iterator  # Compile-time error
```

The compiler and core language mechanisms may internally refer to canonical interfaces by declaration identity, but that does not make those interface names valid value types in source code.

### 13.15 Ordering of Interface Implementations

Interface implementation declarations and their concrete external definitions are module-level declarations and are semantically order-independent.

Both orders are valid:

```arkoi
implements Hashable for Account

fun Account.hash(
    self @&Account,
) @u64:
    return self.id
```

```arkoi
fun Account.hash(
    self @&Account,
) @u64:
    return self.id

implements Hashable for Account
```

The compiler collects the complete module before validating interface conformance. A required definition therefore does not need to appear textually before or after its `implements` declaration.

```arkoi
implements Hashable for Account

fun Account.hash(
    self @&Account,
) @u64:
    return self.id
```

When associated types are present, their bindings likewise appear before the implementing functions.

```arkoi
implements Iterator for AccountIterator:
    type Item = &Account

fun AccountIterator.__next__(
    self @&mut AccountIterator,
) @?&Account:
    # ...
```

### 13.16 Grouping Interface-Implementing Definitions

Interface-implementing functions remain ordinary external functions and methods of the concrete type.

Arkoi does not introduce:

- An implementation namespace.
- An implementation-method block.
- Interface-qualified method definitions.
- A separate dispatch table in source.
- Any special grouping syntax for implementing definitions.

```arkoi
implements Hashable for Account
implements Displayable for Account

fun Account.hash(
    self @&Account,
) @u64:
    # ...

fun Account.display(
    self @&Account,
) @string:
    # ...

fun Account.internal_helper(
    self @&Account,
):
    # ...
```

The `implements` declarations identify which contracts the type satisfies. The concrete functions themselves remain members of the type's ordinary function namespace.

An implementing function may therefore also be called through its concrete type
when its visibility permits it.

```arkoi
hash @u64 = Account.hash(&account)
```

A receiver requirement may also be selected through the interface that declares
it. The receiver's concrete type selects the implementation statically.

```arkoi
hash @u64 = Hashable.hash(&account)
```

This is a style convention only. Module-level order independence remains unchanged.

### 13.17 Interface Storage Requirements

Interfaces cannot declare or require fields.

```arkoi
interface Positioned:
    field x @f64  # Compile-time error
    field y @f64  # Compile-time error
```

Interfaces describe behavior and type relationships only. They may contain:

- Associated-type requirements.
- Required instance methods.
- Required associated functions.
- Required reserved hooks.

They may not contain:

- Fields.
- Stored properties.
- Required offsets.
- Required field visibility.
- Required aggregate layout.
- Required representation or storage.

A capability that conceptually exposes stored information must be expressed through a required function.

```arkoi
interface Positioned:
    fun x(
        self @&Self,
    ) @f64

    fun y(
        self @&Self,
    ) @f64
```

Each implementing type may choose its own representation.

```arkoi
data CartesianPoint:
    x_value @f64
    y_value @f64

data PolarPoint:
    radius @f64
    angle @f64
```

Both types may implement `Positioned` while storing their data differently.

The concrete method definitions may read fields, calculate values, delegate to other objects, or obtain the result through any other valid Arkoi implementation.

This preserves representation independence and prevents interfaces from imposing aggregate storage or ABI requirements.

### 13.18 Interface Type-Level Values

Interfaces cannot declare or require associated constants.

```arkoi
interface FixedCapacity:
    CAPACITY @const usize  # Compile-time error
```

An `implements` declaration cannot bind or define interface constants.

```arkoi
implements FixedCapacity for PacketBuffer:
    CAPACITY @const usize = 4096  # Compile-time error
```

Interfaces may require associated types and functions, but not constant values.

A type-level value required by a contract must be exposed through an associated function.

```arkoi
interface FixedCapacity:
    fun capacity() @usize
```

The implementing type provides an ordinary external associated-function definition.

```arkoi
implements FixedCapacity for PacketBuffer

fun PacketBuffer.capacity() @usize:
    return 4096
```

This avoids introducing interface-specific constant bindings, qualified associated constants, and additional constant-expression dependency rules.

Ordinary module and local constants remain unchanged and may still be used internally by concrete implementations.

### 13.19 Module-Level Interface Declarations

Interfaces and `implements` declarations are permitted only at module level.

```arkoi
module collections

interface Iterable:
    # ...

implements Iterable for Collection
```

They cannot appear inside:

- Functions.
- Methods.
- Associated functions.
- Aggregate declarations.
- Other interface bodies.
- `if`, `while`, `for`, or `loop` blocks.
- Failure handlers.
- Any other local or nested scope.

```arkoi
fun configure():
    interface LocalCapability:  # Compile-time error
        # ...

    implements LocalCapability for LocalType  # Compile-time error
```

Interface extension is declared only as part of a module-level interface declaration.

```arkoi
interface Child extends Parent:
    # ...
```

Restricting interfaces and implementations to module scope ensures that:

- Conformance is global for the compiled program.
- Conformance does not depend on runtime control flow.
- Conformance does not vary by lexical scope.
- Module-level declaration order remains irrelevant.
- Duplicate and conflicting implementations can be diagnosed across the complete program.
- The implementation ownership rule remains well-defined.

Interface declarations participate in the module's ordinary order-independent declaration collection.

### 13.20 Empty Marker Interfaces

Arkoi permits interfaces with no associated types and no required functions.

```arkoi
interface ThreadSafe:

interface PlainData:
```

Such an interface is a marker interface. It represents an explicit compile-time classification rather than a behavioral contract.

A named user-defined aggregate may implement a marker interface through the ordinary conformance syntax.

```arkoi
implements ThreadSafe for MessageQueue
```

Marker interfaces:

- Have no runtime representation.
- Add no fields, methods, hooks, or executable code.
- Introduce no dynamic dispatch.
- Do not change layout or ABI.
- Do not imply conformance from structure or naming.
- Require an explicit `implements` declaration.
- Follow the ordinary implementation ownership, uniqueness, visibility, and module-level declaration rules.
- May extend other interfaces or be extended by other interfaces.

```arkoi
interface Sendable:

interface ConcurrentResource extends Sendable:
```

Implementing a child marker interface also provides conformance to its inherited parent interfaces under the ordinary interface-extension rules.

Until such additional compile-time uses are introduced, a marker interface still records a valid nominal relationship but does not by itself enable runtime behavior or language syntax.

### 13.21 Visibility of Parent Interfaces

A public interface may extend only parent interfaces that are public and accessible wherever the child interface is exposed.

```arkoi
pub interface Base:

pub interface PublicCapability extends Base:
```

A public interface cannot extend a private parent.

```arkoi
interface InternalCapability:

pub interface PublicCapability extends InternalCapability:
    # Compile-time error
```

This rule applies to every direct parent and therefore to the complete transitive parent graph.

A private interface may extend either:

- A public accessible interface.
- A private interface accessible within the declaring module.

```arkoi
pub interface PublicBase:

interface InternalBase:

interface InternalCapability extends PublicBase, InternalBase:
```

A public child also cannot extend a parent that is public in its own module but inaccessible because it is not exported through the relevant module boundary.

The compiler validates that every parent contract of a public interface can be named and inspected by code that can access the child.

This prevents a public contract from depending on hidden requirements or associated types that external modules cannot inspect.

The rule does not change implementation visibility. Concrete functions satisfying inherited requirements retain their own independent `pub` or private visibility.

### 13.22 Inherited Interface Implementations

When a concrete type implements an interface, it automatically implements every transitive parent interface.

```arkoi
interface Readable:
    fun read(
        self @&Self,
    )

interface Stream extends Readable:
    fun flush(
        self @&mut Self,
    )

implements Stream for FileStream
```

`FileStream` is thereby also an implementation of `Readable`.

A separate implementation declaration for an inherited parent is invalid.

```arkoi
implements Stream for FileStream
implements Readable for FileStream  # Compile-time error
```

This rule applies whether the parent is inherited directly or through multiple interface-extension levels.

```arkoi
interface Base:
interface Middle extends Base:
interface Final extends Middle:

implements Final for Type
implements Base for Type  # Compile-time error
```

Redundant parent declarations are rejected even when they repeat identical associated-type bindings and requirements.

A type must have one canonical conformance path for each interface. This prevents:

- Duplicate implementation declarations.
- Competing associated-type bindings.
- Conflicting visibility or ownership assumptions.
- Ambiguity about which declaration establishes conformance.

If a type must implement a parent independently, it must not simultaneously implement a child interface that already provides that parent conformance.

### 13.23 Associated-Type Name Reuse Across Unrelated Interfaces

Unrelated interfaces may declare associated types with the same name.

```arkoi
interface Iterator:
    type Item

interface Container:
    type Item
```

A concrete type may implement both interfaces and bind each associated type independently.

```arkoi
implements Iterator for Buffer:
    type Item = &Byte

implements Container for Buffer:
    type Item = Byte
```

The two `Item` bindings belong to different interface implementations and may resolve to different concrete types.

Associated-type identity includes the declaring interface, not only the associated type's unqualified name.

Therefore:

```text
Iterator.Item
Container.Item
```

represent distinct requirements internally, even though associated-type projection syntax is not available in Arkoi source code.

The same associated-type name may be reused across any number of unrelated interfaces.

A conflict occurs only when interface extension combines unrelated associated-type requirements with the same name into one child contract.

```arkoi
interface First:
    type Item

interface Second:
    type Item

interface Combined extends First, Second:
    # Compile-time error: unrelated inherited associated types share the name Item
```

If the same originating associated type is inherited through multiple paths, it remains one merged requirement under the ordinary diamond-inheritance rule.

```arkoi
interface Base:
    type Item

interface Left extends Base:
interface Right extends Base:

interface Combined extends Left, Right:
    # One inherited Base.Item requirement
```

Independent implementations never conflict merely because their associated-type names match.

### 13.24 Same-Named Interface Requirements as Overloads

Unrelated interfaces may require functions or methods with the same name when
their substituted overload identities are validly distinct under Arkoi's
ordinary overload rules.

```arkoi
interface NumericParser:
    fun parse(
        self @&Self,
        value @u64,
    )

interface TextParser:
    fun parse(
        self @&Self,
        value @string_view,
    )
```

A concrete type may implement both interfaces and provide both overloads.

```arkoi
implements NumericParser for Parser
implements TextParser for Parser

fun Parser.parse(
    self @&Parser,
    value @u64,
):
    # ...

fun Parser.parse(
    self @&Parser,
    value @string_view,
):
    # ...
```

The concrete definitions belong to one ordinary overload set on `Parser`.

Interface requirements may coexist when their function identities differ by:

- Parameter count.
- Ordered parameter types.
- Ownership modes.
- Reference mutability modes.
- Fallibility mode.

They do not form distinct overloads when they differ only by:

- Parameter names.
- Return type.
- Exact failure-set type when both requirements are fallible.
- Visibility.

For example, these requirements conflict:

```arkoi
interface First:
    fun parse(
        self @&Self,
        value @u64,
    ) @bool

interface Second:
    fun parse(
        self @&Self,
        value @u64,
    ) @u32
```

Return type does not distinguish overloads, so one concrete type cannot implement both requirements unless they are otherwise the same exact requirement, which they are not.

An infallible and a fallible requirement form distinct overloads:

```arkoi
interface First:
    fun reset(
        self @&mut Self,
    )

interface Second:
    fun reset(
        self @&mut Self,
    ) !ResetFail
```

One implementing type may provide both definitions. Each definition satisfies
only the requirement with the same fallibility mode and exact contract.

Two otherwise identical fallible requirements still conflict when they name
different failure sets, because the exact failure-set type does not distinguish
fallible overloads.

All ordinary exact-overload-resolution rules continue to apply to calls. Interfaces do not introduce return-type-based selection, conversion ranking, dynamic dispatch, or interface-specific overload lookup.

### 13.25 Cross-Module Access for Interface Implementations

An `implements` declaration may be written in either:

- The module that defines the concrete type.
- The module that defines the interface.

When the declaration is written in the concrete type's own module, private concrete definitions may satisfy the interface requirements.

```arkoi
module application.user

import serialization

implements serialization.Serializable for User

fun User.serialize(
    self @&User,
) @string:
    # Private definition valid here
```

When the declaration is written outside the concrete type's module, only concrete definitions that are externally accessible from the declaring module may satisfy the requirements.

```arkoi
module serialization

import application.user

implements Serializable for user.User
```

In this case, the matching concrete definition must be public.

```arkoi
pub fun User.serialize(
    self @&User,
) @string:
    # ...
```

A private definition in `application.user` cannot satisfy conformance declared in `serialization`.

```arkoi
fun User.serialize(
    self @&User,
) @string:
    # Compile-time error for the external implementation declaration
```

The implementation checker uses the same visibility rules as ordinary source code in the module containing the `implements` declaration.

This prevents another module from establishing conformance through behavior that the concrete type's module intentionally kept private.

The rule also applies to:

- Instance methods.
- Associated functions.
- Reserved hooks.
- Every overload required by the interface.
- Definitions satisfying inherited parent-interface requirements.

Associated-type bindings remain valid in either permitted implementation location because they are declared directly in the `implements` block.

A public concrete definition may satisfy conformance declared in either the interface module or the concrete type module.

The `implements` declaration itself still has no visibility modifier, and once valid, conformance remains globally recognized wherever the interface and type are accessible.

### 13.26 Unsafe Interface Requirements

Interfaces may require unsafe functions, methods, associated functions, and reserved hooks.

```arkoi
interface RawReadable:
    unsafe fun read_raw(
        self @&Self,
        destination @*c.unsigned_char,
        length @usize,
    )
```

The matching concrete external definition must also be declared `unsafe`.

```arkoi
implements RawReadable for Device

unsafe fun Device.read_raw(
    self @&Device,
    destination @*c.unsigned_char,
    length @usize,
):
    # ...
```

Safety is part of the exact interface requirement.

Therefore:

- A safe concrete definition cannot satisfy an unsafe requirement.
- An unsafe concrete definition cannot satisfy a safe requirement.
- Two requirements that differ only in safety do not share one definition.
- Same-named requirements that differ only in safety conflict because safety does not create an overload distinction.

```arkoi
interface SafeReadable:
    fun read_raw(
        self @&Self,
        destination @*c.unsigned_char,
        length @usize,
    )

interface UnsafeReadable:
    unsafe fun read_raw(
        self @&Self,
        destination @*c.unsigned_char,
        length @usize,
    )
```

A concrete type cannot satisfy both requirements with one definition.

Calling an unsafe interface-required function remains subject to Arkoi's ordinary unsafe-context rules.

```arkoi
unsafe:
    UnsafeReadable.read_raw(&device, pointer, length)
```

A call from safe code is a compile-time error.

```arkoi
UnsafeReadable.read_raw(&device, pointer, length)  # Compile-time error outside unsafe context
```

Interface conformance does not weaken, hide, or automatically discharge an unsafe requirement. It only verifies that the concrete type provides a matching unsafe external definition.

Unsafe interface requirements remain statically resolved and introduce no dynamic dispatch or runtime safety checks.

### 13.27 Parameter Names in Interface Conformance

Parameter names do not affect interface conformance.

```arkoi
interface Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
    )
```

The following concrete definition may satisfy that requirement:

```arkoi
implements Writer for File

fun File.write(
    self @&mut File,
    bytes @[]u8,
):
    # ...
```

The parameter names `data` and `bytes` differ, but the substituted ordered parameter types, receiver mode, return type, failure effect, safety, and other exact contract properties match.

Parameter names remain excluded from:

- Function identity.
- Overload identity.
- Interface requirement matching.
- Determining whether one concrete definition may satisfy multiple interface requirements.

Parameter names still form part of the source-level named-argument API of the signature visible at the call site.

A direct concrete call uses the concrete definition's parameter names.

```arkoi
File.write(
    &mut file,
    bytes = content,
)
```

Using the interface requirement's name for a direct concrete call is invalid when the concrete definition uses a different name.

```arkoi
File.write(
    &mut file,
    data = content,
)  # Compile-time error
```

```arkoi
Writer.write(
    &mut value,
    data = content,
)
```

Such a call is still resolved statically to the concrete definition. The compiler maps arguments by parameter position after validating the names against the visible interface signature.

This rule introduces no runtime argument-name metadata, adapters, wrapper functions, or dynamic dispatch.

### 13.28 Inherited Parameter-Name Conflicts

When interface extension combines requirements with the same function identity, those requirements may merge only when their parameter names also match.

```arkoi
interface First:
    fun write(
        self @&mut Self,
        data @[]u8,
    )

interface Second:
    fun write(
        self @&mut Self,
        bytes @[]u8,
    )

interface Combined extends First, Second:
    # Compile-time error
```

Although `First.write` and `Second.write` have the same function identity for overload and conformance purposes, they expose different named-argument APIs.

The child interface cannot select one inherited parameter name over the other and cannot expose both names for the same parameter position.

Requirements with the same function identity may merge when:

- Their complete contract properties match.
- Their parameter names match position by position.
- Or they are the same originating requirement inherited through multiple paths.

```arkoi
interface Base:
    fun write(
        self @&mut Self,
        data @[]u8,
    )

interface Left extends Base:
interface Right extends Base:

interface Combined extends Left, Right:
    # Valid: both paths inherit the same Base.write declaration
```

This diamond case preserves the parameter names from the single originating declaration.

The same rule applies to:

- Instance methods.
- Associated functions.
- Reserved hooks.
- Every overload within an inherited overload set.

Parameter names remain excluded from ordinary function identity and concrete conformance matching. This stricter rule applies only when multiple inherited interface declarations must be merged into one visible child-interface requirement.

### 13.29 Inherited Requirement Redeclaration

A child interface cannot redeclare a requirement already inherited from a parent.

```arkoi
interface Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
    )

interface BufferedWriter extends Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
    )  # Compile-time error
```

The redeclaration is invalid even when it is textually and semantically identical to the inherited requirement.

A child also cannot redeclare an inherited requirement to change:

- Parameter names.
- Parameter types or ownership modes.
- Receiver mode.
- Return type.
- Failure effect.
- Safety.
- Any other contract property.

```arkoi
interface BufferedWriter extends Writer:
    fun write(
        self @&mut Self,
        bytes @[]u8,
    ) !WriteFail  # Compile-time error
```

Inherited requirements are already part of the child's flattened contract and remain owned by their original declaring interface.

A child interface may add a genuinely distinct overload under Arkoi's ordinary overload rules.

```arkoi
interface BufferedWriter extends Writer:
    fun write(
        self @&mut Self,
        data @[]u8,
        flush @bool,
    )
```

This new declaration is valid because its ordered parameter signature differs from the inherited overload.

The rule applies to:

- Instance methods.
- Associated functions.
- Reserved hooks.
- Every inherited overload.
- Associated-type declarations with an inherited name.

An inherited associated type likewise cannot be redeclared by the child.

```arkoi
interface Sequence:
    type Item

interface MutableSequence extends Sequence:
    type Item  # Compile-time error
```

This keeps the flattened interface contract unambiguous and prevents child interfaces from overriding, refining, renaming, or duplicating inherited requirements.

### 13.30 Accessibility of Types in Public Interface Contracts

Every type and failure appearing anywhere in a public interface's complete flattened contract must be public and externally accessible wherever the interface itself is accessible.

```arkoi
data InternalBuffer:
    # ...

pub interface Encoder:
    fun encode(
        self @&Self,
        output @&mut InternalBuffer,
    )  # Compile-time error
```

A public interface cannot expose a private or inaccessible type through:

- Parameter types.
- Receiver-adjacent requirement types.
- Return types.
- Failure effects.
- References.
- Raw pointers to C ABI types.
- Optionals.
- Arrays.
- Slices.
- Any other nested or composite type expression.

```arkoi
failure InternalFail:
    invalid_state

pub interface Loader:
    fun load(
        self @&Self,
    ) !InternalFail  # Compile-time error
```

The compiler checks the complete transitive contract after interface extension has been flattened.

Therefore, a public child interface is invalid when it inherits a requirement that exposes a type inaccessible at the child's public boundary, even if the original parent declaration was valid only within a narrower context.

A private interface may reference private types and failures that are accessible from its declaring module.

```arkoi
data InternalBuffer:
    # ...

interface InternalEncoder:
    fun encode(
        self @&Self,
        output @&mut InternalBuffer,
    )
```

Publicly accessible type aliases do not make an inaccessible underlying type valid when aliases are transparent and reveal the same type identity.

This rule ensures that external modules can name, inspect, and use every part of a public interface contract without depending on hidden declarations.

### 13.31 Accessibility of Associated-Type Bindings

When a public concrete type implements a public interface, every associated-type binding exposed by that conformance must be public and externally accessible wherever both the type and interface are accessible.

```arkoi
data InternalIterator:
    # ...

pub data Collection:
    # ...

implements Iterable for Collection:
    type Iterator = InternalIterator  # Compile-time error
```

The binding is invalid because external code can observe that `Collection` implements `Iterable` but cannot name or inspect `InternalIterator`.

A valid binding uses an externally accessible type.

```arkoi
pub data CollectionIterator:
    # ...

implements Iterable for Collection:
    type Iterator = CollectionIterator
```

The rule applies recursively to the complete bound type expression, including:

- References.
- Raw pointers to C ABI types.
- Optionals.
- Arrays.
- Slices.
- Type aliases.
- Other composite types.

```arkoi
data InternalItem:
    # ...

implements Iterable for Collection:
    type Item = ?&InternalItem  # Compile-time error
```

Transparent public aliases do not make a private underlying type externally accessible.

If either the concrete type or the interface is private, the associated-type binding must only be accessible within every context where that conformance is visible and usable.

The compiler checks associated-type bindings after substituting and flattening all inherited interface requirements.

### 13.32 `Self` in Associated-Type Bindings

`Self` is valid only inside interface declarations.

It cannot appear in an `implements` declaration or in any associated-type binding.

```arkoi
implements Linked for Node:
    type Next = ?&Self  # Compile-time error
```

The implementation must name the concrete type explicitly.

```arkoi
implements Linked for Node:
    type Next = ?&Node
```

This rule applies to every position inside the bound type expression.

```arkoi
implements Graph for Node:
    type Edge = []?&Self  # Compile-time error
```

Associated-type bindings are ordinary concrete Arkoi type expressions after the implementing type is known. They may contain:

- The explicit concrete implementing type.
- Other named concrete types.
- References.
- Raw pointers to C ABI types.
- Optionals.
- Arrays.
- Slices.
- Transparent aliases.
- Any other otherwise-valid concrete type expression.

They may not contain:

- `Self`.
- Unbound associated-type names.
- Interface names as general type expressions.

Keeping `Self` exclusive to interface declarations gives it one meaning: the unknown concrete type that will satisfy the interface contract.

Inside an implementation, that unknown has already been resolved, so the concrete type name must be used directly.

### 13.33 Associated-Type Binding Independence

An associated-type binding cannot refer to another associated-type binding from the same `implements` declaration.

```arkoi
implements Collection for Buffer:
    type Item = Byte
    type OptionalItem = ?Item  # Compile-time error
```

Each binding must be a complete and independent concrete Arkoi type expression.

```arkoi
implements Collection for Buffer:
    type Item = Byte
    type OptionalItem = ?Byte
```

Associated-type binding names are not introduced as local type aliases inside the implementation block.

Therefore, a binding cannot refer to:

- An earlier sibling binding.
- A later sibling binding.
- An inherited associated-type name.
- Any associated-type name from the implemented interface.
- Any associated-type name from another interface implementation.

```arkoi
implements Mapping for Table:
    type Key = string
    type Entry = Pair<Key, Value>  # Compile-time error
    type Value = Record
```

The concrete types must be written explicitly.

This keeps associated-type bindings:

- Order-independent.
- Free of local dependency cycles.
- Independent from declaration order.
- Simple to substitute during conformance checking.
- Consistent with the absence of associated-type projection syntax.

Ordinary named module-level type aliases may still be used when visible.

```arkoi
type TableKey = string

implements Mapping for Table:
    type Key = TableKey
```

The restriction applies only to references to associated-type bindings, not to normal named concrete types or transparent aliases.

### 13.34 Associated-Type Visibility

Associated-type declarations and associated-type bindings cannot use `pub` or any other individual visibility modifier.

Inside an interface, the associated type inherits the visibility of the interface itself.

```arkoi
pub interface Iterable:
    type Item
```

The following is invalid:

```arkoi
pub interface Iterable:
    pub type Item  # Compile-time error
```

Inside an `implements` declaration, the binding likewise has no individual visibility modifier.

```arkoi
implements Iterable for Collection:
    type Item = Element
```

The following is invalid:

```arkoi
implements Iterable for Collection:
    pub type Item = Element  # Compile-time error
```

An associated type's effective accessibility is determined by:

- The visibility and accessibility of the interface.
- The visibility and accessibility of the concrete implementing type.
- The visibility and accessibility of the bound concrete type.
- The contexts in which the conformance is visible and usable.

A binding cannot be hidden independently from an otherwise accessible conformance, and it cannot be exposed independently from a private interface or private concrete type.

This preserves the rule that every associated type required by an externally usable conformance is available throughout that conformance's accessible API.

This restriction applies specifically to associated-type declarations and bindings. Ordinary named module-level type declarations continue to follow Arkoi's existing visibility rules.

### 13.35 One Interface per `implements` Declaration

Each `implements` declaration names exactly one interface and one concrete type.

```arkoi
implements Hashable for Account
implements Displayable for Account
```

A combined conformance declaration is invalid.

```arkoi
implements Hashable, Displayable for Account  # Compile-time error
```

This remains true when one or more interfaces require associated-type bindings.

```arkoi
implements Iterator for AccountIterator:
    type Item = &Account

implements Displayable for AccountIterator
```

Bindings inside an `implements` block belong only to the single interface named by that declaration.

One-interface declarations keep the following independent for every conformance:

- Associated-type bindings.
- Implementation ownership validation.
- Visibility and accessibility checks.
- Requirement matching.
- Inherited-conformance conflict detection.
- Diagnostics and source locations.

Interface extension remains the mechanism for defining a single contract that includes multiple parent contracts.

```arkoi
interface SerializableRecord extends Serializable, Record:
    # ...
```

Implementing `SerializableRecord` provides its inherited conformances under the ordinary extension rules. This does not permit a comma-separated list in an `implements` declaration.

### 13.36 Interface Conformance Through Transparent Aliases

A transparent type alias has exactly the same interface conformances as its underlying type because both names have the same type identity.

```arkoi
implements Iterable for Buffer:
    type Item = Byte

type ByteBuffer = Buffer
```

`ByteBuffer` is recognized as implementing `Iterable` wherever the alias is visible.

The alias does not create a separate conformance record, implementation identity, or associated-type binding. All conformance information comes from the underlying type.

A separate implementation cannot target the transparent alias.

```arkoi
implements Displayable for ByteBuffer  # Compile-time error
```

The implementation must target the underlying named aggregate.

```arkoi
implements Displayable for Buffer
```

This rule also prevents defining a second implementation of an interface through a different alias of the same type.

```arkoi
type FirstBuffer = Buffer
type SecondBuffer = Buffer

implements Hashable for FirstBuffer  # Compile-time error
implements Hashable for SecondBuffer  # Compile-time error
```

Transparent aliases therefore:

- Use the underlying type's associated-type bindings.
- Use the underlying type's implementing functions and hooks.
- Cannot add, remove, replace, or specialize conformances.
- Cannot establish a distinct canonical conformance path.
- Do not affect implementation uniqueness.

Only the underlying named user-defined data or resource aggregate may appear as the concrete type in an `implements` declaration.

### 13.37 Overlapping Inherited Conformance Paths

A concrete type may independently implement multiple child interfaces that share one or more transitive parent interfaces.

```arkoi
interface Identifiable:
    type Id

    fun id(
        self @&Self,
    ) @Id

interface Storable extends Identifiable:
    fun store(
        self @&Self,
    )

interface DisplayableRecord extends Identifiable:
    fun display(
        self @&Self,
    )
```

The type may implement both children.

```arkoi
implements Storable for Record:
    type Id = u64

implements DisplayableRecord for Record:
    type Id = u64
```

The overlapping inherited paths are valid only when they establish exactly the same conformance to every shared parent.

For the shared parent conformance, all paths must agree on:

- Every associated-type binding.
- Every concrete definition satisfying a required method.
- Every concrete definition satisfying a required associated function.
- Every concrete definition satisfying a reserved hook.
- Receiver and parameter types and modes.
- Return types.
- Failure effects.
- Safety.
- Every other exact contract property.

When all paths agree, the compiler merges them into one canonical parent-interface conformance.

```text
Record implements Identifiable
```

The parent conformance is not duplicated and does not acquire multiple implementation identities.

Conflicting associated-type bindings are invalid.

```arkoi
implements Storable for Record:
    type Id = u64

implements DisplayableRecord for Record:
    type Id = string  # Compile-time error
```

Conflicting concrete definitions for the same shared requirement are likewise invalid.

The same consistency rule applies to:

- Directly shared parents.
- Parents shared through deeper transitive extension.
- Multiple overlapping parent interfaces.
- Diamond-shaped and more complex acyclic extension graphs.

This rule does not permit a separate explicit `implements` declaration for the shared parent. Such a declaration remains a redundant inherited implementation and is rejected.

A concrete type therefore has at most one canonical conformance to any interface, even when that conformance is reached through multiple independently implemented child interfaces.

### 13.38 Overloads Within One Interface

A single interface may declare multiple functions, methods, associated functions, or hooks with the same name when they form a valid Arkoi overload set.

```arkoi
interface Formatter:
    fun format(
        self @&Self,
        value @u64,
    ) @string

    fun format(
        self @&Self,
        value @f64,
    ) @string
```

Each overload is an independent interface requirement.

A concrete type implementing the interface must provide every required overload.

```arkoi
implements Formatter for TextFormatter

fun TextFormatter.format(
    self @&TextFormatter,
    value @u64,
) @string:
    # ...

fun TextFormatter.format(
    self @&TextFormatter,
    value @f64,
) @string:
    # ...
```

Providing only part of the required overload set is invalid.

```arkoi
implements Formatter for TextFormatter

fun TextFormatter.format(
    self @&TextFormatter,
    value @u64,
) @string:
    # Missing f64 overload: compile-time error
```

Interface overloads follow Arkoi's ordinary overload identity rules.

They may differ by:

- Parameter count.
- Ordered parameter types.
- Ownership modes.
- Reference mutability modes.
- Fallibility mode.

They may not differ only by:

- Parameter names.
- Return type.
- Exact failure-set type within the fallible mode.
- Visibility.
- Safety.

Invalid duplicate or conflicting declarations inside one interface are compile-time errors.

Each required overload is matched independently against the concrete type's ordinary overload set using exact static signature matching.

Interface overloads introduce no dynamic dispatch, best-match ranking, conversion-based selection, return-type-based selection, or runtime overload metadata.

## 14. Modules and Imports

### 14.1 Module Imports

Arkoi imports modules as qualified namespaces.

```arkoi
import graphics.color
import system.files
```

Imported declarations are accessed through their module path.

```arkoi
color @graphics.color.Color = graphics.color.Color.red
file @system.files.File = system.files.File.open(path)!
```

An import may define a local module alias using `as`.

```arkoi
import graphics.color as color
import system.files as files

value @color.Color = color.Color.red
file @files.File = files.File.open(path)!
```

Imports name complete modules. Individual-declaration and wildcard import forms are compile-time errors.

```arkoi
from graphics.color import Color  # Not supported in Arkoi
import graphics.color.*            # Not supported in Arkoi
```

### 14.2 Module Identity

Each Arkoi source file declares its module explicitly.

```arkoi
module graphics.color
```

The module declaration:

- Must appear exactly once in the source file.
- Identifies the module used by imports and qualified names.
- Must appear before imports and other declarations.
- Is not affected by declaration visibility.

```arkoi
module graphics.color

pub enum Color:
    red
    green
    blue
```

Imports refer to the declared module name.

```arkoi
import graphics.color
```

The build system validates the declared module name against the source file's project path.

For example:

```text
src/graphics/color.ark
```

is expected to declare:

```arkoi
module graphics.color
```

A mismatch is a build-time error.

Arkoi uses one source file per module.

Moving a source file therefore requires updating either its declared module name or its project path so that both remain consistent.

### 14.3 Acyclic Module Dependencies

The Arkoi module dependency graph must be acyclic.

If module `a` imports module `b`, then `b` must not directly or indirectly import `a`.

```arkoi
# module a
module a

import b
```

```arkoi
# module b
module b

import a  # Build-time error: circular module dependency
```

The build system detects both direct and indirect cycles before compilation proceeds.

Shared declarations needed by multiple modules should be moved into a separate lower-level module imported by each dependent module.

```text
module_a ─┐
          ├──> shared
module_b ─┘
```

An import cycle is always a build-time error, regardless of whether the cycle would appear harmless due to declaration-only dependencies or constant initialization.

### 14.4 Absolute Module Imports

Arkoi supports only absolute module imports.

```arkoi
import graphics.color
import math.vector
```

Every import uses an absolute module path; relative import syntax is a compile-time error.

```arkoi
import .color         # Compile-time error
import ..math.vector  # Compile-time error
```

An import therefore names the same module regardless of the importing module's location.

Long module paths may be shortened with an import alias.

```arkoi
import graphics.color as color
```

### 14.5 Import Scope and Re-Exports

An import is visible only inside the importing module and is never re-exported.

An `import` makes a module available only within the importing module.

```arkoi
import graphics.color
```

The imported module is not exposed through the importing module's public API.

`pub import` is a compile-time error.

```arkoi
pub import graphics.color  # Compile-time error
```

Public declarations remain owned by and accessed through their defining module. Code that uses a declaration must import the module that defines it.

### 14.6 Import Placement

All imports must appear directly after the module declaration and before every other module-level declaration.

```arkoi
module graphics.rendering

import graphics.color
import math.vector as vector

pub fun render():
    # ...
```

The required source-file order is:

1. Exactly one module declaration.
2. Zero or more imports.
3. Module-level declarations.

Imports are not allowed inside functions, conditionals, loops, unsafe blocks, or other nested scopes.

```arkoi
fun render():
    import graphics.color  # Compile-time error
```

An import that appears after another module-level declaration is also a compile-time error.

```arkoi
WIDTH @const usize = 100

import graphics.color  # Compile-time error
```

Imports are unconditional and statically known before declaration analysis begins.

### 14.7 Module Declaration Order

Module-level declarations are order-independent.

A declaration may refer to another declaration in the same module even when the referenced declaration appears later in the source file.

```arkoi
module example

fun main():
    run()

fun run():
    print_message()

fun print_message():
    # ...
```

This rule applies to:

- Functions.
- Methods and associated functions.
- Data, resource, and enum types.
- Constants.
- Type aliases.
- Module-level data variables.

The compiler first collects module-level declarations and then resolves their bodies and initializers.

Local declarations remain order-dependent.

```arkoi
fun example():
    print(value)  # Compile-time error
    value @u32 = 10
```

A local binding is not visible before its declaration.

## 15. C Interoperability

### 15.1 C Function Imports and Exports

C interoperability is declared per function. Arkoi does not use foreign-language declaration blocks.

A C function is imported with `import "C"`:

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void

import "C" fun free(
    pointer @*mut c.void,
)
```

An imported C function:

- Is a module-level declaration.
- Has no Arkoi body.
- Uses the platform C calling convention and C symbol linkage.
- May use only types permitted by Arkoi's C-compatible ABI rules.
- Is unsafe to call.
- Must therefore be called from an unsafe context.
- Does not participate in Arkoi failure effects unless wrapped by an Arkoi function.

```arkoi
unsafe:
    pointer @*mut c.void = malloc(size)
```

A function implemented in Arkoi is exported to C with `export "C"`:

```arkoi
export "C" fun arkoi_process(
    value @c.int,
) @c.int:
    return value * 2
```

An exported C function:

- Is an ordinary Arkoi function definition with a body.
- Uses the platform C calling convention.
- Exposes a C-linkage symbol.
- May use only C-compatible parameter and return types.
- Cannot declare an Arkoi failure effect.
- Cannot expose Arkoi ownership, resource, reference, slice, optional, or other non-C-compatible ABI types.
- Must obey the ordinary Arkoi rules within its body.

`import "C"` and `export "C"` are mutually exclusive on the same function.

```arkoi
import "C" export "C" fun invalid()  # Compile-time error
```

They are valid only on module-level functions.

They cannot be used on:

- Methods.
- Associated functions.
- Interface requirements.
- Reserved hooks.
- Local functions.
- `main`.

C linkage is attached to each declaration individually; block linkage syntax is a compile-time error.

```arkoi
import "C":
    fun malloc(size @c.size) @*mut c.void
# Compile-time error
```

Each imported or exported function must carry its own linkage declaration explicitly.

### 15.2 Foreign ABI Names

The string in an `import` or `export` declaration identifies the foreign ABI, including its calling convention, symbol-linkage rules, and permitted boundary representations.

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void
```

`"C"` is the foreign ABI name defined by this specification.

`"C"` means:

- Use the target platform's C calling convention.
- Use C-compatible parameter and return representations.
- Use C symbol linkage.
- Do not apply Arkoi name mangling to the exported or imported symbol.
- Apply Arkoi's C interoperability restrictions.

Any other ABI name is a compile-time error.

```arkoi
import "C++" fun create_object()  # Compile-time error
import "system" fun platform_call()  # Compile-time error
import "wasm" fun host_call()  # Compile-time error
```

### 15.3 C Symbol Names

For both `import "C"` and `export "C"`, the Arkoi function name is exactly the linked C symbol name.

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void
```

This declaration imports the C symbol named `malloc`.

```arkoi
export "C" fun arkoi_process(
    value @c.int,
) @c.int:
    return value * 2
```

This definition exports the C symbol named `arkoi_process`.

This specification does not provide:

- Foreign symbol renaming.
- Link-name attributes.
- Import aliases for C symbols.
- Export aliases.
- Multiple exported C symbols for one Arkoi function.

The following kinds of syntax are not supported:

```arkoi
import "C" fun allocate(
    size @c.size,
) @*mut c.void as "malloc"  # Compile-time error
```

```arkoi
export "C" "arkoi_process_v1" fun process(
    value @c.int,
) @c.int:
    return value * 2
# Compile-time error
```

When an Arkoi-friendly wrapper name is desired, the program must import the exact C symbol and define a separate ordinary Arkoi wrapper.

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void

fun allocation_succeeded(
    size @usize,
) !CoreFail @bool:
    foreign_size @c.size = convert(
        size,
        c.size,
    )!

    unsafe:
        return malloc(foreign_size) != null
```

Because C-linked functions are not Arkoi-mangled, two C imports or exports with the same function name and symbol cannot coexist merely as Arkoi overloads. C symbol identity is name-based, so every linked symbol name must be unique within the final linked program.

### 15.4 ABI Domains and C Scalar Types

Every Arkoi type belongs to an ABI domain.

The default ABI domain is the native Arkoi ABI. Ordinary type declarations require no ABI modifier.

```arkoi
type Count = u32
```

A type belonging to the C ABI is declared with `import "C" type`.

```arkoi
import "C" type int = target_defined_signed_integer_type
```

The `import "C"` modifier on a type declaration selects the C ABI domain. It does not by itself import a linker symbol.

The required core library provides a C interoperability module whose scalar types are selected for the compilation target's C ABI.

Conceptually, that module provides declarations such as:

```arkoi
module c

import "C" type char = target_defined_integer_type
import "C" type signed_char = target_defined_signed_integer_type
import "C" type unsigned_char = target_defined_unsigned_integer_type

import "C" type short = target_defined_signed_integer_type
import "C" type unsigned_short = target_defined_unsigned_integer_type
import "C" type int = target_defined_signed_integer_type
import "C" type unsigned_int = target_defined_unsigned_integer_type
import "C" type long = target_defined_signed_integer_type
import "C" type unsigned_long = target_defined_unsigned_integer_type
import "C" type long_long = target_defined_signed_integer_type
import "C" type unsigned_long_long = target_defined_unsigned_integer_type

import "C" type float = f32
import "C" type double = f64
import "C" type bool = target_defined_boolean_integer_type

import "C" type size = target_defined_unsigned_integer_type
import "C" type ptrdiff = target_defined_signed_integer_type
```

The core library may also provide C ABI fixed-width aliases corresponding to target C definitions such as `uint8_t`, `uint32_t`, or `int64_t`.

The exact names and module packaging of those additional aliases are core-library details.

A C ABI scalar type:

- Has the numeric or Boolean behavior of its resolved storage kind.
- Uses the size, signedness, alignment, value representation, and calling convention required by the active target C ABI.
- Belongs to the C ABI domain.
- Is not type-identical to an Arkoi ABI primitive merely because both use the same storage representation.
- Does not implicitly convert to or from an Arkoi ABI type.
- Has a distinct overload identity from an otherwise representation-identical Arkoi ABI type.
- May be explicitly converted under Arkoi's ordinary conversion rules.

For example, even when `c.int` uses the same storage representation as `s32`, the types remain distinct.

```arkoi
foreign_value @c.int = 10
native_value @s32 = foreign_value
# Compile-time error: ABI-domain mismatch
```

An explicit conversion is required.

```arkoi
foreign_value @c.int = 10
native_value @s32 = convert(
    foreign_value,
    s32,
)!
```

ABI domains cannot be mixed by value when defining another type or callable signature.

A native Arkoi function cannot accept or return a C ABI scalar by value.

```arkoi
fun invalid(
    value @c.int,
) @c.int:
    return value
# Compile-time error
```

A C function cannot accept or return an Arkoi ABI scalar by value.

```arkoi
import "C" fun invalid(
    value @s32,
) @s32
# Compile-time error
```

An ordinary Arkoi-layout aggregate cannot contain a C ABI value field.

```arkoi
data InvalidNative:
    value @c.int
# Compile-time error
```

A C-layout aggregate cannot contain an Arkoi ABI value field.

```arkoi
export "C" data InvalidC:
    value @s32
# Compile-time error
```

A type alias must remain in the ABI domain of the type it aliases.

```arkoi
import "C" type NativeInt = c.int
```

An ordinary unqualified type alias cannot be used to move a C ABI type into the Arkoi ABI domain.

```arkoi
type InvalidAlias = c.int
# Compile-time error
```

The ABI-domain restriction applies to by-value type composition and callable signatures.

Local bindings inside Arkoi function bodies may store values from either ABI domain so that wrappers can call foreign functions and explicitly convert their results.

```arkoi
fun wrapper(
    value @s32,
) !CoreFail @s32:
    foreign_input @c.int = convert(
        value,
        c.int,
    )!

    foreign_result @c.int

    unsafe:
        foreign_result = native_operation(foreign_input)

    return convert(
        foreign_result,
        s32,
    )!
```

An `export "C"` function has a C ABI signature even though its body is written in Arkoi.

An `import "C" type` function signature likewise contains C ABI value types.

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int
```

Raw pointers also belong to an ABI domain through their pointee type.

```arkoi
*u8                 # Arkoi ABI pointer type
*c.unsigned_char    # C ABI pointer type
```

Representation identity does not make these pointer types interchangeable.

A raw pointer appearing in a C ABI declaration must point to a C ABI type.

```arkoi
import "C" fun fill(
    data @*mut c.unsigned_char,
    count @c.size,
)
```

The valid pointee categories at a C boundary are:

- C ABI scalar types declared with `import "C" type`.
- `c.void`.
- Opaque `import "C" data` types.
- Complete `export "C"` data aggregates.
- Complete `export "C"` unions.
- `export "C"` enums.
- Fixed arrays whose element type belongs to the C ABI.
- C ABI function types declared with `import "C" type`.

A pointer to an Arkoi ABI type cannot appear in a C ABI function, global, aggregate field, union field, or callback signature.

```arkoi
import "C" fun invalid(
    data @*mut c.unsigned_char,
)
# Compile-time error
```

Native Arkoi pointers may be converted to C pointer types only through an explicit unsafe reinterpretation when the programmer knows that the storage representation, alignment, lifetime, and foreign API contract are compatible.

```arkoi
buffer @mut [256]u8
native_pointer @*mut u8 = address(buffer[0])
foreign_pointer @*mut c.unsigned_char

unsafe:
    foreign_pointer = reinterpret(
        native_pointer,
        *mut c.unsigned_char,
    )
```

The reinterpretation changes only the pointer type. It does not convert the pointed-to values, extend their lifetime, pin their storage, or make an incompatible object representation valid.

Pointers to ordinary Arkoi aggregates or resources may be exposed only as an explicitly reinterpreted opaque C pointer such as `*c.void` or `*mut c.void`, with the lifetime and ownership contract upheld manually.

Arkoi references and slices remain Arkoi ABI types and cannot appear in C ABI declarations.

A native Arkoi declaration may use an Arkoi reference to locally manipulate a C ABI value because the reference itself never crosses the C boundary.

```arkoi
fun inspect_foreign(
    value @&c.int,
):
    # Native Arkoi function operating on C-layout storage.
```

Fixed arrays inherit the ABI domain of their element type.

```arkoi
[16]c.unsigned_char  # C ABI array type
[16]u8               # Arkoi ABI array type
```

Therefore a fixed array used by value in a C aggregate or C global must have a C ABI element type. A pointer to Arkoi `u8` is not a C ABI pointer; it must be explicitly reinterpreted to a pointer to the corresponding C ABI byte type when such use is valid.

The compiler and required core library together guarantee that every core C ABI type matches the selected target C ABI.

User source does not define target-selection logic for the canonical core C types.

### 15.5 C-Compatible Data Aggregates

A data aggregate requests C-compatible representation by using `export "C"` on the aggregate declaration.

```arkoi
export "C" data Point:
    x @c.int
    y @c.int
```

For a data aggregate, `export "C"` means that the type has the target platform's C-compatible aggregate representation.

It guarantees:

- Fields are laid out in declaration order.
- Field alignment follows the target C ABI.
- Padding follows the target C ABI.
- The aggregate's alignment follows the target C ABI.
- The aggregate can be passed by value or through raw pointers in `import "C"` and `export "C"` function signatures.
- No hidden Arkoi runtime fields are added.
- The type's representation matches an equivalent C `struct` using corresponding field types on the same target ABI.

Only `data` aggregates may use `export "C"`.

```arkoi
export "C" resource Handle:
    value @usize
# Compile-time error
```

Every field of an exported C aggregate must itself have a C-compatible representation.

Valid fields include:

- C ABI scalar types declared with `import "C" type`.
- Raw pointers whose pointee type belongs to the C ABI.
- Fixed arrays whose element type belongs to the C ABI.
- `export "C"` enums.
- Other `export "C"` data aggregates.
- `export "C"` unions.

Invalid fields include:

- Resource types.
- `string`.
- `string_view`.
- References.
- Slices.
- Optionals.
- Failure types.
- Arkoi ABI scalar types used by value.
- Ordinary Arkoi aggregates without `export "C"`.
- Fixed arrays whose element type belongs to the Arkoi ABI.
- Any type with an unspecified or non-C-compatible representation.

```arkoi
data Internal:
    value @u32

export "C" data Invalid:
    field @Internal  # Compile-time error
```

An `export "C"` data aggregate is a C ABI data type that remains usable within Arkoi function bodies:

- It is freely copyable.
- It may be stored in local bindings.
- It may have external methods and associated functions.
- It may implement interfaces.
- Its fields follow their ordinary Arkoi visibility rules.
- It cannot be passed or returned by value through an ordinary Arkoi ABI function.
- It cannot be embedded by value in an ordinary Arkoi-layout aggregate.
- Native wrappers must use references, raw pointers, local storage, or explicit conversion to a separate Arkoi ABI representation.

The aggregate declaration itself may also be `pub`.

```arkoi
pub export "C" data Point:
    x @c.int
    y @c.int
```

`pub` controls Arkoi module visibility. `export "C"` controls representation and C ABI compatibility.

An ordinary Arkoi aggregate without `export "C"` has no guaranteed C layout and cannot cross a C ABI boundary by value or as a pointed-to complete aggregate type.

Arkoi does not provide separate `repr`, `packed`, custom-alignment, or field-offset syntax.

### 15.6 Opaque Imported C Data Types

An opaque C aggregate type is declared with `import "C" data` and a `pass` body.

```arkoi
import "C" data FILE:
    pass
```

The declaration introduces the C type name `FILE` into Arkoi without defining its fields, size, alignment, or complete representation.

The `pass` body is required by Arkoi's general rule that indentation blocks cannot be syntactically empty. In this context, `pass` does not define an empty C struct and does not give the type an Arkoi-known layout.

An imported opaque C data type:

- Is a module-level declaration.
- Refers to an externally defined C aggregate type.
- Has no Arkoi-visible fields.
- Has unknown size and alignment within Arkoi source.
- Cannot be constructed as an Arkoi value.
- Cannot be copied or passed by value.
- Cannot be returned by value.
- Cannot be used as an aggregate field by value.
- Cannot be indexed or accessed through field syntax.
- Cannot be used with operations requiring a complete type.
- May be used behind raw pointers.
- May appear in imported or exported C function signatures only through raw pointers.

```arkoi
import "C" fun fopen(
    path @*c.char,
    mode @*c.char,
) @*FILE

import "C" fun fclose(
    stream @*FILE,
) @c.int
```

A raw pointer to an opaque imported C type follows Arkoi's ordinary raw-pointer rules. Pointer copying, comparison, null handling, and passing are safe operations, while dereferencing remains unsafe. Because the referent type is incomplete, dereferencing cannot expose fields or construct a complete value.

The following are invalid:

```arkoi
file @FILE  # Compile-time error: incomplete type by value
```

```arkoi
value @u32 = pointer.field  # Compile-time error: opaque type has no visible fields
```

```arkoi
export "C" data Wrapper:
    file @FILE  # Compile-time error: incomplete type used by value
```

A pointer field is valid:

```arkoi
export "C" data Wrapper:
    file @*FILE
```

`import "C" data` is distinct from `export "C" data`.

- `import "C" data Name: pass` declares an externally defined opaque incomplete C type.
- `export "C" data Name: ...` defines a complete Arkoi aggregate with C-compatible layout.

An opaque imported C type cannot contain field declarations. Its body must contain only `pass`.

```arkoi
import "C" data FILE:
    descriptor @c.int  # Compile-time error
```

Arkoi does not support importing a complete C aggregate definition directly. A complete C-compatible aggregate must be transcribed as an Arkoi `export "C" data` declaration.

### 15.7 Raw-Pointer Pointee Mutability and C `const`

Raw pointers encode whether the pointee may be mutated through that pointer.

```arkoi
*T
```

is a nullable raw pointer providing read-only access to `T`.

```arkoi
*mut T
```

is a nullable raw pointer providing mutable access to `T`.

Both pointer types have the same pointer-sized runtime representation for the same target ABI. Their difference is enforced by Arkoi's type system.

C pointer declarations map as follows:

```text
const T*  ->  *T
T*        ->  *mut T
```

For example:

```arkoi
import "C" fun strlen(
    text @*c.char,
) @c.size

import "C" fun memset(
    destination @*mut c.void,
    value @c.int,
    count @c.size,
) @*mut c.void
```

Pointer-binding mutability remains independent from pointee mutability.

```arkoi
pointer @*mut c.unsigned_char
# Immutable binding containing a mutable raw pointer.

pointer @mut *mut u8
# Reassignable binding containing a mutable raw pointer.

pointer @mut *u8
# Reassignable binding containing a read-only raw pointer.
```

Both `*T` and `*mut T` remain nullable. The following pointer-value operations remain safe:

- Declaration.
- Copying.
- Assignment.
- Passing and returning.
- Equality and inequality comparison.
- Comparison with `null`.

Dereferencing, pointer arithmetic, indexing, integer conversion, reinterpretation, and pointer-to-reference conversion remain unsafe.

Reading through either pointer kind is permitted only in an unsafe context.

Writing through a raw pointer requires:

- An unsafe context.
- A pointer of type `*mut T`.
- A writable destination according to the pointed-to type.

Writing through `*T` is a compile-time error even inside an unsafe block.

```arkoi
unsafe:
    value @T = *read_pointer
    *write_pointer = replacement
```

where `read_pointer` may be `*T` or `*mut T`, but `write_pointer` must be `*mut T`.

The built-in `readonly(...)` explicitly reduces raw-pointer access:

```arkoi
read_pointer @*T = readonly(write_pointer)
```

This conversion:

- Converts `*mut T` to `*T`.
- Preserves the pointer address and null state.
- Performs no allocation or copy of the pointee.
- Is infallible.
- Is not overloadable.
- Does not invalidate or consume the original mutable pointer.

There is no safe conversion from `*T` to `*mut T`.

Unsafe reinterpretation mechanisms may produce a mutable pointer only when the programmer accepts responsibility for the underlying C and memory-safety contract.

This raw-pointer distinction is part of Arkoi generally, not only its C interoperability layer.

### 15.8 C-Compatible Enums

An enum requests C-compatible representation by using `export "C"` on the enum declaration.

```arkoi
export "C" enum Status:
    success = 0
    failure = 1
```

For an enum, `export "C"` means that the enum uses the target platform's C enum ABI and may cross a C ABI boundary.

An exported C enum:

- Must declare at least one member.
- Must use payload-free enum members.
- May use explicit or implicit integer discriminants under Arkoi's ordinary enum rules.
- Must have discriminant values representable by the target C enum representation selected by the platform ABI.
- May appear in `import "C"` and `export "C"` function parameters and return types.
- May appear as a field of an `export "C" data` aggregate.
- Remains a distinct Arkoi enum type.
- Does not implicitly convert to or from integer types.
- Retains Arkoi's ordinary qualified-member syntax and equality rules.

```arkoi
import "C" fun set_status(
    status @Status,
)

export "C" data Result:
    status @Status
    code @c.int
```

An ordinary Arkoi enum without `export "C"` has no guaranteed C representation and cannot cross a C ABI boundary.

```arkoi
enum InternalStatus:
    ready
    failed

import "C" fun consume(
    status @InternalStatus,
)  # Compile-time error
```

`pub` remains independent from `export "C"`.

```arkoi
pub export "C" enum Status:
    success = 0
    failure = 1
```

`pub` controls Arkoi module visibility. `export "C"` controls representation and C ABI compatibility.

Arkoi does not provide:

- A separate enum representation attribute.
- An explicit underlying integer type on an exported enum.
- Packed or nonstandard C enum representation controls.
- Imported opaque C enum declarations.

When a C API exposes an integer-valued constant set without a complete enum definition, Arkoi source should use the appropriate C integer alias from the core library and define ordinary constants as needed.

The exact width and signedness of an `export "C" enum` follow the active target's C ABI. Programs requiring a fixed-width boundary representation should use a fixed-width integer type rather than a C enum.

### 15.9 Imported and Exported C Global Variables

A C global variable is imported with a module-level `import "C"` variable declaration.

```arkoi
import "C" library_version @c.int
import "C" errno @mut c.int
```

The Arkoi variable name is exactly the linked C symbol name.

An imported global without `@mut` is read-only from Arkoi.

```arkoi
version @c.int = library_version
```

An imported global with `@mut` may be read and written.

```arkoi
errno = 0
current @c.int = errno
```

Reading or writing a correctly declared imported global is not inherently unsafe. It is an ordinary load or store governed by the declaration's type and mutability.

The declaration itself is a trusted foreign ABI contract. The programmer and build system must ensure that the linked C symbol:

- Exists.
- Is an object symbol rather than a function symbol.
- Has a representation compatible with the declared Arkoi type.
- Has compatible mutability and storage duration.
- Is correctly aligned for the declared type.

If the foreign declaration does not match the actual C symbol, program behavior is undefined.

Only C-compatible data types may be used for imported globals.

Valid examples include:

- C ABI scalar types from the required core library.
- Raw pointers whose pointee type belongs to the C ABI.
- `export "C"` enums.
- `export "C"` data aggregates.
- `export "C"` unions.
- Fixed arrays whose element type belongs to the C ABI and whose complete object type matches.

Invalid imported-global types include:

- Resources.
- `string`.
- `string_view`.
- References.
- Slices.
- Optionals.
- Failure types.
- Arkoi ABI scalar values used by value.
- Ordinary Arkoi aggregates or enums without C-compatible representation.
- Opaque imported C data types by value.

Reading a raw-pointer-valued global is safe because it only copies the pointer value.

```arkoi
import "C" global_buffer @*mut c.unsigned_char

pointer @*mut c.unsigned_char = global_buffer
```

Dereferencing or performing pointer arithmetic on the resulting pointer remains unsafe under the ordinary raw-pointer rules.

```arkoi
unsafe:
    *pointer = 10
```

`@mut` controls whether Arkoi may write the imported object. It does not guarantee that no external C code can modify an imported read-only binding.

An Arkoi global variable is exported to C with `export "C"`.

```arkoi
export "C" request_count @mut c.uint = 0
export "C" library_version @c.int = 1
```

An exported C global:

- Is a module-level Arkoi variable definition.
- Uses its Arkoi name as the exact C symbol name.
- Has C linkage and target C ABI representation.
- Must use a C-compatible data type.
- Must use a valid constant initializer.
- May be immutable or mutable according to the ordinary Arkoi declaration syntax.
- Cannot be a resource or require runtime initialization.

`pub` and `export "C"` remain independent. `pub` controls Arkoi module visibility; `export "C"` exposes the C symbol.

Ordinary imported or exported globals do not provide volatile or atomic semantics. Memory-mapped I/O, signal-shared objects, and concurrency-sensitive C objects require separate volatile or atomic operations when those language features are specified.

### 15.10 Failure Boundaries at the C ABI

Functions imported from or exported to C cannot declare Arkoi failure effects.

```arkoi
import "C" fun native_read(
    handle @*mut FILE,
) !IOFail @c.int
# Compile-time error
```

```arkoi
export "C" fun process(
    value @c.int,
) !ProcessFail @c.int:
    return value
# Compile-time error
```

The C ABI has no Arkoi failure-effect channel. A C-linked function communicates only through its declared C-compatible parameters, return value, referenced memory, and external state.

C error conventions therefore use ordinary ABI values, such as:

- Integer status codes.
- Null pointers.
- Sentinel values.
- Output parameters.
- Imported global state such as `errno`.
- Library-specific error-query functions.

An ordinary Arkoi wrapper may translate a C error convention into an Arkoi failure effect.

```arkoi
import "C" fun native_open(
    path @*c.unsigned_char,
) @*mut FILE

fun open_file(
    path @*c.unsigned_char,
) !IOFail @*mut FILE:
    handle @*mut FILE

    unsafe:
        handle = native_open(path)

    if handle == null:
        fail IOFail.open_failed

    return handle
```

Likewise, an exported C function must handle all Arkoi failures internally and convert them into its documented C-facing result convention.

```arkoi
fun perform_operation(
    value @c.int,
) !ProcessFail @c.int:
    # ...

export "C" fun process(
    value @c.int,
) @c.int:
    result @c.int = perform_operation(value) handle failure:
        yield -1

    return result
```

A failure may not propagate across a C boundary.

This rule also means:

- Postfix `!` cannot propagate out of an `export "C"` function.
- A C import is never declared fallible in Arkoi's type system.
- A wrapper must explicitly inspect C results before raising an Arkoi failure.
- C unwinding, `longjmp`, hardware faults, and foreign exceptions are not modeled as Arkoi failure effects.

If a foreign C function violates its declared non-unwinding ABI contract or transfers control across the boundary through an unsupported mechanism, program behavior is undefined.

### 15.11 Imported C Variadic Functions

Only functions imported with `import "C"` may be variadic.

```arkoi
import "C" fun printf(
    format @*c.char,
    ...,
) @c.int
```

Arkoi-defined functions and functions exported with `export "C"` cannot be variadic.

The variadic marker `...`:

- Appears at most once.
- Must be the final entry in the parameter list.
- Requires at least one fixed parameter before it.
- Does not have a parameter name or Arkoi type.
- Is valid only on an `import "C"` function declaration.

A variadic C function is called with ordinary call syntax.

Arguments matching the fixed parameters appear first. Any additional positional arguments form the C variadic argument tail.

```arkoi
unsafe:
    written @c.int = printf(
        format,
        count,
        average,
    )
```

The fixed arguments are checked against their declared Arkoi parameter types.

Variadic-tail arguments:

- Must be positional.
- Cannot use named-argument syntax.
- Are evaluated from left to right with all other arguments.
- Must have a C-compatible scalar, enum, or raw-pointer representation permitted by the variadic ABI.
- Cannot be Arkoi resources, `string_view` values, references, slices, optionals, failure values, or ordinary non-C-compatible aggregates.
- Cannot be passed using an ownership operation such as `move(...)` or `clone(...)`.

Arkoi applies the target C default argument promotions to every variadic-tail argument before the call.

This includes:

- C integer promotions for integer types narrower than the target C `int`.
- Promotion of a target C `float` value to target C `double`.
- Any other default argument promotion required by the active target C ABI.

The required core-library C aliases determine the promoted target types.

```arkoi
small @c.short = 12
decimal @c.float = 3.5

unsafe:
    discard(printf(
        format,
        small,
        decimal,
    ))
```

Here, `small` is passed using the target C integer promotion and `decimal` is passed as `c.double`.

Every call to an imported C variadic function requires an unsafe context.

Arkoi verifies the fixed arguments and the individual ABI compatibility of each variadic argument, but it cannot verify the function-specific relationship between them.

For example, Arkoi cannot prove that a `printf` format string matches the number and types of the following arguments.

A mismatch between the variadic arguments and the convention expected by the C function causes undefined behavior.

```arkoi
unsafe:
    discard(printf(
        format,
        wrong_value,
    ))
```

Arkoi `string` and `string_view` values cannot be passed directly as C strings.
The caller must provide a raw pointer to valid C-compatible null-terminated
storage when an imported function expects a C string.

```arkoi
format @*c.char = obtain_c_string_pointer()

unsafe:
    discard(printf(
        format,
        value,
    ))
```

C variadic support does not introduce general Arkoi variadic functions, argument packs, tuple spreading, or variadic values.

### 15.12 C `void` and `void*`

C `void` is represented in Arkoi in two different ways depending on context.

A C function returning `void` maps to an Arkoi function with no return type.

```arkoi
import "C" fun free(
    pointer @*mut c.void,
)
```

Arkoi does not introduce a general value type for C `void` returns.

For C `void*`, the required core library provides a canonical opaque foreign type named `c.void`.

Conceptually, the core C module declares:

```arkoi
module c

import "C" data void:
    pass
```

`c.void` is not an ordinary complete Arkoi type. It is an opaque type marker used only as the referent of a raw pointer.

Valid uses are:

```arkoi
*c.void
*mut c.void
```

The C mappings are:

```text
const void*  ->  *c.void
void*        ->  *mut c.void
C void return -> no Arkoi return type
```

For example:

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void

import "C" fun free(
    pointer @*mut c.void,
)
```

`c.void` cannot:

- Exist as a value.
- Be constructed.
- Be copied by value.
- Be passed or returned by value.
- Be dereferenced.
- Be indexed.
- Be used as an aggregate field by value.
- Have visible fields.
- Have methods, hooks, or interface implementations attached by user code.

```arkoi
value @c.void  # Compile-time error
```

```arkoi
export "C" data Invalid:
    field @c.void  # Compile-time error
```

Raw pointers to `c.void` remain ordinary nullable raw pointers.

Pointer copying, comparison, passing, returning, and null handling follow Arkoi's ordinary raw-pointer rules.

Converting between a typed raw pointer and a `c.void` raw pointer requires an explicit unsafe reinterpretation.

```arkoi
typed @*mut c.unsigned_char
raw @*mut c.void

unsafe:
    raw = reinterpret(typed, *mut c.void)
    typed = reinterpret(raw, *mut u8)
```

The conversion must preserve pointee mutability:

- `*T` may convert to `*c.void`.
- `*mut T` may convert to `*mut c.void`.
- A mutable pointer may be explicitly reduced to read-only with `readonly(...)`.
- Reinterpretation cannot safely create mutable access from a read-only pointer.

`c.void` is supplied by the required core library and is not a new built-in Arkoi primitive.

### 15.13 Ownership and Lifetimes Across the C ABI

C-compatible ABI types carry no automatic Arkoi ownership or lifetime semantics.

A raw pointer imported from C is only a raw pointer value.

```arkoi
import "C" fun malloc(
    size @c.size,
) @*mut c.void
```

Arkoi does not infer whether a returned pointer is:

- Owned by the caller.
- Borrowed from the library.
- Valid only until another call.
- Valid for the program lifetime.
- Part of an array.
- Null-terminated.
- Required to be released.
- Released with `free` or another function.
- Invalidated by mutation or external state.

Likewise, passing a raw pointer to C does not automatically:

- Transfer ownership.
- Extend the pointee's lifetime.
- Prevent Arkoi from using another alias.
- Guarantee that C will not retain the pointer.
- Guarantee that C will not mutate through a mutable pointer.
- Arrange cleanup when the call returns.

Ownership transfer, borrowing, retention, invalidation, and cleanup follow the documented contract of the imported C API.

The programmer is responsible for ensuring that:

- Every pointer remains valid for every period during which C may use it.
- A pointer is released exactly as required by the C API.
- Owned memory is not released more than once.
- Borrowed memory is not released by Arkoi.
- Memory is not accessed after its lifetime ends.
- Mutable and read-only pointer contracts are respected.
- Any required synchronization is provided.

Violating these requirements causes undefined behavior.

C imports and exports cannot use Arkoi ownership annotations to encode boundary transfer.

In particular:

- `@own` is not permitted on `import "C"` or `export "C"` parameters.
- `move(...)` is not implicitly inserted at a C call.
- Raw pointers remain data values and are freely copied.
- Returning a raw pointer does not produce an Arkoi resource.

```arkoi
import "C" fun consume_buffer(
    buffer @own Buffer,
)
# Compile-time error
```

A safe or higher-level Arkoi wrapper may represent a C ownership contract with an Arkoi resource type.

```arkoi
resource Allocation:
    pointer @*mut c.void

fun Allocation.__drop__(
    self @&mut Allocation,
):
    unsafe:
        free(self.pointer)
```

A constructor wrapper may validate the C result and place the pointer into the resource.

```arkoi
fun Allocation.create(
    size @c.size,
) !AllocationFail @Allocation:
    pointer @*mut c.void

    unsafe:
        pointer = malloc(size)

    if pointer == null:
        fail AllocationFail.out_of_memory

    return Allocation(
        pointer = pointer,
    )
```

The wrapper, not the C ABI declaration, defines:

- Whether the pointer is owned.
- Which cleanup function applies.
- Whether null is permitted.
- Which operations are safe.
- How long derived references or slices remain valid.
- Whether ownership may be transferred.

Exported Arkoi functions likewise receive and return only the C-level pointer value. Any ownership contract visible to C must be documented and enforced by the exported function's implementation and surrounding C API.

The language does not automatically generate ownership adapters, retain/release calls, reference counting, cleanup registration, or lifetime tracking for C boundaries.

### 15.14 C Callback Parameters

Imported C functions may use a raw pointer to a previously declared C ABI function type as a parameter or return type.

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int

import "C" fun register_compare(
    callback @*CompareCallback,
)
```

The callback function type declaration is module-level and uses `import "C" type`; an actual callback value is a raw pointer such as `*CompareCallback`.

A raw C callback pointer carries no captured Arkoi environment and no automatic ownership or lifetime management.

When an Arkoi function address is passed to C, the programmer must ensure that:

- The function has compatible C linkage.
- Its signature matches the callback type exactly.
- C does not invoke it after the containing program or loaded code has become invalid.
- The callback obeys every API-specific reentrancy, threading, and lifetime requirement.
- No Arkoi failure propagates across the callback boundary.

An exported callback function must handle all failures internally before returning to C.

Arkoi supports callbacks only through named function pointers. It does not synthesize closure contexts or trampoline objects.

### 15.15 Taking the Address of Data Places

The built-in `address(...)` operation obtains a raw pointer to a stable, addressable data place.

```arkoi
value @mut c.int = 10

pointer @*mut c.int = address(value)
```

For an immutable place, `address(...)` produces a read-only raw pointer.

```arkoi
value @c.int = 10

pointer @*c.int = address(value)
```

For a mutable place, it produces a mutable raw pointer.

```arkoi
value @mut c.int = 10

pointer @*mut c.int = address(value)
```

The operand must be a stable addressable place, including:

- A local binding with storage.
- A module-level variable.
- An aggregate field.
- A fixed-array element selected by a valid index.
- A place designated by a reference, or by a dereferenced raw pointer, when the resulting place is otherwise valid.
- Another place form explicitly defined as addressable.

Examples:

```arkoi
point @mut Point = Point(
    x = 10,
    y = 20,
)

x_pointer @*mut c.int = address(point.x)
```

```arkoi
values @mut [16]u8

first @*mut u8 = address(values[0])
```

The operand cannot be:

- A compile-time constant, including a field or element selected from one.
- A temporary value.
- A literal.
- A computed expression without stable storage.
- A moved or uninitialized binding.
- A function call result unless first stored in an addressable binding.
- A property-like operation that does not denote an actual place.

```arkoi
pointer @*c.int = address(calculate())
# Compile-time error
```

```arkoi
WIDTH @const usize = 4

pointer @*usize = address(WIDTH)
# Compile-time error: WIDTH has no storage address
```

Taking an address is itself a safe operation. It does not read or write through the pointer.

Dereferencing, pointer arithmetic, indexing through the pointer, and pointer-to-reference conversion remain unsafe under Arkoi's ordinary raw-pointer rules.

The pointer remains valid only while the underlying storage:

- Remains alive.
- Has not been moved.
- Has not been replaced in a way that invalidates its address.
- Has not been released or otherwise invalidated.
- Remains correctly aligned and initialized for the pointed-to type.

Passing the pointer to C does not extend the storage lifetime and does not prevent the foreign code from retaining it.

The programmer must ensure that any pointer retained by C does not outlive or become invalidated by the Arkoi storage it references.

Resource values may be addressed.

```arkoi
resource @mut ResourceType = create_resource()

pointer @*mut ResourceType = address(resource)
```

However, the resource cannot be moved, replaced, or dropped while foreign code may still use that pointer. Violating this requirement causes undefined behavior.

The same `address(...)` spelling is used for named functions and data places:

```arkoi
function_pointer @*Operation = address(add)
data_pointer @*mut c.int = address(value)
```

The operand category and expected type determine whether Arkoi takes a function address or a data address.

This enables ordinary C output-parameter patterns.

```arkoi
import "C" fun read_value(
    output @*mut c.int,
) @c.int

fun load_value() @c.int:
    value @mut c.int = 0

    unsafe:
        discard(read_value(address(value)))

    return value
```

### 15.16 Compile-Time Size and Alignment Queries

Arkoi provides the compile-time built-ins `sizeof(@Type)` and `alignof(@Type)`.

```arkoi
byte_count @usize = sizeof(@Point)
alignment @usize = alignof(@Point)
```

The operand is a type written with Arkoi's ordinary `@Type` notation.

`sizeof(@Type)` returns the number of bytes occupied by a value of the complete type, including any padding required by its representation.

`alignof(@Type)` returns the required storage alignment of the complete type in bytes.

Both operations:

- Produce compile-time `usize` constants.
- Do not evaluate a runtime value.
- Do not call user code.
- Cannot fail.
- Cannot trap when applied to a valid complete type.
- Are not overloadable.
- May appear in ordinary constant expressions.
- Reflect the active compilation target and ABI.

They are valid for complete types including:

- Primitive numeric and Boolean types.
- `char`.
- Raw pointers to C ABI types.
- Raw pointers to function types.
- References and slices when their Arkoi representation is defined.
- Fixed arrays.
- Complete data and resource aggregates.
- Ordinary enums.
- `export "C"` data aggregates.
- `export "C"` enums.
- Optionals whose representation is defined.
- Other complete types with a defined Arkoi representation.

For `export "C"` types, the result follows the active target C ABI.

```arkoi
size @usize = sizeof(@Point)
alignment @usize = alignof(@Point)
```

For a fixed array:

```arkoi
array_size @usize = sizeof(@[16]c.unsigned_char)
```

The operations are invalid for incomplete or non-value types, including:

- `c.void`.
- Opaque `import "C" data` types.
- Function signature types by themselves.
- Interface names.
- Any type whose representation is intentionally unspecified or incomplete in the current context.

```arkoi
size @usize = sizeof(@FILE)
# Compile-time error: FILE is incomplete
```

```arkoi
size @usize = sizeof(@Operation)
# Compile-time error: function signature types are not value types
```

A raw pointer to a function type is complete and may be queried.

```arkoi
pointer_size @usize = sizeof(@*Operation)
```

The result of `sizeof` or `alignof` may be converted to a target C size type when calling C APIs.

```arkoi
size @c.size = convert(
    sizeof(@Point),
    c.size,
)!
```

Even when `c.size` and `usize` use the same representation, the ABI domains remain distinct and an explicit conversion is required.

These built-ins provide representation information only. They do not allocate storage, create a value, construct an object, or guarantee that arbitrary byte storage contains a valid value of the queried type.

### 15.17 C Arrays and Array Parameters

Arkoi fixed arrays use the ordinary `[N]T` type syntax at C-compatible boundaries.

A fixed array is C-compatible when:

- Its element type is C-compatible.
- Its length is a compile-time constant.
- Its complete representation is defined for the active target ABI.

A fixed array may appear as a field of an `export "C" data` aggregate.

```arkoi
export "C" data Packet:
    bytes @[256]c.unsigned_char
```

A fixed array may also be imported or exported as a complete C global object.

```arkoi
import "C" global_buffer @mut [256]c.unsigned_char
```

The array representation follows the ordinary contiguous fixed-array layout, and for C-compatible element types matches the corresponding C array object representation.

C function parameters declared as arrays in C are represented as raw pointers in Arkoi because C adjusts array parameters to pointer parameters.

```c
void process(const unsigned char data[256]);
```

is declared in Arkoi as:

```arkoi
import "C" fun process(
    data @*c.unsigned_char,
)
```

A mutable C array parameter uses a mutable raw pointer.

```arkoi
import "C" fun fill(
    data @*mut c.unsigned_char,
    count @c.size,
)
```

Arkoi does not implicitly convert or decay a fixed array into a raw pointer.

The caller must explicitly take the address of an element.

```arkoi
buffer @mut [256]c.unsigned_char

unsafe:
    fill(
        address(buffer[0]),
        256,
    )
```

For an immutable array:

```arkoi
buffer @[256]c.unsigned_char

unsafe:
    process(
        address(buffer[0]),
    )
```

The resulting pointer addresses the first element only. The pointer value carries no length.

The caller is responsible for ensuring that the C function receives:

- A pointer to valid storage.
- Enough elements for the documented count.
- Correct mutability.
- Correct alignment.
- A lifetime covering the complete period during which C may use or retain the pointer.

Violating these requirements causes undefined behavior.

C-linked functions cannot return a fixed array by value.

```arkoi
import "C" fun invalid() @[16]u8
# Compile-time error
```

A complete C-compatible aggregate containing an array field may still be returned by value when the target C ABI permits that aggregate return.

```arkoi
export "C" data Block:
    bytes @[16]c.unsigned_char

import "C" fun make_block() @Block
```

Fixed arrays are not variadic-tail arguments.

Arkoi does not provide:

- Implicit array-to-pointer conversion.
- C flexible-array-member syntax.
- Variable-length array types.
- Array-parameter length contracts in the type system.
- Automatic null-termination for character arrays.

### 15.18 C Unions

Arkoi supports C-compatible unions through the dedicated `export "C" union` declaration.

```arkoi
export "C" union Value:
    integer @c.int
    decimal @c.double
```

A C union is a complete data type whose fields overlap in storage.

All fields begin at the same address.

Its size, alignment, padding, field alignment, and by-value calling convention follow the active target C ABI.

Only C-compatible data field types are permitted, including:

- C ABI scalar types declared with `import "C" type`.
- Raw pointers to C ABI data types.
- Raw pointers to C ABI function types.
- Fixed arrays whose element type belongs to the C ABI.
- Other complete `export "C"` data aggregates.
- Other complete `export "C"` unions.
- `export "C"` enums.

The following are not permitted as union fields:

- Resources.
- Arkoi references.
- Slices.
- `string` and `string_view`.
- Optionals.
- Failure values.
- Ordinary Arkoi-layout aggregates.
- Incomplete opaque C types by value.
- Function signature types by value.
- Interface types.

A C union must declare at least one field. `pass` alone is invalid.

```arkoi
export "C" union Empty:
    pass
# Compile-time error
```

Field names follow ordinary aggregate-field naming and visibility rules.

The declaration may combine `pub` with `export "C"`.

```arkoi
pub export "C" union PublicValue:
    integer @c.int
    decimal @c.double
```

Reading or writing any C union field requires an unsafe context.

```arkoi
value @mut Value

unsafe:
    value.integer = 10
    integer @c.int = value.integer
```

Arkoi does not track which union field is active.

The programmer is responsible for reading only a field whose object representation is valid for that field type under the surrounding C API contract.

Reading a field whose representation is invalid for that field type causes undefined behavior.

Writing one field replaces the shared storage and may invalidate the value previously represented by another field.

Taking the address of a union field is also unsafe.

```arkoi
pointer @*mut c.int

unsafe:
    pointer = address(value.integer)
```

A C union remains an Arkoi data type and may be copied by value when all of its fields are valid C-compatible data types.

Copying a union copies its complete object representation, including padding, according to the target ABI.

A C union may appear:

- As a C-linked function parameter or return type when supported by the target ABI.
- As a field of an `export "C"` data aggregate.
- As a field of another `export "C"` union.
- As an imported or exported C global type.
- Behind raw pointers.

```arkoi
import "C" fun inspect_value(
    value @Value,
) @c.int
```

Arkoi does not provide:

- Ordinary Arkoi-layout unions.
- Tagged-union semantics through `union`.
- Automatic active-field tracking.
- C bit-fields.
- Flexible array members.
- Packed unions.
- Custom alignment controls.
- Direct import of a complete C union definition.

A complete C union declaration is manually transcribed with `export "C" union`.

An opaque C type whose complete union layout is unavailable continues to use `import "C" data Name: pass` behind raw pointers.

### 15.19 C Callback Entry, Threading, and Reentrancy

An Arkoi function exported with `export "C"` may be entered by foreign code under the calling conditions defined by the surrounding C API.

Arkoi does not automatically assume that a callback:

- Runs on the main thread.
- Runs on a thread previously created or attached by Arkoi.
- Is serialized with other callbacks.
- Is non-reentrant.
- Is invoked only once at a time.
- Runs while Arkoi global state is otherwise idle.
- Runs with any implicit lock held.
- Runs with any particular thread-local state initialized.

Unless the C API guarantees otherwise, foreign code may invoke the callback:

- From an arbitrary native thread.
- Concurrently with other callbacks.
- Concurrently with ordinary Arkoi execution.
- Reentrantly before an earlier callback invocation has returned.

The callback implementation must obey the threading, synchronization, and reentrancy contract of the C API.

Access to shared mutable Arkoi state must use whatever synchronization the language and program provide. The C ABI does not insert locks, serialize callbacks, or create a single-threaded execution guarantee.

The compiler does not infer callback-threading guarantees from a function-pointer type.

```arkoi
import "C" type Callback = fun(
    @c.int,
)

export "C" fun receive_value(
    value @c.int,
):
    # May be called from an arbitrary foreign thread.
```

An exported callback must not allow an Arkoi failure effect to cross the C boundary.

```arkoi
export "C" fun invalid_callback(
    value @c.int,
) !CallbackFail:
    # ...
# Compile-time error
```

Any failure produced inside a callback must be handled before returning to C.

```arkoi
export "C" fun receive_value(
    value @c.int,
):
    result @?CallbackFail = process_value(value)?

    if result != none:
        record_callback_failure(result!)
```

The exact local handling syntax follows Arkoi's ordinary failure-handling rules; the essential requirement is that the callback returns to C without propagating an Arkoi failure.

Foreign exceptions, C++ exceptions, platform unwinding, `longjmp` across Arkoi frames, or any other foreign control transfer across an Arkoi callback boundary are unsupported.

If such a control transfer crosses the boundary, behavior is undefined.

Arkoi likewise does not permit an Arkoi panic, trap, or uncaught failure to unwind through foreign frames. A callback implementation that may encounter such conditions must convert them into a C-compatible result, recorded state, process termination, or another explicitly supported mechanism before control returns to C.

The language does not automatically:

- Attach a foreign thread to an Arkoi runtime.
- Create thread-local Arkoi state.
- Establish a scheduler context.
- Install a failure boundary.
- Catch foreign exceptions.
- Prevent reentrant entry.
- Pin referenced objects.
- Extend borrowed lifetimes.
- Retain callback state.

If a particular Arkoi implementation requires runtime thread attachment before executing ordinary Arkoi code, that attachment must be performed by implementation-provided runtime support or by a manually written C wrapper before entering the exported callback.

Passing an exported function pointer to C carries no guarantee about how or when C will call it.

The programmer must ensure that:

- Any referenced global or external state remains valid.
- Any externally retained data pointers remain valid for the complete callback lifetime.
- The callback remains available for as long as C may invoke its address.
- Shutdown or library unloading does not invalidate a callback still registered with C.
- API-specific thread-affinity rules are obeyed.
- Required synchronization is provided.
- Reentrant invocation does not violate invariants.

Violating these requirements causes undefined behavior.

### 15.20 C Symbol Uniqueness

Every C-linked symbol name must be unique across the complete linked Arkoi program.

The Arkoi declaration name is the exact external C symbol name.

```arkoi
import "C" fun process(
    value @c.int,
)
```

The symbol name `process` may not be declared again by another C-linked function, global, exported object, or other declaration that occupies the same target linker namespace.

These declarations conflict:

```arkoi
import "C" fun process(
    value @c.int,
)

export "C" fun process(
    value @c.int,
):
    pass
```

C-linked functions cannot overload by Arkoi signature because each overload would still use the same external symbol.

```arkoi
import "C" fun convert(
    value @c.int,
) @c.int

import "C" fun convert(
    value @c.double,
) @c.double
# Compile-time error: duplicate C symbol `convert`
```

The uniqueness rule applies even when declarations:

- Appear in different Arkoi modules.
- Have identical signatures.
- Differ in visibility.
- Differ only in whether they are imported or exported.
- Refer to functions versus globals when the target platform places them in the same external namespace.

Each foreign declaration is distinct, and repeated declarations of the same C symbol are compile-time errors.

Repeating an identical declaration in another module is still a compile-time error.

The compiler must diagnose every duplicate C-linked symbol that is visible within the complete Arkoi program.

A conflict with a symbol supplied only by an external native object or library may instead be diagnosed by the platform linker when it cannot be detected during Arkoi compilation.

The external target's linker namespace determines whether different symbol categories conflict. Arkoi rejects all conflicts required by that target.

C ABI type declarations do not create linker symbols.

```arkoi
import "C" type CompareCallback = fun(
    @*c.void,
    @*c.void,
) @c.int
```

Therefore, `import "C" type` declarations do not participate in C symbol uniqueness checks.

Likewise, manually transcribed C type names do not imply that a linker symbol with the same name exists.

## 16. Representative Program

```arkoi
failure IOFail:
    not_found
    permission_denied

pub resource File:
    handle @u64

pub fun File.open(path @string_view) !IOFail @File:
    handle @u64 = open_native_file(path)!

    return File(
        handle = handle,
    )

pub fun File.size(self @&File) @usize:
    return native_file_size(self.handle)

pub fun File.flush(self @&mut File) !IOFail:
    flush_native_file(self.handle)!

pub fun File.into_buffer(self @own File) !IOFail @Buffer:
    buffer @Buffer = read_entire_file(&self)!
    return move(buffer)

fun File.__drop__(self @&mut File):
    close_native_file(self.handle)

fun inspect(file @&File):
    print(File.size(file))

fun main() !IOFail:
    path @const string_view = "input.txt"
    file @mut File = File.open(path)!

    inspect(&file)
    File.flush(&mut file)!

    buffer @Buffer = File.into_buffer(move(file))!
    process_buffer(&buffer)
```
