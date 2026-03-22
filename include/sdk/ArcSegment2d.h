#pragma once

#include <cmath>
#include <numbers>
#include <sstream>
#include <string>

#include "common/Epsilon.h"
#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"
#include "types/Segment2.h"

namespace geometry::sdk
{
using geometry::ArcDirection;

class GEOMETRY_API ArcSegment2d
{
public:
    Point2d center{};
    double radius{0.0};
    double startAngle{0.0};
    double sweepAngle{0.0};

    constexpr ArcSegment2d() = default;

    constexpr ArcSegment2d(
        const Point2d& centerValue,
        double radiusValue,
        double startAngleValue,
        double sweepAngleValue)
        : center(centerValue),
          radius(radiusValue),
          startAngle(startAngleValue),
          sweepAngle(sweepAngleValue)
    {
    }

    [[nodiscard]] static constexpr ArcSegment2d FromCenterRadiusStartSweep(
        const Point2d& centerValue,
        double radiusValue,
        double startAngleValue,
        double sweepAngleValue)
    {
        return ArcSegment2d(centerValue, radiusValue, startAngleValue, sweepAngleValue);
    }

    [[nodiscard]] bool IsValid() const
    {
        return center.IsValid() &&
               std::isfinite(radius) &&
               std::isfinite(startAngle) &&
               std::isfinite(sweepAngle) &&
               radius > 0.0 &&
               std::abs(sweepAngle) > geometry::kDefaultEpsilon &&
               std::abs(sweepAngle) <= 2.0 * std::numbers::pi_v<double> + geometry::kDefaultEpsilon;
    }

    [[nodiscard]] ArcDirection Direction() const
    {
        return sweepAngle >= 0.0 ? ArcDirection::CounterClockwise : ArcDirection::Clockwise;
    }

    [[nodiscard]] double EndAngle() const
    {
        return startAngle + sweepAngle;
    }

    [[nodiscard]] Point2d StartPoint() const
    {
        return PointAtAngle(startAngle);
    }

    [[nodiscard]] Point2d EndPoint() const
    {
        return PointAtAngle(EndAngle());
    }

    [[nodiscard]] double Length() const
    {
        if (!IsValid())
        {
            return 0.0;
        }

        return std::abs(sweepAngle) * radius;
    }

    [[nodiscard]] Box2d Bounds() const
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

    [[nodiscard]] Point2d PointAt(double parameter) const
    {
        return PointAtLength(parameter * Length(), false);
    }

    [[nodiscard]] Point2d PointAtLength(double distanceFromStart, bool clampToSegment = false) const
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

    [[nodiscard]] bool AlmostEquals(
        const ArcSegment2d& other,
        double eps = geometry::kDefaultEpsilon) const
    {
        return center.AlmostEquals(other.center, eps) &&
               std::abs(radius - other.radius) <= eps &&
               std::abs(startAngle - other.startAngle) <= eps &&
               std::abs(sweepAngle - other.sweepAngle) <= eps;
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "ArcSegment2d{center=" << center.DebugString()
               << ", radius=" << radius
               << ", startAngle=" << startAngle
               << ", sweepAngle=" << sweepAngle << "}";
        return stream.str();
    }

    [[nodiscard]] constexpr bool operator==(const ArcSegment2d& other) const = default;
    [[nodiscard]] constexpr bool operator!=(const ArcSegment2d& other) const = default;

private:
    [[nodiscard]] static double NormalizeAngle(double angle)
    {
        angle = std::fmod(angle, 2.0 * std::numbers::pi_v<double>);
        if (angle < 0.0)
        {
            angle += 2.0 * std::numbers::pi_v<double>;
        }
        return angle;
    }

    [[nodiscard]] bool IsAngleOnArc(double candidateAngle) const
    {
        if (sweepAngle >= 0.0)
        {
            const double delta = NormalizeAngle(candidateAngle - startAngle);
            return delta <= sweepAngle + geometry::kDefaultEpsilon;
        }

        const double delta = NormalizeAngle(startAngle - candidateAngle);
        return delta <= (-sweepAngle) + geometry::kDefaultEpsilon;
    }

    [[nodiscard]] Point2d PointAtAngle(double angle) const
    {
        return Point2d{
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle)};
    }
};
} // namespace geometry::sdk
