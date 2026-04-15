#pragma once

#include <vector>

#include "export/GeometryExport.h"
#include "sdk/ArcSegment2d.h"
#include "sdk/LineSegment2d.h"
#include "sdk/Segment2d.h"

namespace Geometry::Sdk
{
[[nodiscard]] GEOMETRY_API std::vector<Point2d> Sample(
    const LineSegment2d& segment,
    std::size_t partCount);
[[nodiscard]] GEOMETRY_API std::vector<Point2d> Sample(
    const ArcSegment2d& segment,
    std::size_t partCount);
[[nodiscard]] GEOMETRY_API std::vector<Point2d> Sample(
    const Segment2d& segment,
    std::size_t partCount);
[[nodiscard]] GEOMETRY_API std::vector<Point2d> SampleByMaxAngle(
    const ArcSegment2d& segment,
    double maxAngleRadians);
} // namespace Geometry::Sdk
