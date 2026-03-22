#pragma once

#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"

namespace geometry::sdk
{
[[nodiscard]] GEOMETRY_API double DistanceSquared(const Point2d& lhs, const Point2d& rhs);
[[nodiscard]] GEOMETRY_API double Distance(const Point2d& lhs, const Point2d& rhs);
[[nodiscard]] GEOMETRY_API SegmentProjection2d ProjectPointToSegment(
    const Point2d& point,
    const Point2d& segmentStart,
    const Point2d& segmentEnd,
    bool clampToSegment = true);
[[nodiscard]] GEOMETRY_API bool Contains(
    const Box2d& box,
    const Point2d& point,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API bool Intersects(
    const Box2d& lhs,
    const Box2d& rhs,
    double eps = 1e-9);
} // namespace geometry::sdk
