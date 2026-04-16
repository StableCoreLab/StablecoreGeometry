#include <gtest/gtest.h>
#include <numbers>
#include <string>

#include "Serialize/GeometryText.h"
#include "Support/GeometryTestSupport.h"

TEST( SerializeTest, CoversCurrentCapabilities )
{
    using Geometry::ArcSegment2d;
    using Geometry::Box2d;
    using Geometry::Point2d;
    using Geometry::Polygon2d;
    using Geometry::Polyline2d;
    using Geometry::PolylineClosure;
    using Geometry::SegmentProjection2d;
    using Geometry::Vector2d;
    using Geometry::Serialize::FromText;
    using Geometry::Serialize::ToText;

    const Point2d point = Point2d::FromXY( 1.25, -3.5 );
    const Vector2d vector = Vector2d::FromXY( -4.0, 8.5 );
    const Box2d box = Box2d::FromMinMax( Point2d{ 0.0, 1.0 }, Point2d{ 2.0, 3.0 } );
    const SegmentProjection2d projection{ Point2d{ 4.0, 5.0 }, 0.25, 12.5, true };
    const ArcSegment2d arc( Point2d{ 0.0, 0.0 }, 1.0, 0.0, std::numbers::pi_v<double> * 0.5 );
    const Polyline2d polyline( { Point2d{ 0.0, 0.0 }, Point2d{ 3.0, 0.0 }, Point2d{ 3.0, 4.0 } },
                               PolylineClosure::Open );
    const Polygon2d polygon(
        Geometry::Test::MakeRectangleRing( Point2d{ 0.0, 0.0 }, Point2d{ 4.0, 4.0 } ),
        { Geometry::Test::MakeRectangleHoleRing( Point2d{ 1.0, 1.0 }, Point2d{ 3.0, 3.0 } ) } );

    const std::string pointText = ToText( point );
    const std::string vectorText = ToText( vector );
    const std::string boxText = ToText( box );
    const std::string projectionText = ToText( projection );
    const std::string arcText = ToText( arc );
    const std::string polylineText = ToText( polyline );
    const std::string polygonText = ToText( polygon );

    ASSERT_EQ( pointText, "Point2d 1.25 -3.5" );
    ASSERT_EQ( vectorText, "Vector2d -4 8.5" );
    ASSERT_EQ( boxText, "Box2d 0 1 2 3" );
    ASSERT_EQ( projectionText, "SegmentProjection2d 4 5 0.25 12.5 1" );
    ASSERT_EQ( arcText, "ArcSegment2d 0 0 1 0 1.5707963267948966" );
    ASSERT_EQ( polylineText, "Polyline2d open 3 0 0 3 0 3 4" );
    ASSERT_EQ( polygonText,
               "Polygon2d Polyline2d closed 4 0 0 4 0 4 4 0 4 1 Polyline2d closed 4 1 1 1 3 3 3 3 1" );

    Point2d parsedPoint{};
    Vector2d parsedVector{};
    Box2d parsedBox{};
    SegmentProjection2d parsedProjection{};
    ArcSegment2d parsedArc{};
    Polyline2d parsedPolyline;
    Polygon2d parsedPolygon;

    ASSERT_TRUE( FromText( pointText, parsedPoint ) );
    ASSERT_TRUE( FromText( vectorText, parsedVector ) );
    ASSERT_TRUE( FromText( boxText, parsedBox ) );
    ASSERT_TRUE( FromText( projectionText, parsedProjection ) );
    ASSERT_TRUE( FromText( arcText, parsedArc ) );
    ASSERT_TRUE( FromText( polylineText, parsedPolyline ) );
    ASSERT_TRUE( FromText( polygonText, parsedPolygon ) );

    ASSERT_EQ( parsedPoint, point );
    ASSERT_EQ( parsedVector, vector );
    ASSERT_EQ( parsedBox, box );
    ASSERT_EQ( parsedProjection.point, projection.point );
    ASSERT_EQ( parsedProjection.parameter, projection.parameter );
    ASSERT_EQ( parsedProjection.distanceSquared, projection.distanceSquared );
    ASSERT_EQ( parsedProjection.isOnSegment, projection.isOnSegment );
    ASSERT_TRUE( parsedArc.AlmostEquals( arc ) );
    GEOMETRY_TEST_ASSERT_POLYLINE_NEAR( parsedPolyline, polyline, 1e-12 );
    GEOMETRY_TEST_ASSERT_POLYGON_NEAR( parsedPolygon, polygon, 1e-12 );

    ASSERT_FALSE( FromText( "Point2d 1.0", parsedPoint ) );
    ASSERT_FALSE( FromText( "Vector2d 1 2 3", parsedVector ) );
    ASSERT_FALSE( FromText( "Box2d 0 0 1", parsedBox ) );
    ASSERT_FALSE( FromText( "SegmentProjection2d 1 2 3 4", parsedProjection ) );
    ASSERT_FALSE( FromText( "ArcSegment2d 0 0 1 0 0", parsedArc ) );
    ASSERT_FALSE( FromText( "Polyline2d open 2 0 0 1", parsedPolyline ) );
    ASSERT_FALSE( FromText( "Polygon2d Polyline2d open 2 0 0 1 0 0", parsedPolygon ) );
}
