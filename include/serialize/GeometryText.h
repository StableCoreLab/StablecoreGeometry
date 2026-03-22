#pragma once

#include <string>
#include <string_view>

#include "export/GeometryExport.h"
#include "sdk/ArcSegment2d.h"
#include "sdk/GeometryAlgorithms.h"
#include "sdk/Polygon2d.h"
#include "sdk/Polyline2d.h"
#include "sdk/GeometryTypes.h"

namespace geometry::serialize
{
[[nodiscard]] GEOMETRY_API std::string ToText(const sdk::Point2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, sdk::Point2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const sdk::Vector2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, sdk::Vector2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const sdk::Box2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, sdk::Box2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const sdk::SegmentProjection2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, sdk::SegmentProjection2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const sdk::ArcSegment2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, sdk::ArcSegment2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const sdk::Polyline2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, sdk::Polyline2d& value);

[[nodiscard]] GEOMETRY_API std::string ToText(const sdk::Polygon2d& value);
[[nodiscard]] GEOMETRY_API bool FromText(std::string_view text, sdk::Polygon2d& value);
} // namespace geometry::serialize
