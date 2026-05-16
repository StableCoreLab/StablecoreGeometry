#pragma once

#include "Export/GeometryExport.h"
#include "Geometry2d/MultiPolygon2d.h"
#include "Geometry2d/Polygon2d.h"
#include "Support/Epsilon.h"

namespace Geometry
{
    [[nodiscard]] GEOMETRY_API MultiPolygon2d Intersect( const Polygon2d &first, const Polygon2d &second,
                                                         double eps = Geometry::kDefaultEpsilon );
    [[nodiscard]] GEOMETRY_API MultiPolygon2d Union( const Polygon2d &first, const Polygon2d &second,
                                                     double eps = Geometry::kDefaultEpsilon );
    [[nodiscard]] GEOMETRY_API MultiPolygon2d Difference( const Polygon2d &first,
                                                          const Polygon2d &second,
                                                          double eps = Geometry::kDefaultEpsilon );
}  // namespace Geometry
