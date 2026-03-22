#pragma once

#include "export/GeometryExport.h"
#include "sdk/ArcSegment2d.h"
#include "sdk/GeometryResults.h"
#include "sdk/LineSegment2d.h"
#include "sdk/Segment2d.h"

namespace geometry::sdk
{
[[nodiscard]] GEOMETRY_API SegmentIntersection2d Intersect(
    const LineSegment2d& first,
    const LineSegment2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API SegmentIntersection2d Intersect(
    const LineSegment2d& first,
    const ArcSegment2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API SegmentIntersection2d Intersect(
    const ArcSegment2d& first,
    const ArcSegment2d& second,
    double eps = 1e-9);
[[nodiscard]] GEOMETRY_API SegmentIntersection2d Intersect(
    const Segment2d& first,
    const Segment2d& second,
    double eps = 1e-9);

[[nodiscard]] GEOMETRY_API bool HasIntersection(
    const Segment2d& first,
    const Segment2d& second,
    double eps = 1e-9);

[[nodiscard]] GEOMETRY_API ClosestPoints2d ClosestPoints(
    const LineSegment2d& first,
    const LineSegment2d& second);
[[nodiscard]] GEOMETRY_API ClosestPoints2d ClosestPoints(
    const LineSegment2d& first,
    const ArcSegment2d& second);
[[nodiscard]] GEOMETRY_API ClosestPoints2d ClosestPoints(
    const ArcSegment2d& first,
    const ArcSegment2d& second);
[[nodiscard]] GEOMETRY_API ClosestPoints2d ClosestPoints(
    const Segment2d& first,
    const Segment2d& second);
} // namespace geometry::sdk
