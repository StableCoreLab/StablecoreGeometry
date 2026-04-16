#pragma once

#include <string>

#include "Export/GeometryExport.h"
#include "Core/GeometryTypes.h"
#include "Geometry2d/Polygon2d.h"

namespace Geometry::Sdk
{
class GEOMETRY_API Circle2d
{
public:
    Point2d center{};
    double radius{0.0};

    Circle2d() = default;
    Circle2d(const Point2d& center, double radius);

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] double Area() const;
    [[nodiscard]] double Perimeter() const;
    [[nodiscard]] Box2d Bounds() const;
    [[nodiscard]] Point2d PointAtAngle(double angleRadians) const;
    [[nodiscard]] Polygon2d ToPolygon(std::size_t segmentCount = 64) const;
    [[nodiscard]] std::string DebugString() const;
};
} // namespace Geometry::Sdk
