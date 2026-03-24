#pragma once

#include "export/GeometryExport.h"
#include "sdk/ArcSegment2d.h"
#include "sdk/LineSegment2d.h"
#include "sdk/Polygon2d.h"
#include "sdk/Polyline2d.h"
#include "sdk/Segment2d.h"

namespace geometry::sdk
{
enum class PointContainment2d
{
    Outside,
    OnBoundary,
    Inside
};

[[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
    const Point2d& point,
    const LineSegment2d& segment,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
    const Point2d& point,
    const ArcSegment2d& segment,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
    const Point2d& point,
    const Segment2d& segment,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
    const Point2d& point,
    const Polyline2d& polyline,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
    const Point2d& point,
    const Polygon2d& polygon,
    double eps = 1e-9);

[[nodiscard]] GEOMETRY_API bool IsParallel(
    const LineSegment2d& first,
    const LineSegment2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API bool IsAntiParallel(
    const LineSegment2d& first,
    const LineSegment2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API bool IsSameDirection(
    const LineSegment2d& first,
    const LineSegment2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API bool IsEqual(
    const LineSegment2d& first,
    const LineSegment2d& second,
    bool ignoreDirection = true,
    double eps = 1e-9);
} // namespace geometry::sdk
