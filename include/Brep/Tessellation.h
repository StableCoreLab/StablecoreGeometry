#pragma once

#include <cstddef>

#include "Export/GeometryExport.h"
#include "Geometry3d/PlaneSurface.h"
#include "Geometry3d/TriangleMesh.h"

namespace Geometry::Sdk
{
[[nodiscard]] GEOMETRY_API TriangleMesh Tessellate(
    const PlaneSurface& surface,
    std::size_t uSegments = 1,
    std::size_t vSegments = 1);
} // namespace Geometry::Sdk
