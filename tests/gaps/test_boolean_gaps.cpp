#include <gtest/gtest.h>

#include "sdk/GeometryBoolean.h"
#include "sdk/GeometryShapeOps.h"

using geometry::sdk::Point2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;

TEST(BooleanGapTest, NearDegenerateRepeatedOverlapFamiliesRemainOpen)
{
    const Polygon2d first(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{8.0, 0.0}, Point2d{8.0, 2.0}, Point2d{5.0, 2.0}, Point2d{5.0, 2.000001}, Point2d{0.0, 2.000001}},
            PolylineClosure::Closed));
    const Polygon2d second(
        Polyline2d(
            {Point2d{3.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 2.000001}, Point2d{6.0, 2.000001}, Point2d{6.0, 4.0}, Point2d{3.0, 4.0}},
            PolylineClosure::Closed));
    (void)first;
    (void)second;
    GTEST_SKIP() << "Known gap: near-degenerate repeated-overlap families still need stronger boolean recovery.";
}

