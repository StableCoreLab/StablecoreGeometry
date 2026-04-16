#include <gtest/gtest.h>
#include <vector>

#include "Core/Boolean.h"
#include "Core/Relation.h"
#include "Core/ShapeOps.h"
#include "Core/Validation.h"
#include "Support/GeometryTestSupport.h"

using Geometry::Difference;
using Geometry::Intersect;
using Geometry::LineSegment2d;
using Geometry::LocatePoint;
using Geometry::MultiPolygon2d;
using Geometry::Point2d;
using Geometry::PointContainment2d;
using Geometry::Polygon2d;
using Geometry::Polyline2d;
using Geometry::PolylineClosure;
using Geometry::Union;
using Geometry::Validate;

namespace
{
    double TotalArea( const MultiPolygon2d &polygons )
    {
        double total = 0.0;
        for( std::size_t i = 0; i < polygons.Count(); ++i )
        {
            total += polygons[i].Area();
        }
        return total;
    }
}  // namespace

TEST( RelationBooleanTest, CoversCurrentCapabilities )
{
    const LineSegment2d horizontal( Point2d{ 0.0, 0.0 }, Point2d{ 4.0, 0.0 } );
    ASSERT_EQ( LocatePoint( Point2d{ 2.0, 0.0 }, horizontal ), PointContainment2d::OnBoundary );
    ASSERT_EQ( LocatePoint( Point2d{ 2.0, 1.0 }, horizontal ), PointContainment2d::Outside );

    const Polyline2d squareRing(
        { Point2d{ 0.0, 0.0 }, Point2d{ 4.0, 0.0 }, Point2d{ 4.0, 4.0 }, Point2d{ 0.0, 4.0 } },
        PolylineClosure::Closed );
    const Polygon2d square( squareRing );
    ASSERT_EQ( LocatePoint( Point2d{ 2.0, 2.0 }, square ), PointContainment2d::Inside );
    ASSERT_EQ( LocatePoint( Point2d{ 5.0, 2.0 }, square ), PointContainment2d::Outside );
    ASSERT_TRUE( Validate( square ).valid );

    const Polyline2d clipRing(
        { Point2d{ 2.0, 0.0 }, Point2d{ 6.0, 0.0 }, Point2d{ 6.0, 4.0 }, Point2d{ 2.0, 4.0 } },
        PolylineClosure::Closed );
    const Polygon2d clip( clipRing );
    const auto intersection = Intersect( square, clip );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( intersection ), 8.0, 1e-9 );

    const auto united = Union( square, clip );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( united ), 24.0, 1e-9 );

    const Polyline2d innerRing(
        { Point2d{ 1.0, 1.0 }, Point2d{ 3.0, 1.0 }, Point2d{ 3.0, 3.0 }, Point2d{ 1.0, 3.0 } },
        PolylineClosure::Closed );
    const Polygon2d inner( innerRing );
    const auto difference = Difference( square, inner );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( difference ), 12.0, 1e-9 );

    const Polygon2d horizontalBar( Polyline2d(
        { Point2d{ 0.0, 1.0 }, Point2d{ 4.0, 1.0 }, Point2d{ 4.0, 3.0 }, Point2d{ 0.0, 3.0 } },
        PolylineClosure::Closed ) );
    const Polygon2d verticalBar( Polyline2d(
        { Point2d{ 1.0, 0.0 }, Point2d{ 3.0, 0.0 }, Point2d{ 3.0, 4.0 }, Point2d{ 1.0, 4.0 } },
        PolylineClosure::Closed ) );

    const auto crossUnion = Union( horizontalBar, verticalBar );
    ASSERT_GE( crossUnion.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( crossUnion ), 12.0, 1e-9 );

    const auto crossDifference = Difference( horizontalBar, verticalBar );
    ASSERT_GE( crossDifference.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( crossDifference ), 4.0, 1e-9 );

    const Polygon2d outer( Polyline2d(
        { Point2d{ 0.0, 0.0 }, Point2d{ 6.0, 0.0 }, Point2d{ 6.0, 6.0 }, Point2d{ 0.0, 6.0 } },
        PolylineClosure::Closed ) );
    const Polygon2d nested( Polyline2d(
        { Point2d{ 2.0, 2.0 }, Point2d{ 4.0, 2.0 }, Point2d{ 4.0, 4.0 }, Point2d{ 2.0, 4.0 } },
        PolylineClosure::Closed ) );

    const auto containedUnion = Union( outer, nested );
    ASSERT_GE( containedUnion.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( containedUnion ), 36.0, 1e-9 );

    const auto containedIntersection = Intersect( outer, nested );
    ASSERT_GE( containedIntersection.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( containedIntersection ), 4.0, 1e-9 );

    const auto containedDifference = Difference( outer, nested );
    ASSERT_GE( containedDifference.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( containedDifference ), 32.0, 1e-9 );

    const Polygon2d overlapStripA( Polyline2d(
        { Point2d{ 0.0, 0.0 }, Point2d{ 4.0, 0.0 }, Point2d{ 4.0, 2.0 }, Point2d{ 0.0, 2.0 } },
        PolylineClosure::Closed ) );
    const Polygon2d overlapStripB( Polyline2d(
        { Point2d{ 2.0, 0.0 }, Point2d{ 6.0, 0.0 }, Point2d{ 6.0, 2.0 }, Point2d{ 2.0, 2.0 } },
        PolylineClosure::Closed ) );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( overlapStripA, overlapStripB ) ), 4.0, 1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( overlapStripA, overlapStripB ) ), 12.0, 1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( overlapStripA, overlapStripB ) ), 4.0, 1e-9 );

    const Polygon2d overlapFamilyA(
        Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 8.0, 0.0 }, Point2d{ 8.0, 2.0 }, Point2d{ 5.0, 2.0 },
                      Point2d{ 5.0, 4.0 }, Point2d{ 0.0, 4.0 } },
                    PolylineClosure::Closed ) );
    const Polygon2d overlapFamilyB(
        Polyline2d( { Point2d{ 3.0, 0.0 }, Point2d{ 10.0, 0.0 }, Point2d{ 10.0, 3.0 },
                      Point2d{ 6.0, 3.0 }, Point2d{ 6.0, 5.0 }, Point2d{ 3.0, 5.0 } },
                    PolylineClosure::Closed ) );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( overlapFamilyA, overlapFamilyB ) ), 14.0, 1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( overlapFamilyA, overlapFamilyB ) ), 39.0, 1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( overlapFamilyA, overlapFamilyB ) ), 12.0, 1e-9 );

    const Polygon2d nearDegenerateOverlapA(
        Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 8.0, 0.0 }, Point2d{ 8.0, 2.0 }, Point2d{ 5.0, 2.0 },
                      Point2d{ 5.0, 2.000001 }, Point2d{ 0.0, 2.000001 } },
                    PolylineClosure::Closed ) );
    const Polygon2d nearDegenerateOverlapB(
        Polyline2d( { Point2d{ 3.0, 0.0 }, Point2d{ 10.0, 0.0 }, Point2d{ 10.0, 2.000001 },
                      Point2d{ 6.0, 2.000001 }, Point2d{ 6.0, 4.0 }, Point2d{ 3.0, 4.0 } },
                    PolylineClosure::Closed ) );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( nearDegenerateOverlapA, nearDegenerateOverlapB ) ),
                               10.000002, 1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( nearDegenerateOverlapA, nearDegenerateOverlapB ) ),
                               26.000007, 1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( nearDegenerateOverlapA, nearDegenerateOverlapB ) ),
                               6.000003, 1e-9 );

    const Polygon2d ultraThinOverlapA(
        Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 8.0, 0.0 }, Point2d{ 8.0, 2.0 }, Point2d{ 5.0, 2.0 },
                      Point2d{ 5.0, 2.00000001 }, Point2d{ 0.0, 2.00000001 } },
                    PolylineClosure::Closed ) );
    const Polygon2d ultraThinOverlapB(
        Polyline2d( { Point2d{ 3.0, 0.0 }, Point2d{ 10.0, 0.0 }, Point2d{ 10.0, 2.00000001 },
                      Point2d{ 6.0, 2.00000001 }, Point2d{ 6.0, 4.0 }, Point2d{ 3.0, 4.0 } },
                    PolylineClosure::Closed ) );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( ultraThinOverlapA, ultraThinOverlapB ) ),
                               10.00000002, 1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( ultraThinOverlapA, ultraThinOverlapB ) ), 26.00000007,
                               1e-9 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( ultraThinOverlapA, ultraThinOverlapB ) ),
                               6.00000003, 1e-9 );

    const auto equalIntersection = Intersect( square, square );
    ASSERT_GE( equalIntersection.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( equalIntersection ), 16.0, 1e-9 );

    const auto equalDifference = Difference( square, square );
    ASSERT_EQ( equalDifference.Count(), 0 );

    const Polygon2d disjointOther( Polyline2d(
        { Point2d{ 10.0, 0.0 }, Point2d{ 12.0, 0.0 }, Point2d{ 12.0, 2.0 }, Point2d{ 10.0, 2.0 } },
        PolylineClosure::Closed ) );
    const auto disjointUnion = Union( square, disjointOther );
    ASSERT_GE( disjointUnion.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( disjointUnion ), 20.0, 1e-9 );

    const Polygon2d edgeTouch( Polyline2d(
        { Point2d{ 4.0, 1.0 }, Point2d{ 6.0, 1.0 }, Point2d{ 6.0, 3.0 }, Point2d{ 4.0, 3.0 } },
        PolylineClosure::Closed ) );
    const auto touchingDifference = Difference( square, edgeTouch );
    ASSERT_GE( touchingDifference.Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( touchingDifference ), 16.0, 1e-9 );

    const Polygon2d duplicateEdgeFamilyA(
        Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 6.0, 0.0 }, Point2d{ 6.0, 1.0 }, Point2d{ 4.0, 1.0 },
                      Point2d{ 4.0, 1.000000001 }, Point2d{ 6.0, 1.000000001 }, Point2d{ 6.0, 4.0 },
                      Point2d{ 0.0, 4.0 } },
                    PolylineClosure::Closed ) );
    const Polygon2d duplicateEdgeFamilyB( Polyline2d(
        { Point2d{ 2.0, -1.0 }, Point2d{ 8.0, -1.0 }, Point2d{ 8.0, 3.0 }, Point2d{ 2.0, 3.0 } },
        PolylineClosure::Closed ) );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( duplicateEdgeFamilyA, duplicateEdgeFamilyB ) ),
                               12.0, 1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( duplicateEdgeFamilyA, duplicateEdgeFamilyB ) ), 36.0,
                               1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( duplicateEdgeFamilyA, duplicateEdgeFamilyB ) ),
                               12.0, 1e-8 );

    const Polygon2d repeatedCollinearChainA( Polyline2d(
        { Point2d{ 0.0, 0.0 }, Point2d{ 2.0, 0.0 }, Point2d{ 4.0, 0.0 }, Point2d{ 6.0, 0.0 },
          Point2d{ 6.0, 3.0 }, Point2d{ 4.0, 3.0 }, Point2d{ 2.0, 3.0 }, Point2d{ 0.0, 3.0 } },
        PolylineClosure::Closed ) );
    const Polygon2d repeatedCollinearChainB(
        Polyline2d( { Point2d{ 3.0, -1.0 }, Point2d{ 7.0, -1.0 }, Point2d{ 7.0, 2.0 },
                      Point2d{ 5.0, 2.0 }, Point2d{ 5.0, 2.000000001 }, Point2d{ 3.0, 2.000000001 } },
                    PolylineClosure::Closed ) );
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea( Intersect( repeatedCollinearChainA, repeatedCollinearChainB ) ), 6.000000003, 1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( repeatedCollinearChainA, repeatedCollinearChainB ) ),
                               24.0, 1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR(
        TotalArea( Difference( repeatedCollinearChainA, repeatedCollinearChainB ) ), 11.999999997,
        1e-8 );
}

TEST( RelationBooleanTest, HandlesHigherDegreeRepeatedCollinearFamilyOverlap )
{
    constexpr double delta = 1e-9;

    const Polygon2d repeatedFamilyA(
        Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 6.0, 0.0 }, Point2d{ 6.0, 4.0 }, Point2d{ 5.0, 4.0 },
                      Point2d{ 5.0, 4.0 - delta }, Point2d{ 4.0, 4.0 - delta }, Point2d{ 4.0, 4.0 },
                      Point2d{ 3.0, 4.0 }, Point2d{ 3.0, 4.0 - delta }, Point2d{ 2.0, 4.0 - delta },
                      Point2d{ 2.0, 4.0 }, Point2d{ 0.0, 4.0 } },
                    PolylineClosure::Closed ) );
    const Polygon2d repeatedFamilyB( Polyline2d(
        { Point2d{ 3.0, -1.0 }, Point2d{ 5.0, -1.0 }, Point2d{ 5.0, 5.0 }, Point2d{ 3.0, 5.0 } },
        PolylineClosure::Closed ) );

    ASSERT_TRUE( Validate( repeatedFamilyA ).valid );
    ASSERT_TRUE( Validate( repeatedFamilyB ).valid );

    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( repeatedFamilyA, repeatedFamilyB ) ), 8.0 - delta,
                               1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( repeatedFamilyA, repeatedFamilyB ) ), 28.0 - delta,
                               1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( repeatedFamilyA, repeatedFamilyB ) ), 16.0 - delta,
                               1e-8 );
}

TEST( RelationBooleanTest, HandlesNearDegenerateIntersectionClusters )
{
    constexpr double delta = 1e-8;

    const Polygon2d clusterA(
        Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 8.0, 0.0 }, Point2d{ 8.0, 4.0 - delta },
                      Point2d{ 8.0, 4.0 + delta }, Point2d{ 8.0, 8.0 }, Point2d{ 0.0, 8.0 } },
                    PolylineClosure::Closed ) );
    const Polygon2d clusterB(
        Polyline2d( { Point2d{ 3.0, -1.0 }, Point2d{ 5.0, -1.0 }, Point2d{ 5.0, 4.0 - delta },
                      Point2d{ 5.0, 4.0 + delta }, Point2d{ 5.0, 9.0 }, Point2d{ 3.0, 9.0 } },
                    PolylineClosure::Closed ) );

    ASSERT_TRUE( Validate( clusterA ).valid );
    ASSERT_TRUE( Validate( clusterB ).valid );

    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( clusterA, clusterB ) ), 16.0, 1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( clusterA, clusterB ) ), 68.0, 1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( clusterA, clusterB ) ), 48.0, 1e-8 );
}

TEST( RelationBooleanTest, HandlesBelowToleranceArrangementDegeneracies )
{
    constexpr double delta = 1e-12;

    const Polygon2d first(
        Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 8.0, 0.0 }, Point2d{ 8.0, 2.0 }, Point2d{ 5.0, 2.0 },
                      Point2d{ 5.0, 2.0 + delta }, Point2d{ 0.0, 2.0 + delta } },
                    PolylineClosure::Closed ) );
    const Polygon2d second(
        Polyline2d( { Point2d{ 3.0, 0.0 }, Point2d{ 10.0, 0.0 }, Point2d{ 10.0, 2.0 + delta },
                      Point2d{ 6.0, 2.0 + delta }, Point2d{ 6.0, 4.0 }, Point2d{ 3.0, 4.0 } },
                    PolylineClosure::Closed ) );

    ASSERT_GE( Intersect( first, second ).Count(), 1 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Intersect( first, second ) ), 10.0, 1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Union( first, second ) ), 26.0, 1e-8 );
    GEOMETRY_TEST_ASSERT_NEAR( TotalArea( Difference( first, second ) ), 6.0, 1e-8 );
}
