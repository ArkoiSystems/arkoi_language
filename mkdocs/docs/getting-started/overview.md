# Overview

Arkoi is a systems language built around a direct compilation pipeline. The language is compact so the compiler, syntax, and runtime model stay easy to understand.

## Design goals

- Keep the core language concise and explicit.
- Make types visible in the source instead of relying on heavy inference.
- Keep the control flow readable and close to the generated machine-level behavior.
- Make the compiler pipeline observable with IL, CFG, and assembly output.

## Paradigm

Arkoi behaves like a procedural language with selective multi-paradigm traits:

- Functions are the main unit of composition.
- Typed variables and typed parameters keep data flow explicit.
- Expressions map directly to compiler stages.
- Control flow is structured with `if`, `else`, `while`, and `return`.

## What Arkoi does not include

The language does not include the following features:

- Classes and object systems
- Modules and imports
- Generics or templates
- Pattern matching
- Automatic garbage collection
- A large standard library

## Read next

Read the [grammar](grammar.md) for the language shape, then continue to [language rules](rules.md), [functions](functions.md), and [examples](examples.md).