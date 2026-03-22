#include <cassert>
#include <string>

#include "serialize/GeometryText.h"

int main()
{
    using geometry::sdk::Box2d;
    using geometry::sdk::Point2d;
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

    const std::string pointText = ToText(point);
    const std::string vectorText = ToText(vector);
    const std::string boxText = ToText(box);
    const std::string projectionText = ToText(projection);

    assert(pointText == "Point2d 1.25 -3.5");
    assert(vectorText == "Vector2d -4 8.5");
    assert(boxText == "Box2d 0 1 2 3");
    assert(projectionText == "SegmentProjection2d 4 5 0.25 12.5 1");

    Point2d parsedPoint{};
    Vector2d parsedVector{};
    Box2d parsedBox{};
    SegmentProjection2d parsedProjection{};

    assert(FromText(pointText, parsedPoint));
    assert(FromText(vectorText, parsedVector));
    assert(FromText(boxText, parsedBox));
    assert(FromText(projectionText, parsedProjection));

    assert(parsedPoint == point);
    assert(parsedVector == vector);
    assert(parsedBox == box);
    assert(parsedProjection.point == projection.point);
    assert(parsedProjection.parameter == projection.parameter);
    assert(parsedProjection.distanceSquared == projection.distanceSquared);
    assert(parsedProjection.isOnSegment == projection.isOnSegment);

    assert(!FromText("Point2d 1.0", parsedPoint));
    assert(!FromText("Vector2d 1 2 3", parsedVector));
    assert(!FromText("Box2d 0 0 1", parsedBox));
    assert(!FromText("SegmentProjection2d 1 2 3 4", parsedProjection));

    return 0;
}
