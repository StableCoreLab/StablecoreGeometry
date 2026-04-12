# stablecore-geometry

A lightweight C++ geometry library for engineering computation.

## Goals

- Build reusable geometry components for engineering software
- Provide stable and testable 2D geometry algorithms
- Support future quantity-takeoff and structural calculation products

## Current Scope

- Basic 2D geometric types
- Distance and projection
- Segment intersection
- Bounding box utilities
- Tolerance-based geometric predicates

## SDK Entry

For external consumers, prefer the umbrella header:

```cpp
#include "sdk/Geometry.h"
```

The first SDK surface is intentionally narrow:

- `geometry::sdk::GeoPoint2d`
- `geometry::sdk::GeoVector2d`
- `geometry::sdk::GeoBox2d`
- point-to-point distance and point-to-segment projection helpers
- box containment and box intersection helpers

## Design Principles

- Engineering-oriented, not academic-only
- Clear data structures
- Explicit tolerance handling
- Test-first for core algorithms

## Build

Visual Studio 2022 x64 is the default local build setup.

Quick options:

1. Generate the solution with the helper script:

```bat
generate_vs_solution.bat
```

This generates into `../StablecoreGeometryBuild` relative to the repository root and produces `stablecore_geometry.sln`.

2. Or use the checked-in CMake preset:

```bat
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-build --config Release
cmake --build --preset vs2022-x64-build --config RelWithDebInfo
```

The workspace also includes `.vscode/settings.json` so VS Code CMake Tools can use the same preset automatically.

## Install

The library installs into a `bin/lib/include` layout that separates configuration and platform:

- `Bin/Release/x64`
- `Bin/RelWithDebInfo/x64`
- `Lib/Release/x64`
- `Lib/RelWithDebInfo/x64`
- `include`
- `lib/cmake/stablecore_geometry`

After installing, downstream projects can consume it with CMake:

```cmake
find_package(stablecore_geometry CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE stablecore::geometry)
```

For a local install prefix, you can use either configuration:

```bat
cmake --install build --config Release --prefix D:\code\stablecore\Geometry\install
cmake --install build --config RelWithDebInfo --prefix D:\code\stablecore\Geometry\install
```
