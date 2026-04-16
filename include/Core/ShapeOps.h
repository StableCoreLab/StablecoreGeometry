#pragma once

#include <string>

#include "Core/Results.h"
#include "Export/GeometryExport.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/Polygon2d.h"
#include "Geometry2d/Polyline2d.h"
#include "Geometry2d/Segment2d.h"

namespace Geometry
{
    [[nodiscard]] GEOMETRY_API double Perimeter( const Polygon2d &polygon );
    [[nodiscard]] GEOMETRY_API RingOrientation2d Orientation( const Polyline2d &ring );
    [[nodiscard]] GEOMETRY_API bool IsClockwise( const Polyline2d &ring );
    [[nodiscard]] GEOMETRY_API bool IsCounterClockwise( const Polyline2d &ring );
    [[nodiscard]] GEOMETRY_API Point2d Centroid( const Polygon2d &polygon );

    [[nodiscard]] GEOMETRY_API LineSegment2d Reverse( const LineSegment2d &segment );
    [[nodiscard]] GEOMETRY_API ArcSegment2d Reverse( const ArcSegment2d &segment );
    [[nodiscard]] GEOMETRY_API Polyline2d Reverse( const Polyline2d &polyline );
    [[nodiscard]] GEOMETRY_API Polyline2d Close( const Polyline2d &polyline );
}  // namespace Geometry
