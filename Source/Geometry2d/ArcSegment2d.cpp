#include "Geometry2d/ArcSegment2d.h"

#include <cmath>
#include <memory>
#include <numbers>
#include <sstream>

#include "Support/Epsilon.h"

namespace Geometry
{
ArcSegment2d::ArcSegment2d(
    const Point2d& center,
    double radius,
    double startAngle,
    double sweepAngle)
    : center(center),
      radius(radius),
      startAngle(startAngle),
      sweepAngle(sweepAngle)
{
}

ArcSegment2d::ArcSegment2d(
    const Point2d& center,
    double radius,
    double startAngle,
    double endAngle,
    ArcDirection direction)
    : center(center),
      radius(radius),
      startAngle(startAngle),
      sweepAngle(endAngle - startAngle)
{
    if (direction == ArcDirection::CounterClockwise)
    {
        while (sweepAngle < 0.0)
        {
            sweepAngle += 2.0 * std::numbers::pi_v<double>;
        }
    }
    else
    {
        while (sweepAngle > 0.0)
        {
            sweepAngle -= 2.0 * std::numbers::pi_v<double>;
        }
    }
}

ArcSegment2d ArcSegment2d::FromCenterRadiusStartSweep(
    const Point2d& center,
    double radius,
    double startAngle,
    double sweepAngle)
{
    return ArcSegment2d(center, radius, startAngle, sweepAngle);
}

SegmentKind2 ArcSegment2d::Kind() const
{
    return SegmentKind2::Arc;
}

bool ArcSegment2d::IsValid() const
{
    return center.IsValid() &&
           std::isfinite(radius) &&
           std::isfinite(startAngle) &&
           std::isfinite(sweepAngle) &&
           radius > 0.0 &&
           std::abs(sweepAngle) > Geometry::kArcSegmentDefaultEpsilon &&
           std::abs(sweepAngle) <= 2.0 * std::numbers::pi_v<double> + Geometry::kArcSegmentDefaultEpsilon;
}

ArcDirection ArcSegment2d::Direction() const
{
    return sweepAngle >= 0.0 ? ArcDirection::CounterClockwise : ArcDirection::Clockwise;
}

double ArcSegment2d::EndAngle() const
{
    return startAngle + sweepAngle;
}

Point2d ArcSegment2d::StartPoint() const
{
    return PointAtAngle(startAngle);
}

Point2d ArcSegment2d::EndPoint() const
{
    return PointAtAngle(EndAngle());
}

double ArcSegment2d::Length() const
{
    if (!IsValid())
    {
        return 0.0;
    }

    return std::abs(sweepAngle) * radius;
}

Box2d ArcSegment2d::Bounds() const
{
    if (!IsValid())
    {
        return Box2d{};
    }

    Box2d box;
    box.ExpandToInclude(StartPoint());
    box.ExpandToInclude(EndPoint());

    constexpr double kCriticalAngles[] = {
        0.0,
        std::numbers::pi_v<double> * 0.5,
        std::numbers::pi_v<double>,
        std::numbers::pi_v<double> * 1.5};

    for (double angle : kCriticalAngles)
    {
        if (IsAngleOnArc(angle))
        {
            box.ExpandToInclude(PointAtAngle(angle));
        }
    }

    return box;
}

Point2d ArcSegment2d::PointAt(double parameter) const
{
    return PointAtLength(parameter * Length(), false);
}

Point2d ArcSegment2d::PointAtLength(double distanceFromStart, bool clampToSegment) const
{
    if (!IsValid())
    {
        return StartPoint();
    }

    const double length = Length();
    if (length <= 0.0)
    {
        return StartPoint();
    }

    if (clampToSegment)
    {
        if (distanceFromStart < 0.0)
        {
            distanceFromStart = 0.0;
        }
        else if (distanceFromStart > length)
        {
            distanceFromStart = length;
        }
    }

    return PointAtAngle(startAngle + sweepAngle * (distanceFromStart / length));
}

bool ArcSegment2d::AlmostEquals(const ArcSegment2d& other, double eps) const
{
    return center.AlmostEquals(other.center, eps) &&
           std::abs(radius - other.radius) <= eps &&
           std::abs(startAngle - other.startAngle) <= eps &&
           std::abs(sweepAngle - other.sweepAngle) <= eps;
}

std::string ArcSegment2d::DebugString() const
{
    std::ostringstream stream;
    stream << "ArcSegment2d{center=" << center.DebugString()
           << ", radius=" << radius
           << ", startAngle=" << startAngle
           << ", sweepAngle=" << sweepAngle << "}";
    return stream.str();
}

std::unique_ptr<Segment2d> ArcSegment2d::Clone() const
{
    return std::make_unique<ArcSegment2d>(*this);
}

double ArcSegment2d::NormalizeAngle(double angle)
{
    angle = std::fmod(angle, 2.0 * std::numbers::pi_v<double>);
    if (angle < 0.0)
    {
        angle += 2.0 * std::numbers::pi_v<double>;
    }
    return angle;
}

bool ArcSegment2d::IsAngleOnArc(double candidateAngle) const
{
    if (sweepAngle >= 0.0)
    {
        const double delta = NormalizeAngle(candidateAngle - startAngle);
        return delta <= sweepAngle + Geometry::kArcSegmentDefaultEpsilon;
    }

    const double delta = NormalizeAngle(startAngle - candidateAngle);
    return delta <= (-sweepAngle) + Geometry::kArcSegmentDefaultEpsilon;
}

Point2d ArcSegment2d::PointAtAngle(double angle) const
{
    return Point2d{
        center.x + radius * std::cos(angle),
        center.y + radius * std::sin(angle)};
}
} // namespace Geometry


