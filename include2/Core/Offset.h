#pragma once

#include "Export/GeometryExport.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/MultiPolygon2d.h"
#include "Geometry2d/MultiPolyline2d.h"
#include "Geometry2d/Polygon2d.h"
#include "Geometry2d/Polyline2d.h"

namespace Geometry
{
    struct GEOMETRY_API OffsetOptions2d
    {
        double miterLimit{ 4.0 };
    };

    [[nodiscard]] GEOMETRY_API LineSegment2d Offset( const LineSegment2d &segment, double distance );
    [[nodiscard]] GEOMETRY_API ArcSegment2d Offset( const ArcSegment2d &segment, double distance );
    [[nodiscard]] GEOMETRY_API Polyline2d Offset( const Polyline2d &polyline, double distance,
                                                  OffsetOptions2d options = {} );
    [[nodiscard]] GEOMETRY_API Polygon2d Offset( const Polygon2d &polygon, double distance,
                                                 OffsetOptions2d options = {} );
    [[nodiscard]] GEOMETRY_API MultiPolygon2d OffsetToMultiPolygon( const Polygon2d &polygon,
                                                                    double distance,
                                                                    OffsetOptions2d options = {} );
    [[nodiscard]] GEOMETRY_API MultiPolyline2d Offset( const MultiPolyline2d &polylines, double distance,
                                                       OffsetOptions2d options = {} );
    [[nodiscard]] GEOMETRY_API MultiPolygon2d Offset( const MultiPolygon2d &polygons, double distance,
                                                      OffsetOptions2d options = {} );
}  // namespace Geometry
