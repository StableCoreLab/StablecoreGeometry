# Stable API Follow-Up Todo

This file is the lightweight follow-up list for API stabilization work.
The goal is to keep `include/sdk` as the only product-facing surface while
continuing to shrink the internal helper surface behind it.

## Current Focus

- Keep Delphi-facing interfaces stable.
- Prefer interface-first follow-up work over exposing new internal helpers.
- Move items from gap docs into capability tests only when the subset is stable.
- Keep the follow-up list short and action-oriented.

## P1: Public API Surface

- `GeometrySearchPoly`
  - Keep `include/sdk/GeometrySearchPoly.h` as the stable entry point.
  - Preserve the current subset: invalid-input contract, candidate ranking, branch scoring, candidate-level fake-edge diagnostics, result/diagnostics consistency, auto-flag gating, and smallest-containing lookup.
  - Next follow-up prompt: deepen richer fake-edge explanation and ambiguous recovery without regressing result consistency.

- `GeometryBodyBoolean`
  - Keep `include/sdk/GeometryBodyBoolean.h` stable.
  - Preserve the current subset: invalid-input contract, identical/disjoint closed-body capability, axis-aligned single-box overlap, and face-touching union.
  - Next follow-up prompt: deepen overlap semantics carefully while keeping touching intersection/difference and shell-policy in gap state.

- `Geometry.h`
  - Keep the umbrella header as the only product-facing aggregator.
  - Do not re-expose temporary helpers through the umbrella header.

## P2: Internal Algorithm Split

- `GeometryBrepConversion`
  - Keep the current non-planar repair decomposition aligned with:
    - support-plane scoring
    - representative target aggregation
    - cross-face snapping
    - topology reconciliation
  - Follow-up prompt: only add new repair subsets when the current representative-average coverage is still stable.

- `GeometryHealing`
  - Preserve the current split between conservative trim-backfill, shell-cap fallback, and aggressive closure.
  - Follow-up prompt: expand shell repair only when the new subset can be described clearly in the capability matrix.

- `GeometrySection`
  - Keep the current contour-processing split stable.
  - Follow-up prompt: continue toward more general mixed coplanar / non-planar arbitration without collapsing the gap boundary.

## P3: Documentation Hygiene

- Keep these documents synchronized after every batch:
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
- If the interface matrix changes, update:
  - `docs/delphi-interface-fasttrack.md`
  - `docs/delphi-test-fasttrack-matrix.md`
- If a capability becomes stable, move it out of the gap inventory in the same batch.

## Follow-Up Prompt Template

- "Deepen `GeometrySearchPoly` from the current branch-scored + fake-edge diagnostic subset toward richer explanation and ambiguous recovery."
- "Deepen `GeometryBodyBoolean` from identical/disjoint + axis-aligned single-box overlap / face-touching union toward richer overlap while keeping gap contracts stable."
- "Keep `GeometrySection` / `GeometryHealing` split clean and update capability/gap inventory in the same pass."

