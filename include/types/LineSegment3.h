#pragma once

#include <sstream>
#include <string>

#include "types/Box3.h"
#include "types/Direction3.h"

namespace Geometry
{
struct LineSegment3d
{
    Point3d startPoint{};
    Point3d endPoint{};

    [[nodiscard]] static LineSegment3d FromStartEnd(const Point3d& startValue, const Point3d& endValue)
    {
        return LineSegment3d{startValue, endValue};
    }

    [[nodiscard]] bool IsValid(double eps = kDefaultEpsilon) const
    {
        return startPoint.IsValid() && endPoint.IsValid() && (endPoint - startPoint).Length() > eps;
    }

    [[nodiscard]] double Length() const
    {
        return (endPoint - startPoint).Length();
    }

    [[nodiscard]] Box3d Bounds() const
    {
        if (!IsValid())
        {
            return {};
        }

        Box3d box;
        box.ExpandToInclude(startPoint);
        box.ExpandToInclude(endPoint);
        return box;
    }

    [[nodiscard]] Point3d PointAt(double parameter) const
    {
        return startPoint + (endPoint - startPoint) * parameter;
    }

    [[nodiscard]] Point3d PointAtLength(double distance, bool clampToSegment = false) const
    {
        if (!IsValid())
        {
            return startPoint;
        }

        const double length = Length();
        if (length <= kDefaultEpsilon)
        {
            return startPoint;
        }

        if (clampToSegment)
        {
            distance = std::clamp(distance, 0.0, length);
        }
        return PointAt(distance / length);
    }

    [[nodiscard]] Direction3d Direction(double eps = kDefaultEpsilon) const
    {
        return Direction3d::FromVector(endPoint - startPoint, eps);
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "LineSegment3d{startPoint=" << startPoint.DebugString()
               << ", endPoint=" << endPoint.DebugString() << "}";
        return stream.str();
    }
};
} // namespace Geometry
