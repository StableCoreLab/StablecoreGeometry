#pragma once

#include "Export/GeometryExport.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Core/GeometryTypes.h"
#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/Segment2d.h"

namespace Geometry::Sdk
{
[[nodiscard]] GEOMETRY_API double DistanceSquared(const Point2d& lhs, const Point2d& rhs);
[[nodiscard]] GEOMETRY_API double Distance(const Point2d& lhs, const Point2d& rhs);

[[nodiscard]] GEOMETRY_API double DistanceSquared(const Point2d& point, const LineSegment2d& segment);
[[nodiscard]] GEOMETRY_API double DistanceSquared(const Point2d& point, const ArcSegment2d& segment);
[[nodiscard]] GEOMETRY_API double DistanceSquared(const Point2d& point, const Segment2d& segment);

[[nodiscard]] GEOMETRY_API double Distance(const Point2d& point, const LineSegment2d& segment);
[[nodiscard]] GEOMETRY_API double Distance(const Point2d& point, const ArcSegment2d& segment);
[[nodiscard]] GEOMETRY_API double Distance(const Point2d& point, const Segment2d& segment);

[[nodiscard]] GEOMETRY_API bool Contains(const Box2d& box, const Point2d& point, double eps = 1e-9);
[[nodiscard]] GEOMETRY_API bool Intersects(const Box2d& lhs, const Box2d& rhs, double eps = 1e-9);
} // namespace Geometry::Sdk
