#include "sdk/GeometryMetrics.h"

#include <cmath>

#include "sdk/GeometryProjection.h"

namespace geometry::sdk
{
double DistanceSquared(const Point2d& lhs, const Point2d& rhs)
{
    return (rhs - lhs).LengthSquared();
}

double Distance(const Point2d& lhs, const Point2d& rhs)
{
    return std::sqrt(DistanceSquared(lhs, rhs));
}

double DistanceSquared(const Point2d& point, const LineSegment2d& segment)
{
    return ProjectPointToLineSegment(point, segment, true).distanceSquared;
}

double DistanceSquared(const Point2d& point, const ArcSegment2d& segment)
{
    return ProjectPointToArcSegment(point, segment, true).distanceSquared;
}

double DistanceSquared(const Point2d& point, const Segment2d& segment)
{
    return ProjectPointToSegment(point, segment, true).distanceSquared;
}

double Distance(const Point2d& point, const LineSegment2d& segment)
{
    return std::sqrt(DistanceSquared(point, segment));
}

double Distance(const Point2d& point, const ArcSegment2d& segment)
{
    return std::sqrt(DistanceSquared(point, segment));
}

double Distance(const Point2d& point, const Segment2d& segment)
{
    return std::sqrt(DistanceSquared(point, segment));
}

double Length(const LineSegment2d& segment)
{
    return segment.Length();
}

double Length(const ArcSegment2d& segment)
{
    return segment.Length();
}

double Length(const Segment2d& segment)
{
    return segment.Length();
}

Box2d Bounds(const LineSegment2d& segment)
{
    return segment.Bounds();
}

Box2d Bounds(const ArcSegment2d& segment)
{
    return segment.Bounds();
}

Box2d Bounds(const Segment2d& segment)
{
    return segment.Bounds();
}

bool Contains(const Box2d& box, const Point2d& point, double eps)
{
    if (!box.IsValid())
    {
        return false;
    }

    return point.x >= box.GetMinPoint().x - eps &&
           point.y >= box.GetMinPoint().y - eps &&
           point.x <= box.GetMaxPoint().x + eps &&
           point.y <= box.GetMaxPoint().y + eps;
}

bool Intersects(const Box2d& lhs, const Box2d& rhs, double eps)
{
    if (!lhs.IsValid() || !rhs.IsValid())
    {
        return false;
    }

    return !(lhs.GetMaxPoint().x < rhs.GetMinPoint().x - eps ||
             lhs.GetMaxPoint().y < rhs.GetMinPoint().y - eps ||
             rhs.GetMaxPoint().x < lhs.GetMinPoint().x - eps ||
             rhs.GetMaxPoint().y < lhs.GetMinPoint().y - eps);
}
} // namespace geometry::sdk
