#include "sdk/GeometryMetrics.h"

#include <cmath>

#include "sdk/GeometryProjection.h"

namespace geometry::sdk
{
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
} // namespace geometry::sdk
