#include <gtest/gtest.h>
#include <vector>

#include "sdk/Boolean.h"
#include "sdk/Relation.h"
#include "sdk/ShapeOps.h"
#include "sdk/Validation.h"
#include "support/GeometryTestSupport.h"

using Geometry::Sdk::LineSegment2d;
using Geometry::Sdk::LocatePoint;
using Geometry::Sdk::Point2d;
using Geometry::Sdk::PointContainment2d;
using Geometry::Sdk::Polygon2d;
using Geometry::Sdk::Polyline2d;
using Geometry::Sdk::PolylineClosure;

namespace
{
double TotalArea(const Geometry::Sdk::MultiPolygon2d& polygons)
{
    double total = 0.0;
    for (std::size_t i = 0; i < polygons.Count(); ++i)
    {
        total += polygons[i].Area();
    }
    return total;
}
}

TEST(RelationBooleanTest, CoversCurrentCapabilities)
{
    const LineSegment2d horizontal(Point2d{0.0, 0.0}, Point2d{4.0, 0.0});
    ASSERT_EQ(LocatePoint(Point2d{2.0, 0.0}, horizontal), PointContainment2d::OnBoundary);
    ASSERT_EQ(LocatePoint(Point2d{2.0, 1.0}, horizontal), PointContainment2d::Outside);

    const Polyline2d squareRing(
        {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
        PolylineClosure::Closed);
    const Polygon2d square(squareRing);
    ASSERT_EQ(LocatePoint(Point2d{2.0, 2.0}, square), PointContainment2d::Inside);
    ASSERT_EQ(LocatePoint(Point2d{5.0, 2.0}, square), PointContainment2d::Outside);
    ASSERT_TRUE(Geometry::Sdk::Validate(square).valid);

    const Polyline2d clipRing(
        {Point2d{2.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 4.0}, Point2d{2.0, 4.0}},
        PolylineClosure::Closed);
    const Polygon2d clip(clipRing);
    const auto intersection = Geometry::Sdk::Intersect(square, clip);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(intersection), 8.0, 1e-9);

    const auto united = Geometry::Sdk::Union(square, clip);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(united), 24.0, 1e-9);

    const Polyline2d innerRing(
        {Point2d{1.0, 1.0}, Point2d{3.0, 1.0}, Point2d{3.0, 3.0}, Point2d{1.0, 3.0}},
        PolylineClosure::Closed);
    const Polygon2d inner(innerRing);
    const auto difference = Geometry::Sdk::Difference(square, inner);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(difference), 12.0, 1e-9);

    const Polygon2d horizontalBar(
        Polyline2d(
            {Point2d{0.0, 1.0}, Point2d{4.0, 1.0}, Point2d{4.0, 3.0}, Point2d{0.0, 3.0}},
            PolylineClosure::Closed));
    const Polygon2d verticalBar(
        Polyline2d(
            {Point2d{1.0, 0.0}, Point2d{3.0, 0.0}, Point2d{3.0, 4.0}, Point2d{1.0, 4.0}},
            PolylineClosure::Closed));

    const auto crossUnion = Geometry::Sdk::Union(horizontalBar, verticalBar);
    ASSERT_GE(crossUnion.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(crossUnion), 12.0, 1e-9);

    const auto crossDifference = Geometry::Sdk::Difference(horizontalBar, verticalBar);
    ASSERT_GE(crossDifference.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(crossDifference), 4.0, 1e-9);

    const Polygon2d outer(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 6.0}, Point2d{0.0, 6.0}},
            PolylineClosure::Closed));
    const Polygon2d nested(
        Polyline2d(
            {Point2d{2.0, 2.0}, Point2d{4.0, 2.0}, Point2d{4.0, 4.0}, Point2d{2.0, 4.0}},
            PolylineClosure::Closed));

    const auto containedUnion = Geometry::Sdk::Union(outer, nested);
    ASSERT_GE(containedUnion.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(containedUnion), 36.0, 1e-9);

    const auto containedIntersection = Geometry::Sdk::Intersect(outer, nested);
    ASSERT_GE(containedIntersection.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(containedIntersection), 4.0, 1e-9);

    const auto containedDifference = Geometry::Sdk::Difference(outer, nested);
    ASSERT_GE(containedDifference.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(containedDifference), 32.0, 1e-9);

    const Polygon2d overlapStripA(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 2.0}, Point2d{0.0, 2.0}},
            PolylineClosure::Closed));
    const Polygon2d overlapStripB(
        Polyline2d(
            {Point2d{2.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 2.0}, Point2d{2.0, 2.0}},
            PolylineClosure::Closed));
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Intersect(overlapStripA, overlapStripB)), 4.0, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Union(overlapStripA, overlapStripB)), 12.0, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Difference(overlapStripA, overlapStripB)), 4.0, 1e-9);

    const Polygon2d overlapFamilyA(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{8.0, 0.0}, Point2d{8.0, 2.0}, Point2d{5.0, 2.0}, Point2d{5.0, 4.0}, Point2d{0.0, 4.0}},
            PolylineClosure::Closed));
    const Polygon2d overlapFamilyB(
        Polyline2d(
            {Point2d{3.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 3.0}, Point2d{6.0, 3.0}, Point2d{6.0, 5.0}, Point2d{3.0, 5.0}},
            PolylineClosure::Closed));
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Intersect(overlapFamilyA, overlapFamilyB)), 14.0, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Union(overlapFamilyA, overlapFamilyB)), 39.0, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Difference(overlapFamilyA, overlapFamilyB)), 12.0, 1e-9);

    const Polygon2d nearDegenerateOverlapA(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{8.0, 0.0}, Point2d{8.0, 2.0}, Point2d{5.0, 2.0}, Point2d{5.0, 2.000001}, Point2d{0.0, 2.000001}},
            PolylineClosure::Closed));
    const Polygon2d nearDegenerateOverlapB(
        Polyline2d(
            {Point2d{3.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 2.000001}, Point2d{6.0, 2.000001}, Point2d{6.0, 4.0}, Point2d{3.0, 4.0}},
            PolylineClosure::Closed));
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Intersect(nearDegenerateOverlapA, nearDegenerateOverlapB)), 10.000002, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Union(nearDegenerateOverlapA, nearDegenerateOverlapB)), 26.000007, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Difference(nearDegenerateOverlapA, nearDegenerateOverlapB)), 6.000003, 1e-9);

    const Polygon2d ultraThinOverlapA(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{8.0, 0.0}, Point2d{8.0, 2.0}, Point2d{5.0, 2.0}, Point2d{5.0, 2.00000001}, Point2d{0.0, 2.00000001}},
            PolylineClosure::Closed));
    const Polygon2d ultraThinOverlapB(
        Polyline2d(
            {Point2d{3.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 2.00000001}, Point2d{6.0, 2.00000001}, Point2d{6.0, 4.0}, Point2d{3.0, 4.0}},
            PolylineClosure::Closed));
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Intersect(ultraThinOverlapA, ultraThinOverlapB)), 10.00000002, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Union(ultraThinOverlapA, ultraThinOverlapB)), 26.00000007, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Difference(ultraThinOverlapA, ultraThinOverlapB)), 6.00000003, 1e-9);

    const auto equalIntersection = Geometry::Sdk::Intersect(square, square);
    ASSERT_GE(equalIntersection.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(equalIntersection), 16.0, 1e-9);

    const auto equalDifference = Geometry::Sdk::Difference(square, square);
    ASSERT_EQ(equalDifference.Count(), 0);

    const Polygon2d disjointOther(
        Polyline2d(
            {Point2d{10.0, 0.0}, Point2d{12.0, 0.0}, Point2d{12.0, 2.0}, Point2d{10.0, 2.0}},
            PolylineClosure::Closed));
    const auto disjointUnion = Geometry::Sdk::Union(square, disjointOther);
    ASSERT_GE(disjointUnion.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(disjointUnion), 20.0, 1e-9);

    const Polygon2d edgeTouch(
        Polyline2d(
            {Point2d{4.0, 1.0}, Point2d{6.0, 1.0}, Point2d{6.0, 3.0}, Point2d{4.0, 3.0}},
            PolylineClosure::Closed));
    const auto touchingDifference = Geometry::Sdk::Difference(square, edgeTouch);
    ASSERT_GE(touchingDifference.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(touchingDifference), 16.0, 1e-9);

    const Polygon2d duplicateEdgeFamilyA(
        Polyline2d(
            {Point2d{0.0, 0.0},
             Point2d{6.0, 0.0},
             Point2d{6.0, 1.0},
             Point2d{4.0, 1.0},
             Point2d{4.0, 1.000000001},
             Point2d{6.0, 1.000000001},
             Point2d{6.0, 4.0},
             Point2d{0.0, 4.0}},
            PolylineClosure::Closed));
    const Polygon2d duplicateEdgeFamilyB(
        Polyline2d(
            {Point2d{2.0, -1.0}, Point2d{8.0, -1.0}, Point2d{8.0, 3.0}, Point2d{2.0, 3.0}},
            PolylineClosure::Closed));
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Intersect(duplicateEdgeFamilyA, duplicateEdgeFamilyB)),
        12.0,
        1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Union(duplicateEdgeFamilyA, duplicateEdgeFamilyB)),
        36.0,
        1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Difference(duplicateEdgeFamilyA, duplicateEdgeFamilyB)),
        12.0,
        1e-8);

    const Polygon2d repeatedCollinearChainA(
        Polyline2d(
            {Point2d{0.0, 0.0},
             Point2d{2.0, 0.0},
             Point2d{4.0, 0.0},
             Point2d{6.0, 0.0},
             Point2d{6.0, 3.0},
             Point2d{4.0, 3.0},
             Point2d{2.0, 3.0},
             Point2d{0.0, 3.0}},
            PolylineClosure::Closed));
    const Polygon2d repeatedCollinearChainB(
        Polyline2d(
            {Point2d{3.0, -1.0},
             Point2d{7.0, -1.0},
             Point2d{7.0, 2.0},
             Point2d{5.0, 2.0},
             Point2d{5.0, 2.000000001},
             Point2d{3.0, 2.000000001}},
            PolylineClosure::Closed));
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Intersect(repeatedCollinearChainA, repeatedCollinearChainB)),
        6.000000003,
        1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Union(repeatedCollinearChainA, repeatedCollinearChainB)),
        24.0,
        1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Difference(repeatedCollinearChainA, repeatedCollinearChainB)),
        11.999999997,
        1e-8);
}

TEST(RelationBooleanTest, HandlesHigherDegreeRepeatedCollinearFamilyOverlap)
{
    constexpr double delta = 1e-9;

    const Polygon2d repeatedFamilyA(
        Polyline2d(
            {Point2d{0.0, 0.0},
             Point2d{6.0, 0.0},
             Point2d{6.0, 4.0},
             Point2d{5.0, 4.0},
             Point2d{5.0, 4.0 - delta},
             Point2d{4.0, 4.0 - delta},
             Point2d{4.0, 4.0},
             Point2d{3.0, 4.0},
             Point2d{3.0, 4.0 - delta},
             Point2d{2.0, 4.0 - delta},
             Point2d{2.0, 4.0},
             Point2d{0.0, 4.0}},
            PolylineClosure::Closed));
    const Polygon2d repeatedFamilyB(
        Polyline2d(
            {Point2d{3.0, -1.0}, Point2d{5.0, -1.0}, Point2d{5.0, 5.0}, Point2d{3.0, 5.0}},
            PolylineClosure::Closed));

    ASSERT_TRUE(Geometry::Sdk::Validate(repeatedFamilyA).valid);
    ASSERT_TRUE(Geometry::Sdk::Validate(repeatedFamilyB).valid);

    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Intersect(repeatedFamilyA, repeatedFamilyB)),
        8.0 - delta,
        1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Union(repeatedFamilyA, repeatedFamilyB)),
        28.0 - delta,
        1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea(Geometry::Sdk::Difference(repeatedFamilyA, repeatedFamilyB)),
        16.0 - delta,
        1e-8);
}

TEST(RelationBooleanTest, HandlesNearDegenerateIntersectionClusters)
{
    constexpr double delta = 1e-8;

    const Polygon2d clusterA(
        Polyline2d(
            {Point2d{0.0, 0.0},
             Point2d{8.0, 0.0},
             Point2d{8.0, 4.0 - delta},
             Point2d{8.0, 4.0 + delta},
             Point2d{8.0, 8.0},
             Point2d{0.0, 8.0}},
            PolylineClosure::Closed));
    const Polygon2d clusterB(
        Polyline2d(
            {Point2d{3.0, -1.0},
             Point2d{5.0, -1.0},
             Point2d{5.0, 4.0 - delta},
             Point2d{5.0, 4.0 + delta},
             Point2d{5.0, 9.0},
             Point2d{3.0, 9.0}},
            PolylineClosure::Closed));

    ASSERT_TRUE(Geometry::Sdk::Validate(clusterA).valid);
    ASSERT_TRUE(Geometry::Sdk::Validate(clusterB).valid);

    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Intersect(clusterA, clusterB)), 16.0, 1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Union(clusterA, clusterB)), 68.0, 1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Difference(clusterA, clusterB)), 48.0, 1e-8);
}

TEST(RelationBooleanTest, HandlesBelowToleranceArrangementDegeneracies)
{
    constexpr double delta = 1e-12;

    const Polygon2d first(
        Polyline2d(
            {Point2d{0.0, 0.0},
             Point2d{8.0, 0.0},
             Point2d{8.0, 2.0},
             Point2d{5.0, 2.0},
             Point2d{5.0, 2.0 + delta},
             Point2d{0.0, 2.0 + delta}},
            PolylineClosure::Closed));
    const Polygon2d second(
        Polyline2d(
            {Point2d{3.0, 0.0},
             Point2d{10.0, 0.0},
             Point2d{10.0, 2.0 + delta},
             Point2d{6.0, 2.0 + delta},
             Point2d{6.0, 4.0},
             Point2d{3.0, 4.0}},
            PolylineClosure::Closed));

    ASSERT_GE(Geometry::Sdk::Intersect(first, second).Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Intersect(first, second)), 10.0, 1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Union(first, second)), 26.0, 1e-8);
    GEOMETRY_TEST_ASSERT_NEAR(TotalArea(Geometry::Sdk::Difference(first, second)), 6.0, 1e-8);
}




