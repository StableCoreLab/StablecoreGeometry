# stablecore-geometry

A C++ geometry library for engineering computation.

## Goals

- Build reusable geometry components for engineering software
- Provide stable and testable 2D and 3D geometry services
- Support current section, healing, boolean, conversion, validation, and search workflows

## Current Scope

- 2D value types and geometry services
- 3D value types, curves, surfaces, BRep, and polyhedron support
- Section, healing, boolean, projection, relation, measurement, validation, and mesh conversion services
- Text serialization helpers and search/index helpers

## Include Entry

For external consumers, prefer the umbrella header:

```cpp
#include "Geometry.h"
```

`Geometry.h` is the stable umbrella entry point. It pulls in the main public headers for core operations, 2D/3D geometry, BRep, and shared support utilities.

When you want a narrower include set, the surface is split into the following module areas:

- `Core` for umbrella types and cross-cutting operations
- `Geometry2d` for 2D primitives and path utilities
- `Geometry3d` for curves, surfaces, and 3D evaluation helpers
- `Brep` for BRep, polyhedron, mesh, healing, and topology services

The lower-level surface is also split more explicitly:

- `Support/Epsilon.h` for shared tolerance constants
- `Support/Geometry2d` for 2D scalar, predicate, and normalization helpers
- `Types/Geometry2d` for 2D value types
- `Types/Geometry3d` for 3D value types and linear algebra helpers
- `Types/Detail` for implementation-detail type helpers
- `Serialize/GeometryText.h` for text serialization helpers
- `Export/GeometryExport.h` for import/export annotations

Public value types and result structures that need to cross DLL boundaries are exported from `Core/GeometryTypes.h` and the other public headers with `GEOMETRY_API`.

Common aliases available through the umbrella include:

- `Point2d`, `Vector2d`, `Box2d`
- `Point3d`, `Vector3d`, `Direction3d`, `Box3d`, `Intervald`
- `Line3d`, `Ray3d`, `LineSegment3d`, `Triangle3d`
- `Transform3d`, `Matrix3d`, `Plane`

If you only need a narrow area, include the specific header directly instead of the umbrella.

The release install is intentionally curated:

- Compatibility wrappers and detail headers are not part of the default installed public surface.
- The shipped header set is limited to the supported publish API.

## Design Principles

- Engineering-oriented, not academic-only
- Clear data structures
- Explicit tolerance handling
- Test-first for core algorithms

For deeper architecture notes, see [`docs/library-design.md`](docs/library-design.md).
For stable public naming rules, see [`docs/public-api-naming.md`](docs/public-api-naming.md).
For the current symbol audit, see [`docs/public-api-naming-audit.md`](docs/public-api-naming-audit.md).
For the final per-directory naming plan, see [`docs/final-naming-plan.md`](docs/final-naming-plan.md).
For code follow-up items, see [`docs/rename-followup-todo.md`](docs/rename-followup-todo.md).

## Build

Visual Studio 2022 x64 is the default local build setup.

Quick options:

1. Generate the solution with the helper script:

```bat
GenerateGeometryVS2022.bat
```

This generates into `../Build/GeometryVs2022` relative to the repository root and opens `SCGeometry.sln`.

2. Or use the checked-in CMake preset:

```bat
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-build --config Release
cmake --build --preset vs2022-x64-build --config RelWithDebInfo
```

The workspace also includes `.vscode/settings.json` so VS Code CMake Tools can use the same preset automatically.

## Install

The library installs into a release-friendly layout that separates configuration and platform:

### Release Layout

- `bin/Release/x64` for DLLs and executables
- `bin/RelWithDebInfo/x64` for DLLs and executables with debug info
- `lib/Release/x64` for import libraries and static archives
- `lib/RelWithDebInfo/x64` for import libraries and static archives with debug info
- `Include` for public headers
- `lib/cmake/SCGeometry` for CMake package files

After installing, downstream projects can consume it with CMake:

```cmake
find_package(SCGeometry CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE SCGeometry::Geometry)
```

For a local install prefix, you can use either configuration:

```bat
cmake --install build --config Release --prefix D:\code\stablecore\Geometry\install
cmake --install build --config RelWithDebInfo --prefix D:\code\stablecore\Geometry\install
```
