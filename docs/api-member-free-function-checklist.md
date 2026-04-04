# Member Method And Free Function Checklist

## Purpose

This checklist keeps the API surface stable while the geometry stack continues to converge.

## Rules

- Prefer member methods for object-native properties.
- Prefer free functions for algorithms that involve multiple objects, tolerances, context, or structured results.
- Do not re-expand the surface with temporary helpers just because they are convenient internally.

## Member-Method Candidates

These are still the right shape for member methods:

- `Count()`
- `Bounds()`
- `Length()`
- `Area()`
- `IsClosed()`
- `IsEmpty()`
- `VertexCount()`
- `VertexAt(...)`
- `StartPoint()`
- `EndPoint()`
- `OuterRing()`
- `HoleCount()`
- `HoleAt(...)`

## Free-Function Candidates

These remain better as free functions:

- distance
- projection
- intersection
- containment and relation queries
- boolean
- offset
- section / cut / split
- topology build
- rebuild / repair / preprocess

## Current Follow-Up Guidance

- Keep `GeometrySearchPoly` and `GeometryBodyBoolean` on the stable SDK path in `include/sdk`.
- Keep `GeometrySection`, `GeometryHealing`, and `GeometryBrepConversion` as internal implementation spaces with clear pass boundaries.
- Keep the product-facing `Options / Result / Issue` style consistent when adding new surface area.
- If a helper becomes a product dependency, promote it through the fast-track matrices before exposing it widely.

## Checklist For New APIs

1. Does the API depend only on the object itself?
2. Does it require tolerance, context, or multiple operands?
3. Does it return structured data rather than a simple property?
4. Does it leak implementation helpers into the public surface?
5. Does it fit the existing `include/sdk` style?

If any of 2 through 5 is yes, prefer a free function and document it in the fast-track matrix.

## Related Documents

- `docs/delphi-interface-fasttrack.md`
- `docs/delphi-test-fasttrack-matrix.md`
- `docs/rename-followup-todo.md`
- `docs/session-handoff.md`
