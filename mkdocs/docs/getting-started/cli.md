# CLI usage

Arkoi builds a single compiler executable. The CLI is centered around compiling one or more `.ark` inputs and selecting which pipeline stages should run.

## Basic usage

```bash
arkoi_language [options] <inputs...>
```

## Common options

- `-h`, `--help` shows the help text
- `--version` prints the compiler version
- `-o`, `--output <file>` sets the output file name
- `-v`, `--verbose` prints the commands used during compilation
- `-c`, `--compile-only` stops after generating assembly
- `-a`, `--assemble-only` stops after assembling object files
- `-l`, `--link-only` stops after linking
- `-r`, `--run` links and runs the output binary
- `--emit-asm` writes `.s` files
- `--emit-cfg` writes `.dot` control-flow graph files
- `--emit-il` writes `.il` files

## Examples

Compile and run a program:

```bash
arkoi_language example/hello_world/hello_world.ark --run
```

Generate assembly without linking:

```bash
arkoi_language example/hello_world/hello_world.ark --compile-only --emit-asm
```

Emit intermediate language and a CFG:

```bash
arkoi_language example/hello_world/hello_world.ark --emit-il --emit-cfg
```

Use a custom output name:

```bash
arkoi_language example/hello_world/hello_world.ark --output hello
```