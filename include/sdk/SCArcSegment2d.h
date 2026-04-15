#pragma once

#include <memory>
#include <string>

#include "export/GeometryExport.h"
#include "sdk/SCSegment2d.h"
#include "sdk/GeometryTypes.h"
#include "types/ISCSegment2.h"

namespace Geometry::Sdk
{
using Geometry::ISCArcDirection;

class GEOMETRY_API SCArcSegment2d : public SCSegment2d
{
public:
    SCPoint2d center{};
    double radius{0.0};
    double startAngle{0.0};
    double sweepAngle{0.0};

    SCArcSegment2d() = default;
    SCArcSegment2d(
        const SCPoint2d& centerValue,
        double radiusValue,
        double startAngleValue,
        double sweepAngleValue);

    [[nodiscard]] static SCArcSegment2d FromCenterRadiusStartSweep(
        const SCPoint2d& centerValue,
        double radiusValue,
        double startAngleValue,
        double sweepAngleValue);

    [[nodiscard]] ISCSegmentKind2 Kind() const override;
    [[nodiscard]] bool IsValid() const override;
    [[nodiscard]] ISCArcDirection Direction() const;
    [[nodiscard]] double EndAngle() const;
    [[nodiscard]] SCPoint2d StartPoint() const override;
    [[nodiscard]] SCPoint2d EndPoint() const override;
    [[nodiscard]] double Length() const override;
    [[nodiscard]] SCBox2d Bounds() const override;
    [[nodiscard]] SCPoint2d PointAt(double parameter) const override;
    [[nodiscard]] SCPoint2d PointAtLength(double distanceFromStart, bool clampToSegment = false) const override;
    [[nodiscard]] bool AlmostEquals(const SCArcSegment2d& other, double eps = Geometry::kDefaultEpsilon) const;
    [[nodiscard]] std::string DebugString() const override;
    [[nodiscard]] std::unique_ptr<SCSegment2d> Clone() const override;

    [[nodiscard]] constexpr bool operator==(const SCArcSegment2d& other) const = default;
    [[nodiscard]] constexpr bool operator!=(const SCArcSegment2d& other) const = default;

private:
    [[nodiscard]] static double NormalizeAngle(double angle);
    [[nodiscard]] bool IsAngleOnArc(double candidateAngle) const;
    [[nodiscard]] SCPoint2d PointAtAngle(double angle) const;
};
} // namespace Geometry::Sdk
