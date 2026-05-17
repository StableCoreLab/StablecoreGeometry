#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "Core/Relation.h"
#include "Core/ShapeOps.h"
#include "Core/Validation.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/Polygon2d.h"
#include "Support/GeometryTestSupport.h"

using Geometry::ArcDirection;
using Geometry::ArcSegment2d;
using Geometry::Box2d;
using Geometry::IsEqual;
using Geometry::LineSegment2d;
using Geometry::LocatePoint;
using Geometry::Orientation;
using Geometry::Point2d;
using Geometry::PointContainment2d;
using Geometry::Polygon2d;
using Geometry::PolygonValidation2d;
using Geometry::Polyline2d;
using Geometry::PolylineClosure;
using Geometry::RingOrientation2d;
using Geometry::Validate;

namespace
{
    Polyline2d MakeClosedRing( const std::vector<std::pair<Point2d, Point2d>> &edges )
    {
        std::vector<std::shared_ptr<Geometry::Segment2d>> segments;
        segments.reserve( edges.size() );
        for( const auto &edge : edges )
        {
            segments.push_back( std::make_shared<LineSegment2d>( edge.first, edge.second ) );
        }
        return Polyline2d( std::move( segments ), PolylineClosure::Closed );
    }
}  // namespace

TEST( PolygonTest, CoversCurrentCapabilities )
{
    Polyline2d outerRing = MakeClosedRing( {
        { Point2d( 0.0, 0.0 ), Point2d( 4.0, 0.0 ) },
        { Point2d( 4.0, 0.0 ), Point2d( 4.0, 4.0 ) },
        { Point2d( 4.0, 4.0 ), Point2d( 0.0, 4.0 ) },
        { Point2d( 0.0, 4.0 ), Point2d( 0.0, 0.0 ) },
    } );

    Polyline2d holeRing = MakeClosedRing( {
        { Point2d( 1.0, 1.0 ), Point2d( 1.0, 3.0 ) },
        { Point2d( 1.0, 3.0 ), Point2d( 3.0, 3.0 ) },
        { Point2d( 3.0, 3.0 ), Point2d( 3.0, 1.0 ) },
        { Point2d( 3.0, 1.0 ), Point2d( 1.0, 1.0 ) },
    } );

    Polygon2d polygon( outerRing, { holeRing } );
    ASSERT_TRUE( polygon.IsValid() );
    ASSERT_TRUE( polygon.OuterRing().IsClosed() );
    ASSERT_EQ( polygon.HoleCount(), 1 );
    ASSERT_TRUE( polygon.HoleAt( 0 ).IsClosed() );

    ASSERT_LT( std::abs( polygon.Area() - 12.0 ), 1e-12 );
    ASSERT_LT( std::abs( polygon.Perimeter() - 24.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( polygon.Centroid(), Point2d( 2.0, 2.0 ), 1e-12 );

    const Box2d box = polygon.Bounds();
    ASSERT_TRUE( box.IsValid() );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( box.MinPoint(), Point2d( 0.0, 0.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( box.MaxPoint(), Point2d( 4.0, 4.0 ), 1e-12 );

    Polyline2d badHole = MakeClosedRing( {
        { Point2d( 1.0, 1.0 ), Point2d( 3.0, 1.0 ) },
        { Point2d( 3.0, 1.0 ), Point2d( 3.0, 3.0 ) },
        { Point2d( 3.0, 3.0 ), Point2d( 1.0, 3.0 ) },
        { Point2d( 1.0, 3.0 ), Point2d( 1.0, 1.0 ) },
    } );
    Polygon2d invalidPolygon( outerRing, { badHole } );
    ASSERT_FALSE( invalidPolygon.IsValid() );

    Polyline2d outsideHole = MakeClosedRing( {
        { Point2d( 5.0, 5.0 ), Point2d( 6.0, 5.0 ) },
        { Point2d( 6.0, 5.0 ), Point2d( 6.0, 6.0 ) },
        { Point2d( 6.0, 6.0 ), Point2d( 5.0, 6.0 ) },
        { Point2d( 5.0, 6.0 ), Point2d( 5.0, 5.0 ) },
    } );
    ASSERT_FALSE( Polygon2d( outerRing, { outsideHole } ).IsValid() );

    Polyline2d crossingHole = MakeClosedRing( {
        { Point2d( 3.0, 3.0 ), Point2d( 5.0, 3.0 ) },
        { Point2d( 5.0, 3.0 ), Point2d( 5.0, 5.0 ) },
        { Point2d( 5.0, 5.0 ), Point2d( 3.0, 5.0 ) },
        { Point2d( 3.0, 5.0 ), Point2d( 3.0, 3.0 ) },
    } );
    ASSERT_FALSE( Polygon2d( outerRing, { crossingHole } ).IsValid() );

    Polyline2d nestedHoleA = MakeClosedRing( {
        { Point2d( 0.5, 0.5 ), Point2d( 0.5, 3.5 ) },
        { Point2d( 0.5, 3.5 ), Point2d( 3.5, 3.5 ) },
        { Point2d( 3.5, 3.5 ), Point2d( 3.5, 0.5 ) },
        { Point2d( 3.5, 0.5 ), Point2d( 0.5, 0.5 ) },
    } );
    Polyline2d nestedHoleB = MakeClosedRing( {
        { Point2d( 1.5, 1.5 ), Point2d( 1.5, 2.5 ) },
        { Point2d( 1.5, 2.5 ), Point2d( 2.5, 2.5 ) },
        { Point2d( 2.5, 2.5 ), Point2d( 2.5, 1.5 ) },
        { Point2d( 2.5, 1.5 ), Point2d( 1.5, 1.5 ) },
    } );
    ASSERT_FALSE( Polygon2d( outerRing, { nestedHoleA, nestedHoleB } ).IsValid() );

    std::vector<std::shared_ptr<Geometry::Segment2d>> arcSegments;
    arcSegments.push_back( std::make_shared<LineSegment2d>( Point2d( 0.0, 0.0 ), Point2d( 1.0, 0.0 ) ) );
    arcSegments.push_back( std::make_shared<ArcSegment2d>(
        Point2d( 0.0, 0.0 ), 1.0, 0.0, std::acos( -1.0 ) * 0.5, ArcDirection::CounterClockwise ) );
    arcSegments.push_back( std::make_shared<LineSegment2d>( Point2d( 0.0, 1.0 ), Point2d( 0.0, 0.0 ) ) );

    Polygon2d quarterDisk( Polyline2d( std::move( arcSegments ), PolylineClosure::Closed ) );
    ASSERT_TRUE( quarterDisk.IsValid() );
    ASSERT_LT( std::abs( quarterDisk.Area() - ( std::acos( -1.0 ) / 4.0 ) ), 1e-12 );
    ASSERT_LT( std::abs( quarterDisk.Perimeter() - ( 2.0 + std::acos( -1.0 ) * 0.5 ) ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR(
        quarterDisk.Centroid(),
        Point2d( 4.0 / ( 3.0 * std::acos( -1.0 ) ), 4.0 / ( 3.0 * std::acos( -1.0 ) ) ), 1e-12 );
    ASSERT_EQ( LocatePoint( Point2d( 0.2, 0.2 ), quarterDisk ), PointContainment2d::Inside );
    ASSERT_EQ( LocatePoint( Point2d( 0.9, 0.9 ), quarterDisk ), PointContainment2d::Outside );
    ASSERT_EQ( LocatePoint( Point2d( std::sqrt( 0.5 ), std::sqrt( 0.5 ) ), quarterDisk ),
               PointContainment2d::OnBoundary );

    const double kPiValue = std::acos( -1.0 );
    std::vector<std::shared_ptr<Geometry::Segment2d>> circleSegments;
    circleSegments.push_back(
        std::make_shared<ArcSegment2d>( Point2d( 0.0, 0.0 ), 1.0, 0.0, kPiValue ) );
    circleSegments.push_back(
        std::make_shared<ArcSegment2d>( Point2d( 0.0, 0.0 ), 1.0, kPiValue, kPiValue ) );
    Polyline2d fullCircleRing( std::move( circleSegments ), PolylineClosure::Closed );
    Polygon2d fullCircle( fullCircleRing );
    const PolygonValidation2d fullCircleValidation = Validate( fullCircleRing );
    ASSERT_TRUE( fullCircleValidation.valid );
    ASSERT_TRUE( fullCircle.IsValid() );
    ASSERT_EQ( Orientation( fullCircleRing ), RingOrientation2d::CounterClockwise );
    ASSERT_EQ( LocatePoint( Point2d( 0.0, 0.0 ), fullCircle ), PointContainment2d::Inside );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( Geometry::Centroid( fullCircle ), Point2d( 0.0, 0.0 ), 1e-12 );
}
