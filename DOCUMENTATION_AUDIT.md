# Arkoi documentation audit

Audit date: 2026-08-17

This report reviews the documentation as it exists in this working tree. It is
an issue register, not a patch: no specification, guide, example, compiler
source, or build configuration was changed as part of the review.

The findings and original line references below are preserved as the audit
baseline. The remediation section records later changes made in response to the
audit.

## Scope and interpretation

The review covered:

- the attached target source, `mkdocs/Arkoi_Language_Specification_v150.md`;
- all 77 authored Markdown pages under `mkdocs/docs`;
- `mkdocs/mkdocs.yml`, `mkdocs/hooks.py`, the documentation stylesheet, and the
  GitHub Pages workflow;
- the root `README.md` and `ROADMAP.md`; and
- current-compiler claims against the parser, driver, CLI, checked-in examples,
  and a locally built `arkoi_language_app`.

Generated MkDoxy pages were checked structurally, but their generated prose was
not copy-edited line by line.

Path shorthands used below:

- `S` means `mkdocs/Arkoi_Language_Specification_v150.md`.
- `T/` means `mkdocs/docs/language-specification/1.0.0/`.
- `C/` means `mkdocs/docs/getting-started/`.

Line numbers refer to the working tree on the audit date and will naturally move
as issues are fixed.

## Remediation progress — 2026-08-17

The first repair pass addressed issues whose intended result was already clear;
it did not choose among unresolved language semantics.

Resolved in this pass:

- **Status and scope:** GOV-01 and GOV-03. The source now identifies language
  target `1.0.0`, document revision `v150`, and target status separately. The
  learner-facing current section now contains only an invocation quickstart and
  the compatibility boundary; implementation detail lives under Development.
- **Target examples:** EX-03, EX-04, EX-05, EX-07, EX-08, EX-09, EX-10, EX-11,
  EX-13, and EX-14. The corresponding source and split examples were repaired
  where each artifact was affected.
- **Current compiler:** CUR-01 through CUR-07. Onboarding, executable help,
  compatibility claims, entry-point wording, dependency coverage, the component
  inventory, and checked Fibonacci artifacts now match compiler 0.1.0.
- **Site and editorial mechanics:** DOC-02 through DOC-08 and DOC-11. Pull
  requests now receive a strict non-deploying build; duplicate interface-page
  identities, metadata-table headers, landing routes, page metadata, notice
  freshness/search pollution, duplicated quickstart setup, and the compile-time
  diagnostic presentation class were corrected.

Partially resolved:

- **EX-01:** 38 unambiguous comment-only bodies now use `pass`; 45
  result-producing implementation placeholders remain because inventing their
  results would add language or library design.
- **EX-06:** the positive empty-interface bodies and C pointer mismatch are
  corrected in the source, but the broader source-of-truth/errata requirement in
  GOV-02 remains open.
- **EX-12:** raw pointers are initialized before reinterpretation and use a
  consistent C pointee type. Union first-write initialization remains open.
- **DOC-01:** the required `mkdocs/` working directory and complete local build
  procedure are documented. MkDoxy 1.2.8 still resolves those paths against the
  process working directory, so root-level invocation remains unsupported.

All SPEC findings, GOV-02, EX-02, the union half of EX-12, and the remaining
terminology/style decisions stay open for an explicit design pass.

## Priority guide

- **P0 — Blocker:** contradicts a central safety or correctness guarantee.
- **P1 — High:** can misdirect implementations or users, or leaves a foundational
  language rule undefined.
- **P2 — Medium:** a real inconsistency, invalid example, discoverability problem,
  or meaningful maintenance risk.
- **P3 — Low:** polish, accessibility hardening, or a useful preventive check.

## Executive summary

The published site's overall separation between the current compiler and the
Arkoi 1.0 target is good. Navigation is complete, local links and fragments are
sound, a strict build succeeds from the expected directory, and the documented
current examples run as stated.

The main problems are deeper than the site shell:

1. The target calls itself both the current accepted language and an unimplemented
   target, and the monolithic source and split reference both appear canonical
   even though the split pages silently normalize source conflicts.
2. The safety model says safe operations cannot cause undefined behavior, while
   the safe iterator rules explicitly allow exactly that.
3. A complete lexical grammar, expression grammar, precedence table, numeric
   semantic model, and callable lifetime contract are not yet defined well enough
   for independent implementations to agree.
4. Both target artifacts contain examples that violate their own rules.
5. The current-compiler section is much more detailed than the requested scope,
   while the root README—the most visible current entry point—is stale and can
   send a new user through a broken build/run path.

Recommended documentation shape for the current implementation:

- keep one **Install and run the current compiler** page;
- include one very small runnable example and expected result;
- keep a short, prominent **current compiler is not Arkoi 1.0** boundary note;
- move the detailed CLI behavior, stress fixtures, IL/CFG/assembly walkthrough,
  and driver limitations under **Compiler development**; and
- nest the quickstart under **Current compiler** rather than leaving it as an
  ambiguous top-level destination.

## 1. Status, source of truth, and audience

### GOV-01 — Version and lifecycle vocabulary conflict

**Priority:** P1

**Type:** Confirmed inconsistency

`S:3-8` calls itself language version `1`, the “Current language
specification,” and the definition of “accepted syntax and semantics.” The split
reference instead uses `1.0`, `1.0.0`, source revision `v150`, “normative target,”
and “Target specification” (`T/index.md:2-21`; `mkdocs/docs/index.md:11-12`).

This is especially confusing because “current” elsewhere means compiler 0.1.0.

**Fix direction:** define three separate fields and use them consistently:

- language version, for example `1.0.0`;
- document revision, for example `v150`; and
- lifecycle status, for example `Draft target specification`.

Do not call unimplemented target syntax “current” or “accepted.”

### GOV-02 — Two artifacts appear canonical, but their semantics differ

**Priority:** P1

**Type:** Governance and maintenance defect

The monolith presents itself as the current specification, while
`T/index.md:12-15` presents the split reference as canonical. The specification
map admits that the split pages normalize conflicts (`T/specification-map.md:3-11`),
but there is no public errata or decision log recording each semantic change.

Confirmed silent normalizations include string conversion, slicing selection,
empty interface bodies, a made-up slicing hook, and a C pointer type mismatch;
they are detailed in section 3 below.

**Fix direction:** choose one normative source. Prefer generating the other form
from it. Until generation exists, maintain an explicit errata/normalization log
with rationale, approval status, and the affected source revision.

### GOV-03 — The current-state material exceeds the requested scope

**Priority:** P1

**Type:** User-scope and information-architecture mismatch

`mkdocs/mkdocs.yml:63-70` exposes seven current-compiler destinations totaling
about 724 lines. They include detailed mode precedence, retained artifacts,
permission behavior, stress fixtures, and a full IL/CFG/assembly tutorial. The
homepage also promotes inspection of pipeline artifacts
(`mkdocs/docs/index.md:17-23`).

That level of implementation detail makes the current subset look like a second
language manual and increases drift against a substantially different target.

**Fix direction:** use the compact current-documentation shape in the executive
summary. Preserve the detailed material, but relocate it under development rather
than making it part of the learner-facing language path.

## 2. Target-language definition

### SPEC-01 — Safe iterator use can cause undefined behavior

**Priority:** P0

**Type:** Confirmed safety contradiction

The target says undefined behavior is assigned only to unsafe or foreign code and
that safe operations do not produce it (`S:8`; `T/glossary.md:99-101`;
`T/index.md:67-71`). Ordinary custom iteration is not unsafe, but
`T/iterator-values-and-lifetimes.md:44-68` permits a yielded reference to be
stored and says later safe advancement may invalidate it and cause undefined
behavior. `T/for-loops.md:112-129` similarly allows a reference from a hidden
data temporary to escape and later become undefined behavior. The same conflict
exists at `S:5646-5694` and `S:6046-6101`.

**Fix direction:** choose one enforceable model:

- expire or reject earlier references before advancement;
- make the relevant protocol, hook, or iteration operation explicitly unsafe;
- give yielded references a lifetime the compiler can enforce; or
- broaden the stated undefined-behavior model and stop claiming safe code cannot
  cause it.

The last option would materially weaken Arkoi's stated safety contract.

### SPEC-02 — There is no complete lexical or source-file grammar

**Priority:** P1

**Type:** Missing normative foundation

`S:28-297` and `T/source-syntax.md` explain blocks, `pass`, commas, and continued
expressions, but the audit found no complete lexical grammar or EBNF. At minimum,
the following remain undefined or incomplete:

- identifier syntax and Unicode policy;
- a complete keyword/reserved-word list;
- source encoding, byte-order mark, and line-ending handling;
- tabs versus spaces, mixed indentation, indentation width, and the treatment of
  blank or comment-only lines;
- token boundaries and comment termination; and
- string-literal escape rules. Character escapes are defined at
  `T/types-values.md:128-142`, but the string section at `:144-176` does not
  define corresponding literal escapes.

**Fix direction:** add a normative lexical grammar and a complete source grammar,
then make prose sections explain that grammar rather than substitute for it.

### SPEC-03 — Expression precedence and associativity are almost entirely absent

**Priority:** P1

**Type:** Missing normative foundation

The only explicit precedence rule found is that `|>` is below every other
operator and groups left to right (`S:4642-4644`; `T/pipelines.md:114-116`). No
complete rule orders postfix operations, calls/member/indexing, unary operators,
arithmetic, shifts, bitwise operators, comparisons, Boolean operators, `??`, and
the pipeline operator. Associativity is likewise missing for nearly all binary
operators.

**Fix direction:** publish either an expression grammar or a precedence and
associativity table covering every operator and postfix form.

### SPEC-04 — Primitive numeric and comparison semantics are incomplete

**Priority:** P1

**Type:** Missing normative semantics

The target gives fixed-width names and says `f32`/`f64` are IEEE 754
(`T/types-values.md:76-85`), but does not fully define:

- signed integer representation and ranges, including whether the representation
  is two's complement;
- the meaning of wrapping/truncation/bitcast operations on signed values;
- unary `+`, unary `-`, `~`, and built-in integer bitwise operators;
- floating-point rounding modes, NaN behavior, infinities, signed zero,
  exceptions, division by zero, overflow, and comparison behavior; and
- which built-in comparisons exist for numbers, `bool`, `char`, `string`,
  `string_view`, and failure values.

`T/expressions-and-numeric-operations.md:72-106` covers only part of integer
arithmetic. `T/comparisons-and-membership.md:21-35` calls itself an audit matrix
but omits those primitive families; its only numeric rule is the common-conversion
sentence at `:19`.

**Fix direction:** add a complete primitive-operation matrix with operand types,
result type, conversion, trap/failure behavior, and relevant IEEE 754 details.

### SPEC-05 — Reference-return relationships are not part of callable contracts

**Priority:** P1

**Type:** Safety-critical underspecification

`T/references-lifetimes.md:137-180` permits returning a parameter-derived
reference when the relationship is “lexically evident.” A function signature
does not say which input a returned reference borrows from, what happens with
multiple reference inputs, or how the relationship survives a declaration-only
API, interface requirement, function pointer, or separate compilation.

**Fix direction:** define explicit lifetime-elision rules that cover all callable
forms, add lifetime relationships to the type system, or conservatively forbid
ambiguous reference returns. Include analogous rules for returned slices and
receiver-derived hook results.

### SPEC-06 — Trap unwinding and cleanup are not coherently defined

**Priority:** P1

**Type:** Conflicting/unfinished runtime model

`T/resource-lifecycle.md:63-75` lists normal completion, return, failure,
propagation, break, and continue as cleanup paths, but not traps. In contrast,
`T/evaluation-order.md:77-81` says already constructed temporaries are cleaned up
when an expression traps. `T/resource-lifecycle.md:44-53` also allows `__drop__`
itself to trap.

It is therefore unclear whether a trap unwinds scopes, aborts immediately, runs
all drops, or what happens if a drop traps during an existing trap.

**Fix direction:** define trap termination/unwinding, cleanup guarantees, nested
trap behavior, and the observable boundary between a trap and process abort.

### SPEC-07 — C-callback trap guidance asks for an impossible recovery

**Priority:** P1

**Type:** Confirmed contradiction

`T/c-interoperability/callback-execution-symbols.md:50-63` says a callback that
may encounter a panic, trap, or uncaught failure must translate it before
returning to C. Yet traps are defined as unrecoverable
(`T/failures.md:124-128`; `T/glossary.md:95-101`), so Arkoi code cannot catch and
translate one. `panic` is not defined elsewhere, and an uncaught recoverable
failure should be rejected statically at an `export "C"` boundary. `S:11215`
contains the same problem.

**Fix direction:** separately specify recoverable failure translation,
unrecoverable trap/process termination, and unsupported foreign unwinding. Remove
or define `panic`.

### SPEC-08 — The contexts for implicit numeric conversion conflict

**Priority:** P1

**Type:** Confirmed ambiguity

`T/expressions-and-numeric-operations.md:24-32` generally permits lossless
implicit numeric conversions such as same-signed widening. Yet
`T/calls-overloads.md:141-159` labels `u16` to `u32` in a call as “no implicit
numeric conversion.” The source suggests that conversions are not introduced
merely to make an overload applicable (`S:1404-1416`), but never reconciles that
exception with its general conversion rule at `S:3554-3564`.

**Fix direction:** enumerate conversion contexts explicitly: initialization,
assignment, return, arithmetic common-type selection, a call with one candidate,
overload applicability/ranking, aggregate fields, and comparison.

### SPEC-09 — `move(_)` conflicts with the stated `move` operand rule

**Priority:** P2

**Type:** Confirmed special-case inconsistency

`T/ownership-moves.md:78-89` says `move` accepts only a whole named resource
binding, and `:147-163` rejects temporaries. `T/pipelines.md:40-41` says `_` is
not a binding, while `:84-98` permits `move(_)`, including a temporary carrier.
The same conflict appears at `S:1888-1912`, `S:4597-4599`, and `S:4632-4637`.

**Fix direction:** if the carrier is an intentional pseudo-place with stable
storage, define that exception on the ownership page and in the quick reference,
not only in the pipeline chapter.

### SPEC-10 — Fallible `convert` has no constant-evaluation failure rule

**Priority:** P2

**Type:** Missing normative rule

`T/constants-globals.md:64-75` allows `convert` in constant expressions while
forbidding failure propagation and handlers. Ordinary checked conversion is
fallible and written with `!` (`T/expressions-and-numeric-operations.md:34-38`).

**Fix direction:** state whether constant `convert` is a distinct compile-time
form, whether an out-of-range conversion makes the declaration a compile-time
error, and what syntax it uses. Include the diagnostic behavior.

### SPEC-11 — Optional typing and resource ownership are incomplete

**Priority:** P2

**Type:** Underspecified design

`T/types-values.md:204-230` defines `?.`, `??`, postfix `?`, and postfix `!` at a
high level, but does not fully define:

- implicit lifting from `T` to `?T`, even though examples rely on it;
- the exact type calculation for `?.` and `??`;
- conversion and ownership requirements for the fallback of `??`;
- whether unwrapping a named `?Resource` moves, borrows, or is rejected; and
- how those forms interact with `take`, which is the only operation explicitly
  defined to extract a resource while restoring the optional place
  (`T/places-replacement.md:49-73`).

**Fix direction:** add an optional-operation table covering data/resource,
place/temporary, present/absent, result type, source state, cleanup, and failure.

### SPEC-12 — “Failure” is both terminologically inconsistent and hard to inspect

**Priority:** P2

**Type:** Terminology defect and open design question

The reference uses “failure” for a flat set, a declaration/type, a member/value,
a control-flow effect, and an exit path (`T/failures.md:11,26-28,65,116`), while
the glossary defines it only as a typed effect (`T/glossary.md:91-94`). The source
adds “discriminant,” “failure set,” and “failure type” (`S:3429-3477`).

Separately, a handler exposes `failure`, but Arkoi 1.0 defines neither general
pattern matching nor failure-value equality (`mkdocs/docs/learn/failures.md:120-124`).
That means a handler cannot directly select recovery by failure member.

**Fix direction:** define separate canonical terms for failure declaration/set,
member/value, effect, and path. Then either define failure inspection/equality or
mark the one-policy handler limitation as an explicit accepted 1.0 design choice.

### SPEC-13 — The project/module resolution model stops at a path example

**Priority:** P2

**Type:** Missing build-language contract

`T/modules-and-imports.md:16-40` says `src/graphics/color.ark` must declare
`module graphics.color`, but does not define project root, source roots, file
extension rules, module search paths, package identity, duplicate module
resolution, or how the build graph supplies imported modules.

The ostensibly exhaustive declaration-order list at `:170-190` also omits
failures, interfaces, `implements`, C declarations, and unions, even though other
pages say some of them are order-independent.

**Fix direction:** define the project-to-module mapping as a separate normative
contract and make declaration-order coverage complete.

### SPEC-14 — Raw-pointer and slice conversion operations are not a closed API

**Priority:** P1

**Type:** Missing normative operation surface

Several operations are used or promised without complete definitions:

- `reinterpret(...)` is used at
  `T/c-interoperability/functions-and-abi.md:306-325` and
  `T/c-interoperability/globals-errors-variadics-void.md:304-318`, but only
  `bitcast(...)` is canonically defined
  (`T/expressions-and-numeric-operations.md:34-70`).
- `T/slices.md:175-190` promises construction of a slice from a raw pointer and
  length without giving a name, signature, result type, mutability form, or
  failure/trap behavior.
- `T/raw-pointers.md:38-46` names pointer reinterpretation and pointer-to-reference
  conversion without canonical syntax.
- `pointer_address(...)` and `pointer_from_address(...)` at
  `T/raw-pointers.md:131-145` lack exact signatures and zero/null/provenance rules.
- `address(place)` is defined under C interoperability
  (`T/c-interoperability/ownership-callbacks-addresses.md:144-232`), while the
  quick reference links it to the raw-pointer page, which does not define it.

**Fix direction:** create one normative built-in operations catalog with exact
signatures, safety context, access-mode preservation, provenance, lifetime,
null/zero behavior, and failure/trap behavior.

### SPEC-15 — Hook names and lookup rules are incomplete

**Priority:** P2

**Type:** Missing reserved-name and lookup contract

`T/operator-hooks.md:64-72` gives checked/wrapping hook names for add, subtract,
and multiply, then says division, remainder, and shifts “likewise” have checked
hooks without naming the reserved identifiers or reverse variants.

Generic overload wording says resolution collects visible declarations
(`T/calls-overloads.md:76-85,190-195`), while private hooks may still enable public
syntax (`T/interface-visibility.md:53-90` and
`T/comparisons-and-membership.md:118-130`). Hook lookup therefore cannot simply be
ordinary visible-overload lookup.

The navigation label “Hook index” (`mkdocs/mkdocs.yml:91`) compounds the problem:
`T/compiler-hooks.md:17-33` contains only families and examples, not a complete
name/signature/fallibility table.

**Fix direction:** define compiler hook lookup separately from direct-call
visibility and add a real exhaustive hook index, or rename the existing page to
“Compiler hooks overview.”

### SPEC-16 — Concurrency is admitted without a memory model

**Priority:** P2

**Type:** Underspecified systems contract

`T/c-interoperability/callback-execution-symbols.md:5-37` permits arbitrary
native-thread, concurrent, and reentrant callback entry and requires
program-provided synchronization. However, the language defines no data-race,
memory-order, atomic, volatile, or synchronization semantics. The C globals page
defers volatile/atomic features to a future version
(`T/c-interoperability/globals-errors-variadics-void.md:87-89`).

**Fix direction:** either define the minimum memory model and synchronization
surface required by the callback contract, constrain 1.0 callbacks to a model
that can be expressed, or state that all shared-state access must be encapsulated
behind specified unsafe C operations.

### SPEC-17 — Several boundary cases still have no defined result

**Priority:** P2

**Type:** Underspecified design

- `pointer(empty_slice)` is said to return a pointer to the first element
  (`T/slices.md:175-184`), but an empty slice has no first element. Null,
  one-past, sentinel, and C-call validity are undefined.
- C may produce an `export "C" enum` value that matches no Arkoi member, but
  `T/c-interoperability/types-and-layout.md:186-236` does not say whether it is
  accepted, trapped, rejected at the boundary, or undefined behavior.
- Raw-pointer arithmetic in `T/raw-pointers.md:74-129` does not say whether it is
  permitted for `*c.void`, opaque/incomplete pointees, or function pointers.
- Built-in array and slice indexing/range rules do not state the exact accepted
  index and bound types; custom slicing does explicitly require `?usize`.

**Fix direction:** add these cases to the relevant operation matrices and tests.

### SPEC-18 — Canonical and illustrative iteration interfaces collide

**Priority:** P2

**Type:** Cross-page model drift

`T/interfaces-overview.md:17-26` and
`T/interface-associated-types.md:12-20,69-114` use illustrative interfaces named
`Iterator` with `next` and varying associated types. The compiler-recognized
canonical `Iterator` instead has `Item` and `__next__`
(`T/iteration-protocols.md:20-50,197-203`).

`T/interface-implementations.md:154-165` also binds only `Item` in an
`implements Iterable for Buffer`, while canonical `Iterable` requires both
`Item` and `Iterator` (`T/iteration-protocols.md:27,52-60`).

**Fix direction:** rename noncanonical teaching interfaces, or use the canonical
shape everywhere and provide all required bindings.

### SPEC-19 — Future-facing placeholders appear in normative 1.0 text

**Priority:** P3

**Type:** Normative-scope leak

Examples include “when generic constraints are introduced”
(`T/interface-extension.md:36-38`), volatile/atomic operations “when Arkoi
specifies those features”
(`T/c-interoperability/globals-errors-variadics-void.md:87-89`), and undefined
`panic` terminology in the callback chapter.

**Fix direction:** move these to clearly non-normative future notes or remove them
from the 1.0 contract.

## 3. Invalid, contradictory, or misleading target examples

`T/index.md:67-71` promises that examples not labeled as compile-time errors obey
the target. The cases below break that promise or are too ambiguous to be used as
normative examples.

### EX-01 — The source uses comment placeholders as block bodies

**Priority:** P1

**Artifact:** Attached monolith

The monolith contains 83 literal `# ...` placeholders plus `# fields`. Many are
the only apparent body of a positive example, including `S:420-423`,
`S:1285-1291`, `S:1818-1820`, `S:3844-3861`, and `S:8031-8038`. The same source
says comments do not satisfy the nonempty-block rule and `pass` is required
(`S:49-70,125-160`). Some functions also promise a result but contain only a
comment.

None of the monolith's 806 Arkoi fences is contextually titled as a fragment,
pseudocode, complete program, or intentional error.

**Fix direction:** use `pass` where a valid empty body is intended, supply a real
return where needed, and label fragments/pseudocode/errors consistently.

### EX-02 — The source returns `string_view` where `string` is required

**Priority:** P1

**Artifact:** Attached monolith

`S:1095-1099` declares `StartSignal.display(...) @string` and returns the literal
`"start"`. String literals have type `string_view`, and the target says conversion
to owned `string` is explicit and fallible (`T/types-values.md:151-176`). The
split reference silently omitted the example.

**Fix direction:** return `string("start")!` with an appropriate failure effect,
return `string_view`, or change the API contract deliberately.

### EX-03 — Source slicing rules contradict one another

**Priority:** P1

**Artifact:** Attached monolith

- `S:6640-6697` says read-only contexts select `__slice__` and mutable contexts
  select `__slice_mut__`; `S:6797-6832` later says ordinary slicing is always
  read-only and only `&mut` on the complete slice expression selects mutable
  slicing.
- Initial hook examples use `usize` bounds (`S:6647-6661`), while the later
  normative representation is `?usize` (`S:6701-6748`).
- `S:6771-6781` presents `__slice_copy__` as if it were a recognized hook even
  though only language-defined double-underscore names are reserved.

The split page silently chooses the later rule and renames the helper to
`Container.slice_copy` (`T/slicing.md:7-27,87-120`).

**Fix direction:** make the split rule the single source rule and record the
normalization in errata.

### EX-04 — The source stores slices of array temporaries

**Priority:** P2

**Artifact:** Attached monolith

`S:4231-4235` initializes stored slices from `[1, 2][..]` and
`[1, 2, 3][..]`. A slice may not outlive its backing storage
(`T/slices.md:9-13,120-130`), and Arkoi has no general temporary lifetime
extension. The split comparison page removed these bindings.

**Fix direction:** bind the arrays first, or keep the values inside a single
expression whose lifetime rule is explicitly defined.

### EX-05 — The source representative program omits its required module

**Priority:** P2

**Artifact:** Attached monolith

The representative program at `S:11316-11358` is presented as a program but has
no `module` declaration. Every source file must start with exactly one
(`T/modules-and-imports.md:16-31`). The split page correctly adds
`module example.file_demo` and states its external assumptions
(`T/representative-program.md:14-27`).

**Fix direction:** keep the corrected split form and update the source.

### EX-06 — Additional source-to-split repairs are undocumented

**Priority:** P2

**Artifact:** Both target forms

- Empty marker-interface bodies around `S:8077-8110` violate the empty-block rule;
  split pages add `pass` (`T/interfaces-overview.md:99-123`).
- `S:10547-10554` produces `*mut u8` and assigns it to
  `*mut c.unsigned_char`; the split corrects the destination type but retains a
  separate initialization defect.

**Fix direction:** add both changes to the normalization/errata log and correct
the source.

### EX-07 — A split enum example refers to an undeclared enum

**Priority:** P1

**Artifact:** Split reference

`T/aggregates-enums.md:151-178` declares `Code` but then converts
`HttpMethod.get` and `HttpMethod`. The source had a preceding `HttpMethod`
declaration; extraction removed the required context.

**Fix direction:** use `Code` consistently or restore a self-contained
`HttpMethod` declaration.

### EX-08 — An immutable array is later mutated

**Priority:** P1

**Artifact:** Both target forms

`T/arrays.md:21-23` declares `values @[4]u32`, then `:97-104` writes
`values[2] = 42`, despite `:43-44` requiring mutable array access. The source has
the same problem at `S:2812-2814` and `S:2905-2914`.

**Fix direction:** declare a separate `values @mut [4]u32` for mutation examples.

### EX-09 — An indexing example uses unsupported instance-dot call syntax

**Priority:** P1

**Artifact:** Both target forms

`T/indexing.md:34-41` contains
`container[index]!.mutating_method()`, while Arkoi requires a declaring type or
interface and an explicit receiver (`T/methods-visibility.md:45-75`). The source
has the same invalid form at `S:6412-6419`.

**Fix direction:** use a form such as
`Element.mutating_method(&mut container[index]!)` with a declared signature.

### EX-10 — A moved replacement is moved a second time

**Priority:** P1

**Artifact:** Split reference

`T/places-replacement.md:152-160` moves `replacement` at line 156 and reuses it at
line 159 without reinitialization. Extraction merged examples into a single code
block and made the use-after-move unambiguous.

**Fix direction:** use two variables, separate explicitly independent fragments,
or reinitialize a mutable binding.

### EX-11 — Import examples create forbidden resource globals

**Priority:** P1

**Artifact:** Both target forms

`T/modules-and-imports.md:84-92,115-121` places fallible `File.open(path)!`
initializers at module scope. Resource globals are forbidden and module
initializers must be static (`T/constants-globals.md:116-153`). The examples also
depend on runtime `path`. They derive from `S:9202-9225`.

**Fix direction:** retain module-level imports, but put the value uses inside a
function.

### EX-12 — C examples use storage before it is initialized

**Priority:** P1

**Artifact:** Both target forms

- `T/c-interoperability/globals-errors-variadics-void.md:304-314` declares
  `typed` without an initializer and immediately reads it in `reinterpret`.
- `T/c-interoperability/layout-arrays-unions.md:150-170` declares an
  uninitialized union, then writes a field. General definite initialization
  defines assignment of the whole binding, not field access on an uninitialized
  aggregate.

The source contains both patterns (`S:10547-10554`, `S:11088-11106`).

**Fix direction:** initialize the pointer from an actual value. For unions,
either add construction syntax or normatively state that the first union-field
write initializes the complete union before using that example.

### EX-13 — Alternatives and compiler pseudocode are presented as ordinary code

**Priority:** P2

**Artifact:** Both target forms

- `T/iterator-values-and-lifetimes.md:36-40` declares `type Item` three times in
  one code block. They are alternatives, but ordinary Arkoi sees duplicates.
- Conceptual lowering at `T/iteration-protocols.md:104-129` uses
  `account @&Account = next!`. Optional `!` requires a permitting failure effect;
  the preceding `none` test would need flow-sensitive optional refinement, which
  the target does not define.

**Fix direction:** use a table or separate alternative blocks for the aliases and
label lowering as compiler pseudocode with intentionally non-source operations.

### EX-14 — Some iteration examples omit required protocol pieces

**Priority:** P2

**Artifact:** Split reference

`T/iterator-values-and-lifetimes.md:10-30` shows `FileLoader` only as a
`FallibleIterator`, then uses `for! file in loader`. A loop source first requires
an iterable creation interface (`T/iteration-protocols.md:10-29`). This may rely
on omitted context, but the example does not identify it.

**Fix direction:** show the creation implementation or explicitly state the
assumed surrounding declaration.

## 4. Current-compiler documentation

### CUR-01 — The root README gives stale and broken onboarding

**Priority:** P1

**Type:** Confirmed current-state drift

The most visible repository document conflicts with the build and current site:

- `README.md:56-57` allows CMake 3.29.6 and MSVC; `CMakeLists.txt:2` requires
  CMake 4.2, and the driver uses POSIX APIs plus GNU `as`/`ld`
  (`src/arkoi_language/utils/driver.cpp:7-8,221-266`).
- `README.md:64` uses the old `Excse/arkoi_language` clone location, while the
  repository remote and current site use `ArkoiSystems/arkoi_language`.
- `README.md:69-75` omits the required `pretty_diagnostics` package and prefix
  (`CMakeLists.txt:25`).
- `README.md:80` runs `./arkoi_language`; the actual target is
  `build/arkoi_language_app` (`CMakeLists.txt:145`).
- The command also omits `--emit-asm`. On a fresh input, later stages consume the
  expected assembly path although assembly is written only with that flag
  (`src/main.cpp:63-85`).
- The copied help at `README.md:97-128` omits `-O0`/`-O1` and repeats incorrect
  mode claims.
- The project tree at `README.md:133-160` is stale and calls `example/test` an
  example of every Arkoi feature, which is false for the 1.0 target.
- It never directs readers to the current/target documentation split.

**Fix direction:** replace the duplicated setup and CLI transcript with a short
supported-platform statement, one canonical build/run command, and links to the
current quickstart and Arkoi 1.0 target.

### CUR-02 — Built-in `--help` contradicts actual driver behavior

**Priority:** P1

**Type:** Confirmed implementation/help mismatch

`src/main.cpp:148-152` says compile-only generates `.s` and assemble-only
generates `.o`, but assembly output is opened only when `--emit-asm` is set
(`src/main.cpp:63-67`) and the assemble stage consumes the `.s` path
unconditionally (`:78-85`). `C/cli.md:34-39` correctly warns about the current
limitation, but the executable and copied README do not.

The help source also embeds backspace characters in descriptions
(`src/main.cpp:138-142`) and misspells “Optimization” as “Optimiozation” at
`:174`.

**Fix direction:** preferably fix the driver so required intermediate assembly
is always produced. Then make built-in help, README, and the CLI page share the
same contract and remove the control characters/typo.

### CUR-03 — The compatibility guide overstates current cast syntax

**Priority:** P2

**Type:** Confirmed implementation/doc mismatch

`C/compatibility.md:26` says the current parser accepts postfix casts such as
`value @u32`. It recognizes `@type` only immediately after a numeric token
(`src/arkoi_language/front/parser.cpp:492-503`); identifiers and calls return
without accepting the suffix (`:505-509`). `C/examples.md:101` similarly calls
the stress fixture's behavior “casts” when it mainly exercises literal typing and
implicit conversions that later appear as cast IL.

**Fix direction:** say “numeric-literal type annotation and implicit numeric
conversion,” not general postfix expression casting.

### CUR-04 — The target entry-point comparison is too absolute

**Priority:** P2

**Type:** Confirmed cross-page mismatch

`C/compatibility.md:19` says the target form is exactly a parameterless,
value-less `fun main():`. The target permits `main` to declare a failure effect
(`T/functions-returns.md:177-188`), and the Learn capstone uses
`fun main() !ReportFail:` (`mkdocs/docs/learn/capstone.md:125`).

**Fix direction:** say “parameterless and without a return type; may declare a
failure effect.”

### CUR-05 — The roadmap implies completion but is neither a target roadmap nor current

**Priority:** P2

**Type:** Scope and freshness defect

Every `ROADMAP.md` item is checked even though `README.md:167` calls it a list of
planned features. It does not say that it inventories compiler internals rather
than Arkoi 1.0, whose major systems remain unimplemented
(`C/compatibility.md:27`). Paths such as `include/front/...` and `test/...` are
stale (`ROADMAP.md:14-29,97-106`), and the optimization list omits implemented
constant and copy propagation
(`src/arkoi_language/utils/driver.cpp:107-111`).

**Fix direction:** make it either a real Arkoi 1.0 implementation-status matrix
or an explicitly historical/current-compiler component inventory. Do not call a
fully checked inventory a plan.

### CUR-06 — Fresh installation omits a build backend

**Priority:** P3

**Type:** Setup completeness gap

`C/installation.md:13-20,27-35` lists CMake, GCC, Git, and binutils, but not Make
or Ninja. The configure commands do not select a generator, so an otherwise
minimal system using an official CMake binary may lack a usable backend.

**Fix direction:** install `make`/`build-essential`, or install `ninja-build` and
use `-G Ninja` consistently.

### CUR-07 — Checked Fibonacci artifacts are not reproducibly current

**Priority:** P3

**Type:** Maintenance/freshness issue

`C/pipeline.md:17-24,50-63` presents checked-in IL, DOT, and assembly as output
from its `-O1` command. Regeneration with the current executable changes internal
temporary/block IDs, for example checked `example/fibonacci/fibonacci.il:4` uses
`$05.0` while fresh output uses `$04.0`; DOT and assembly comments also differ.
The narrative remains correct, and line 63 already warns that IDs can change.

**Fix direction:** regenerate before publishing or add a documentation freshness
check that generates and diffs these artifacts.

## 5. Site structure, build, and editorial consistency

### DOC-01 — Documentation builds depend on the current working directory

**Priority:** P2

**Type:** Reproducibility defect

`mkdocs/mkdocs.yml:166-171` configures MkDoxy with `../include/arkoi_language`
and `../Doxyfile`. From the repository root, this command fails because
`../Doxyfile` is resolved from the wrong directory:

```bash
mkdocs/.venv/bin/mkdocs build --strict \
  --config-file mkdocs/mkdocs.yml \
  --site-dir /tmp/arkoi-doc-audit-site
```

Running from `mkdocs/` succeeds. CI happens to set that working directory, but
`mkdocs/docs/development/index.md:47-61` gives no local preview/build procedure.

**Fix direction:** make plugin paths resolve relative to the configuration or
repository root. At minimum, document `cd mkdocs`, dependency installation,
`serve`, and `build --strict`.

### DOC-02 — Documentation breakage is not checked strictly on pull requests

**Priority:** P2

**Type:** Preventive tooling gap

`.github/workflows/github-pages.yml:3-6` runs only on pushes to `main` or manual
dispatch, and its build at `:39-41` omits `--strict`. A broken doc change can
therefore merge before the deployment workflow finds it.

**Fix direction:** add a non-deploying pull-request job that installs the same
dependencies and runs the documented strict build from the supported directory.

### DOC-03 — Two adjacent interface pages have the same identity

**Priority:** P2

**Type:** Navigation/search ambiguity

`T/static-interfaces/index.md:2,6` and `T/interfaces-overview.md:2,6` both use
metadata title `Arkoi 1.0 static interfaces` and H1 `Static interfaces`. Both are
adjacent in navigation (`mkdocs/mkdocs.yml:131-134`), so browser tabs and search
results cannot distinguish the guide from the normative model.

**Fix direction:** use distinct names such as “Static interfaces guide” and
“Static-interface model and declarations.”

### DOC-04 — The reference metadata table has empty headers

**Priority:** P2

**Type:** Accessibility defect

`T/index.md:8-17` uses a Markdown table with two blank header cells. It renders as
empty `<th>` elements and gives assistive technology no column context.

**Fix direction:** use a definition list or visible `Property` and `Value`
headers.

### DOC-05 — The reference landing page bypasses its section guides

**Priority:** P2

**Type:** Information-architecture inconsistency

Cards at `T/index.md:25-56` link directly to individual leaf pages, while
navigation has dedicated foundations, ownership, expressions/control, and static
interface guides (`mkdocs/mkdocs.yml:92-149`). Static interfaces lack a landing
card, and C interoperability has no direct call to action.

**Fix direction:** make landing-page groups match navigation groups and link each
card to the corresponding section guide.

### DOC-06 — Most normative leaf pages lack page-specific metadata

**Priority:** P2

**Type:** Search/share metadata gap

Only 8 of 57 target-reference Markdown files have YAML title/description
frontmatter. The remaining 49 inherit the generic site description; examples
include `T/types-values.md:1` and `T/failures.md:1`.

**Fix direction:** add concise topic-specific title/description metadata that
also identifies the page as Arkoi 1.0 target material.

### DOC-07 — Audience notices depend on rendering and pollute search

**Priority:** P2

**Type:** Status/discoverability issue

`mkdocs/hooks.py:50-63` injects the target warning only during MkDocs rendering,
so a leaf page read as raw Markdown has no target status. The identical warning
then appears in the built search corpus for 56 pages, making searches for
“target,” “current,” or “compiler” match nearly the whole reference.

Current notices also use temporal “today” wording (`mkdocs/hooks.py:76-80`), as do
several Learn pages, so the text ages without a release identifier.

**Fix direction:** put durable target version/status metadata in each source page,
keep a visible rendered badge/admonition, exclude boilerplate from search, and
refer to a compiler release or docs revision instead of “today.”

### DOC-08 — Quickstart and installation duplicate a long setup procedure

**Priority:** P2

**Type:** Drift risk

`C/quickstart.md:8-40` and `C/installation.md:11-87` both contain the
`pretty_diagnostics` plus Arkoi clone/configure/build workflow, with already
different flags. This is a likely source of future inconsistency and contributes
to the excessive current-state scope.

**Fix direction:** make Installation the canonical command source. Keep
Quickstart to prerequisites, invocation, one example, and expected output, or
include one checked shared snippet.

### DOC-09 — Canonical names drift across H1s, navigation, maps, and prose

**Priority:** P2

**Type:** Editorial consistency defect

Examples:

- `T/access-reduction.md:1` says “Access reduction,” while navigation, the map,
  and quick reference say “Read-only conversion.”
- `T/ownership-moves.md:1` says “Ownership and moves,” while navigation/map say
  “Moving resources.”
- `T/places-replacement.md:1` says “Places and replacement,” while navigation and
  quick reference say “Resource-valued places.”
- `T/constants-globals.md:1` says “module globals,” while the defined concept is
  “module variables” at `:106`.
- Learn navigation labels at `mkdocs/mkdocs.yml:75-85` shorten or change several
  H1s.

**Fix direction:** establish a canonical page-title and link-text list. Short nav
labels may differ deliberately, but should not introduce a competing technical
term.

### DOC-10 — Core concepts use competing terms

**Priority:** P2

**Type:** Terminology consistency defect

- `@own` parameters are called “ownership-taking,” “owning,” and “consuming”
  (`T/types-values.md:10,177`; `T/ownership-moves.md:117,184`;
  `T/specification-map.md:32`).
- The glossary's “stable addressable place” becomes “stable addressable storage”
  and “stable place with storage” elsewhere (`T/glossary.md:43-45`;
  `T/references-lifetimes.md:111-113`;
  `T/c-interoperability/ownership-callbacks-addresses.md:160`).
- Functions without a return type are described as “value-less,” “no-value,”
  “returns no value,” and “without a declared return type.”

**Fix direction:** choose one glossary term for each concept. “Ownership-taking
parameter,” “stable addressable place,” and “function without a return type” are
the clearest current candidates.

### DOC-11 — Compile-time errors use the `failure` presentation class

**Priority:** P2

**Type:** Taxonomy/presentation conflict

There are 53 `!!! failure "Compile-time error ..."` admonitions across 19
reference pages, including `T/bindings-initialization.md:53`,
`T/ownership-moves.md:30`, and `T/source-syntax.md:71`. The language explicitly
distinguishes compile-time errors from recoverable failures
(`T/index.md:67-71`; `T/glossary.md:91-105`).

**Fix direction:** use an `error` or neutral admonition class for compile-time
diagnostics so the visual taxonomy reinforces the language taxonomy.

### DOC-12 — The monolith is difficult to navigate and structurally imbalanced

**Priority:** P2

**Type:** Source-document usability issue

The source has 11,358 lines, 166 numbered sections, and 77 fourth-level
subsections, but its contents at `S:10-26` links only 16 chapters. Chapter 8 has
two numbered sections while chapter 13 has 38.

**Fix direction:** if the monolith remains maintained, generate a detailed TOC
and reconsider chapter boundaries, especially Static Interfaces. A generated
monolith from the split source would remove much manual navigation work.

### DOC-13 — Generated API search is disabled despite a documented search task

**Priority:** P3

**Type:** Discoverability tradeoff needing resolution

`mkdocs/hooks.py:38-40` excludes every generated API page from search, while
`mkdocs/docs/development/index.md:24-26` tells contributors that finding a C++
type, namespace, or header is a primary task. The built search index contains no
generated API entries.

**Fix direction:** provide a separate API search, or index only API page titles
and symbols if full generated prose is too noisy.

### DOC-14 — The normative typography is unusually small

**Priority:** P3

**Type:** Readability/accessibility recommendation

`mkdocs/docs/stylesheets/extra.css` sets body text to `0.81rem` (`:195`), mobile
body text to `0.78rem` (`:551`), navigation to `0.69rem` (`:132`), and table
headers to `0.7rem` (`:369`). This is not automatically a standards violation,
but it makes dense normative material harder to read.

**Fix direction:** test browser zoom and small screens with representative dense
pages; approximately `1rem` body text is a safer default.

### DOC-15 — Source provenance is not accessible from the published reference

**Priority:** P3

**Type:** Traceability gap

`T/index.md:14` names v150 as plain text, and `T/specification-map.md:7` refers to
`Arkoi_Language_Specification_v150.md`, but that file is outside `docs_dir` and is
not linked on the site.

**Fix direction:** publish/link the exact source revision and errata, or state
that the map is an internal maintenance artifact and remove the public-looking
provenance claim.

### DOC-16 — Heading-case policy is inconsistent

**Priority:** P3

**Type:** Editorial polish

Fourth-level headings in the source alternate between sentence case and title
case, for example `S:305,313,325` versus `S:336,2110,2277`.

**Fix direction:** choose a heading-case policy and apply it mechanically after
the semantic edits, to avoid churn while sections are moving.

## 6. Checks that passed

The following areas were checked and do not currently need issue entries:

- All 77 authored Markdown pages are represented in navigation.
- The three nav targets absent before build are expected MkDoxy-generated class,
  namespace, and file indexes.
- A strict MkDocs build from `mkdocs/` succeeds with MkDocs 1.6.1 and no project
  warnings.
- Built-site validation found zero missing local files and zero missing fragment
  identifiers.
- Source-level checks found no broken authored local links or anchors, excluding
  the expected generated MkDoxy targets.
- Every authored rendered page has one H1; there are no skipped heading levels.
- The duplicate Static Interfaces pair is the only duplicate authored H1 and
  metadata title.
- All published code fences have a language label, all checked snippet targets
  exist, and no inconsistent Markdown table row widths were found.
- Every numbered source section appears in `T/specification-map.md`; no whole
  numbered section was lost during extraction.
- No literal `TODO`, `TBD`, or `FIXME` remains in the target material.
- Audience separation on the rendered site is otherwise strong: the homepage,
  Learn landing page, every Learn chapter, and injected page notices distinguish
  target examples from the current executable.
- The documented current hello-world and factorial examples compile/run with
  status `0`; Fibonacci reports and returns `109`, as documented.
- Current MkDocs commands that include `--emit-asm` match the observed driver
  behavior, including the linked-file permission and multi-source caveats.

## 7. Suggested fixing order

1. Resolve `SPEC-01` before treating the target as a safe-language contract.
2. Decide `GOV-01` and `GOV-02`: version/status vocabulary and the one normative
   source of truth.
3. Close foundational definition gaps `SPEC-02` through `SPEC-08` and `SPEC-14`.
4. Repair all invalid examples in section 3 and add an example-validation policy.
5. Fix the public current entry points `CUR-01` and `CUR-02`.
6. Reduce/reorganize the current-state section per `GOV-03`, preserving detailed
   compiler material under Development.
7. Address remaining semantic/design questions, then terminology and site/tooling
   improvements.

For each semantic issue, record the chosen rule in a small decision log before
editing both target forms. That will prevent another silent normalization pass
from creating two plausible but different specifications.
