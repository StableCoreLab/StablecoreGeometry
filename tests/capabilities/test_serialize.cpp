#include <gtest/gtest.h>
#include <cassert>
#include <numbers>
#include <string>

#include "serialize/GeometryText.h"
#include "support/GTestCompat.h"
#include "support/GeometryTestSupport.h"

TEST(SerializeTest, CoversCurrentCapabilities)
{
    using geometry::sdk::Box2d;
    using geometry::sdk::Point2d;
    using geometry::sdk::ArcSegment2d;
    using geometry::sdk::Polygon2d;
    using geometry::sdk::Polyline2d;
    using geometry::sdk::PolylineClosure;
    using geometry::sdk::SegmentProjection2d;
    using geometry::sdk::Vector2d;
    using geometry::serialize::FromText;
    using geometry::serialize::ToText;

    const Point2d point = Point2d::FromXY(1.25, -3.5);
    const Vector2d vector = Vector2d::FromXY(-4.0, 8.5);
    const Box2d box = Box2d::FromMinMax(Point2d{0.0, 1.0}, Point2d{2.0, 3.0});
    const SegmentProjection2d projection{
        Point2d{4.0, 5.0},
        0.25,
        12.5,
        true};
    const ArcSegment2d arc(
        Point2d{0.0, 0.0},
        1.0,
        0.0,
        std::numbers::pi_v<double> * 0.5);
    const Polyline2d polyline(
        {Point2d{0.0, 0.0}, Point2d{3.0, 0.0}, Point2d{3.0, 4.0}},
        PolylineClosure::Open);
    const Polygon2d polygon(
        geometry::test::MakeSdkRectangleRing(Point2d{0.0, 0.0}, Point2d{4.0, 4.0}),
        {geometry::test::MakeSdkRectangleHoleRing(Point2d{1.0, 1.0}, Point2d{3.0, 3.0})});

    const std::string pointText = ToText(point);
    const std::string vectorText = ToText(vector);
    const std::string boxText = ToText(box);
    const std::string projectionText = ToText(projection);
    const std::string arcText = ToText(arc);
    const std::string polylineText = ToText(polyline);
    const std::string polygonText = ToText(polygon);

    assert(pointText == "Point2d 1.25 -3.5");
    assert(vectorText == "Vector2d -4 8.5");
    assert(boxText == "Box2d 0 1 2 3");
    assert(projectionText == "SegmentProjection2d 4 5 0.25 12.5 1");
    assert(arcText == "ArcSegment2d 0 0 1 0 1.5707963267948966");
    assert(polylineText == "Polyline2d open 3 0 0 3 0 3 4");
    assert(
        polygonText ==
        "Polygon2d Polyline2d closed 4 0 0 4 0 4 4 0 4 1 Polyline2d closed 4 1 1 1 3 3 3 3 1");

    Point2d parsedPoint{};
    Vector2d parsedVector{};
    Box2d parsedBox{};
    SegmentProjection2d parsedProjection{};
    ArcSegment2d parsedArc{};
    Polyline2d parsedPolyline;
    Polygon2d parsedPolygon;

    assert(FromText(pointText, parsedPoint));
    assert(FromText(vectorText, parsedVector));
    assert(FromText(boxText, parsedBox));
    assert(FromText(projectionText, parsedProjection));
    assert(FromText(arcText, parsedArc));
    assert(FromText(polylineText, parsedPolyline));
    assert(FromText(polygonText, parsedPolygon));

    assert(parsedPoint == point);
    assert(parsedVector == vector);
    assert(parsedBox == box);
    assert(parsedProjection.point == projection.point);
    assert(parsedProjection.parameter == projection.parameter);
    assert(parsedProjection.distanceSquared == projection.distanceSquared);
    assert(parsedProjection.isOnSegment == projection.isOnSegment);
    assert(parsedArc.AlmostEquals(arc));
    GEOMETRY_TEST_ASSERT_POLYLINE_NEAR(parsedPolyline, polyline, 1e-12);
    GEOMETRY_TEST_ASSERT_POLYGON_NEAR(parsedPolygon, polygon, 1e-12);

    assert(!FromText("Point2d 1.0", parsedPoint));
    assert(!FromText("Vector2d 1 2 3", parsedVector));
    assert(!FromText("Box2d 0 0 1", parsedBox));
    assert(!FromText("SegmentProjection2d 1 2 3 4", parsedProjection));
    assert(!FromText("ArcSegment2d 0 0 1 0 0", parsedArc));
    assert(!FromText("Polyline2d open 2 0 0 1", parsedPolyline));
    assert(!FromText("Polygon2d Polyline2d open 2 0 0 1 0 0", parsedPolygon));
}


