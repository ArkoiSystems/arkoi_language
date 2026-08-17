---
title: Current compiler CLI reference
description: Choose Arkoi compilation stages, optimization levels, output names, and emitted pipeline artifacts.
---

# Current compiler CLI reference

Run the executable from the repository root so the example paths below resolve:

```bash
./build/arkoi_language_app [options] <inputs...>
```

At least one input is required for compilation. `--help` and `--version` exit
before input validation.

## Pipeline modes and precedence

Modes determine the last stage that runs and which required intermediate files
remain afterward. Choose exactly one mode. The current parser accepts combined
mode flags and resolves them in this order: `--compile-only`,
`--assemble-only`, `--link-only`, then `--run`. Lower-priority flags are
silently ignored, so combining them is best treated as an error at the call
site.

| Mode | Stages | Files retained with `--emit-asm` |
| --- | --- | --- |
| no mode flag | compile → assemble → link | `.s`, `.o`, and linked output (`a.out` unless `-o` is used) |
| `-c`, `--compile-only` | compile | one `.s` beside each input |
| `-a`, `--assemble-only` | compile → assemble | one `.s` and one `.o` beside each input |
| `-l`, `--link-only` | compile → assemble → link | `.s`, `.o`, and linked output; currently the same stages as no mode flag |
| `-r`, `--run` | compile → assemble → link → run | `.s` and `.o`; the linked output is removed after execution |

!!! warning "Generate fresh assembly explicitly"

    In the current driver, use `--emit-asm` for **every** mode that should
    compile source into assembly, including `--run` and the default linked
    mode. Without it, compilation does not write the derived `.s` file; a later
    stage may consume a stale file with that name or fail when none exists.

The current driver retains generated `.s` and `.o` intermediates; it does not
clean them after later stages.

## Output and inspection options

| Option | Effect |
| --- | --- |
| `-o FILE`, `--output FILE` | Name the linked ELF output; default: `a.out` |
| `--emit-il` | Keep one `.il` file beside each source input |
| `--emit-cfg` | Keep one Graphviz `.dot` file beside each source input |
| `--emit-asm` | Write one `.s` file beside each source input; currently required to refresh assembly |
| `-v`, `--verbose` | Write stage progress and external `as`/`ld` commands to standard error |

The emit options do not stop the pipeline. They preserve a view produced during
compilation in addition to the selected mode's normal result. Existing files
with the same derived names are overwritten.

`-o` applies only to the linked executable. With `--run`, that executable is
deleted after the program finishes even when a custom output name is supplied.

!!! warning "Retained linked output needs an execute bit"

    A retained linked file is currently created without executable permission.
    Run `chmod +x FILE` before invoking it directly. `--run` handles permission
    internally and then deletes its linked output.

## Optimization options

| Option | Effect |
| --- | --- |
| `-O0` | No optimization passes; this is the default |
| `-O1` | Constant folding and propagation, copy propagation, dead-code elimination, and CFG simplification |

Emitted IL and CFG files reflect the selected optimization level. Choose one
optimization flag. If both are supplied, the current parser accepts them and
`-O0` takes precedence.

## General options

| Option | Effect |
| --- | --- |
| `-h`, `--help` | Show command help and exit |
| `--version` | Print the compiler version and exit |

## Common commands

Compile, link, and retain `hello`:

```bash
./build/arkoi_language_app \
  example/hello_world/hello_world.ark \
  --emit-asm --output hello
chmod +x hello
./hello
```

Compile and run without retaining the linked executable:

```bash
./build/arkoi_language_app \
  example/hello_world/hello_world.ark \
  --run --emit-asm
```

Stop after assembly generation:

```bash
./build/arkoi_language_app \
  example/factorial/factorial.ark \
  --compile-only --emit-asm
```

Generate an object file and keep the assembly too:

```bash
./build/arkoi_language_app \
  example/factorial/factorial.ark \
  --assemble-only --emit-asm
```

!!! warning "Multi-source linking is not supported yet"

    The parser accepts several input paths, but every compiled unit currently
    emits its own `_start` entry symbol. Linking more than one normally fails
    with duplicate-symbol diagnostics. Compile one source program at a time.

Input-derived `.il`, `.dot`, `.s`, and `.o` files are written beside their
respective source files. The linked output is written at the path supplied to
`--output`, relative to the current directory when that path is relative.

## Exit status

Compilation, assembly, or linking errors produce a nonzero status. In `--run`
mode, the driver prints `Executed with exit code: N` and returns the Arkoi
program's exit status itself. This matters for examples such as Fibonacci that
intentionally return a nonzero value.

See [examples](examples.md) for expected results and the
[pipeline tutorial](pipeline.md) for the meaning of each emitted file.
