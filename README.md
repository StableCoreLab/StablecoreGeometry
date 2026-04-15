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

## SDK Entry

For external consumers, prefer the umbrella header:

```cpp
#include "sdk/Geometry.h"
```

`sdk/Geometry.h` is the stable umbrella entry point. It pulls in `GeometryApi`, `GeometryEpsilon`, `GeometrySearchPoly`, `GeometryBodyBoolean`, and the main 2D / 3D SDK headers.

Common aliases available through `Geometry::Sdk::GeometryTypes` include:

- `Point2d`, `Vector2d`, `Box2d`
- `Point3d`, `Vector3d`, `Direction3d`, `Box3d`, `Intervald`
- `Line3d`, `Ray3d`, `LineSegment3d`, `Triangle3d`
- `Transform3d`, `Matrix3d`, `Plane`

If you only need a narrow area, include the specific header directly instead of the umbrella.

## Design Principles

- Engineering-oriented, not academic-only
- Clear data structures
- Explicit tolerance handling
- Test-first for core algorithms

For deeper architecture notes, see [`docs/library-design.md`](docs/library-design.md).
For stable public naming rules, see [`docs/public-api-naming.md`](docs/public-api-naming.md).
For the current symbol audit, see [`docs/public-api-naming-audit.md`](docs/public-api-naming-audit.md).
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
- `include` for public headers
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
