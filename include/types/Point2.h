#pragma once

#include <cmath>
#include <sstream>
#include <string>
#include <type_traits>

#include "common/Epsilon.h"

namespace geometry
{
template <typename T>
struct Vector2;

template <typename T>
struct Vec2Storage
{
    T x{};
    T y{};

    constexpr Vec2Storage() = default;
    constexpr Vec2Storage(T xValue, T yValue) : x(xValue), y(yValue) {}

    constexpr void Set(T xValue, T yValue)
    {
        x = xValue;
        y = yValue;
    }

    [[nodiscard]] constexpr bool IsValid() const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return std::isfinite(x) && std::isfinite(y);
        }

        return true;
    }

    [[nodiscard]] constexpr bool AlmostEquals(
        const Vec2Storage& other,
        double eps = kDefaultEpsilon) const
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return std::abs(static_cast<double>(x - other.x)) <= eps &&
                   std::abs(static_cast<double>(y - other.y)) <= eps;
        }

        (void)eps;
        return *this == other;
    }

    [[nodiscard]] std::string DebugString(const char* typeName) const
    {
        std::ostringstream stream;
        stream << typeName << "{x=" << x << ", y=" << y << "}";
        return stream.str();
    }

    [[nodiscard]] constexpr bool operator==(const Vec2Storage& other) const = default;
    [[nodiscard]] constexpr bool operator!=(const Vec2Storage& other) const = default;
};

template <typename T>
struct Point2 : Vec2Storage<T>
{
    using Vec2Storage<T>::Vec2Storage;

    [[nodiscard]] static constexpr Point2 FromXY(T xValue, T yValue)
    {
        return Point2(xValue, yValue);
    }

    [[nodiscard]] std::string DebugString() const
    {
        return this->Vec2Storage<T>::DebugString("Point2");
    }
};

template <typename T>
[[nodiscard]] constexpr Point2<T> operator+(const Point2<T>& point, const Vector2<T>& vector)
{
    return Point2<T>(point.x + vector.x, point.y + vector.y);
}

template <typename T>
[[nodiscard]] constexpr Point2<T> operator-(const Point2<T>& point, const Vector2<T>& vector)
{
    return Point2<T>(point.x - vector.x, point.y - vector.y);
}

using Point2d = Point2<double>;
using Point2i = Point2<int>;
}
