#pragma once

#include <string>
#include <string_view>

#include "Export/GeometryExport.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Core/Algorithms.h"
#include "Geometry2d/Polygon2d.h"
#include "Geometry2d/Polyline2d.h"
#include "Core/GeometryTypes.h"

namespace Geometry::Serialize
{
[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Point2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Point2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Vector2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Vector2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Box2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Box2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::SegmentProjection2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::SegmentProjection2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::ArcSegment2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::ArcSegment2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Polyline2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Polyline2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Polygon2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Polygon2d& value);
} // namespace Geometry::Serialize

