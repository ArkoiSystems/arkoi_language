# Current Compiler Component Inventory

Last verified: 2026-08-17

This file inventories components present in the current experimental compiler.
It is not an Arkoi 1.0 implementation roadmap and does not mean each component
is feature-complete or stable. The
[compatibility guide](mkdocs/docs/getting-started/compatibility.md) describes the
boundary between this implementation snapshot and the target language.

Checklist convention:

- Present in the current source tree: `[x]`

## 1. Language frontend

- [x] Scanner (lexer)
  - Summary: Converts source text into a stream of tokens with locations
  - Files: `include/arkoi_language/front/scanner.hpp`, `src/arkoi_language/front/scanner.cpp`
- [x] Parser
  - Summary: Builds the AST from tokens with basic error recovery
  - Files: `include/arkoi_language/front/parser.hpp`, `src/arkoi_language/front/parser.cpp`
- [x] Tokens
  - Summary: Token specification and helpers used by the lexer/parser
  - Files: `include/arkoi_language/front/token.hpp`, `src/arkoi_language/front/token.cpp`
- [x] AST nodes and visitor
  - Summary: Node types and visitor utilities for traversals
  - Files: `include/arkoi_language/ast/`, `src/arkoi_language/ast/`
- [x] Symbol table
  - Summary: Scoped symbol storage and lookup for declarations and uses
  - Files: `include/arkoi_language/sem/symbol_table.hpp`, `src/arkoi_language/sem/symbol_table.cpp`, `src/arkoi_language/sem/symbol_table.tpp`
- [x] Name resolver
  - Summary: Binds identifiers to declarations across scopes
  - Files: `include/arkoi_language/sem/name_resolver.hpp`, `src/arkoi_language/sem/name_resolver.cpp`, `src/arkoi_language/sem/name_resolver.tpp`
- [x] Type resolver
  - Summary: Computes expression and declaration types
  - Files: `include/arkoi_language/sem/type_resolver.hpp`, `src/arkoi_language/sem/type_resolver.cpp`

## 2. Intermediate representation

- [x] IL generator from AST
  - Summary: Lowers AST into IL modules, functions, blocks, and instructions
  - Files: `include/arkoi_language/il/generator.hpp`, `src/arkoi_language/il/generator.cpp`
- [x] Core IL structures
  - Summary: Models IL modules, functions, basic blocks, instructions, and operands
  - Files: `include/arkoi_language/il/`, `src/arkoi_language/il/`
- [x] Control-flow graph (CFG)
  - Summary: Builds and traverses CFGs and exposes their edges
  - Files: `include/arkoi_language/il/cfg.hpp`, `src/arkoi_language/il/cfg.cpp`, `src/arkoi_language/il/cfg.tpp`
- [x] Dataflow framework
  - Summary: Provides generic forward and backward analyses with lattices and transfer functions
  - Files: `include/arkoi_language/il/dataflow.hpp`, `src/arkoi_language/il/dataflow.tpp`
- [x] Liveness analyses
  - Summary: Computes live-in and live-out sets for values and registers
  - Files: `include/arkoi_language/il/analyses.hpp`, `src/arkoi_language/il/analyses.cpp`
- [x] IL printer
  - Summary: Writes textual IL for inspection and debugging
  - Files: `include/arkoi_language/il/il_printer.hpp`, `src/arkoi_language/il/il_printer.cpp`
- [x] CFG printer
  - Summary: Writes Graphviz DOT representations of control-flow graphs
  - Files: `include/arkoi_language/il/cfg_printer.hpp`, `src/arkoi_language/il/cfg_printer.cpp`

## 3. Optimizations

- [x] Pass manager
  - Summary: Manages a sequence of optimization passes and their dependencies
  - Files: `include/arkoi_language/opt/pass.hpp`, `src/arkoi_language/opt/pass.cpp`, `src/arkoi_language/opt/pass.tpp`
- [x] Constant folding
  - Summary: Replaces compile-time computable expressions with constants
  - Files: `include/arkoi_language/opt/constant_folding.hpp`, `src/arkoi_language/opt/constant_folding.cpp`
- [x] Constant propagation
  - Summary: Replaces variables with known constant values where possible
  - Files: `include/arkoi_language/opt/constant_propagation.hpp`, `src/arkoi_language/opt/constant_propagation.cpp`
- [x] Copy propagation
  - Summary: Replaces variables with the values copied into them where possible
  - Files: `include/arkoi_language/opt/copy_propagation.hpp`, `src/arkoi_language/opt/copy_propagation.cpp`
- [x] Dead-code elimination
  - Summary: Removes instructions with no observable side effects
  - Files: `include/arkoi_language/opt/dead_code_elimination.hpp`, `src/arkoi_language/opt/dead_code_elimination.cpp`
- [x] CFG simplification
  - Summary: Merges trivial blocks and removes unreachable edges
  - Files: `include/arkoi_language/opt/simplify_cfg.hpp`, `src/arkoi_language/opt/simplify_cfg.cpp`

## 4. x86-64 code generation

- [x] Instruction selection and code generation
  - Summary: Lowers IL to GNU Intel-syntax x86-64 assembly
  - Files: `include/arkoi_language/x86_64/generator.hpp`, `src/arkoi_language/x86_64/generator.cpp`
- [x] Operand and register modeling
  - Summary: Models registers, immediates, and memory operands
  - Files: `include/arkoi_language/x86_64/operand.hpp`, `src/arkoi_language/x86_64/operand.cpp`
- [x] Register allocation
  - Summary: Assigns virtual registers to physical registers and handles spills
  - Files: `include/arkoi_language/x86_64/allocator.hpp`, `src/arkoi_language/x86_64/allocator.cpp`
- [x] Basic DWARF generation (stepping only)
  - Summary: Adds stepping-oriented DWARF debug information to generated assembly
  - Files: `include/arkoi_language/x86_64/generator.hpp`, `src/arkoi_language/x86_64/generator.cpp`
- [x] Interference graph utility
  - Summary: Builds and queries the interference graph used by register allocation
  - Files: `include/arkoi_language/utils/interference_graph.hpp`, `src/arkoi_language/utils/interference_graph.tpp`
- [x] Calling-convention and operand mapping helpers
  - Summary: Maps values and operands for x86-64 code generation
  - Files: `include/arkoi_language/x86_64/resolver.hpp`, `src/arkoi_language/x86_64/resolver.cpp`

## 5. Testing

- [x] GoogleTest integration
  - Summary: Provides the test runner and assertions
  - Files: `tests/CMakeLists.txt`, `tests/main.cpp`
  - Target: `arkoi_language_tests`
- [x] Scanner snapshot tests
  - Summary: Check that tokenization output remains stable
  - Files: `tests/arkoi_language/snapshot/test_snapshot.cpp`, `tests/arkoi_language/snapshot/scanner/`
- [x] Parser snapshot tests
  - Summary: Check that parsed output remains stable
  - Files: `tests/arkoi_language/snapshot/test_snapshot.cpp`, `tests/arkoi_language/snapshot/parser/`
- [x] End-to-end tests
  - Summary: Compile and execute programs and validate their results
  - Files: `tests/arkoi_language/e2e/`
- [x] Unit tests
  - Summary: Validate CFG utilities and interference-graph behavior
  - Files: `tests/arkoi_language/il/test_cfg.cpp`, `tests/arkoi_language/utils/test_interference.cpp`
