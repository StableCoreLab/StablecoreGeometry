#include <cassert>
#include <concepts>
#include <cmath>

#include "algorithm/Normalize2.h"

using geometry::Cross;
using geometry::Dot;
using geometry::IsEqual;
using geometry::IsZero;
using geometry::Point2d;
using geometry::Point2i;
using geometry::TryNormalize;
using geometry::Vector2d;
using geometry::Vector2i;

template <typename T>
concept SupportsPointPlusPoint = requires(T a, T b) {
    a + b;
};

template <typename T>
concept SupportsPointScalarMul = requires(T a) {
    a * 2;
};

template <typename T>
concept SupportsPointNegate = requires(T a) {
    -a;
};

int main()
{
    static_assert(!std::same_as<Point2d, Vector2d>);
    static_assert(!SupportsPointPlusPoint<Point2d>);
    static_assert(!SupportsPointScalarMul<Point2d>);
    static_assert(!SupportsPointNegate<Point2d>);

    constexpr Point2i pointA(1, 2);
    constexpr Point2i pointB(4, 6);
    constexpr Vector2i displacement = pointB - pointA;
    static_assert(displacement == Vector2i(3, 4));
    static_assert(pointA + displacement == pointB);
    static_assert(pointB - displacement == pointA);

    constexpr Vector2i vectorA(3, 4);
    constexpr Vector2i vectorB(-1, 2);
    static_assert(vectorA + vectorB == Vector2i(2, 6));
    static_assert(vectorA - vectorB == Vector2i(4, 2));
    static_assert(-vectorB == Vector2i(1, -2));
    static_assert(vectorA * 2 == Vector2i(6, 8));
    static_assert(2 * vectorA == Vector2i(6, 8));
    static_assert(vectorA / 2 == Vector2i(1, 2));
    static_assert(vectorA.LengthSquared() == 25.0);
    assert(std::abs(vectorA.Length() - 5.0) < 1e-12);

    static_assert(Dot(vectorA, vectorB) == 5);
    static_assert(Cross(vectorA, vectorB) == 10);
    static_assert(IsZero(0));
    static_assert(!IsZero(1));
    static_assert(IsZero(Vector2i{}));
    static_assert(!IsZero(vectorA));
    static_assert(IsEqual(Point2d(1.0, 2.0), Point2d(1.0 + 1e-10, 2.0 - 1e-10)));

    Vector2d normalized;
    const bool normalizedOk = TryNormalize(Vector2d(3.0, 4.0), normalized);
    assert(normalizedOk);
    assert(std::abs(normalized.x - 0.6) < 1e-12);
    assert(std::abs(normalized.y - 0.8) < 1e-12);
    assert(std::abs(normalized.Length() - 1.0) < 1e-12);

    Vector2d zeroNormalized(7.0, 9.0);
    const bool zeroOk = TryNormalize(Vector2d(1e-12, 0.0), zeroNormalized);
    assert(!zeroOk);
    assert(zeroNormalized == Vector2d(7.0, 9.0));

    return 0;
}
