---
title: Arkoi 1.0 modules and imports
description: Arkoi 1.0 target-language rules for module identity, source-file correspondence, imports, aliases, and visibility.
---

# Modules and imports

<!-- spec-sections: 14.1, 14.2, 14.3, 14.4, 14.5, 14.6, 14.7 -->

An Arkoi source file is one explicitly named module. Imports add other modules as
qualified namespaces; they never copy declarations into the current namespace.

!!! abstract "At a glance"

    - A file starts with exactly one `module` declaration.
    - Imports are absolute, module-wide, and placed immediately after `module`.
    - Imported names stay qualified, unless the module has a local alias.
    - Module dependencies must form an acyclic graph.
    - Top-level declarations are order-independent; local bindings are not.

## Module identity and file structure

Every source file declares its module before anything else:

```arkoi
module graphics.color

pub enum Color:
    red
    green
    blue
```

The declaration occurs exactly once, supplies the identity used by imports and
qualified names, and is unaffected by whether the module's declarations are public.
Arkoi assigns one source file to each module.

The build system also checks that the declared name matches the file's project path:

| Project path | Required declaration |
| --- | --- |
| `src/graphics/color.ark` | `module graphics.color` |

A mismatch is a build-time error. Moving the file therefore requires changing either
the project path or its declared name so they continue to agree.

### Required top-level order

Each source file has this fixed shape:

1. One `module` declaration.
2. Zero or more `import` declarations.
3. All other module-level declarations.

Imports are unconditional and known before declaration analysis. They cannot occur
inside a function, conditional, loop, `unsafe` block, or any other nested scope, and
they cannot follow another top-level declaration.

```arkoi
module graphics.rendering

import graphics.color
import math.vector as vector

pub fun render():
    pass
```

```arkoi
module graphics.rendering

WIDTH @const usize = 100
import graphics.color
# Compile-time error: an import follows another module-level declaration
```

```arkoi
module graphics.rendering

fun render():
    import graphics.color
    # Compile-time error: imports are not permitted in nested scopes
```

## Importing modules

`import` always names a complete module by an absolute path:

```arkoi
module app

import graphics.color
import system.files

fun use_imports(path @string_view):
    selected @graphics.color.Color = graphics.color.Color.red
    file @system.files.File = system.files.File.open(path) handle failure:
        return
```

The absolute name resolves to the same module from every importing location. Arkoi
has no relative, individual-declaration, or wildcard imports:

```arkoi
import .color
# Compile-time error: relative module imports are unsupported

import ..math.vector
# Compile-time error: relative module imports are unsupported

from graphics.color import Color
# Compile-time error: imports must name a complete module

import graphics.color.*
# Compile-time error: wildcard imports are unsupported
```

### Local aliases

`as` gives the imported module a shorter name inside the current module:

```arkoi
module app

import graphics.color as color
import system.files as files

fun use_aliases(path @string_view):
    selected @color.Color = color.Color.red
    file @files.File = files.File.open(path) handle failure:
        return
```

The alias changes only local qualification; it does not change the imported module's
identity.

### Scope and re-exports

An import is visible only in the module that contains it. It does not become part of
that module's public API, and Arkoi provides no re-export syntax:

```arkoi
pub import graphics.color
# Compile-time error: imports cannot be re-exported
```

Public declarations remain owned by their defining module. Every module that uses
one must import that defining module itself.

## Dependency graph

Module imports must form a directed acyclic graph. If `a` imports `b`, then `b`
cannot import `a`, either directly or through any longer path. The build system
detects both kinds of cycle before compilation continues.

```arkoi
# src/a.ark
module a

import b
```

```arkoi
# src/b.ark
module b

import a
# Build-time error: circular module dependency
```

Every cycle is rejected, including one made only from declaration dependencies or
constant initialization. Shared declarations should instead move to a lower-level
module imported by each consumer:

```text
module_a ─┐
          ├──> shared
module_b ─┘
```

## Declaration lookup order

The compiler first collects every module-level declaration, then resolves bodies and
initializers. Consequently, a top-level declaration can refer to a later one:

```arkoi
module example

fun main():
    run()

fun run():
    print_message()

fun print_message():
    pass
```

This order independence covers functions, methods, associated functions, data types,
resource types, enums, constants, type aliases, and module-level data variables.

Local bindings remain order-dependent and are invisible before their declaration:

```arkoi
fun example():
    print(value)
    # Compile-time error: local binding `value` is not yet declared

    value @u32 = 10
```

## Related topics

- [Source syntax](source-syntax.md)
- [Bindings and initialization](bindings-initialization.md)
- [Calls and overloads](calls-overloads.md)
- [Methods and visibility](methods-visibility.md)
