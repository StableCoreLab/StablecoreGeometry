#include <gtest/gtest.h>

#include "sdk/GeometryTopology.h"

using geometry::sdk::Point2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;

TEST(RelationGapTest, RicherRelationHierarchyRemainsOpen)
{
    const Polygon2d first(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{8.0, 0.0}, Point2d{8.0, 6.0}, Point2d{0.0, 6.0}},
            PolylineClosure::Closed));
    const Polygon2d second(
        Polyline2d(
            {Point2d{2.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 6.0}, Point2d{2.0, 6.0}},
            PolylineClosure::Closed));
    (void)first;
    (void)second;
    GTEST_SKIP() << "Known gap: relation hierarchy still needs deeper Delphi-like aggregation beyond current equal/touching/intersecting/contains states.";
}
