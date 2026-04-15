#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

namespace geometry
{
namespace detail
{
constexpr double kPi = 3.141592653589793238462643383279502884;
constexpr double kTwoPi = 2.0 * kPi;

[[nodiscard]] inline double ClampDouble(double value, double low, double high)
{
    return std::max(low, std::min(value, high));
}

[[nodiscard]] inline double NormalizeAngle(double angle)
{
    angle = std::fmod(angle, kTwoPi);
    if (angle < 0.0)
    {
        angle += kTwoPi;
    }
    return angle;
}

[[nodiscard]] inline double SignedSweep(double startAngle, double endAngle, ArcDirection direction)
{
    if (direction == ArcDirection::CounterClockwise)
    {
        return NormalizeAngle(endAngle - startAngle);
    }

    return -NormalizeAngle(startAngle - endAngle);
}

[[nodiscard]] inline bool IsAngleOnArc(double candidateAngle, double startAngle, double signedSweep, double eps = kDefaultEpsilon)
{
    if (signedSweep >= 0.0)
    {
        const double delta = NormalizeAngle(candidateAngle - startAngle);
        return delta <= signedSweep + eps;
    }

    const double delta = NormalizeAngle(startAngle - candidateAngle);
    return delta <= (-signedSweep) + eps;
}

template <typename T>
[[nodiscard]] constexpr bool IsFiniteValue(T value)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        return std::isfinite(static_cast<double>(value));
    }
    else
    {
        return true;
    }
}
} // namespace detail
} // namespace geometry
