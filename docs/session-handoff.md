# Session Handoff

## Current Date Context

- Workspace: `D:\code\stablecore-geometry`
- Handoff written on: `2026-03-27`
- User clarified: `python` is now installed and available for future sessions.
- User also clarified: future sessions should focus on writing code and documents only; compile/run/build will be done manually by the user.
- User said: do not worry about `gtest` environment provisioning; they will modify CMake/build side manually as needed.

## High-Level State

This repository has been worked on for Delphi / GGP parity around polygon-related geometry algorithms.

Known current focus order remains:
1. boolean degeneracy / overlap robustness
2. offset recovery in harder cases
3. SearchPoly-style branch scoring / fake-edge ranking
4. richer relation hierarchy
5. polygon cut with holes

## What Was Confirmed Earlier In This Session

A design review was completed and written to:
- `docs/sdk-type-design-review.md`

That review concluded the main API/design concerns are:
- public naming style is not fully unified
- segment member methods and free functions overlap too much
- `Polyline2d` / `Polygon2d` use PImpl while sibling value types do not
- mutable `Data()` exposure is too loose on indexing classes
- `Geometry*` module boundaries have some overlap

## Geometry / Algorithm Status To Remember

Already implemented before this handoff:
- `BuildMultiPolygonByLines`
  - open line-network polygon build
  - intersection splitting
  - duplicate-edge cleanup
  - near-endpoint auto-close
  - simple projection-style auto-extend
  - dangling branch pruning
  - outer / hole nesting merge
- Boolean
  - arrangement face extraction
  - bounded-face classification
  - result boundary rebuild
  - ordinary crossing / containment
  - duplicate-edge preprocessing
  - tiny sliver face filtering
  - relation-aware fast path for `Equal`, `Disjoint`, `FirstContainsSecond`, `SecondContainsFirst`, `Touching`
- Offset
  - rebuild from offset rings
  - basic concave / multipolygon recovery
  - collapsed / near-zero ring filtering
  - better single-polygon candidate selection
  - split result kept for `MultiPolygon2d` offset
- Topology / Relation
  - touching / intersecting / contains / equal distinctions
  - shared-edge / collinear overlap no longer always misclassified as crossing
  - stricter interior checks with edge-midpoint assistance
  - equal duplicate polygon deterministic parent tie-break in topology
- SearchPoly-style improvements
  - synthetic/fake edge tracking in `BuildMultiPolygonByLines`
  - candidate loop area threshold filtering
  - fake-edge-dominated tiny-loop suppression
  - real-edge-first duplicate retention

## Important Geometry Docs Already Present

Use these first next time:
- `docs/next-task-prompt.md`
- `docs/delphi-geometry-parity.md`
- `docs/sdk-type-design-review.md`

## Test-System Status

A partial migration toward gtest was started conceptually in this session, but it should NOT be trusted as completed unless re-checked from local files in the next session.

What was intended:
- split tests into:
  - `tests/capabilities`
  - `tests/gaps`
- move existing tests under `tests/capabilities`
- convert standalone `main`-style tests to gtest `TEST(...)`
- add gap-characterization files under `tests/gaps`
- add a test capability coverage document

However, before ending this session, the worktree status only clearly showed one untracked file:
- `docs/sdk-type-design-review.md`

So next session must re-check the actual on-disk state before assuming any test migration landed.

## Safe Assumption For Next Session

When next session starts:
- first read `docs/next-task-prompt.md`
- then read `docs/sdk-type-design-review.md`
- then inspect current `tests/` directory and `tests/CMakeLists.txt`
- do not assume the gtest migration is either fully present or fully absent without checking the files directly

## User Preferences / Constraints

- only write code and documentation
- do not compile
- do not run builds
- do not depend on compile results
- do not revert existing user changes
- keep current API style unless there is a strong reason not to
- use local resources first

## Recommended Next Action

For the next session, the most reliable first task is:
- verify actual `tests/` tree state
- if needed, re-do the gtest/capability-gap test migration cleanly from the current on-disk baseline
- otherwise continue the geometry parity work, with priority on boolean robustness
