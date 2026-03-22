#pragma once

#include <sstream>
#include <string>

#include "common/Epsilon.h"
#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"

namespace geometry::sdk
{
class GEOMETRY_API LineSegment2d
{
public:
    Point2d startPoint{};
    Point2d endPoint{};

    constexpr LineSegment2d() = default;

    constexpr LineSegment2d(const Point2d& startPointValue, const Point2d& endPointValue)
        : startPoint(startPointValue), endPoint(endPointValue)
    {
    }

    [[nodiscard]] static constexpr LineSegment2d FromEndpoints(
        const Point2d& startPointValue,
        const Point2d& endPointValue)
    {
        return LineSegment2d(startPointValue, endPointValue);
    }

    [[nodiscard]] bool IsValid() const
    {
        return startPoint.IsValid() && endPoint.IsValid() && !startPoint.AlmostEquals(endPoint);
    }

    [[nodiscard]] double Length() const
    {
        return (endPoint - startPoint).Length();
    }

    [[nodiscard]] Box2d Bounds() const
    {
        if (!IsValid())
        {
            return Box2d{};
        }

        Box2d box;
        box.ExpandToInclude(startPoint);
        box.ExpandToInclude(endPoint);
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
            return startPoint;
        }

        const double length = Length();
        if (length <= 0.0)
        {
            return startPoint;
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

        const double ratio = distanceFromStart / length;
        return Point2d{
            startPoint.x + (endPoint.x - startPoint.x) * ratio,
            startPoint.y + (endPoint.y - startPoint.y) * ratio};
    }

    [[nodiscard]] bool AlmostEquals(
        const LineSegment2d& other,
        double eps = geometry::kDefaultEpsilon) const
    {
        return startPoint.AlmostEquals(other.startPoint, eps) &&
               endPoint.AlmostEquals(other.endPoint, eps);
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "LineSegment2d{start=" << startPoint.DebugString()
               << ", end=" << endPoint.DebugString() << "}";
        return stream.str();
    }

    [[nodiscard]] constexpr bool operator==(const LineSegment2d& other) const = default;
    [[nodiscard]] constexpr bool operator!=(const LineSegment2d& other) const = default;
};
} // namespace geometry::sdk
