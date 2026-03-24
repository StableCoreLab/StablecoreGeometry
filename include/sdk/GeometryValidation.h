#pragma once

#include "export/GeometryExport.h"
#include "sdk/Polygon2d.h"
#include "sdk/Polyline2d.h"

namespace geometry::sdk
{
enum class PolygonValidationIssue2d
{
    None,
    TooFewPoints,
    NotClosed,
    InvalidOrientation,
    SelfIntersection
};

struct GEOMETRY_API PolygonValidation2d
{
    bool valid{false};
    PolygonValidationIssue2d issue{PolygonValidationIssue2d::None};
};

[[nodiscard]] GEOMETRY_API bool HasSelfIntersection(const Polyline2d& ring, double eps = 1e-9);
[[nodiscard]] GEOMETRY_API PolygonValidation2d Validate(const Polyline2d& ring, double eps = 1e-9);
[[nodiscard]] GEOMETRY_API PolygonValidation2d Validate(const Polygon2d& polygon, double eps = 1e-9);
} // namespace geometry::sdk
