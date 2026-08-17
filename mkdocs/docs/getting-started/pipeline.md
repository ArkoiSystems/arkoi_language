---
title: Current compiler pipeline tutorial
description: Follow an Arkoi program through IL, a control-flow graph, x86-64 assembly, linking, and execution.
---

# From Arkoi source to a native program

This tutorial follows the checked-in Fibonacci example through the current
compiler. It assumes the compiler is already built; complete the
[quickstart](quickstart.md) first if needed.

## Generate every inspectable artifact

From the repository root, run:

```bash
./build/arkoi_language_app \
  example/fibonacci/fibonacci.ark \
  -O1 \
  --link-only --emit-il --emit-cfg --emit-asm \
  --output fibonacci
```

This refreshes the emitted files beside the source and retains the linked ELF:

| File | Role |
| --- | --- |
| `example/fibonacci/fibonacci.ark` | Arkoi source |
| `example/fibonacci/fibonacci.il` | Typed, SSA-form intermediate language |
| `example/fibonacci/fibonacci.dot` | Control-flow graph in Graphviz DOT format |
| `example/fibonacci/fibonacci.s` | Linux x86-64 assembly |
| `example/fibonacci/fibonacci.o` | Assembled object file |
| `fibonacci` | Linked ELF output |

The current driver retains the `.s` and `.o` intermediates after linking. The
`.il` and `.dot` files remain because their `--emit-*` options requested them;
`--emit-asm` both refreshes the assembly and makes later stages reliable.

## 1. Source

```arkoi
--8<-- "example/fibonacci/fibonacci.ark"
```

The source has two functions. `main` calls `fib(20)`; `fib` selects a base case
or makes two recursive calls.

## 2. Intermediate language

```text
--8<-- "example/fibonacci/fibonacci.il"
```

The IL makes operations that were implicit in the source explicit:

- `arg` and `call` describe function calls;
- labeled blocks such as `L2`, `L5`, and `L4` describe possible paths;
- `if ... then ... else ...` chooses a successor block; and
- `phi` merges values arriving from different control-flow paths.

This tutorial uses the same `-O1` level as the checked-in artifacts. Omit it
(or pass `-O0`) to inspect the larger, unoptimized form. Internal block and
temporary-value identifiers can change as the compiler evolves.

!!! note "Current IL diagnostic artifact"

    After `-O1` CFG simplification, a printed `phi` may retain a predecessor
    label for a block that was folded away (such as `L6` in this example). That
    stale label is a current compiler diagnostic issue, not another reachable
    source-level path.

## 3. Control-flow graph

```dot
--8<-- "example/fibonacci/fibonacci.dot"
```

The DOT file contains the same blocks and branch edges in graph form. With
Graphviz installed, render it to SVG:

```bash
dot -Tsvg example/fibonacci/fibonacci.dot -o fibonacci-cfg.svg
```

Open `fibonacci-cfg.svg` and follow the branch from the comparison block to the
base-case and recursive paths, then back to their merge.

## 4. x86-64 assembly

The assembly begins with a Linux `_start` entry point, transfers Arkoi's return
value to the process exit status, and then defines `main` and `fib`:

```asm
--8<-- "example/fibonacci/fibonacci.s:1:32"
```

Later instructions implement the recursive calls and branches. The output uses
GNU assembler's Intel syntax and the Linux x86-64 ABI; it is not portable
assembly for other operating systems or architectures.

## 5. Assembly, linking, and execution

After code generation, the driver invokes GNU `as` to create an object file and
GNU `ld` to create `fibonacci`. Add `--verbose` to the generation command to see
the stages and external commands on standard error.

The retained file currently needs an execute bit before direct invocation:

```bash
chmod +x fibonacci
./fibonacci
echo $?
```

It reports `109`: `fib(20)` is `6765`, and Linux exposes its low eight bits as
the process exit status.

For a one-command compile-and-run cycle, use:

```bash
./build/arkoi_language_app \
  example/fibonacci/fibonacci.ark \
  --run --emit-asm
```

That form prints `Executed with exit code: 109` and removes the linked output
afterward. See the [CLI reference](cli.md) for the precise retention rules or
return to the [examples](examples.md) for smaller programs.
