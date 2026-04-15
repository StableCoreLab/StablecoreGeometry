#pragma once

#include <string>
#include <string_view>

#include "export/GeometryExport.h"
#include "sdk/ArcSegment2d.h"
#include "sdk/Algorithms.h"
#include "sdk/Polygon2d.h"
#include "sdk/Polyline2d.h"
#include "sdk/GeometryTypes.h"

namespace Geometry::Serialize
{
[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Sdk::Point2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Sdk::Point2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Sdk::Vector2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Sdk::Vector2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Sdk::Box2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Sdk::Box2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Sdk::SegmentProjection2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Sdk::SegmentProjection2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Sdk::ArcSegment2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Sdk::ArcSegment2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Sdk::Polyline2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Sdk::Polyline2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const Geometry::Sdk::Polygon2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, Geometry::Sdk::Polygon2d& value);
} // namespace Geometry::Serialize
