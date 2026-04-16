#include "Core/Editing.h"

#include <vector>

#include "Core/Relation.h"
#include "Core/ShapeOps.h"

namespace Geometry
{
namespace
{
[[nodiscard]] std::vector<Point2d> NormalizedPoints(const Polyline2d& polyline, double eps)
{
    std::vector<Point2d> points;
    if (polyline.PointCount() == 0)
    {
        return points;
    }

    points.reserve(polyline.PointCount());
    for (std::size_t i = 0; i < polyline.PointCount(); ++i)
    {
        const Point2d point = polyline.PointAt(i);
        if (!points.empty() && points.back().AlmostEquals(point, eps))
        {
            continue;
        }
        points.push_back(point);
    }

    if (polyline.IsClosed() && points.size() > 1 && points.front().AlmostEquals(points.back(), eps))
    {
        points.pop_back();
    }
    return points;
}
} // namespace

Polyline2d Normalize(const Polyline2d& polyline, double eps)
{
    return Polyline2d(
        NormalizedPoints(polyline, eps),
        polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open);
}

Polygon2d Normalize(const Polygon2d& polygon, double eps)
{
    std::vector<Polyline2d> holes;
    holes.reserve(polygon.HoleCount());
    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        holes.push_back(Normalize(polygon.HoleAt(i), eps));
    }
    return Polygon2d(Normalize(polygon.OuterRing(), eps), std::move(holes));
}

Polyline2d InsertPoint(const Polyline2d& polyline, const Point2d& point, double eps)
{
    std::vector<Point2d> points;
    points.reserve(polyline.PointCount() + 1);

    bool inserted = false;
    const std::size_t n = polyline.PointCount();
    const std::size_t segmentCount = polyline.IsClosed() ? n : (n > 0 ? n - 1 : 0);
    for (std::size_t i = 0; i < n; ++i)
    {
        points.push_back(polyline.PointAt(i));
        if (!inserted && i < segmentCount)
        {
            const Point2d next = polyline.PointAt((i + 1) % n);
            if (LocatePoint(point, LineSegment2d(polyline.PointAt(i), next), eps) ==
                    PointContainment2d::OnBoundary &&
                !point.AlmostEquals(polyline.PointAt(i), eps) &&
                !point.AlmostEquals(next, eps))
            {
                points.push_back(point);
                inserted = true;
            }
        }
    }

    return Polyline2d(std::move(points), polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open);
}

Polygon2d InsertPoint(const Polygon2d& polygon, const Point2d& point, double eps)
{
    return Polygon2d(InsertPoint(polygon.OuterRing(), point, eps));
}
} // namespace Geometry

