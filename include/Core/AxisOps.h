#pragma once
#include <span>
#include "Core/Results.h"
#include "Export/GeometryExport.h"
#include "Geometry2d/Segment2d.h"

namespace Geometry
{
    [[nodiscard]] GEOMETRY_API AxisSample2d SampleAxis( const Segment2d &segment, double parameter );
    [[nodiscard]] GEOMETRY_API AxisSample2d SampleAxisAtLength( const Segment2d &segment, double length,
                                                                bool clampToSegment = true );
    [[nodiscard]] GEOMETRY_API AxisProjection2d ProjectPointToAxis( const Point2d &point,
                                                                    const Segment2d &segment );

    [[nodiscard]] GEOMETRY_API std::unique_ptr<Segment2d> Reverse( const Segment2d &segment );

    [[nodiscard]] GEOMETRY_API SegmentSplit2d SplitSegment( const Segment2d &segment, double parameter );
    [[nodiscard]] GEOMETRY_API SegmentTrim2d TrimSegment( const Segment2d &segment,
                                                          double startParameter, double endParameter );

    [[nodiscard]] GEOMETRY_API SnapResult2d SnapPointToSegments(
        const Point2d &point, std::span<const Segment2d *const> segments, double maxDistance );
}  // namespace Geometry
