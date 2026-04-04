# AI Task Routing

## Goal

This file classifies remaining work by the AI capability it needs, so tasks can be assigned to different models without repeatedly re-evaluating scope.

## Routing Rules

- Use frontier, high-reasoning models for algorithm-strategy work.
- Use mid-tier coding models for bounded implementation and test expansion.
- Use lighter models for documentation, matrices, and synchronization work.
- Prefer stable SDK-facing work over ad hoc internal entry points.

## Task Classes

### Class A: Frontier Model + High Reasoning

Recommended models:

- `gpt-5.4`
- `gpt-5.2`
- `gpt-5.3-codex`

Recommended reasoning:

- `high`
- `xhigh`

Use this class for:

- `GeometryBrepConversion`
  - continue non-planar repair algorithm work
  - split and deepen:
    - support-plane scoring
    - representative target aggregation
    - cross-face snapping
    - topology reconciliation
- `GeometryBodyBoolean`
  - move beyond identical/disjoint deterministic subsets
  - add richer overlap `intersect / union / difference` behavior
- `GeometrySearchPoly`
  - Delphi-grade smart-search deepening
  - branch scoring
  - fake-edge explanation
  - ambiguous-result recovery
- `GeometryHealing`
  - more general aggressive shell policy
- `GeometrySection`
  - non-planar dominant contour stitching
  - higher-order coplanar fragment merge semantics

Why this class:

- strategy matters more than raw edit volume
- errors can destabilize SDK contracts
- tasks require balancing API, tests, and algorithm behavior together

### Class B: Mid-Tier Model + Medium/High Reasoning

Recommended models:

- `gpt-5.4-mini`
- `gpt-5.2-codex`
- `gpt-5.1-codex-max`

Recommended reasoning:

- `medium`
- `high`

Use this class for:

- expanding already-established `GeometryBodyBoolean` safe subsets
- expanding `GeometrySearchPoly` contract tests, diagnostics tests, and result consistency
- refactoring internal helpers after the high-level algorithm direction is already decided
- splitting large implementation files into clearer internal passes
- unifying `Options / Result / Issue` style across SDKs
- tightening `include/sdk` exposure and umbrella-header organization
- adding representative capability tests and deterministic assertions
- converting small, well-understood gap subsets into capability tests

Why this class:

- tasks are implementation-heavy but strategy-light
- they benefit from code understanding, not deep new algorithm design
- they parallelize well with low write-conflict risk if ownership is clear

### Class C: Lightweight Model + Low/Medium Reasoning

Recommended models:

- `gpt-5.4-mini`

Recommended reasoning:

- `low`
- `medium`

Use this class for:

- `docs/session-handoff.md`
- `docs/next-task-prompt.md`
- `docs/test-capability-coverage.md`
- `docs/design-doc-sync-tracker.md`
- `docs/delphi-interface-fasttrack.md`
- `docs/delphi-test-fasttrack-matrix.md`
- `docs/rename-followup-todo.md`
- commit message suggestions
- capability/gap inventory cleanup
- matrix and checklist maintenance
- preparing follow-up prompts for other AIs

Why this class:

- work is mostly synchronization and packaging
- low algorithmic risk
- useful sidecar work while stronger models stay on core geometry problems

## Current Recommended Assignment

### High Priority A-Class

- `GeometryBodyBoolean`
  - richer overlap subsets beyond identical/disjoint bodies
- `GeometryBrepConversion`
  - continue non-planar repair pass decomposition

### High Priority B-Class

- `GeometrySearchPoly`
  - branch scoring and fake-edge explanation prototype under stable SDK surface
- `GeometryHealing`
  - split conservative and aggressive internal paths
- `GeometrySection`
  - split contour extraction, deterministic segment normalization, and coplanar merge into clearer stages

### Ongoing C-Class

- keep all handoff, next-task, coverage, and fast-track matrix docs synchronized after every batch

## Suggested Ready-To-Assign Work Items

### Work Item 1

- area: `GeometryBrepConversion`
- class: `A`
- best fit: frontier coding model
- reasoning: `xhigh`
- deliverable:
  - advance non-planar repair
  - preserve SDK contract
  - add representative capability/gap updates

### Work Item 2

- area: `GeometryBodyBoolean`
- class: `A`
- best fit: frontier coding model
- reasoning: `high`
- deliverable:
  - add first overlap subset
  - keep invalid/unsupported contract stable
  - update capability/gap tests

### Work Item 3

- area: `GeometrySearchPoly`
- class: `A` or `B`
- best fit: stronger model if algorithm scoring changes, mid-tier if implementing already-decided direction
- reasoning: `high`
- deliverable:
  - deepen smart-search behavior under stable SDK surface

### Work Item 4

- area: `GeometryHealing`
- class: `B`
- best fit: mid-tier coding model
- reasoning: `medium/high`
- deliverable:
  - internal split of conservative vs aggressive paths
  - representative tests

### Work Item 5

- area: `GeometrySection`
- class: `B`
- best fit: mid-tier coding model
- reasoning: `medium/high`
- deliverable:
  - internal stage split
  - representative tests

### Work Item 6

- area: documentation and matrices
- class: `C`
- best fit: lightweight model
- reasoning: `low/medium`
- deliverable:
  - sync docs after each code batch

## Assignment Notes

- If the task changes algorithm strategy, move it up to Class A.
- If the task mostly follows an agreed plan and edits a bounded slice, Class B is usually enough.
- If the task is only documentation or matrix maintenance, keep it in Class C.
- Do not assign broad API redesign to lightweight models.
