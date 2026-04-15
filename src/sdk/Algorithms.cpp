#include "sdk/Algorithms.h"

#include <algorithm>
#include <cmath>

#include "algorithm/Predicate2.h"
#include "common/Epsilon.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] geometry::Point2d Interpolate(
    const geometry::Point2d& startPoint,
    const geometry::Point2d& endPoint,
    double parameter)
{
    return geometry::Point2d(
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
    const geometry::Point2d internalPoint = point;
    const geometry::Point2d internalStart = segmentStart;
    const geometry::Point2d internalEnd = segmentEnd;
    const geometry::Vector2d segmentVector = internalEnd - internalStart;
    const double segmentLengthSquared = segmentVector.LengthSquared();

    if (geometry::IsZero(segmentLengthSquared))
    {
        return SegmentProjection2d{
            segmentStart,
            0.0,
            DistanceSquared(point, segmentStart),
            true};
    }

    const geometry::Vector2d startToPoint = internalPoint - internalStart;
    const double rawParameter = geometry::Dot(startToPoint, segmentVector) / segmentLengthSquared;
    const double parameter = clampToSegment ? std::clamp(rawParameter, 0.0, 1.0) : rawParameter;
    const geometry::Point2d projectedPoint = Interpolate(internalStart, internalEnd, parameter);

    return SegmentProjection2d{
        projectedPoint,
        parameter,
        DistanceSquared(point, projectedPoint),
        clampToSegment ||
            (rawParameter >= -geometry::kAlgorithmsDefaultEpsilon &&
             rawParameter <= 1.0 + geometry::kAlgorithmsDefaultEpsilon)};
}
} // namespace geometry::sdk

