#include <gtest/gtest.h>

#include "sdk/GeometryOffset.h"

using geometry::sdk::Point2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;

TEST(OffsetGapTest, ReverseEdgeAndSelfIntersectionRecoveryRemainOpen)
{
    const Polygon2d source(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 1.0}, Point2d{2.5, 1.0}, Point2d{2.5, 4.0}, Point2d{6.0, 4.0},
             Point2d{6.0, 5.0}, Point2d{0.0, 5.0}},
            PolylineClosure::Closed));
    (void)source;
    GTEST_SKIP() << "Known gap: reverse-edge, self-intersection, and multi-failure offset recovery are not closed yet.";
}
