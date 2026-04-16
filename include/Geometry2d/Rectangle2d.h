#pragma once

#include <array>
#include <string>

#include "Export/GeometryExport.h"
#include "Core/GeometryTypes.h"
#include "Geometry2d/Polygon2d.h"

namespace Geometry
{
class GEOMETRY_API Rectangle2d
{
public:
    Point2d center{};
    double width{0.0};
    double height{0.0};
    double angleRadians{0.0};

    Rectangle2d() = default;
    Rectangle2d(const Point2d& center, double width, double height, double angleRadians = 0.0);

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] double Area() const;
    [[nodiscard]] double Perimeter() const;
    [[nodiscard]] std::array<Point2d, 4> Corners() const;
    [[nodiscard]] Box2d Bounds() const;
    [[nodiscard]] Polygon2d ToPolygon() const;
    [[nodiscard]] std::string DebugString() const;
};
} // namespace Geometry

