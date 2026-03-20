#pragma once

#include <type_traits>

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

    [[nodiscard]] constexpr bool operator==(const Vec2Storage& other) const = default;
    [[nodiscard]] constexpr bool operator!=(const Vec2Storage& other) const = default;
};

template <typename T>
struct Point2 : Vec2Storage<T>
{
    using Vec2Storage<T>::Vec2Storage;
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
