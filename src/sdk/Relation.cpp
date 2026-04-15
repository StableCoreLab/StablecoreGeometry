#include "sdk/Relation.h"

#include <cmath>
#include <vector>

#include "algorithm/Predicate2.h"
#include "common/Epsilon.h"
#include "sdk/Intersection.h"
#include "sdk/Projection.h"
#include "sdk/ShapeOps.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] double Cross2(const Point2d& a, const Point2d& b, const Point2d& c)
{
    return Cross(b - a, c - a);
}

[[nodiscard]] bool IsPointOnLineSegment(const Point2d& point, const LineSegment2d& segment, double eps)
{
    const auto projection = ProjectPointToLineSegment(point, segment, true);
    return projection.distanceSquared <= eps * eps;
}

[[nodiscard]] bool IsPointOnArcSegment(const Point2d& point, const ArcSegment2d& segment, double eps)
{
    const auto projection = ProjectPointToArcSegment(point, segment, true);
    return projection.distanceSquared <= eps * eps;
}

[[nodiscard]] PointContainment2d LocatePointInRing(const Point2d& point, const Polyline2d& ring, double eps)
{
    if (!ring.IsClosed() || ring.PointCount() < 3)
    {
        return PointContainment2d::Outside;
    }

    bool inside = false;
    for (std::size_t i = 0; i < ring.PointCount(); ++i)
    {
        const Point2d a = ring.PointAt(i);
        const Point2d b = ring.PointAt((i + 1) % ring.PointCount());
        if (LocatePoint(point, LineSegment2d(a, b), eps) == PointContainment2d::OnBoundary)
        {
            return PointContainment2d::OnBoundary;
        }

        const bool intersects = ((a.y > point.y) != (b.y > point.y)) &&
                                (point.x < (b.x - a.x) * (point.y - a.y) / (b.y - a.y) + a.x);
        if (intersects)
        {
            inside = !inside;
        }
    }

    return inside ? PointContainment2d::Inside : PointContainment2d::Outside;
}
} // namespace

PointContainment2d LocatePoint(const Point2d& point, const LineSegment2d& segment, double eps)
{
    if (!segment.IsValid())
    {
        return PointContainment2d::Outside;
    }

    if (IsPointOnLineSegment(point, segment, eps))
    {
        return PointContainment2d::OnBoundary;
    }

    return PointContainment2d::Outside;
}

PointContainment2d LocatePoint(const Point2d& point, const ArcSegment2d& segment, double eps)
{
    if (!segment.IsValid())
    {
        return PointContainment2d::Outside;
    }

    if (IsPointOnArcSegment(point, segment, eps))
    {
        return PointContainment2d::OnBoundary;
    }

    return PointContainment2d::Outside;
}

PointContainment2d LocatePoint(const Point2d& point, const Segment2d& segment, double eps)
{
    if (segment.Kind() == geometry::SegmentKind2::Line)
    {
        return LocatePoint(point, static_cast<const LineSegment2d&>(segment), eps);
    }
    if (segment.Kind() == geometry::SegmentKind2::Arc)
    {
        return LocatePoint(point, static_cast<const ArcSegment2d&>(segment), eps);
    }
    return PointContainment2d::Outside;
}

PointContainment2d LocatePoint(const Point2d& point, const Polyline2d& polyline, double eps)
{
    if (!polyline.IsValid())
    {
        return PointContainment2d::Outside;
    }

    if (!polyline.IsClosed())
    {
        for (std::size_t i = 0; i + 1 < polyline.PointCount(); ++i)
        {
            if (LocatePoint(point, LineSegment2d(polyline.PointAt(i), polyline.PointAt(i + 1)), eps) ==
                PointContainment2d::OnBoundary)
            {
                return PointContainment2d::OnBoundary;
            }
        }
        return PointContainment2d::Outside;
    }

    return LocatePointInRing(point, polyline, eps);
}

PointContainment2d LocatePoint(const Point2d& point, const Polygon2d& polygon, double eps)
{
    if (!polygon.IsValid())
    {
        return PointContainment2d::Outside;
    }

    const PointContainment2d outer = LocatePointInRing(point, polygon.OuterRing(), eps);
    if (outer == PointContainment2d::Outside)
    {
        return PointContainment2d::Outside;
    }
    if (outer == PointContainment2d::OnBoundary)
    {
        return PointContainment2d::OnBoundary;
    }

    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        const PointContainment2d hole = LocatePointInRing(point, polygon.HoleAt(i), eps);
        if (hole == PointContainment2d::OnBoundary)
        {
            return PointContainment2d::OnBoundary;
        }
        if (hole == PointContainment2d::Inside)
        {
            return PointContainment2d::Outside;
        }
    }

    return PointContainment2d::Inside;
}

bool IsParallel(const LineSegment2d& first, const LineSegment2d& second, double eps)
{
    if (!first.IsValid() || !second.IsValid())
    {
        return false;
    }

    return std::abs(Cross(first.endPoint - first.startPoint, second.endPoint - second.startPoint)) <= eps;
}

bool IsAntiParallel(const LineSegment2d& first, const LineSegment2d& second, double eps)
{
    if (!IsParallel(first, second, eps))
    {
        return false;
    }

    return Dot(first.endPoint - first.startPoint, second.endPoint - second.startPoint) < 0.0;
}

bool IsSameDirection(const LineSegment2d& first, const LineSegment2d& second, double eps)
{
    if (!IsParallel(first, second, eps))
    {
        return false;
    }

    return Dot(first.endPoint - first.startPoint, second.endPoint - second.startPoint) > 0.0;
}

bool IsEqual(const LineSegment2d& first, const LineSegment2d& second, bool ignoreDirection, double eps)
{
    if (first.startPoint.AlmostEquals(second.startPoint, eps) &&
        first.endPoint.AlmostEquals(second.endPoint, eps))
    {
        return true;
    }

    return ignoreDirection &&
           first.startPoint.AlmostEquals(second.endPoint, eps) &&
           first.endPoint.AlmostEquals(second.startPoint, eps);
}
} // namespace geometry::sdk
