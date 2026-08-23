---
title: Runnable current compiler examples
description: Learn the implemented Arkoi subset from complete programs with commands and expected exit statuses.
---

# Current compiler examples

These complete programs are accepted by the current compiler. They return
numeric results through the process exit status; they do not print application
output. Run commands from the repository root after completing the
[quickstart](quickstart.md).

The commands include `--emit-asm` because the current driver needs it to
refresh assembly before assembling and linking.

!!! note "Exit statuses are eight-bit values"

    Linux exposes only the low eight bits of a normal process exit status.
    `--run` prints that observed status and returns it from the compiler command.

## Minimal program

```arkoi
--8<-- "example/hello_world/hello_world.ark"
```

This shows a typed, parameterless `main` and an explicit return—the entry form
used by the current compiler.

```bash
./build/arkoi_language_app \
  example/hello_world/hello_world.ark \
  --run --emit-asm
```

Expected final line:

```text
Executed with exit code: 0
```

Try changing the returned value to `7`; the reported exit code becomes `7`.

## Recursive and iterative factorial

```arkoi
--8<-- "example/factorial/factorial.ark"
```

The two functions calculate `7!` in different ways. `main` subtracts the loop
result from the recursive result, so success is observable as zero.

```bash
./build/arkoi_language_app \
  example/factorial/factorial.ark \
  --run --emit-asm
```

Expected final line:

```text
Executed with exit code: 0
```

This example introduces calls, parameters, recursion, local variables,
reassignment, `if`/`else`, and `while`. Try changing only one of the two `7`
arguments in `main`; a nonzero result then exposes the disagreement.

## Recursive Fibonacci

```arkoi
--8<-- "example/fibonacci/fibonacci.ark"
```

`fib(20)` evaluates to `6765`. The native exit status contains its low eight
bits, so the observed result is `109`.

```bash
./build/arkoi_language_app \
  example/fibonacci/fibonacci.ark \
  --run --emit-asm
```

Expected final line:

```text
Executed with exit code: 109
```

The compiler command also exits with status `109`; this is an expected program
result, not a compilation failure. Try reducing the input to a Fibonacci value
below 256, such as `12`, to see its full value in the exit status.

## Compiler stress fixtures

The remaining programs are useful implementation fixtures rather than gentle
introductions:

| File | What it exercises |
| --- | --- |
| `example/test/test.ark` | Mixed numeric types, numeric-literal type annotations, implicit conversions, boolean/numeric interactions, calls, inline branches, and reassignment |
| `example/cc/cc.ark` | Integer and floating-point argument registers and the current `&&` / `||` operators |

Their deliberately dense code includes behavior that differs from the target
1.0 language. Read the [compatibility guide](compatibility.md) before using
these fixtures as syntax models.

Each example directory also contains checked-in `.il`, `.dot`, and `.s`
artifacts. Follow the [pipeline tutorial](pipeline.md) to regenerate and read
them, or use the [CLI reference](cli.md) to choose only the files you need.
