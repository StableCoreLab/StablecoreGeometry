#pragma once

#include "export/GeometryExport.h"
#include "sdk/Polygon2d.h"
#include "sdk/Polyline2d.h"

namespace geometry::sdk
{
[[nodiscard]] GEOMETRY_API Polyline2d Normalize(const Polyline2d& polyline, double eps = 1e-9);
[[nodiscard]] GEOMETRY_API Polygon2d Normalize(const Polygon2d& polygon, double eps = 1e-9);
[[nodiscard]] GEOMETRY_API Polyline2d InsertPoint(
    const Polyline2d& polyline,
    const Point2d& point,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API Polygon2d InsertPoint(
    const Polygon2d& polygon,
    const Point2d& point,
    double eps = 1e-9);
} // namespace geometry::sdk
