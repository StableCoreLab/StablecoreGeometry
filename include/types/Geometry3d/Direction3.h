#pragma once

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

#include "Support/Epsilon.h"
#include "Types/Geometry3d/Vector3.h"

namespace Geometry
{
struct Direction3d
{
    Vector3d value{};

    [[nodiscard]] static Direction3d FromVector(const Vector3d& vector, double eps = kDefaultEpsilon)
    {
        Direction3d direction;
        if (vector.IsValid() && vector.Length() > eps)
        {
            direction.value = vector.Normalized(eps);
        }
        return direction;
    }

    [[nodiscard]] bool IsValid(double eps = kDefaultEpsilon) const
    {
        return value.IsValid() && value.Length() > eps;
    }

    [[nodiscard]] Vector3d Vector() const
    {
        return value;
    }

    [[nodiscard]] Direction3d Reverse() const
    {
        return Direction3d::FromVector(-value);
    }

    [[nodiscard]] bool AlmostEquals(const Direction3d& other, double angleEps = kDefaultEpsilon) const
    {
        if (!IsValid() || !other.IsValid())
        {
            return false;
        }

        const double cosine = std::clamp(Dot(value, other.value), -1.0, 1.0);
        return cosine >= std::cos(angleEps);
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "Direction3d{value=" << value.DebugString() << "}";
        return stream.str();
    }
};
} // namespace Geometry
