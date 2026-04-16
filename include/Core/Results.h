#pragma once

#include <array>
#include <cstddef>
#include <cmath>
#include <memory>
#include <span>
#include <string>

#include "Support/Epsilon.h"
#include "Export/GeometryExport.h"
#include "Core/GeometryTypes.h"
#include "Geometry2d/Segment2d.h"

namespace Geometry::Sdk
{
enum class RingOrientation2d
{
    Unknown,
    Clockwise,
    CounterClockwise
};

enum class IntersectionKind2d
{
    None,
    Point,
    Overlap,
    Tangent
};

struct GEOMETRY_API IntersectionPoint2d
{
    Point2d point{};
    double parameterOnFirst{0.0};
    double parameterOnSecond{0.0};

    [[nodiscard]] bool IsValid() const
    {
        return point.IsValid();
    }

    [[nodiscard]] bool AlmostEquals(
        const IntersectionPoint2d& other,
        double eps = Geometry::kDefaultEpsilon) const
    {
        return point.AlmostEquals(other.point, eps) &&
               std::abs(parameterOnFirst - other.parameterOnFirst) <= eps &&
               std::abs(parameterOnSecond - other.parameterOnSecond) <= eps;
    }
};

struct GEOMETRY_API SegmentIntersection2d
{
    IntersectionKind2d kind{IntersectionKind2d::None};
    std::array<IntersectionPoint2d, 2> points{};
    std::size_t pointCount{0};

    [[nodiscard]] bool HasIntersection() const
    {
        return kind != IntersectionKind2d::None;
    }
};

struct GEOMETRY_API ClosestPoints2d
{
    Point2d firstPoint{};
    Point2d secondPoint{};
    double parameterOnFirst{0.0};
    double parameterOnSecond{0.0};
    double distanceSquared{0.0};

    [[nodiscard]] bool IsValid() const
    {
        return firstPoint.IsValid() && secondPoint.IsValid() && distanceSquared >= 0.0;
    }
};

struct GEOMETRY_API AxisSample2d
{
    Point2d point{};
    Vector2d tangent{};
    Vector2d normal{};
    double parameter{0.0};

    [[nodiscard]] bool IsValid() const
    {
        return point.IsValid() && tangent.IsValid() && normal.IsValid();
    }
};

struct GEOMETRY_API AxisProjection2d
{
    SegmentProjection2d projection{};
    Vector2d tangent{};
    Vector2d normal{};

    [[nodiscard]] bool IsValid() const
    {
        return projection.IsValid() && tangent.IsValid() && normal.IsValid();
    }
};

struct GEOMETRY_API SegmentTrim2d
{
    bool success{false};
    std::unique_ptr<Segment2d> segment{};

    SegmentTrim2d() = default;
    SegmentTrim2d(SegmentTrim2d&&) noexcept = default;
    SegmentTrim2d& operator=(SegmentTrim2d&&) noexcept = default;
    SegmentTrim2d(const SegmentTrim2d&) = delete;
    SegmentTrim2d& operator=(const SegmentTrim2d&) = delete;
};

struct GEOMETRY_API SegmentSplit2d
{
    bool success{false};
    std::unique_ptr<Segment2d> first{};
    std::unique_ptr<Segment2d> second{};

    SegmentSplit2d() = default;
    SegmentSplit2d(SegmentSplit2d&&) noexcept = default;
    SegmentSplit2d& operator=(SegmentSplit2d&&) noexcept = default;
    SegmentSplit2d(const SegmentSplit2d&) = delete;
    SegmentSplit2d& operator=(const SegmentSplit2d&) = delete;
};

struct GEOMETRY_API SnapResult2d
{
    bool snapped{false};
    Point2d point{};
    double distanceSquared{0.0};
    std::size_t segmentIndex{0};
    double parameter{0.0};

    [[nodiscard]] bool IsValid() const
    {
        return !snapped || point.IsValid();
    }
};
} // namespace Geometry::Sdk
