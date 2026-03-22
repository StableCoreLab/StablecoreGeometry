#pragma once

#include <cmath>
#include <sstream>
#include <string>

#include "common/Epsilon.h"
#include "export/GeometryExport.h"
#include "types/Box2.h"
#include "types/Point2.h"
#include "types/Vector2.h"

namespace geometry::sdk
{
using geometry::Box2d;
using geometry::Point2d;
using geometry::Vector2d;

struct GEOMETRY_API SegmentProjection2d
{
    Point2d point{};
    double parameter{0.0};
    double distanceSquared{0.0};
    bool isOnSegment{false};

    [[nodiscard]] bool IsValid() const
    {
        return point.IsValid() && std::isfinite(parameter) && std::isfinite(distanceSquared) &&
               distanceSquared >= 0.0;
    }

    [[nodiscard]] bool AlmostEquals(
        const SegmentProjection2d& other,
        double eps = geometry::kDefaultEpsilon) const
    {
        return point.AlmostEquals(other.point, eps) &&
               std::abs(parameter - other.parameter) <= eps &&
               std::abs(distanceSquared - other.distanceSquared) <= eps &&
               isOnSegment == other.isOnSegment;
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "SegmentProjection2d{point=" << point.DebugString()
               << ", parameter=" << parameter
               << ", distanceSquared=" << distanceSquared
               << ", isOnSegment=" << (isOnSegment ? "true" : "false") << "}";
        return stream.str();
    }
};

}
