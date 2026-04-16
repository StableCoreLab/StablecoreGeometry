# Final Naming Plan

This document is the authoritative naming rule for the published Geometry surface.

## Rules

- Keep `SC` only for engineering identities: repository name, solution name, CMake target name, package name, and internal build/test labels.
- Do not use `SC` in public type names, public function names, or public data structures.
- Do not use `ISC` in the default shipped API surface.
- Public ABI-stable data structures remain exported with `GEOMETRY_API`.
- The published API is organized by directory and by semantic type name, not by product prefix.

Namespace note:
- The current published API namespace is `Geometry`.
- `Geometry::Sdk` is only a temporary compatibility alias during the migration window.
- This naming plan applies to files, types, results, and public wrappers, not to the engineering namespace marker itself.

## Namespace Migration

Target public namespace:
- `Geometry`

Move out of `Geometry::Sdk`:
- `Include/Core/GeometryTypes.h`
- `Include/Core/Results.h`
- `Include/Core/Validation.h`
- `Include/Core/Algorithms.h`
- `Include/Core/AxisOps.h`
- `Include/Core/Boolean.h`
- `Include/Core/Editing.h`
- `Include/Core/Intersection.h`
- `Include/Core/Measure.h`
- `Include/Core/Metrics.h`
- `Include/Core/Offset.h`
- `Include/Core/Projection.h`
- `Include/Core/Relation.h`
- `Include/Core/Sampling.h`
- `Include/Core/SearchPoly.h`
- `Include/Core/Section.h`
- `Include/Core/ShapeOps.h`
- `Include/Core/Transform.h`
- `Include/Geometry2d/*.h`
- `Include/Geometry3d/*.h`
- `Include/Brep/*.h`
- `Include/Serialize/GeometryText.h`

Already in the final namespace:
- `Include/Types/*`
- `Include/Support/*`
- `Include/Export/*`
- `Include/Geometry.h` umbrella header itself

Compatibility strategy:
- Keep a single transition alias from `Geometry::Sdk` to `Geometry` instead of duplicating wrappers.
- Do not reintroduce `SC*` or `ISC*` compatibility classes.
- Do not ship wrapper headers as part of the default installed surface.
- Use the alias only as a temporary source-compatibility bridge for old call sites.
- Remove the alias after the migration window is closed.
- Prefer updating source, tests, and docs to `Geometry::...` directly rather than relying on the alias long term.

Migration checklist:
1. Move all public declarations in the listed headers to `namespace Geometry`.
2. Update source files to include and use `namespace Geometry` directly.
3. Update tests and documentation to reference `Geometry::...`.
4. Keep only the `Geometry::Sdk` namespace alias as the temporary compatibility layer.
5. Remove the alias once the release migration window is complete.

## Include/Core

Keep:
- `GeometryApi.h`
- `GeometryTypes.h`
- `Algorithms.h`
- `AxisOps.h`
- `Boolean.h`
- `Editing.h`
- `Intersection.h`
- `Measure.h`
- `Metrics.h`
- `Offset.h`
- `Projection.h`
- `Relation.h`
- `Results.h`
- `Sampling.h`
- `SearchPoly.h`
- `Section.h`
- `ShapeOps.h`
- `Transform.h`
- `Validation.h`

Delete from the public surface:
- `SC*` compatibility names
- `ISC*` compatibility names

## Include/Geometry2d

Keep:
- `ArcSegment2d.h`
- `BoxTree2d.h`
- `Circle2d.h`
- `Ellipse2d.h`
- `KDTree2d.h`
- `LineSegment2d.h`
- `MultiPolygon2d.h`
- `MultiPolyline2d.h`
- `PathOps.h`
- `Polygon2d.h`
- `Polyline2d.h`
- `Rectangle2d.h`
- `Segment2d.h`
- `SegmentSearch2d.h`

Delete from the public surface:
- `SCArcSegment2d.h`
- `SCCircle2d.h`
- `SCLineSegment2d.h`
- `SCMultiPolygon2d.h`
- `SCMultiPolyline2d.h`
- `SCSegment2d.h`
- `ISCPolygon2d.h`
- `ISCPolyline2d.h`

## Include/Geometry3d

Keep:
- `Curve3d.h`
- `CurveOnSurface.h`
- `LineCurve3d.h`
- `NurbsCurve3d.h`
- `NurbsSurface.h`
- `OffsetSurface.h`
- `PlaneSurface.h`
- `RuledSurface.h`
- `Surface.h`
- `TriangleMesh.h`

Delete from the public surface:
- `SCCurve3d.h`

## Include/Brep

Keep:
- `BodyBoolean.h`
- `BrepBody.h`
- `BrepCoedge.h`
- `BrepConversion.h`
- `BrepEdge.h`
- `BrepEditing.h`
- `BrepFace.h`
- `BrepLoop.h`
- `BrepShell.h`
- `BrepVertex.h`
- `Healing.h`
- `MeshConversion.h`
- `MeshOps.h`
- `MeshRepair.h`
- `PolyhedronBody.h`
- `PolyhedronFace3d.h`
- `PolyhedronLoop3d.h`
- `Tessellation.h`
- `Topology.h`

Delete from the public surface:
- No additional public wrapper names are kept here.

## Include/Support and Include/Types

Keep:
- `Support/Epsilon.h`
- `Support/Geometry2d/Normalize2.h`
- `Support/Geometry2d/Predicate2.h`
- `Types/Geometry2d/*` value types
- `Types/Geometry3d/*` value types
- `Types/Detail/Segment2Detail.h`

Delete from the public surface:
- Unused integer aliases and compatibility-only helpers
- Any `SC*` or `ISC*` wrapper headers

## ABI Notes

- All public value types that must cross DLL boundaries should stay in exported headers and keep `GEOMETRY_API`.
- Do not introduce `SC` prefixes into value-type names just to indicate DLL usage.
- If a type is only needed internally, keep it in a detail header or source file instead of shipping it in `Include/`.

