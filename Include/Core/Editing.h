#pragma once

#include "Export/GeometryExport.h"
#include "Geometry2d/Polygon2d.h"
#include "Geometry2d/Polyline2d.h"
#include "Support/Epsilon.h"

namespace Geometry
{
    [[nodiscard]] GEOMETRY_API Polyline2d Normalize( const Polyline2d &polyline,
                                                     double eps = Geometry::kDefaultEpsilon );
    [[nodiscard]] GEOMETRY_API Polygon2d Normalize( const Polygon2d &polygon,
                                                    double eps = Geometry::kDefaultEpsilon );
    [[nodiscard]] GEOMETRY_API Polyline2d InsertPoint( const Polyline2d &polyline, const Point2d &point,
                                                       double eps = Geometry::kDefaultEpsilon );
    [[nodiscard]] GEOMETRY_API Polygon2d InsertPoint( const Polygon2d &polygon, const Point2d &point,
                                                      double eps = Geometry::kDefaultEpsilon );
}  // namespace Geometry
