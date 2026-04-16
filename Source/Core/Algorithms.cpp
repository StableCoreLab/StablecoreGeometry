#include "Core/Algorithms.h"

#include <algorithm>
#include <cmath>

#include "Support/Geometry2d/Predicate2.h"
#include "Support/Epsilon.h"

namespace Geometry::Sdk
{
namespace
{
[[nodiscard]] Geometry::Point2d Interpolate(
    const Geometry::Point2d& startPoint,
    const Geometry::Point2d& endPoint,
    double parameter)
{
    return Geometry::Point2d(
        startPoint.x + (endPoint.x - startPoint.x) * parameter,
        startPoint.y + (endPoint.y - startPoint.y) * parameter);
}
} // namespace

SegmentProjection2d ProjectPointToSegment(
    const Point2d& point,
    const Point2d& segmentStart,
    const Point2d& segmentEnd,
    bool clampToSegment)
{
    const Geometry::Point2d internalPoint = point;
    const Geometry::Point2d internalStart = segmentStart;
    const Geometry::Point2d internalEnd = segmentEnd;
    const Geometry::Vector2d segmentVector = internalEnd - internalStart;
    const double segmentLengthSquared = segmentVector.LengthSquared();

    if (Geometry::IsZero(segmentLengthSquared))
    {
        return SegmentProjection2d{
            segmentStart,
            0.0,
            DistanceSquared(point, segmentStart),
            true};
    }

    const Geometry::Vector2d startToPoint = internalPoint - internalStart;
    const double rawParameter = Geometry::Dot(startToPoint, segmentVector) / segmentLengthSquared;
    const double parameter = clampToSegment ? std::clamp(rawParameter, 0.0, 1.0) : rawParameter;
    const Geometry::Point2d projectedPoint = Interpolate(internalStart, internalEnd, parameter);

    return SegmentProjection2d{
        projectedPoint,
        parameter,
        DistanceSquared(point, projectedPoint),
        clampToSegment ||
            (rawParameter >= -Geometry::kAlgorithmsDefaultEpsilon &&
             rawParameter <= 1.0 + Geometry::kAlgorithmsDefaultEpsilon)};
}
} // namespace Geometry::Sdk
