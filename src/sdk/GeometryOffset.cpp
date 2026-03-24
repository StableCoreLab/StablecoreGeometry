#include "sdk/GeometryOffset.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "common/Epsilon.h"
#include "sdk/GeometryShapeOps.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] Vector2d LeftNormal(const Vector2d& direction)
{
    const double length = direction.Length();
    if (length <= geometry::kDefaultEpsilon)
    {
        return Vector2d{};
    }

    return Vector2d{-direction.y / length, direction.x / length};
}

[[nodiscard]] bool IsZeroVector(const Vector2d& vector)
{
    return vector.LengthSquared() <= geometry::kDefaultEpsilon * geometry::kDefaultEpsilon;
}

[[nodiscard]] Point2d IntersectLines(
    const Point2d& firstPoint,
    const Vector2d& firstDirection,
    const Point2d& secondPoint,
    const Vector2d& secondDirection,
    bool& success)
{
    const double denom = firstDirection.x * secondDirection.y - firstDirection.y * secondDirection.x;
    if (std::abs(denom) <= geometry::kDefaultEpsilon)
    {
        success = false;
        return Point2d{};
    }

    const Vector2d delta = secondPoint - firstPoint;
    const double t = (delta.x * secondDirection.y - delta.y * secondDirection.x) / denom;
    success = std::isfinite(t);
    return Point2d{
        firstPoint.x + firstDirection.x * t,
        firstPoint.y + firstDirection.y * t};
}

[[nodiscard]] std::vector<Point2d> BuildOffsetVertices(
    const std::vector<Point2d>& vertices,
    bool closed,
    double distance)
{
    std::vector<Point2d> result;
    if (vertices.size() < 2)
    {
        return result;
    }

    const std::size_t count = vertices.size();
    const std::size_t segmentCount = closed ? count : count - 1;

    std::vector<Vector2d> directions;
    directions.reserve(segmentCount);
    std::vector<Vector2d> normals;
    normals.reserve(segmentCount);
    for (std::size_t i = 0; i < segmentCount; ++i)
    {
        const Point2d& start = vertices[i];
        const Point2d& end = vertices[(i + 1) % count];
        const Vector2d direction = end - start;
        directions.push_back(direction);
        normals.push_back(LeftNormal(direction));
    }

    result.reserve(count);
    if (!closed)
    {
        result.push_back(vertices.front() + normals.front() * distance);
        for (std::size_t i = 1; i + 1 < count; ++i)
        {
            const std::size_t prev = i - 1;
            const std::size_t next = i;
            const Point2d prevPoint = vertices[i] + normals[prev] * distance;
            const Point2d nextPoint = vertices[i] + normals[next] * distance;
            bool success = false;
            Point2d joined = IntersectLines(
                prevPoint,
                directions[prev],
                nextPoint,
                directions[next],
                success);
            if (!success)
            {
                const Vector2d blended = normals[prev] + normals[next];
                joined = vertices[i] + (IsZeroVector(blended) ? normals[next] : blended / blended.Length()) * distance;
            }
            result.push_back(joined);
        }
        result.push_back(vertices.back() + normals.back() * distance);
        return result;
    }

    for (std::size_t i = 0; i < count; ++i)
    {
        const std::size_t prev = (i + count - 1) % count;
        const std::size_t next = i % segmentCount;
        const Point2d prevPoint = vertices[i] + normals[prev] * distance;
        const Point2d nextPoint = vertices[i] + normals[next] * distance;
        bool success = false;
        Point2d joined = IntersectLines(
            prevPoint,
            directions[prev],
            nextPoint,
            directions[next],
            success);
        if (!success)
        {
            const Vector2d blended = normals[prev] + normals[next];
            joined = vertices[i] + (IsZeroVector(blended) ? normals[next] : blended / blended.Length()) * distance;
        }
        result.push_back(joined);
    }

    return result;
}

[[nodiscard]] Polyline2d BuildPolylineFromVertices(
    std::vector<Point2d> vertices,
    bool closed)
{
    if (vertices.size() < 2)
    {
        return Polyline2d(closed ? PolylineClosure::Closed : PolylineClosure::Open);
    }

    return Polyline2d(std::move(vertices), closed ? PolylineClosure::Closed : PolylineClosure::Open);
}
} // namespace

LineSegment2d Offset(const LineSegment2d& segment, double distance)
{
    if (!segment.IsValid())
    {
        return segment;
    }

    const Vector2d direction = segment.endPoint - segment.startPoint;
    const Vector2d normal = LeftNormal(direction);
    if (IsZeroVector(normal))
    {
        return segment;
    }

    return LineSegment2d(segment.startPoint + normal * distance, segment.endPoint + normal * distance);
}

ArcSegment2d Offset(const ArcSegment2d& segment, double distance)
{
    if (!segment.IsValid())
    {
        return segment;
    }

    const double adjustedRadius =
        segment.Direction() == ArcDirection::CounterClockwise ? segment.radius - distance
                                                              : segment.radius + distance;
    return ArcSegment2d(segment.center, adjustedRadius, segment.startAngle, segment.sweepAngle);
}

Polyline2d Offset(const Polyline2d& polyline, double distance)
{
    if (!polyline.IsValid() || polyline.PointCount() < 2)
    {
        return Polyline2d(polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open);
    }

    std::vector<Point2d> vertices;
    vertices.reserve(polyline.PointCount());
    for (std::size_t i = 0; i < polyline.PointCount(); ++i)
    {
        vertices.push_back(polyline.PointAt(i));
    }

    return BuildPolylineFromVertices(
        BuildOffsetVertices(vertices, polyline.IsClosed(), distance),
        polyline.IsClosed());
}

Polygon2d Offset(const Polygon2d& polygon, double distance)
{
    if (!polygon.IsValid())
    {
        return Polygon2d();
    }

    const Polyline2d outerRing = polygon.OuterRing();
    const double outerDistance =
        Orientation(outerRing) == RingOrientation2d::CounterClockwise ? -distance : distance;
    Polyline2d offsetOuter = Offset(outerRing, outerDistance);

    std::vector<Polyline2d> offsetHoles;
    offsetHoles.reserve(polygon.HoleCount());
    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        const Polyline2d hole = polygon.HoleAt(i);
        const double holeDistance = -distance;
        offsetHoles.push_back(Offset(hole, holeDistance));
    }

    return Polygon2d(std::move(offsetOuter), std::move(offsetHoles));
}
} // namespace geometry::sdk
