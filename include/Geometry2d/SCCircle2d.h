#pragma once

#include <string>

#include "Export/GeometryExport.h"
#include "Core/GeometryTypes.h"
#include "Geometry2d/ISCPolygon2d.h"

namespace Geometry::Sdk
{
class GEOMETRY_API SCCircle2d
{
public:
    SCPoint2d center{};
    double radius{0.0};

    SCCircle2d() = default;
    SCCircle2d(const SCPoint2d& centerValue, double radiusValue);

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] double Area() const;
    [[nodiscard]] double Perimeter() const;
    [[nodiscard]] SCBox2d Bounds() const;
    [[nodiscard]] SCPoint2d PointAtAngle(double angleRadians) const;
    [[nodiscard]] ISCPolygon2d ToPolygon(std::size_t segmentCount = 64) const;
    [[nodiscard]] std::string DebugString() const;
};
} // namespace Geometry::Sdk
