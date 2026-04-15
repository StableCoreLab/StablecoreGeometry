#pragma once

#include <sstream>
#include <string>

#include "types/Box3.h"
#include "types/Plane.h"

namespace Geometry
{
struct Triangle3d
{
    Point3d a{};
    Point3d b{};
    Point3d c{};

    [[nodiscard]] bool IsDegenerate(double eps = kDefaultEpsilon) const
    {
        return Cross(b - a, c - a).Length() <= eps;
    }

    [[nodiscard]] bool IsValid(double eps = kDefaultEpsilon) const
    {
        return a.IsValid() && b.IsValid() && c.IsValid() && !IsDegenerate(eps);
    }

    [[nodiscard]] Box3d Bounds() const
    {
        if (!a.IsValid() || !b.IsValid() || !c.IsValid())
        {
            return {};
        }

        Box3d box;
        box.ExpandToInclude(a);
        box.ExpandToInclude(b);
        box.ExpandToInclude(c);
        return box;
    }

    [[nodiscard]] Vector3d Normal() const
    {
        return Cross(b - a, c - a);
    }

    [[nodiscard]] double Area() const
    {
        return 0.5 * Normal().Length();
    }

    [[nodiscard]] Point3d Centroid() const
    {
        return Point3d{(a.x + b.x + c.x) / 3.0,
                       (a.y + b.y + c.y) / 3.0,
                       (a.z + b.z + c.z) / 3.0};
    }

    [[nodiscard]] Plane Plane() const
    {
        return Geometry::Plane::FromPointAndNormal(a, Normal());
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "Triangle3d{a=" << a.DebugString()
               << ", b=" << b.DebugString()
               << ", c=" << c.DebugString() << "}";
        return stream.str();
    }
};
} // namespace Geometry
