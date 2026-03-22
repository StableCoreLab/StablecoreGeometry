#pragma once

#include "sdk/GeometryTypes.h"
#include "types/Box2.h"
#include "types/Point2.h"
#include "types/Vector2.h"

namespace geometry::sdk::detail
{
[[nodiscard]] inline geometry::Point2d ToInternal(const Point2d& point)
{
    return geometry::Point2d(point.x, point.y);
}

[[nodiscard]] inline geometry::Vector2d ToInternal(const Vector2d& vector)
{
    return geometry::Vector2d(vector.x, vector.y);
}

[[nodiscard]] inline geometry::Box2d ToInternal(const Box2d& box)
{
    return box;
}

[[nodiscard]] inline Point2d ToSdk(const geometry::Point2d& point)
{
    return Point2d{point.x, point.y};
}

[[nodiscard]] inline Vector2d ToSdk(const geometry::Vector2d& vector)
{
    return Vector2d{vector.x, vector.y};
}

[[nodiscard]] inline Box2d ToSdk(const geometry::Box2d& box)
{
    return box;
}
} // namespace geometry::sdk::detail
