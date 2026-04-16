#include "Core/Transform.h"

#include <cmath>
#include <vector>

#include "Support/Geometry2d/Predicate2.h"
#include "Support/Epsilon.h"
#include "Core/Metrics.h"
#include "Core/ShapeOps.h"

namespace Geometry::Sdk
{
namespace
{
[[nodiscard]] Point2d RotatePoint(const Point2d& point, const Point2d& origin, double angle)
{
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    const Vector2d delta = point - origin;
    return Point2d{
        origin.x + delta.x * c - delta.y * s,
        origin.y + delta.x * s + delta.y * c};
}

[[nodiscard]] Point2d MirrorPoint(const Point2d& point, const Point2d& linePoint, const Vector2d& lineDir)
{
    const double lengthSquared = lineDir.LengthSquared();
    if (lengthSquared <= Geometry::kTransformDefaultEpsilon)
    {
        return point;
    }
    const Vector2d local = point - linePoint;
    const Vector2d unit = lineDir / std::sqrt(lengthSquared);
    const double along = Dot(local, unit);
    const Point2d projected = linePoint + unit * along;
    return projected + (projected - point);
}

[[nodiscard]] std::vector<Point2d> PolylinePoints(const Polyline2d& polyline)
{
    std::vector<Point2d> points;
    points.reserve(polyline.PointCount());
    for (std::size_t i = 0; i < polyline.PointCount(); ++i)
    {
        points.push_back(polyline.PointAt(i));
    }
    return points;
}
} // namespace

Point2d Translate(const Point2d& point, const Vector2d& offset) { return point + offset; }

LineSegment2d Translate(const LineSegment2d& segment, const Vector2d& offset)
{
    return LineSegment2d(segment.startPoint + offset, segment.endPoint + offset);
}

ArcSegment2d Translate(const ArcSegment2d& segment, const Vector2d& offset)
{
    return ArcSegment2d(segment.center + offset, segment.radius, segment.startAngle, segment.sweepAngle);
}

Polyline2d Translate(const Polyline2d& polyline, const Vector2d& offset)
{
    std::vector<Point2d> points = PolylinePoints(polyline);
    for (Point2d& point : points)
    {
        point = point + offset;
    }
    return Polyline2d(std::move(points), polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open);
}

Polygon2d Translate(const Polygon2d& polygon, const Vector2d& offset)
{
    std::vector<Polyline2d> holes;
    holes.reserve(polygon.HoleCount());
    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        holes.push_back(Translate(polygon.HoleAt(i), offset));
    }
    return Polygon2d(Translate(polygon.OuterRing(), offset), std::move(holes));
}

MultiPolyline2d Translate(const MultiPolyline2d& polylines, const Vector2d& offset)
{
    std::vector<Polyline2d> result;
    result.reserve(polylines.Count());
    for (std::size_t i = 0; i < polylines.Count(); ++i)
    {
        result.push_back(Translate(polylines[i], offset));
    }
    return MultiPolyline2d(std::move(result));
}

MultiPolygon2d Translate(const MultiPolygon2d& polygons, const Vector2d& offset)
{
    std::vector<Polygon2d> result;
    result.reserve(polygons.Count());
    for (std::size_t i = 0; i < polygons.Count(); ++i)
    {
        result.push_back(Translate(polygons[i], offset));
    }
    return MultiPolygon2d(std::move(result));
}

Point2d Rotate(const Point2d& point, const Point2d& origin, double angleRadians)
{
    return RotatePoint(point, origin, angleRadians);
}

LineSegment2d Rotate(const LineSegment2d& segment, const Point2d& origin, double angleRadians)
{
    return LineSegment2d(
        RotatePoint(segment.startPoint, origin, angleRadians),
        RotatePoint(segment.endPoint, origin, angleRadians));
}

ArcSegment2d Rotate(const ArcSegment2d& segment, const Point2d& origin, double angleRadians)
{
    return ArcSegment2d(
        RotatePoint(segment.center, origin, angleRadians),
        segment.radius,
        segment.startAngle + angleRadians,
        segment.sweepAngle);
}

Polyline2d Rotate(const Polyline2d& polyline, const Point2d& origin, double angleRadians)
{
    std::vector<Point2d> points = PolylinePoints(polyline);
    for (Point2d& point : points)
    {
        point = RotatePoint(point, origin, angleRadians);
    }
    return Polyline2d(std::move(points), polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open);
}

Polygon2d Rotate(const Polygon2d& polygon, const Point2d& origin, double angleRadians)
{
    std::vector<Polyline2d> holes;
    holes.reserve(polygon.HoleCount());
    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        holes.push_back(Rotate(polygon.HoleAt(i), origin, angleRadians));
    }
    return Polygon2d(Rotate(polygon.OuterRing(), origin, angleRadians), std::move(holes));
}

Point2d Mirror(const Point2d& point, const Point2d& linePoint, const Vector2d& lineDir)
{
    return MirrorPoint(point, linePoint, lineDir);
}

LineSegment2d Mirror(const LineSegment2d& segment, const Point2d& linePoint, const Vector2d& lineDir)
{
    return LineSegment2d(
        MirrorPoint(segment.startPoint, linePoint, lineDir),
        MirrorPoint(segment.endPoint, linePoint, lineDir));
}

ArcSegment2d Mirror(const ArcSegment2d& segment, const Point2d& linePoint, const Vector2d& lineDir)
{
    const Point2d newCenter = MirrorPoint(segment.center, linePoint, lineDir);
    return ArcSegment2d(
        newCenter,
        segment.radius,
        2.0 * std::atan2(lineDir.y, lineDir.x) - segment.startAngle,
        -segment.sweepAngle);
}

Polyline2d Mirror(const Polyline2d& polyline, const Point2d& linePoint, const Vector2d& lineDir)
{
    std::vector<Point2d> points = PolylinePoints(polyline);
    for (Point2d& point : points)
    {
        point = MirrorPoint(point, linePoint, lineDir);
    }
    return Polyline2d(std::move(points), polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open);
}

Polygon2d Mirror(const Polygon2d& polygon, const Point2d& linePoint, const Vector2d& lineDir)
{
    std::vector<Polyline2d> holes;
    holes.reserve(polygon.HoleCount());
    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        holes.push_back(Reverse(Mirror(polygon.HoleAt(i), linePoint, lineDir)));
    }
    return Polygon2d(Reverse(Mirror(polygon.OuterRing(), linePoint, lineDir)), std::move(holes));
}

Point2d Stretch(const Point2d& point, const Box2d& region, const Vector2d& offset)
{
    return Contains(region, point) ? point + offset : point;
}

LineSegment2d Stretch(const LineSegment2d& segment, const Box2d& region, const Vector2d& offset)
{
    return LineSegment2d(Stretch(segment.startPoint, region, offset), Stretch(segment.endPoint, region, offset));
}

ArcSegment2d Stretch(const ArcSegment2d& segment, const Box2d& region, const Vector2d& offset)
{
    const Point2d start = Stretch(segment.StartPoint(), region, offset);
    const Point2d end = Stretch(segment.EndPoint(), region, offset);
    if (start.AlmostEquals(segment.StartPoint()) && end.AlmostEquals(segment.EndPoint()))
    {
        return segment;
    }
    return ArcSegment2d(segment.center, segment.radius, std::atan2(start.y - segment.center.y, start.x - segment.center.x), segment.sweepAngle);
}

Polyline2d Stretch(const Polyline2d& polyline, const Box2d& region, const Vector2d& offset)
{
    std::vector<Point2d> points = PolylinePoints(polyline);
    for (Point2d& point : points)
    {
        point = Stretch(point, region, offset);
    }
    return Polyline2d(std::move(points), polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open);
}

Polygon2d Stretch(const Polygon2d& polygon, const Box2d& region, const Vector2d& offset)
{
    std::vector<Polyline2d> holes;
    holes.reserve(polygon.HoleCount());
    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        holes.push_back(Stretch(polygon.HoleAt(i), region, offset));
    }
    return Polygon2d(Stretch(polygon.OuterRing(), region, offset), std::move(holes));
}
} // namespace Geometry::Sdk
