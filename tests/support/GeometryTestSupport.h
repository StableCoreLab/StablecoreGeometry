#pragma once

#include <cmath>
#include <cstdlib>
#include <concepts>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

namespace geometry::test
{
namespace detail
{
template <typename T>
concept HasXY = requires(const T& value) {
    value.x;
    value.y;
};

template <typename T>
concept HasMinMaxFields = requires(const T& value) {
    value.minPoint;
    value.maxPoint;
};

template <typename T>
concept HasMinMaxGetters = requires(const T& value) {
    value.GetMinPoint();
    value.GetMaxPoint();
};

template <typename T>
concept HasProjectionFields = requires(const T& value) {
    value.point;
    value.parameter;
    value.distanceSquared;
    value.isOnSegment;
};

template <typename T>
concept HasIsValid = requires(const T& value) {
    { value.IsValid() } -> std::convertible_to<bool>;
};

template <typename T>
[[nodiscard]] decltype(auto) GetMinPointLike(const T& value)
{
    if constexpr (HasMinMaxFields<T>)
    {
        return (value.minPoint);
    }
    else
    {
        return (value.GetMinPoint());
    }
}

template <typename T>
[[nodiscard]] decltype(auto) GetMaxPointLike(const T& value)
{
    if constexpr (HasMinMaxFields<T>)
    {
        return (value.maxPoint);
    }
    else
    {
        return (value.GetMaxPoint());
    }
}

template <typename T>
[[nodiscard]] std::string FormatNumber(T value)
{
    std::ostringstream out;
    out << std::setprecision(std::numeric_limits<long double>::max_digits10)
        << static_cast<long double>(value);
    return out.str();
}

[[nodiscard]] inline std::string FormatLocation(std::string_view file, int line)
{
    std::ostringstream out;
    out << file << ':' << line;
    return out.str();
}

[[noreturn]] inline void Fail(
    std::string_view kind,
    std::string_view file,
    int line,
    std::string_view message)
{
    std::cerr << FormatLocation(file, line) << ": " << kind << ": " << message << '\n';
    std::abort();
}

template <typename T>
[[nodiscard]] bool NearlyEqual(T lhs, T rhs, double eps)
{
    using std::abs;
    return abs(static_cast<long double>(lhs) - static_cast<long double>(rhs)) <= eps;
}

template <HasXY T>
[[nodiscard]] std::string DescribePointLike(const T& value)
{
    std::ostringstream out;
    out << '(' << FormatNumber(value.x) << ", " << FormatNumber(value.y) << ')';
    return out.str();
}

template <typename T>
[[nodiscard]] std::string DescribeBoxLike(const T& value)
{
    std::ostringstream out;
    out << '{';
    if constexpr (HasMinMaxFields<T>)
    {
        out << "min=" << DescribePointLike(GetMinPointLike(value))
            << ", max=" << DescribePointLike(GetMaxPointLike(value));
        if constexpr (HasIsValid<T>)
        {
            out << ", valid=" << (value.IsValid() ? "true" : "false");
        }
    }
    else if constexpr (HasMinMaxGetters<T>)
    {
        out << "min=" << DescribePointLike(GetMinPointLike(value))
            << ", max=" << DescribePointLike(GetMaxPointLike(value));
        if constexpr (HasIsValid<T>)
        {
            out << ", valid=" << (value.IsValid() ? "true" : "false");
        }
    }
    else
    {
        out << "<unformattable box-like type>";
    }
    out << '}';
    return out.str();
}

template <typename T>
[[nodiscard]] std::string DescribeProjectionLike(const T& value)
{
    std::ostringstream out;
    out << "{point=" << DescribePointLike(value.point)
        << ", parameter=" << FormatNumber(value.parameter)
        << ", distanceSquared=" << FormatNumber(value.distanceSquared)
        << ", isOnSegment=" << (value.isOnSegment ? "true" : "false")
        << '}';
    return out.str();
}
} // namespace detail

template <detail::HasXY Actual, detail::HasXY Expected>
inline void AssertPointNear(
    const Actual& actual,
    const Expected& expected,
    double eps,
    std::string_view actualExpr,
    std::string_view expectedExpr,
    std::string_view file,
    int line)
{
    if (detail::NearlyEqual(actual.x, expected.x, eps) &&
        detail::NearlyEqual(actual.y, expected.y, eps))
    {
        return;
    }

    detail::Fail(
        "point mismatch",
        file,
        line,
        std::string(actualExpr) + " = " + detail::DescribePointLike(actual) +
            ", " + std::string(expectedExpr) + " = " + detail::DescribePointLike(expected) +
            ", eps = " + detail::FormatNumber(eps));
}

template <detail::HasXY Actual, detail::HasXY Expected>
inline void AssertVectorNear(
    const Actual& actual,
    const Expected& expected,
    double eps,
    std::string_view actualExpr,
    std::string_view expectedExpr,
    std::string_view file,
    int line)
{
    AssertPointNear(actual, expected, eps, actualExpr, expectedExpr, file, line);
}

template <typename Actual, typename Expected>
inline void AssertNear(
    const Actual& actual,
    const Expected& expected,
    double eps,
    std::string_view actualExpr,
    std::string_view expectedExpr,
    std::string_view file,
    int line)
{
    if (detail::NearlyEqual(actual, expected, eps))
    {
        return;
    }

    detail::Fail(
        "value mismatch",
        file,
        line,
        std::string(actualExpr) + " = " + detail::FormatNumber(actual) +
            ", " + std::string(expectedExpr) + " = " + detail::FormatNumber(expected) +
            ", eps = " + detail::FormatNumber(eps));
}

template <typename Actual, typename Expected>
inline void AssertBoxNear(
    const Actual& actual,
    const Expected& expected,
    double eps,
    std::string_view actualExpr,
    std::string_view expectedExpr,
    std::string_view file,
    int line)
{
    if constexpr (detail::HasIsValid<Actual> && detail::HasIsValid<Expected>)
    {
        const bool actualValid = actual.IsValid();
        const bool expectedValid = expected.IsValid();
        if (actualValid != expectedValid)
        {
            detail::Fail(
                "box validity mismatch",
                file,
                line,
                std::string(actualExpr) + " = " + detail::DescribeBoxLike(actual) +
                    ", " + std::string(expectedExpr) + " = " + detail::DescribeBoxLike(expected));
        }

        if (!actualValid && !expectedValid)
        {
            return;
        }
    }

    AssertPointNear(
        detail::GetMinPointLike(actual),
        detail::GetMinPointLike(expected),
        eps,
        actualExpr,
        expectedExpr,
        file,
        line);
    AssertPointNear(
        detail::GetMaxPointLike(actual),
        detail::GetMaxPointLike(expected),
        eps,
        actualExpr,
        expectedExpr,
        file,
        line);
}

template <typename Actual, typename Expected>
inline void AssertProjectionNear(
    const Actual& actual,
    const Expected& expected,
    double eps,
    std::string_view actualExpr,
    std::string_view expectedExpr,
    std::string_view file,
    int line)
{
    AssertPointNear(
        actual.point,
        expected.point,
        eps,
        actualExpr,
        expectedExpr,
        file,
        line);
    AssertNear(
        actual.parameter,
        expected.parameter,
        eps,
        actualExpr,
        expectedExpr,
        file,
        line);
    AssertNear(
        actual.distanceSquared,
        expected.distanceSquared,
        eps,
        actualExpr,
        expectedExpr,
        file,
        line);

    if (actual.isOnSegment != expected.isOnSegment)
    {
        detail::Fail(
            "projection flag mismatch",
            file,
            line,
            std::string(actualExpr) + " = " + detail::DescribeProjectionLike(actual) +
                ", " + std::string(expectedExpr) + " = " + detail::DescribeProjectionLike(expected));
    }
}
} // namespace geometry::test

#define GEOMETRY_TEST_ASSERT_NEAR(actual, expected, eps) \
    ::geometry::test::AssertNear((actual), (expected), (eps), #actual, #expected, __FILE__, __LINE__)

#define GEOMETRY_TEST_ASSERT_POINT_NEAR(actual, expected, eps) \
    ::geometry::test::AssertPointNear((actual), (expected), (eps), #actual, #expected, __FILE__, __LINE__)

#define GEOMETRY_TEST_ASSERT_VECTOR_NEAR(actual, expected, eps) \
    ::geometry::test::AssertVectorNear((actual), (expected), (eps), #actual, #expected, __FILE__, __LINE__)

#define GEOMETRY_TEST_ASSERT_BOX_NEAR(actual, expected, eps) \
    ::geometry::test::AssertBoxNear((actual), (expected), (eps), #actual, #expected, __FILE__, __LINE__)

#define GEOMETRY_TEST_ASSERT_PROJECTION_NEAR(actual, expected, eps) \
    ::geometry::test::AssertProjectionNear((actual), (expected), (eps), #actual, #expected, __FILE__, __LINE__)
