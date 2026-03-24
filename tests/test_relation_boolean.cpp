#include <cassert>

#include "sdk/GeometryBoolean.h"
#include "sdk/GeometryRelation.h"
#include "sdk/GeometryShapeOps.h"
#include "sdk/GeometryValidation.h"
#include "support/GeometryTestSupport.h"

using geometry::sdk::LineSegment2d;
using geometry::sdk::LocatePoint;
using geometry::sdk::Point2d;
using geometry::sdk::PointContainment2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;

int main()
{
    const LineSegment2d horizontal(Point2d{0.0, 0.0}, Point2d{4.0, 0.0});
    assert(LocatePoint(Point2d{2.0, 0.0}, horizontal) == PointContainment2d::OnBoundary);
    assert(LocatePoint(Point2d{2.0, 1.0}, horizontal) == PointContainment2d::Outside);

    const Polyline2d squareRing(
        {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
        PolylineClosure::Closed);
    const Polygon2d square(squareRing);
    assert(LocatePoint(Point2d{2.0, 2.0}, square) == PointContainment2d::Inside);
    assert(LocatePoint(Point2d{5.0, 2.0}, square) == PointContainment2d::Outside);
    assert(geometry::sdk::Validate(square).valid);

    const Polyline2d clipRing(
        {Point2d{2.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 4.0}, Point2d{2.0, 4.0}},
        PolylineClosure::Closed);
    const Polygon2d clip(clipRing);
    const auto intersection = geometry::sdk::Intersect(square, clip);
    assert(intersection.Count() == 1);
    GEOMETRY_TEST_ASSERT_NEAR(geometry::sdk::Area(intersection[0]), 8.0, 1e-9);

    return 0;
}
