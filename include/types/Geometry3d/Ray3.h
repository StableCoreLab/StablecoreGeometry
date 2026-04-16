#pragma once

#include <sstream>
#include <string>

#include "Types/Geometry3d/Direction3.h"
#include "Types/Geometry3d/Point3.h"

namespace Geometry
{
struct Ray3d
{
    Point3d origin{};
    Direction3d direction{};

    [[nodiscard]] static Ray3d FromOriginAndDirection(const Point3d& originValue, const Direction3d& directionValue)
    {
        return Ray3d{originValue, directionValue};
    }

    [[nodiscard]] bool IsValid(double eps = kDefaultEpsilon) const
    {
        return origin.IsValid() && direction.IsValid(eps);
    }

    [[nodiscard]] Point3d PointAt(double parameter) const
    {
        return origin + direction.Vector() * parameter;
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "Ray3d{origin=" << origin.DebugString() << ", direction=" << direction.DebugString() << "}";
        return stream.str();
    }
};
} // namespace Geometry
