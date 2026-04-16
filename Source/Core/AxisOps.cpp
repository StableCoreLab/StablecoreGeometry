#include "Core/AxisOps.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <numbers>
#include <utility>

#include "Support/Geometry2d/Predicate2.h"
#include "Support/Epsilon.h"
#include "Core/Algorithms.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"

namespace Geometry
{
namespace
{
[[nodiscard]] double Clamp01(double value)
{
    return std::clamp(value, 0.0, 1.0);
}

[[nodiscard]] double NormalizeAngle(double angle)
{
    constexpr double kTwoPi = 2.0 * std::numbers::pi_v<double>;
    angle = std::fmod(angle, kTwoPi);
    if (angle < 0.0)
    {
        angle += kTwoPi;
    }
    return angle;
}

[[nodiscard]] Point2d Interpolate(
    const Point2d& startPoint,
    const Point2d& endPoint,
    double parameter)
{
    return Point2d{
        startPoint.x + (endPoint.x - startPoint.x) * parameter,
        startPoint.y + (endPoint.y - startPoint.y) * parameter};
}

[[nodiscard]] SegmentProjection2d ProjectPointToLineSegmentImpl(
    const Point2d& point,
    const LineSegment2d& segment,
    bool clampToSegment)
{
    if (!segment.IsValid())
    {
        return SegmentProjection2d{
            segment.startPoint,
            0.0,
            DistanceSquared(point, segment.startPoint),
            true};
    }

    const Vector2d direction = segment.endPoint - segment.startPoint;
    const double lengthSquared = direction.LengthSquared();
    if (lengthSquared <= Geometry::kAxisOpsDefaultEpsilon * Geometry::kAxisOpsDefaultEpsilon)
    {
        return SegmentProjection2d{
            segment.startPoint,
            0.0,
            DistanceSquared(point, segment.startPoint),
            true};
    }

    const double rawParameter = Dot(point - segment.startPoint, direction) / lengthSquared;
    const double parameter = clampToSegment ? Clamp01(rawParameter) : rawParameter;
    const Point2d projectedPoint = Interpolate(segment.startPoint, segment.endPoint, parameter);
    return SegmentProjection2d{
        projectedPoint,
        parameter,
        DistanceSquared(point, projectedPoint),
        clampToSegment ||
            (rawParameter >= -Geometry::kAxisOpsDefaultEpsilon &&
             rawParameter <= 1.0 + Geometry::kAxisOpsDefaultEpsilon)};
}

[[nodiscard]] double ParameterOnArc(const ArcSegment2d& segment, double angle)
{
    if (segment.sweepAngle >= 0.0)
    {
        return NormalizeAngle(angle - segment.startAngle) / segment.sweepAngle;
    }

    return NormalizeAngle(segment.startAngle - angle) / (-segment.sweepAngle);
}

[[nodiscard]] Point2d PointAtAngle(const ArcSegment2d& segment, double angle)
{
    return Point2d{
        segment.center.x + segment.radius * std::cos(angle),
        segment.center.y + segment.radius * std::sin(angle)};
}

[[nodiscard]] SegmentProjection2d ProjectPointToArcSegmentImpl(
    const Point2d& point,
    const ArcSegment2d& segment,
    bool clampToSegment)
{
    if (!segment.IsValid())
    {
        return SegmentProjection2d{
            Point2d{},
            0.0,
            -1.0,
            false};
    }

    const Vector2d radial = point - segment.center;
    const double radialLengthSquared = radial.LengthSquared();
    if (radialLengthSquared <= Geometry::kAxisOpsDefaultEpsilon * Geometry::kAxisOpsDefaultEpsilon)
    {
        const Point2d startPoint = segment.StartPoint();
        return SegmentProjection2d{
            startPoint,
            0.0,
            DistanceSquared(point, startPoint),
            clampToSegment};
    }

    const double angle = std::atan2(radial.y, radial.x);
    const double rawParameter = ParameterOnArc(segment, angle);
    const double parameter = clampToSegment ? Clamp01(rawParameter) : rawParameter;
    const Point2d projectedPoint = segment.PointAt(parameter);
    return SegmentProjection2d{
        projectedPoint,
        parameter,
        DistanceSquared(point, projectedPoint),
        clampToSegment ||
            (rawParameter >= -Geometry::kAxisOpsDefaultEpsilon &&
             rawParameter <= 1.0 + Geometry::kAxisOpsDefaultEpsilon)};
}

[[nodiscard]] Vector2d UnitTangentAt(const LineSegment2d& segment)
{
    const Vector2d direction = segment.endPoint - segment.startPoint;
    const double length = direction.Length();
    if (length <= Geometry::kAxisOpsDefaultEpsilon)
    {
        return Vector2d{};
    }

    return direction / length;
}

[[nodiscard]] Vector2d UnitTangentAt(const ArcSegment2d& segment, double parameter)
{
    if (!segment.IsValid())
    {
        return Vector2d{};
    }

    const Point2d point = segment.PointAt(parameter);
    Vector2d radial = point - segment.center;
    const double radialLength = radial.Length();
    if (radialLength <= Geometry::kAxisOpsDefaultEpsilon)
    {
        return Vector2d{};
    }

    radial = radial / radialLength;
    if (segment.sweepAngle >= 0.0)
    {
        return Vector2d{-radial.y, radial.x};
    }

    return Vector2d{radial.y, -radial.x};
}

[[nodiscard]] Vector2d LeftNormal(const Vector2d& tangent)
{
    return Vector2d{-tangent.y, tangent.x};
}
} // namespace

AxisSample2d SampleAxis(const Segment2d& segment, double parameter)
{
    const Point2d point = segment.PointAt(parameter);
    const Vector2d tangent = segment.Kind() == SegmentKind2::Line
        ? UnitTangentAt(static_cast<const LineSegment2d&>(segment))
        : UnitTangentAt(static_cast<const ArcSegment2d&>(segment), parameter);
    const Vector2d normal = LeftNormal(tangent);
    return AxisSample2d{point, tangent, normal, parameter};
}

AxisSample2d SampleAxisAtLength(const Segment2d& segment, double length, bool clampToSegment)
{
    const double totalLength = segment.Length();
    if (totalLength <= Geometry::kAxisOpsDefaultEpsilon)
    {
        return SampleAxis(segment, 0.0);
    }

    if (clampToSegment)
    {
        length = std::clamp(length, 0.0, totalLength);
    }

    return SampleAxis(segment, length / totalLength);
}

AxisProjection2d ProjectPointToAxis(const Point2d& point, const Segment2d& segment)
{
    if (segment.Kind() == SegmentKind2::Line)
    {
        const LineSegment2d& line = static_cast<const LineSegment2d&>(segment);
        const SegmentProjection2d projection = ProjectPointToLineSegmentImpl(point, line, true);
        const Vector2d tangent = UnitTangentAt(line);
        return AxisProjection2d{projection, tangent, LeftNormal(tangent)};
    }

    const ArcSegment2d& arc = static_cast<const ArcSegment2d&>(segment);
    const SegmentProjection2d projection = ProjectPointToArcSegmentImpl(point, arc, true);
    const Vector2d tangent = UnitTangentAt(arc, projection.parameter);
    return AxisProjection2d{projection, tangent, LeftNormal(tangent)};
}

std::unique_ptr<Segment2d> Reverse(const Segment2d& segment)
{
    if (segment.Kind() == SegmentKind2::Line)
    {
        const LineSegment2d& line = static_cast<const LineSegment2d&>(segment);
        return std::make_unique<LineSegment2d>(line.endPoint, line.startPoint);
    }

    const ArcSegment2d& arc = static_cast<const ArcSegment2d&>(segment);
    return std::make_unique<ArcSegment2d>(arc.center, arc.radius, arc.EndAngle(), -arc.sweepAngle);
}

SegmentSplit2d SplitSegment(const Segment2d& segment, double parameter)
{
    SegmentSplit2d result;
    if (!segment.IsValid() || !std::isfinite(parameter) || parameter <= 0.0 || parameter >= 1.0)
    {
        return result;
    }

    if (segment.Kind() == SegmentKind2::Line)
    {
        const LineSegment2d& line = static_cast<const LineSegment2d&>(segment);
        const Point2d splitPoint = line.PointAt(parameter);
        result.first = std::make_unique<LineSegment2d>(line.startPoint, splitPoint);
        result.second = std::make_unique<LineSegment2d>(splitPoint, line.endPoint);
    }
    else
    {
        const ArcSegment2d& arc = static_cast<const ArcSegment2d&>(segment);
        const double firstSweep = arc.sweepAngle * parameter;
        result.first = std::make_unique<ArcSegment2d>(arc.center, arc.radius, arc.startAngle, firstSweep);
        result.second = std::make_unique<ArcSegment2d>(
            arc.center,
            arc.radius,
            arc.startAngle + firstSweep,
            arc.sweepAngle - firstSweep);
    }

    result.success = result.first != nullptr && result.second != nullptr &&
                     result.first->IsValid() && result.second->IsValid();
    return result;
}

SegmentTrim2d TrimSegment(const Segment2d& segment, double startParameter, double endParameter)
{
    SegmentTrim2d result;
    if (!segment.IsValid() || !std::isfinite(startParameter) || !std::isfinite(endParameter))
    {
        return result;
    }

    if (endParameter < startParameter)
    {
        std::swap(startParameter, endParameter);
    }

    startParameter = std::clamp(startParameter, 0.0, 1.0);
    endParameter = std::clamp(endParameter, 0.0, 1.0);
    if (endParameter - startParameter <= Geometry::kAxisOpsDefaultEpsilon)
    {
        return result;
    }

    if (segment.Kind() == SegmentKind2::Line)
    {
        const LineSegment2d& line = static_cast<const LineSegment2d&>(segment);
        result.segment = std::make_unique<LineSegment2d>(
            line.PointAt(startParameter),
            line.PointAt(endParameter));
    }
    else
    {
        const ArcSegment2d& arc = static_cast<const ArcSegment2d&>(segment);
        result.segment = std::make_unique<ArcSegment2d>(
            arc.center,
            arc.radius,
            arc.startAngle + arc.sweepAngle * startParameter,
            arc.sweepAngle * (endParameter - startParameter));
    }

    result.success = result.segment != nullptr && result.segment->IsValid();
    return result;
}

SnapResult2d SnapPointToSegments(
    const Point2d& point,
    std::span<const Segment2d* const> segments,
    double maxDistance)
{
    SnapResult2d result;
    if (!(maxDistance >= 0.0))
    {
        return result;
    }

    double bestDistanceSquared = maxDistance * maxDistance;
    for (std::size_t i = 0; i < segments.size(); ++i)
    {
        const Segment2d* segment = segments[i];
        if (segment == nullptr || !segment->IsValid())
        {
            continue;
        }

        const SegmentProjection2d projection = ProjectPointToAxis(point, *segment).projection;
        if (!projection.IsValid() || projection.distanceSquared > bestDistanceSquared)
        {
            continue;
        }

        result.snapped = true;
        result.point = projection.point;
        result.distanceSquared = projection.distanceSquared;
        result.segmentIndex = i;
        result.parameter = projection.parameter;
        bestDistanceSquared = projection.distanceSquared;
    }

    return result;
}
} // namespace Geometry

