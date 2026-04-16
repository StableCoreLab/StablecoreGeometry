#pragma once

#include <memory>
#include <string>

#include "Export/GeometryExport.h"
#include "Core/GeometryTypes.h"
#include "Geometry2d/SCSegment2d.h"

namespace Geometry::Sdk
{
class GEOMETRY_API SCLineSegment2d : public SCSegment2d
{
public:
    SCPoint2d startPoint{};
    SCPoint2d endPoint{};

    SCLineSegment2d() = default;
    SCLineSegment2d(const SCPoint2d& startPointValue, const SCPoint2d& endPointValue);

    [[nodiscard]] static SCLineSegment2d FromEndpoints(
        const SCPoint2d& startPointValue,
        const SCPoint2d& endPointValue);

    [[nodiscard]] ISCSegmentKind2 Kind() const override;
    [[nodiscard]] bool IsValid() const override;
    [[nodiscard]] SCPoint2d StartPoint() const override;
    [[nodiscard]] SCPoint2d EndPoint() const override;
    [[nodiscard]] double Length() const override;
    [[nodiscard]] SCBox2d Bounds() const override;
    [[nodiscard]] SCPoint2d PointAt(double parameter) const override;
    [[nodiscard]] SCPoint2d PointAtLength(double distanceFromStart, bool clampToSegment = false) const override;
    [[nodiscard]] bool AlmostEquals(const SCLineSegment2d& other, double eps = Geometry::kDefaultEpsilon) const;
    [[nodiscard]] std::string DebugString() const override;
    [[nodiscard]] std::unique_ptr<SCSegment2d> Clone() const override;

    [[nodiscard]] constexpr bool operator==(const SCLineSegment2d& other) const = default;
    [[nodiscard]] constexpr bool operator!=(const SCLineSegment2d& other) const = default;
};
} // namespace Geometry::Sdk
