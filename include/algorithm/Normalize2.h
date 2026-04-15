#pragma once

#include <type_traits>

#include "algorithm/Predicate2.h"

namespace Geometry
{
template <typename T>
[[nodiscard]] bool TryNormalize(
    const Vector2<T>& vector,
    Vector2<double>& normalized,
    double eps = kDefaultEpsilon)
{
    if (IsZero(vector, eps))
    {
        return false;
    }

    const double length = static_cast<double>(vector.Length());
    normalized = Vector2<double>(
        static_cast<double>(vector.x) / length,
        static_cast<double>(vector.y) / length);
    return true;
}
}
