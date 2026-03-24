#pragma once

#include "export/GeometryExport.h"
#include "sdk/ArcSegment2d.h"
#include "sdk/LineSegment2d.h"
#include "sdk/Polygon2d.h"
#include "sdk/Polyline2d.h"

namespace geometry::sdk
{
[[nodiscard]] GEOMETRY_API LineSegment2d Offset(const LineSegment2d& segment, double distance);
[[nodiscard]] GEOMETRY_API ArcSegment2d Offset(const ArcSegment2d& segment, double distance);
[[nodiscard]] GEOMETRY_API Polyline2d Offset(const Polyline2d& polyline, double distance);
[[nodiscard]] GEOMETRY_API Polygon2d Offset(const Polygon2d& polygon, double distance);
} // namespace geometry::sdk
