#pragma once

#include <vector>

#include "Export/GeometryExport.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/Segment2d.h"
#include "Types/Geometry2d/Point2.h"

namespace Geometry
{
    [[nodiscard]] GEOMETRY_API std::vector<Point2d> Sample( const LineSegment2d &segment,
                                                            std::size_t partCount );
    [[nodiscard]] GEOMETRY_API std::vector<Point2d> Sample( const ArcSegment2d &segment,
                                                            std::size_t partCount );
    [[nodiscard]] GEOMETRY_API std::vector<Point2d> Sample( const Segment2d &segment,
                                                            std::size_t partCount );
    [[nodiscard]] GEOMETRY_API std::vector<Point2d> SampleByMaxAngle( const ArcSegment2d &segment,
                                                                      double maxAngleRadians );
}  // namespace Geometry
