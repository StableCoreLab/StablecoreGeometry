#pragma once

#include "Export/GeometryExport.h"
#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/MultiPolygon2d.h"
#include "Geometry2d/MultiPolyline2d.h"
#include "Geometry2d/Polygon2d.h"
#include "Geometry2d/Polyline2d.h"
#include "Support/Epsilon.h"

namespace Geometry
{
    struct GEOMETRY_API PolygonCutResult2d
    {
        bool success{ false };
        MultiPolygon2d left{};
        MultiPolygon2d right{};
    };

    [[nodiscard]] GEOMETRY_API Polyline2d SubPolyline( const Polyline2d &polyline, double startLength,
                                                       double endLength );

    [[nodiscard]] GEOMETRY_API PolygonCutResult2d CutPolygon( const Polygon2d &polygon,
                                                              const LineSegment2d &cutter,
                                                              double eps = Geometry::kDefaultEpsilon );

    [[nodiscard]] GEOMETRY_API MultiPolygon2d BuildMultiPolygonByLines( const MultiPolyline2d &polylines,
                                                                        double eps = Geometry::kDefaultEpsilon );

    [[nodiscard]] GEOMETRY_API Polygon2d NormalizePolygonByLines( const Polygon2d &polygon,
                                                                  double eps = Geometry::kDefaultEpsilon );
}  // namespace Geometry
