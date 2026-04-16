#pragma once
#include <string>
#include "Export/GeometryExport.h"
#include "Geometry2d/Polygon2d.h"

namespace Geometry
{
    class GEOMETRY_API Ellipse2d
    {
    public:
        Point2d center{};
        double majorRadius{ 0.0 };
        double minorRadius{ 0.0 };
        double angleRadians{ 0.0 };

        Ellipse2d() = default;
        Ellipse2d( const Point2d &center, double majorRadius, double minorRadius,
                   double angleRadians = 0.0 );

        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] double Area() const;
        [[nodiscard]] double Perimeter() const;
        [[nodiscard]] Point2d PointAtAngle( double angleRadians ) const;
        [[nodiscard]] Box2d Bounds( std::size_t segmentCount = 128 ) const;
        [[nodiscard]] Polygon2d ToPolygon( std::size_t segmentCount = 128 ) const;
        [[nodiscard]] std::string DebugString() const;
    };
}  // namespace Geometry
