# Implementation Roadmap

Last updated: 2025-12-02

---

Checklist convention:
- Implemented/present: [x]
- Planned: [ ]

## 1. Language Frontend
- [x] Scanner (Lexer)
  - Summary: Converts source text into a stream of tokens with locations
  - Files: include/front/scanner.hpp, src/front/scanner.cpp
- [x] Parser 
  - Summary: Builds the AST from tokens with basic error recovery
  - Files: include/front/parser.hpp, src/front/parser.cpp
- [x] Tokens
  - Summary: Token specification and helpers used by the lexer/parser
  - Files: include/front/token.hpp, src/front/token.cpp
- [x] AST nodes and visitor
  - Summary: Node types and visitor utilities for traversals
  - Files: include/ast/*
- [x] Symbol table
  - Summary: Scoped symbol storage and lookup for declarations/uses
  - Files: include/sem/symbol_table.hpp, src/sem/symbol_table.tpp
- [x] Name resolver
  - Summary: Binds identifiers to declarations across scopes
  - Files: include/sem/name_resolver.hpp, src/sem/name_resolver.cpp, src/sem/name_resolver.tpp
- [x] Type resolver
  - Summary: Computes expression and declaration types
  - Files: include/sem/type_resolver.hpp, src/sem/type_resolver.cpp

## 2. Intermediate Representation
- [x] IR generator from AST
  - Summary: Lowers AST into IL modules, functions, blocks, and instructions
  - Files: include/il/generator.hpp, src/il/generator.cpp
- [x] Core IR structures
  - Summary: Core data model for the IL like modules, functions, basic blocks, and instructions
  - Files: see include/il/* and src/il/*
- [x] Control Flow Graph (CFG)
  - Summary: Build/traverse CFGs and query edges
  - Files: include/il/cfg.hpp, src/il/cfg.cpp, src/il/cfg.tpp
- [x] Dataflow framework
  - Summary: Generic forward/backward analyses with lattices/transfer
  - Files: include/il/dataflow.hpp, src/il/dataflow.tpp
- [x] Liveness analyses
  - Summary: Computes live-in/live-out for values and registers
  - Files: include/il/analyses.hpp, src/il/analyses.cpp
- [x] ILPrinter
  - Summary: Textual IL pretty-printer for inspection and debugging
  - Files: include/il/il_printer.hpp, src/il/il_printer.cpp
- [x] CFGPrinter
  - Summary: Graphviz DOT exporter for CFG visualization
  - Files: include/il/cfg_printer.hpp, src/il/cfg_printer.cpp

## 3. Optimizations
- [x] Pass manager
  - Summary: Manages a sequence of optimization passes and their dependencies
  - Files: include/opt/pass.hpp, src/opt/pass.cpp
- [x] Constant Folding
  - Summary: Replaces compile-time computable expressions with constants
  - Files: include/opt/constant_folding.hpp, src/opt/constant_folding.cpp
- [x] Dead Code Elimination
  - Summary: Removes instructions with no observable side effects
  - Files: include/opt/dead_code_elimination.hpp, src/opt/dead_code_elimination.cpp
- [x] Simplify CFG
  - Summary: Merges trivial blocks and removes unreachable edges
  - Files: include/opt/simplify_cfg.hpp, src/opt/simplify_cfg.cpp

## 4. Code Generation (x86_64)
- [x] Instruction selection / code generation
  - Summary: Lowers IL to GNU Intel x86_64 assembly
  - Files: include/x86_64/generator.hpp, src/x86_64/generator.cpp
- [x] Operand/register modeling
  - Summary: Abstractions for registers, immediates, and memory
  - Files: include/x86_64/operand.hpp, src/x86_64/operand.cpp
- [x] Register allocation
  - Summary: Assigns virtual registers to physical and handles spills
  - Files: include/x86_64/register_allocation.hpp, src/x86_64/register_allocation.cpp
- [x] Simple DWARF generation (only stepping)
  - Summary: Generates DWARF debug information for the generated assembly
  - Files: include/x86_64/generator.hpp, src/x86_64/generator.cpp
- [x] Interference graph utility
  - Summary: Builds and queries interference for allocation decisions
  - Files: include/utils/interference_graph.hpp, src/utils/interference_graph.tpp
- [x] Mapping helpers
  - Summary: Helpers for calling conventions and operand mappings
  - Files: include/x86_64/mapper.hpp, src/x86_64/mapper.cpp

## 5. Testing
- [x] GoogleTest integration
  - Summary: Third-party test framework wired into the build
  - Target: arkoi_language_tests
- [x] Snapshot tests for scanner
  - Summary: Ensures tokenization output remains stable
  - Files: test/snapshot/scanner/*
- [x] Snapshot tests for parser
  - Summary: Ensures parsed output remains stable
  - Files: test/snapshot/parser/*
- [x] End-to-end tests
  - Summary: Compiles and executes a program and validates output
  - Files: test/e2e/*
- [x] Unit tests
  - Summary: Validates CFG utilities and interference graph behavior
  - Files: test/test_cfg.cpp, test/test_interference.cpp
