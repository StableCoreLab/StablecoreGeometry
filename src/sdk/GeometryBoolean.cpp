#include "sdk/GeometryBoolean.h"

#include <algorithm>
#include <vector>

#include "algorithm/Predicate2.h"
#include "sdk/GeometryRelation.h"
#include "sdk/GeometryShapeOps.h"
#include "sdk/GeometryTopology.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] bool IsInsideHalfPlane(const Point2d& point, const Point2d& a, const Point2d& b, double eps)
{
    return Cross(b - a, point - a) >= -eps;
}

[[nodiscard]] Point2d IntersectInfiniteLines(const Point2d& a1, const Point2d& a2, const Point2d& b1, const Point2d& b2)
{
    const Vector2d r = a2 - a1;
    const Vector2d s = b2 - b1;
    const double denom = Cross(r, s);
    if (std::abs(denom) <= geometry::kDefaultEpsilon)
    {
        return a2;
    }
    const double t = Cross(b1 - a1, s) / denom;
    return a1 + r * t;
}

[[nodiscard]] std::vector<Point2d> ClipConvexPolygon(
    std::vector<Point2d> subject,
    const std::vector<Point2d>& clip,
    double eps)
{
    if (subject.empty())
    {
        return {};
    }

    for (std::size_t i = 0; i < clip.size(); ++i)
    {
        const Point2d clipA = clip[i];
        const Point2d clipB = clip[(i + 1) % clip.size()];
        std::vector<Point2d> input = std::move(subject);
        subject.clear();
        if (input.empty())
        {
            break;
        }

        Point2d s = input.back();
        for (const Point2d& e : input)
        {
            const bool eInside = IsInsideHalfPlane(e, clipA, clipB, eps);
            const bool sInside = IsInsideHalfPlane(s, clipA, clipB, eps);
            if (eInside)
            {
                if (!sInside)
                {
                    subject.push_back(IntersectInfiniteLines(s, e, clipA, clipB));
                }
                subject.push_back(e);
            }
            else if (sInside)
            {
                subject.push_back(IntersectInfiniteLines(s, e, clipA, clipB));
            }
            s = e;
        }
    }

    return subject;
}

[[nodiscard]] std::vector<Point2d> RingPoints(const Polyline2d& ring)
{
    std::vector<Point2d> points;
    points.reserve(ring.PointCount());
    for (std::size_t i = 0; i < ring.PointCount(); ++i)
    {
        points.push_back(ring.PointAt(i));
    }
    return points;
}

[[nodiscard]] Polygon2d MakePolygon(std::vector<Point2d> points)
{
    if (points.size() < 3)
    {
        return Polygon2d();
    }
    if (Orientation(Polyline2d(points, PolylineClosure::Closed)) == RingOrientation2d::Clockwise)
    {
        std::reverse(points.begin(), points.end());
    }
    return Polygon2d(Polyline2d(std::move(points), PolylineClosure::Closed));
}
} // namespace

MultiPolygon2d Intersect(const Polygon2d& first, const Polygon2d& second, double eps)
{
    if (!first.IsValid() || !second.IsValid() || first.HoleCount() != 0 || second.HoleCount() != 0)
    {
        return {};
    }

    std::vector<Point2d> clipped =
        ClipConvexPolygon(RingPoints(first.OuterRing()), RingPoints(second.OuterRing()), eps);
    Polygon2d polygon = MakePolygon(std::move(clipped));
    if (!polygon.IsValid())
    {
        return {};
    }
    return MultiPolygon2d{{polygon}};
}

MultiPolygon2d Union(const Polygon2d& first, const Polygon2d& second, double eps)
{
    if (!first.IsValid() || !second.IsValid())
    {
        return {};
    }

    if (Contains(first, second, eps))
    {
        return MultiPolygon2d{{first}};
    }
    if (Contains(second, first, eps))
    {
        return MultiPolygon2d{{second}};
    }

    MultiPolygon2d intersection = Intersect(first, second, eps);
    if (intersection.IsEmpty())
    {
        return MultiPolygon2d{{first, second}};
    }

    Box2d bounds = first.Bounds();
    bounds.ExpandToInclude(second.Bounds());
    std::vector<Point2d> points{
        bounds.GetMinPoint(),
        Point2d{bounds.GetMaxPoint().x, bounds.GetMinPoint().y},
        bounds.GetMaxPoint(),
        Point2d{bounds.GetMinPoint().x, bounds.GetMaxPoint().y}};
    return MultiPolygon2d{{Polygon2d(Polyline2d(std::move(points), PolylineClosure::Closed))}};
}

MultiPolygon2d Difference(const Polygon2d& first, const Polygon2d& second, double eps)
{
    if (!first.IsValid() || !second.IsValid())
    {
        return {};
    }
    if (Contains(second, first, eps))
    {
        return {};
    }
    if (Intersect(first, second, eps).IsEmpty())
    {
        return MultiPolygon2d{{first}};
    }
    return MultiPolygon2d{{first}};
}
} // namespace geometry::sdk
