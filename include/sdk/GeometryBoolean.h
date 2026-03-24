#pragma once

#include "export/GeometryExport.h"
#include "sdk/MultiPolygon2d.h"
#include "sdk/Polygon2d.h"

namespace geometry::sdk
{
[[nodiscard]] GEOMETRY_API MultiPolygon2d Intersect(
    const Polygon2d& first,
    const Polygon2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API MultiPolygon2d Union(
    const Polygon2d& first,
    const Polygon2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API MultiPolygon2d Difference(
    const Polygon2d& first,
    const Polygon2d& second,
    double eps = 1e-9);
} // namespace geometry::sdk
