#include "Core/Intersection.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>
#include <limits>
#include <numbers>
#include <utility>

#include "Support/Geometry2d/Predicate2.h"
#include "Support/Epsilon.h"

namespace Geometry::Sdk
{
namespace
{
constexpr double kPi = std::numbers::pi_v<double>;
constexpr double kTwoPi = 2.0 * kPi;

struct ArcProjectionCandidate
{
    Point2d point{};
    double parameter{0.0};
};

[[nodiscard]] SegmentIntersection2d IntersectLineLineInternal(
    const LineSegment2d& first,
    const LineSegment2d& second,
    double eps);

[[nodiscard]] SegmentIntersection2d IntersectLineArcInternal(
    const LineSegment2d& line,
    const ArcSegment2d& arc,
    double eps);

[[nodiscard]] SegmentIntersection2d IntersectArcArcInternal(
    const ArcSegment2d& first,
    const ArcSegment2d& second,
    double eps);

[[nodiscard]] ArcProjectionCandidate ProjectPointToArcSegmentLocal(
    const Point2d& point,
    const ArcSegment2d& arc,
    double eps);

[[nodiscard]] double Clamp(double value, double low, double high)
{
    return std::max(low, std::min(value, high));
}

[[nodiscard]] double NormalizeAngle(double angle)
{
    angle = std::fmod(angle, kTwoPi);
    if (angle < 0.0)
    {
        angle += kTwoPi;
    }
    return angle;
}

[[nodiscard]] bool AlmostEqual(double lhs, double rhs, double eps)
{
    return std::abs(lhs - rhs) <= eps;
}

[[nodiscard]] bool AlmostEqualPoint(const Point2d& lhs, const Point2d& rhs, double eps)
{
    return lhs.AlmostEquals(rhs, eps);
}

[[nodiscard]] double DistanceSquaredPoints(const Point2d& lhs, const Point2d& rhs)
{
    return (rhs - lhs).LengthSquared();
}

[[nodiscard]] Point2d MakePoint(double x, double y)
{
    return Point2d{x, y};
}

[[nodiscard]] Point2d PointAtAngle(const ArcSegment2d& arc, double angle)
{
    return MakePoint(
        arc.center.x + arc.radius * std::cos(angle),
        arc.center.y + arc.radius * std::sin(angle));
}

[[nodiscard]] bool IsAngleOnArc(const ArcSegment2d& arc, double candidateAngle, double eps)
{
    if (arc.sweepAngle >= 0.0)
    {
        const double delta = NormalizeAngle(candidateAngle - arc.startAngle);
        return delta <= arc.sweepAngle + eps;
    }

    const double delta = NormalizeAngle(arc.startAngle - candidateAngle);
    return delta <= (-arc.sweepAngle) + eps;
}

[[nodiscard]] double ArcParameterAtAngle(const ArcSegment2d& arc, double angle)
{
    if (AlmostEqual(arc.sweepAngle, 0.0, Geometry::kIntersectionDefaultEpsilon))
    {
        return 0.0;
    }

    if (arc.sweepAngle > 0.0)
    {
        return NormalizeAngle(angle - arc.startAngle) / arc.sweepAngle;
    }

    return -NormalizeAngle(arc.startAngle - angle) / arc.sweepAngle;
}

[[nodiscard]] SegmentProjection2d ProjectPointToLineSegmentLocal(
    const Point2d& point,
    const LineSegment2d& segment,
    bool clampToSegment = true)
{
    const Vector2d segmentVector = segment.endPoint - segment.startPoint;
    const double segmentLengthSquared = segmentVector.LengthSquared();
    if (segmentLengthSquared <= Geometry::kIntersectionDefaultEpsilon * Geometry::kIntersectionDefaultEpsilon)
    {
        return SegmentProjection2d{
            segment.startPoint,
            0.0,
            DistanceSquaredPoints(point, segment.startPoint),
            true};
    }

    const Vector2d startToPoint = point - segment.startPoint;
    const double rawParameter = Dot(startToPoint, segmentVector) / segmentLengthSquared;
    const double parameter = clampToSegment ? Clamp(rawParameter, 0.0, 1.0) : rawParameter;
    const Point2d projectedPoint = MakePoint(
        segment.startPoint.x + (segment.endPoint.x - segment.startPoint.x) * parameter,
        segment.startPoint.y + (segment.endPoint.y - segment.startPoint.y) * parameter);

    return SegmentProjection2d{
        projectedPoint,
        parameter,
        DistanceSquaredPoints(point, projectedPoint),
        clampToSegment ||
            (rawParameter >= -Geometry::kIntersectionDefaultEpsilon &&
             rawParameter <= 1.0 + Geometry::kIntersectionDefaultEpsilon)};
}

[[nodiscard]] ArcProjectionCandidate ProjectPointToArcSegmentLocal(
    const Point2d& point,
    const ArcSegment2d& arc,
    double eps)
{
    if (!arc.IsValid())
    {
        return ArcProjectionCandidate{point, 0.0};
    }

    const Vector2d fromCenter = point - arc.center;
    if (fromCenter.LengthSquared() <= eps * eps)
    {
        return ArcProjectionCandidate{arc.StartPoint(), 0.0};
    }

    const double angle = std::atan2(fromCenter.y, fromCenter.x);
    if (IsAngleOnArc(arc, angle, eps))
    {
        return ArcProjectionCandidate{PointAtAngle(arc, angle), ArcParameterAtAngle(arc, angle)};
    }

    const Point2d startPoint = arc.StartPoint();
    const Point2d endPoint = arc.EndPoint();
    const double startDistanceSquared = DistanceSquaredPoints(point, startPoint);
    const double endDistanceSquared = DistanceSquaredPoints(point, endPoint);

    if (startDistanceSquared <= endDistanceSquared)
    {
        return ArcProjectionCandidate{startPoint, 0.0};
    }

    return ArcProjectionCandidate{endPoint, 1.0};
}

[[nodiscard]] ClosestPoints2d MakeClosestPoints(
    const Point2d& firstPoint,
    const Point2d& secondPoint,
    double firstParameter,
    double secondParameter)
{
    return ClosestPoints2d{
        firstPoint,
        secondPoint,
        firstParameter,
        secondParameter,
        DistanceSquaredPoints(firstPoint, secondPoint)};
}

[[nodiscard]] ClosestPoints2d MakeClosestPoints(
    const Point2d& firstPoint,
    const Point2d& secondPoint,
    double firstParameter,
    double secondParameter,
    double distanceSquared)
{
    return ClosestPoints2d{
        firstPoint,
        secondPoint,
        firstParameter,
        secondParameter,
        distanceSquared};
}

void UpdateClosest(
    ClosestPoints2d& best,
    bool& hasBest,
    const Point2d& firstPoint,
    const Point2d& secondPoint,
    double firstParameter,
    double secondParameter)
{
    const double distanceSquared = DistanceSquaredPoints(firstPoint, secondPoint);
    if (!hasBest || distanceSquared + Geometry::kIntersectionDefaultEpsilon < best.distanceSquared)
    {
        best = MakeClosestPoints(firstPoint, secondPoint, firstParameter, secondParameter, distanceSquared);
        hasBest = true;
    }
}

[[nodiscard]] SegmentIntersection2d MakeNoIntersection()
{
    return SegmentIntersection2d{};
}

void AddIntersectionPoint(
    SegmentIntersection2d& result,
    IntersectionKind2d kind,
    const Point2d& point,
    double parameterOnFirst,
    double parameterOnSecond,
    double eps)
{
    const IntersectionPoint2d candidate{point, parameterOnFirst, parameterOnSecond};

    if (result.pointCount == 0)
    {
        result.points[0] = candidate;
        result.pointCount = 1;
        result.kind = kind;
        return;
    }

    if (AlmostEqualPoint(result.points[0].point, candidate.point, eps))
    {
        result.points[0] = candidate;
        return;
    }

    if (result.pointCount == 1)
    {
        result.points[1] = candidate;
        result.pointCount = 2;
        return;
    }

    if (!AlmostEqualPoint(result.points[1].point, candidate.point, eps))
    {
        result.points[1] = candidate;
    }
}

[[nodiscard]] SegmentIntersection2d MakePointIntersection(
    const Point2d& point,
    double firstParameter,
    double secondParameter,
    IntersectionKind2d kind = IntersectionKind2d::Point)
{
    SegmentIntersection2d result;
    result.kind = kind;
    result.pointCount = 1;
    result.points[0] = IntersectionPoint2d{point, firstParameter, secondParameter};
    return result;
}

[[nodiscard]] SegmentIntersection2d MakeOverlapIntersection(
    const IntersectionPoint2d& firstPoint,
    const IntersectionPoint2d& secondPoint)
{
    SegmentIntersection2d result;
    result.kind = IntersectionKind2d::Overlap;
    result.pointCount = 2;
    result.points[0] = firstPoint;
    result.points[1] = secondPoint;
    return result;
}

[[nodiscard]] SegmentIntersection2d IntersectLineLineInternal(
    const LineSegment2d& first,
    const LineSegment2d& second,
    double eps)
{
    const Point2d p = first.startPoint;
    const Point2d q = second.startPoint;
    const Vector2d r = first.endPoint - first.startPoint;
    const Vector2d s = second.endPoint - second.startPoint;
    const Vector2d qp = q - p;

    const double rxs = Cross(r, s);
    const double qpxr = Cross(qp, r);

    if (AlmostEqual(rxs, 0.0, eps))
    {
        if (!AlmostEqual(qpxr, 0.0, eps))
        {
            return MakeNoIntersection();
        }

        const double rr = Dot(r, r);
        const double ss = Dot(s, s);
        if (rr <= eps || ss <= eps)
        {
            return MakeNoIntersection();
        }

        double t0 = Dot(q - p, r) / rr;
        double t1 = Dot(second.endPoint - p, r) / rr;
        if (t0 > t1)
        {
            std::swap(t0, t1);
        }

        const double overlapStart = std::max(0.0, t0);
        const double overlapEnd = std::min(1.0, t1);
        if (overlapEnd + eps < overlapStart)
        {
            return MakeNoIntersection();
        }

        const Point2d firstPoint = first.PointAt(overlapStart);
        const Point2d secondPoint = first.PointAt(overlapEnd);
        const double firstParameterOnSecond =
            Dot(firstPoint - second.startPoint, s) / ss;
        const double secondParameterOnSecond =
            Dot(secondPoint - second.startPoint, s) / ss;

        if (AlmostEqual(overlapStart, overlapEnd, eps))
        {
        return MakePointIntersection(
            firstPoint,
            overlapStart,
                firstParameterOnSecond,
                IntersectionKind2d::Point);
        }

        return MakeOverlapIntersection(
            IntersectionPoint2d{firstPoint, overlapStart, firstParameterOnSecond},
            IntersectionPoint2d{secondPoint, overlapEnd, secondParameterOnSecond});
    }

    const double t = Cross(qp, s) / rxs;
    const double u = Cross(qp, r) / rxs;
    if (t < -eps || t > 1.0 + eps || u < -eps || u > 1.0 + eps)
    {
        return MakeNoIntersection();
    }

    const double clampedT = Clamp(t, 0.0, 1.0);
    const double clampedU = Clamp(u, 0.0, 1.0);
    const Point2d intersectionPoint = first.PointAt(clampedT);
    return MakePointIntersection(intersectionPoint, clampedT, clampedU);
}

[[nodiscard]] SegmentIntersection2d IntersectLineArcInternal(
    const LineSegment2d& line,
    const ArcSegment2d& arc,
    double eps)
{
    if (!line.IsValid() || !arc.IsValid())
    {
        return MakeNoIntersection();
    }

    const Vector2d direction = line.endPoint - line.startPoint;
    const Vector2d fromCenter = line.startPoint - arc.center;
    const double a = Dot(direction, direction);
    if (a <= eps)
    {
        return MakeNoIntersection();
    }

    const double b = 2.0 * Dot(fromCenter, direction);
    const double c = Dot(fromCenter, fromCenter) - arc.radius * arc.radius;
    const double discriminant = b * b - 4.0 * a * c;
    if (discriminant < -eps)
    {
        return MakeNoIntersection();
    }

    const double sqrtDiscriminant = discriminant <= 0.0 ? 0.0 : std::sqrt(discriminant);
    const double invDenominator = 0.5 / a;
    const double roots[2] = {
        (-b - sqrtDiscriminant) * invDenominator,
        (-b + sqrtDiscriminant) * invDenominator};

    SegmentIntersection2d result;
    bool hasPoint = false;
    for (double root : roots)
    {
        if (root < -eps || root > 1.0 + eps)
        {
            continue;
        }

        const double parameter = Clamp(root, 0.0, 1.0);
        const Point2d point = line.PointAt(parameter);
        const double angle = std::atan2(point.y - arc.center.y, point.x - arc.center.x);
        if (!IsAngleOnArc(arc, angle, eps))
        {
            continue;
        }

        const double arcParameter = ArcParameterAtAngle(arc, angle);
        if (!hasPoint)
        {
            result = MakePointIntersection(
                point,
                parameter,
                arcParameter,
                AlmostEqual(discriminant, 0.0, eps) ? IntersectionKind2d::Tangent : IntersectionKind2d::Point);
            hasPoint = true;
            continue;
        }

        AddIntersectionPoint(result, IntersectionKind2d::Point, point, parameter, arcParameter, eps);
    }

    return result;
}

[[nodiscard]] bool SameCircle(const ArcSegment2d& first, const ArcSegment2d& second, double eps)
{
    return first.center.AlmostEquals(second.center, eps) &&
           std::abs(first.radius - second.radius) <= eps;
}

[[nodiscard]] SegmentIntersection2d IntersectArcArcSameCircle(
    const ArcSegment2d& first,
    const ArcSegment2d& second,
    double eps)
{
    const bool firstFullCircle = AlmostEqual(std::abs(first.sweepAngle), kTwoPi, eps);
    const bool secondFullCircle = AlmostEqual(std::abs(second.sweepAngle), kTwoPi, eps);
    if (firstFullCircle && secondFullCircle)
    {
        return MakeOverlapIntersection(
            IntersectionPoint2d{first.StartPoint(), 0.0, 0.0},
            IntersectionPoint2d{first.PointAt(0.5), 0.5, 0.5});
    }

    const Point2d firstStart = first.StartPoint();
    const Point2d firstEnd = first.EndPoint();
    const Point2d secondStart = second.StartPoint();
    const Point2d secondEnd = second.EndPoint();

    const bool firstStartOnSecond = IsAngleOnArc(second, first.startAngle, eps);
    const bool firstEndOnSecond = IsAngleOnArc(second, first.EndAngle(), eps);
    const bool secondStartOnFirst = IsAngleOnArc(first, second.startAngle, eps);
    const bool secondEndOnFirst = IsAngleOnArc(first, second.EndAngle(), eps);

    std::array<IntersectionPoint2d, 4> candidates{};
    std::size_t candidateCount = 0;

    auto addCandidate = [&](const Point2d& point, double parameterOnFirst, double parameterOnSecond)
    {
        for (std::size_t index = 0; index < candidateCount; ++index)
        {
            if (AlmostEqualPoint(candidates[index].point, point, eps))
            {
                candidates[index].parameterOnFirst = parameterOnFirst;
                candidates[index].parameterOnSecond = parameterOnSecond;
                return;
            }
        }

        if (candidateCount < candidates.size())
        {
            candidates[candidateCount++] = IntersectionPoint2d{
                point,
                parameterOnFirst,
                parameterOnSecond};
        }
    };

    if (firstStartOnSecond)
    {
        addCandidate(firstStart, 0.0, ArcParameterAtAngle(second, first.startAngle));
    }

    if (firstEndOnSecond)
    {
        addCandidate(firstEnd, 1.0, ArcParameterAtAngle(second, first.EndAngle()));
    }

    if (secondStartOnFirst)
    {
        addCandidate(secondStart, ArcParameterAtAngle(first, second.startAngle), 0.0);
    }

    if (secondEndOnFirst)
    {
        addCandidate(secondEnd, ArcParameterAtAngle(first, second.EndAngle()), 1.0);
    }

    if (candidateCount == 0)
    {
        return MakeNoIntersection();
    }

    if (candidateCount == 1)
    {
        return MakePointIntersection(
            candidates[0].point,
            candidates[0].parameterOnFirst,
            candidates[0].parameterOnSecond,
            IntersectionKind2d::Point);
    }

    if (candidateCount == 2)
    {
        if (AlmostEqualPoint(candidates[0].point, candidates[1].point, eps))
        {
            return MakePointIntersection(
                candidates[0].point,
                candidates[0].parameterOnFirst,
                candidates[0].parameterOnSecond,
                IntersectionKind2d::Point);
        }

        if (candidates[0].parameterOnFirst > candidates[1].parameterOnFirst)
        {
            std::swap(candidates[0], candidates[1]);
        }

        return MakeOverlapIntersection(candidates[0], candidates[1]);
    }

    std::sort(
        candidates.begin(),
        candidates.begin() + static_cast<std::ptrdiff_t>(candidateCount),
        [](const IntersectionPoint2d& lhs, const IntersectionPoint2d& rhs)
        {
            return lhs.parameterOnFirst < rhs.parameterOnFirst;
        });

    return MakeOverlapIntersection(candidates[0], candidates[1]);
}

[[nodiscard]] ClosestPoints2d ClosestPointsLineLineInternal(
    const LineSegment2d& first,
    const LineSegment2d& second)
{
    const SegmentIntersection2d intersection = IntersectLineLineInternal(first, second, Geometry::kIntersectionDefaultEpsilon);
    if (intersection.HasIntersection())
    {
        const IntersectionPoint2d& point = intersection.points[0];
        return ClosestPoints2d{
            point.point,
            point.point,
            point.parameterOnFirst,
            point.parameterOnSecond,
            0.0};
    }

    ClosestPoints2d best{};
    bool hasBest = false;

    const SegmentProjection2d firstStartProjection = ProjectPointToLineSegmentLocal(first.startPoint, second, true);
    UpdateClosest(best, hasBest, first.startPoint, firstStartProjection.point, 0.0, firstStartProjection.parameter);

    const SegmentProjection2d firstEndProjection = ProjectPointToLineSegmentLocal(first.endPoint, second, true);
    UpdateClosest(best, hasBest, first.endPoint, firstEndProjection.point, 1.0, firstEndProjection.parameter);

    const SegmentProjection2d secondStartProjection = ProjectPointToLineSegmentLocal(second.startPoint, first, true);
    UpdateClosest(best, hasBest, secondStartProjection.point, second.startPoint, secondStartProjection.parameter, 0.0);

    const SegmentProjection2d secondEndProjection = ProjectPointToLineSegmentLocal(second.endPoint, first, true);
    UpdateClosest(best, hasBest, secondEndProjection.point, second.endPoint, secondEndProjection.parameter, 1.0);

    return best;
}

[[nodiscard]] ClosestPoints2d ClosestPointsLineArcInternal(
    const LineSegment2d& line,
    const ArcSegment2d& arc)
{
    const SegmentIntersection2d intersection = IntersectLineArcInternal(line, arc, Geometry::kIntersectionDefaultEpsilon);
    if (intersection.HasIntersection())
    {
        const IntersectionPoint2d& point = intersection.points[0];
        return ClosestPoints2d{
            point.point,
            point.point,
            point.parameterOnFirst,
            point.parameterOnSecond,
            0.0};
    }

    ClosestPoints2d best{};
    bool hasBest = false;

    const auto updateFromLineEndpoint = [&](const Point2d& endpoint, double parameterOnLine)
    {
        const ArcProjectionCandidate projected = ProjectPointToArcSegmentLocal(endpoint, arc, Geometry::kIntersectionDefaultEpsilon);
        UpdateClosest(best, hasBest, endpoint, projected.point, parameterOnLine, projected.parameter);
    };

    const auto updateFromArcEndpoint = [&](const Point2d& endpoint, double parameterOnArc)
    {
        const SegmentProjection2d projection = ProjectPointToLineSegmentLocal(endpoint, line, true);
        UpdateClosest(best, hasBest, projection.point, endpoint, projection.parameter, parameterOnArc);
    };

    updateFromLineEndpoint(line.startPoint, 0.0);
    updateFromLineEndpoint(line.endPoint, 1.0);

    updateFromArcEndpoint(arc.StartPoint(), 0.0);
    updateFromArcEndpoint(arc.EndPoint(), 1.0);

    const SegmentProjection2d centerProjection = ProjectPointToLineSegmentLocal(arc.center, line, true);
    const Vector2d fromCenter = centerProjection.point - arc.center;
    if (fromCenter.LengthSquared() > Geometry::kIntersectionDefaultEpsilon * Geometry::kIntersectionDefaultEpsilon)
    {
        const double angle = std::atan2(fromCenter.y, fromCenter.x);
        if (IsAngleOnArc(arc, angle, Geometry::kIntersectionDefaultEpsilon))
        {
            const Point2d arcPoint = PointAtAngle(arc, angle);
            UpdateClosest(
                best,
                hasBest,
                centerProjection.point,
                arcPoint,
                centerProjection.parameter,
                ArcParameterAtAngle(arc, angle));
        }
    }

    return best;
}

[[nodiscard]] ClosestPoints2d ClosestPointsArcArcInternal(
    const ArcSegment2d& first,
    const ArcSegment2d& second)
{
    const SegmentIntersection2d intersection = IntersectArcArcInternal(first, second, Geometry::kIntersectionDefaultEpsilon);
    if (intersection.HasIntersection())
    {
        const IntersectionPoint2d& point = intersection.points[0];
        return ClosestPoints2d{
            point.point,
            point.point,
            point.parameterOnFirst,
            point.parameterOnSecond,
            0.0};
    }

    ClosestPoints2d best{};
    bool hasBest = false;

    const auto updateFromFirstEndpoint = [&](const Point2d& endpoint, double parameterOnFirst)
    {
        const ArcProjectionCandidate projected = ProjectPointToArcSegmentLocal(endpoint, second, Geometry::kIntersectionDefaultEpsilon);
        UpdateClosest(
            best,
            hasBest,
            endpoint,
            projected.point,
            parameterOnFirst,
            projected.parameter);
    };

    const auto updateFromSecondEndpoint = [&](const Point2d& endpoint, double parameterOnSecond)
    {
        const ArcProjectionCandidate projected = ProjectPointToArcSegmentLocal(endpoint, first, Geometry::kIntersectionDefaultEpsilon);
        UpdateClosest(
            best,
            hasBest,
            projected.point,
            endpoint,
            projected.parameter,
            parameterOnSecond);
    };

    updateFromFirstEndpoint(first.StartPoint(), 0.0);
    updateFromFirstEndpoint(first.EndPoint(), 1.0);
    updateFromSecondEndpoint(second.StartPoint(), 0.0);
    updateFromSecondEndpoint(second.EndPoint(), 1.0);

    const Vector2d centerDelta = second.center - first.center;
    const double centerDistanceSquared = Dot(centerDelta, centerDelta);
    if (centerDistanceSquared > Geometry::kIntersectionDefaultEpsilon * Geometry::kIntersectionDefaultEpsilon)
    {
        const double centerDistance = std::sqrt(centerDistanceSquared);
        const Vector2d unit = centerDelta / centerDistance;
        const Point2d firstCandidate = MakePoint(
            first.center.x + first.radius * unit.x,
            first.center.y + first.radius * unit.y);
        const Point2d secondCandidate = MakePoint(
            second.center.x - second.radius * unit.x,
            second.center.y - second.radius * unit.y);
        const double firstAngle = std::atan2(firstCandidate.y - first.center.y, firstCandidate.x - first.center.x);
        const double secondAngle = std::atan2(secondCandidate.y - second.center.y, secondCandidate.x - second.center.x);
        if (IsAngleOnArc(first, firstAngle, Geometry::kIntersectionDefaultEpsilon) &&
            IsAngleOnArc(second, secondAngle, Geometry::kIntersectionDefaultEpsilon))
        {
            UpdateClosest(
                best,
                hasBest,
                firstCandidate,
                secondCandidate,
                ArcParameterAtAngle(first, firstAngle),
                ArcParameterAtAngle(second, secondAngle));
        }
    }

    return best;
}

[[nodiscard]] SegmentIntersection2d SwapIntersectionResult(SegmentIntersection2d result)
{
    if (result.pointCount >= 1)
    {
        std::swap(result.points[0].parameterOnFirst, result.points[0].parameterOnSecond);
    }

    if (result.pointCount == 2)
    {
        std::swap(result.points[1].parameterOnFirst, result.points[1].parameterOnSecond);
        std::swap(result.points[0], result.points[1]);
    }

    return result;
}

[[nodiscard]] ClosestPoints2d SwapClosestPointsResult(ClosestPoints2d result)
{
    std::swap(result.firstPoint, result.secondPoint);
    std::swap(result.parameterOnFirst, result.parameterOnSecond);
    return result;
}

[[nodiscard]] SegmentIntersection2d IntersectArcArcInternal(
    const ArcSegment2d& first,
    const ArcSegment2d& second,
    double eps)
{
    if (!first.IsValid() || !second.IsValid())
    {
        return MakeNoIntersection();
    }

    if (SameCircle(first, second, eps))
    {
        return IntersectArcArcSameCircle(first, second, eps);
    }

    const Vector2d centerDelta = second.center - first.center;
    const double distanceSquared = Dot(centerDelta, centerDelta);
    if (distanceSquared <= eps * eps)
    {
        return MakeNoIntersection();
    }

    const double distance = std::sqrt(distanceSquared);
    if (distance > first.radius + second.radius + eps ||
        distance < std::abs(first.radius - second.radius) - eps)
    {
        return MakeNoIntersection();
    }

    const double a = (first.radius * first.radius - second.radius * second.radius + distanceSquared) /
                     (2.0 * distance);
    const double hSquared = first.radius * first.radius - a * a;
    if (hSquared < -eps)
    {
        return MakeNoIntersection();
    }

    const double h = hSquared <= 0.0 ? 0.0 : std::sqrt(hSquared);
    const Vector2d unit = centerDelta / distance;
    const Point2d basePoint = MakePoint(
        first.center.x + a * unit.x,
        first.center.y + a * unit.y);
    const Vector2d perpendicular{-unit.y, unit.x};

    const Point2d candidateOne = MakePoint(
        basePoint.x + h * perpendicular.x,
        basePoint.y + h * perpendicular.y);
    const Point2d candidateTwo = MakePoint(
        basePoint.x - h * perpendicular.x,
        basePoint.y - h * perpendicular.y);

    SegmentIntersection2d result;
    bool hasPoint = false;
    for (const Point2d& point : std::array<Point2d, 2>{candidateOne, candidateTwo})
    {
        const double angleFirst = std::atan2(point.y - first.center.y, point.x - first.center.x);
        const double angleSecond = std::atan2(point.y - second.center.y, point.x - second.center.x);
        if (!IsAngleOnArc(first, angleFirst, eps) || !IsAngleOnArc(second, angleSecond, eps))
        {
            continue;
        }

        const double firstParameter = ArcParameterAtAngle(first, angleFirst);
        const double secondParameter = ArcParameterAtAngle(second, angleSecond);
        if (!hasPoint)
        {
            result = MakePointIntersection(
                point,
                firstParameter,
                secondParameter,
                AlmostEqual(h, 0.0, eps) ? IntersectionKind2d::Tangent : IntersectionKind2d::Point);
            hasPoint = true;
            continue;
        }

        AddIntersectionPoint(result, IntersectionKind2d::Point, point, firstParameter, secondParameter, eps);
    }

    return result;
}

[[nodiscard]] ClosestPoints2d ClosestPointsLineArcInternal(
    const LineSegment2d& line,
    const ArcSegment2d& arc,
    double eps)
{
    const SegmentIntersection2d intersection = IntersectLineArcInternal(line, arc, eps);
    if (intersection.HasIntersection())
    {
        const IntersectionPoint2d& point = intersection.points[0];
        return ClosestPoints2d{
            point.point,
            point.point,
            point.parameterOnFirst,
            point.parameterOnSecond,
            0.0};
    }

    ClosestPoints2d best{};
    bool hasBest = false;

    const auto updateFromLineEndpoint = [&](const Point2d& endpoint, double parameterOnLine)
    {
        const ArcProjectionCandidate projected = ProjectPointToArcSegmentLocal(endpoint, arc, eps);
        UpdateClosest(
            best,
            hasBest,
            endpoint,
            projected.point,
            parameterOnLine,
            projected.parameter);
    };

    const auto updateFromArcEndpoint = [&](const Point2d& endpoint, double parameterOnArc)
    {
        const SegmentProjection2d projection = ProjectPointToLineSegmentLocal(endpoint, line, true);
        UpdateClosest(best, hasBest, projection.point, endpoint, projection.parameter, parameterOnArc);
    };

    updateFromLineEndpoint(line.startPoint, 0.0);
    updateFromLineEndpoint(line.endPoint, 1.0);
    updateFromArcEndpoint(arc.StartPoint(), 0.0);
    updateFromArcEndpoint(arc.EndPoint(), 1.0);

    const SegmentProjection2d centerProjection = ProjectPointToLineSegmentLocal(arc.center, line, true);
    const Vector2d fromCenter = centerProjection.point - arc.center;
    if (fromCenter.LengthSquared() > eps * eps)
    {
        const double angle = std::atan2(fromCenter.y, fromCenter.x);
        if (IsAngleOnArc(arc, angle, eps))
        {
            const Point2d arcPoint = PointAtAngle(arc, angle);
            UpdateClosest(
                best,
                hasBest,
                centerProjection.point,
                arcPoint,
                centerProjection.parameter,
                ArcParameterAtAngle(arc, angle));
        }
    }

    return best;
}
} // namespace

SegmentIntersection2d Intersect(
    const LineSegment2d& first,
    const LineSegment2d& second,
    double eps)
{
    return IntersectLineLineInternal(first, second, eps);
}

SegmentIntersection2d Intersect(
    const LineSegment2d& first,
    const ArcSegment2d& second,
    double eps)
{
    return IntersectLineArcInternal(first, second, eps);
}

SegmentIntersection2d Intersect(
    const ArcSegment2d& first,
    const ArcSegment2d& second,
    double eps)
{
    return IntersectArcArcInternal(first, second, eps);
}

SegmentIntersection2d Intersect(
    const Segment2d& first,
    const Segment2d& second,
    double eps)
{
    if (!first.IsValid() || !second.IsValid())
    {
        return SegmentIntersection2d{};
    }

    if (first.Kind() == SegmentKind2::Line && second.Kind() == SegmentKind2::Line)
    {
        return Intersect(
            static_cast<const LineSegment2d&>(first),
            static_cast<const LineSegment2d&>(second),
            eps);
    }

    if (first.Kind() == SegmentKind2::Line && second.Kind() == SegmentKind2::Arc)
    {
        return Intersect(
            static_cast<const LineSegment2d&>(first),
            static_cast<const ArcSegment2d&>(second),
            eps);
    }

    if (first.Kind() == SegmentKind2::Arc && second.Kind() == SegmentKind2::Line)
    {
        return SwapIntersectionResult(Intersect(
            static_cast<const LineSegment2d&>(second),
            static_cast<const ArcSegment2d&>(first),
            eps));
    }

    if (first.Kind() == SegmentKind2::Arc && second.Kind() == SegmentKind2::Arc)
    {
        return Intersect(
            static_cast<const ArcSegment2d&>(first),
            static_cast<const ArcSegment2d&>(second),
            eps);
    }

    return SegmentIntersection2d{};
}

bool HasIntersection(
    const Segment2d& first,
    const Segment2d& second,
    double eps)
{
    return Intersect(first, second, eps).HasIntersection();
}

ClosestPoints2d ClosestPoints(
    const LineSegment2d& first,
    const LineSegment2d& second)
{
    return ClosestPointsLineLineInternal(first, second);
}

ClosestPoints2d ClosestPoints(
    const LineSegment2d& first,
    const ArcSegment2d& second)
{
    return ClosestPointsLineArcInternal(first, second, Geometry::kIntersectionDefaultEpsilon);
}

ClosestPoints2d ClosestPoints(
    const ArcSegment2d& first,
    const ArcSegment2d& second)
{
    return ClosestPointsArcArcInternal(first, second);
}

ClosestPoints2d ClosestPoints(
    const Segment2d& first,
    const Segment2d& second)
{
    if (!first.IsValid() || !second.IsValid())
    {
        return ClosestPoints2d{};
    }

    if (first.Kind() == SegmentKind2::Line && second.Kind() == SegmentKind2::Line)
    {
        return ClosestPoints(
            static_cast<const LineSegment2d&>(first),
            static_cast<const LineSegment2d&>(second));
    }

    if (first.Kind() == SegmentKind2::Line && second.Kind() == SegmentKind2::Arc)
    {
        return ClosestPoints(
            static_cast<const LineSegment2d&>(first),
            static_cast<const ArcSegment2d&>(second));
    }

    if (first.Kind() == SegmentKind2::Arc && second.Kind() == SegmentKind2::Line)
    {
        return SwapClosestPointsResult(ClosestPoints(
            static_cast<const LineSegment2d&>(second),
            static_cast<const ArcSegment2d&>(first)));
    }

    if (first.Kind() == SegmentKind2::Arc && second.Kind() == SegmentKind2::Arc)
    {
        return ClosestPoints(
            static_cast<const ArcSegment2d&>(first),
            static_cast<const ArcSegment2d&>(second));
    }

    return ClosestPoints2d{};
}
} // namespace Geometry::Sdk
