# Session Handoff

## Current Context

- Workspace: `D:\code\stablecore-geometry`
- Handoff updated on: `2026-03-27`
- `python` is available
- future sessions should focus on writing code and documents only
- compile/run/build should be done manually by the user
- do not worry about `gtest` environment provisioning; the user will adjust CMake/build side as needed

## Current Focus Order

1. boolean degeneracy / overlap robustness
2. offset recovery in harder cases
3. SearchPoly-style branch scoring / fake-edge ranking
4. richer relation hierarchy
5. polygon cut with holes

## Current 2D State

### Build / Test State

- `tests/` gtest migration is real current state
- current test tree includes:
  - `tests/capabilities`
  - `tests/gaps`
  - `tests/support`
- `tests/CMakeLists.txt` already wires capability and gap gtest executables
- `stablecore_geometry_capabilities_gtest` has passed after the latest repair pass

### API / Naming State

- old `GetXxx`-style 2D API call sites have been cleared from code
- `MultiPolyline2d::Count()`, `MultiPolygon2d::Count()`, and `PolygonTopology2d::Count()` are the current unified forms
- rename-related compile fallout and capability-test fallout from this round were already repaired
- `docs/sdk-type-design-review.md` remains the main API/design review baseline
- member-method vs free-function boundary was written down explicitly in:
  - `docs/sdk-2d-api-convergence.md`
  - `docs/api-member-free-function-checklist.md`

### 2D Design Doc Sync State

- the core 2D design docs were rewritten/synchronized to current API state:
  - `docs/segment-design.md`
  - `docs/polyline-design.md`
  - `docs/polygon-design.md`
  - `docs/box-design.md`
- current doc-sync progress is tracked in:
  - `docs/design-doc-sync-tracker.md`
- future sessions should update `docs/session-handoff.md` whenever a meaningful task is completed, not only at the end of a long batch

### Geometry / Algorithm State

Already implemented:
- `BuildMultiPolygonByLines`
  - open line-network polygon build
  - intersection splitting
  - duplicate-edge cleanup
  - near-endpoint auto-close
  - simple projection-style auto-extend
  - dangling branch pruning
  - outer / hole nesting merge
  - synthetic/fake edge tracking
  - candidate loop area threshold filtering
  - fake-edge-dominated tiny-loop suppression
  - real-edge-first duplicate retention
- Boolean
  - arrangement face extraction
  - bounded-face classification
  - result boundary rebuild
  - ordinary crossing / containment
  - duplicate-edge preprocessing
  - tiny sliver face filtering
  - stronger face sampling and multi-probe face classification
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

## Current Boolean State

Boolean is no longer mainly blocked on ordinary crossing / containment / equal / touching / simple overlap.

Current remaining boolean gap:
- ultra-thin repeated-overlap families
- harder arrangement degeneracies beyond the current duplicate-edge cleanup, tiny-face filtering, stronger face sampling, and multi-probe face classification

Current capability expectations include:
- larger multi-step overlap family
  - `Intersect = 14.0`
  - `Union = 39.0`
  - `Difference = 12.0`
- near-degenerate repeated-overlap family
  - `Intersect = 10.000002`
  - `Union = 26.000007`
  - `Difference = 6.000003`

## Current Offset / Relation Boundary

The latest capability cleanup confirmed that some cases still belong to gaps rather than stable capabilities:
- single-polygon offset does not yet reliably preserve hole semantics after rebuild
- inward offset of a narrow bridge does not yet reliably split into stable multipolygon output
- hole-aware polygon containment is not yet stable enough to keep in capability coverage
- some boolean and polygon-rebuild tests are stable at area/semantic level but not yet at exact result-shape level

Gap characterization already reflects this in:
- `tests/gaps/test_offset_gaps.cpp`
- `docs/test-capability-coverage.md`

## Important Docs To Read First Next Time

- `docs/next-task-prompt.md`
- `docs/delphi-geometry-parity.md`
- `docs/sdk-type-design-review.md`
- `docs/test-capability-coverage.md`
- `docs/design-doc-sync-tracker.md`

If working specifically on the current boolean gap, inspect these directly:
- `src/sdk/GeometryBoolean.cpp`
- `tests/capabilities/test_relation_boolean.cpp`
- `tests/gaps/test_boolean_gaps.cpp`

## Recommended Next 2D Action

The most reliable next task is:
- continue boolean robustness from the current remaining gap
  - target `tests/gaps/test_boolean_gaps.cpp`
  - focus on ultra-thin repeated-overlap families
  - prefer mechanism-level improvements over one-off case special-casing

If compile/test feedback becomes necessary:
- ask for the narrowest useful build/test step
- prefer one failing target or one focused test executable over broad validation

## Manual Validation Workflow

When feedback is needed:
- specify the smallest useful build/test step
- user runs it locally
- user pastes the relevant output
- patch code from the exact result

Preferred output from the user:
- for compile failure:
  - target name
  - first failing file and line
  - main error text
  - roughly 20 lines before/after if available
- for test failure:
  - executable or ctest target name
  - failing test case name
  - assertion text
  - relevant stdout/stderr
- for successful validation:
  - which target/test was run
  - whether it passed cleanly

## Current 3D Design State

The 3D work now has these design layers on disk:
- overall library direction
- foundational value types
- parametric curve/surface layer
- polyhedron/BRep topology layer
- triangle mesh / conversion layer
- shared 3D services layer
- validation / healing layer
- first-phase implementation roadmap
- section / tessellation / validation integration
- module / package layout

Key 3D conclusions:
- 2D API convergence rules should continue to constrain 3D naming and layering
- `Plane` and `PlaneSurface` should stay separate
- `Curve3d` / `Surface` are the core parametric protocols
- `CurveOnSurface` and preimage curves must be planned before BRep algorithms
- `PolyhedronBody` and `BrepBody` should not be merged early
- `BrepCoedge` is required, not optional

## Current 3D Code State

The first real 3D code slice is present, and the missing Phase A foundational value-type headers were expanded further:
- value-type headers:
  - `include/types/Point3.h`
  - `include/types/Vector3.h`
  - `include/types/Interval.h`
  - `include/types/Box3.h`
  - `include/types/Direction3.h`
  - `include/types/Matrix3.h`
  - `include/types/Transform3.h`
  - `include/types/Plane.h`
  - `include/types/Line3.h`
  - `include/types/Ray3.h`
  - `include/types/LineSegment3.h`
  - `include/types/Triangle3.h`
- `include/sdk/GeometryTypes.h`
  - now exposes `Point3d`, `Vector3d`, `Direction3d`, `Box3d`, `Intervald`, `Matrix3d`, `Transform3d`, `Plane`, `Line3d`, `Ray3d`, `LineSegment3d`, `Triangle3d`
  - adds `GeometryTolerance3d`, `GeometryContext3d`, `LineProjection3d`, `PlaneProjection3d`, `LinePlaneIntersection3d`, `PlanePlaneIntersection3d`
- `include/sdk/GeometryProjection.h`
  - adds `ProjectPointToLine(...)` and `ProjectPointToPlane(...)`
- `include/sdk/GeometryIntersection.h`
  - adds `Intersect(line, plane, tolerance)` and `Intersect(plane, plane, tolerance)`
- `include/sdk/GeometryRelation.h`
  - adds `PointPlaneSide3d`, `LocatePoint(point, plane, tolerance)`, `IsParallel(Vector3d, Vector3d, tolerance)`, `IsPerpendicular(Vector3d, Vector3d, tolerance)`
- `include/sdk/GeometryApi.h`
  - now pulls `GeometryProjection.h` and `GeometryIntersection.h` into the umbrella API
- service implementation files:
  - `src/sdk/GeometryProjection3d.cpp`
  - `src/sdk/GeometryIntersection3d.cpp`
  - `src/sdk/GeometryRelation3d.cpp`
- `CMakeLists.txt`
  - registers the three new 3D `.cpp` files in `stablecore_geometry`

Important current constraints:
- no compile was run yet for the 3D slice
- no tests were added yet
- Phase A is much closer to complete at header/API level, but still not validated by compile/test feedback
- `PlaneSurface`, `LineCurve3d`, and richer 3D object protocols are still design-only
- this slice intentionally stops before section/tessellation/body code

## Recommended Next 3D Action

When returning to 3D code, the most reliable next steps are:
- manually compile just the library target first and report any compile errors
- then, if compile is clean, add `PlaneSurface` and `LineCurve3d` headers/skeletons on top of the new value/service layer
- after that, start the minimal plane-dominant polyhedron section path identified by the design docs
