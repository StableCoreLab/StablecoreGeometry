#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <vector>

#include "types/ArcSegment2.h"
#include "types/LineSegment2.h"
#include "types/Polyline2.h"
#include "support/GeometryTestSupport.h"

using geometry::ArcDirection;
using geometry::ArcSegment2d;
using geometry::Box2d;
using geometry::IsEqual;
using geometry::LineSegment2d;
using geometry::Point2d;
using geometry::Polyline2d;
using geometry::PolylineClosure;

namespace
{
constexpr double kPi = 3.141592653589793238462643383279502884;
}

TEST(PolylineTest, CoversCurrentCapabilities)
{
    Polyline2d emptyPath;
    ASSERT_FALSE(emptyPath.IsValid());
    ASSERT_EQ(emptyPath.SegmentCount(), 0);
    ASSERT_EQ(emptyPath.VertexCount(), 0);

    auto first = std::make_shared<LineSegment2d>(Point2d(0.0, 0.0), Point2d(3.0, 0.0));
    auto second = std::make_shared<LineSegment2d>(Point2d(3.0, 0.0), Point2d(3.0, 4.0));
    Polyline2d openPath({first, second}, PolylineClosure::Open);

    ASSERT_TRUE(openPath.IsValid());
    ASSERT_FALSE(openPath.IsClosed());
    ASSERT_EQ(openPath.SegmentCount(), 2);
    ASSERT_EQ(openPath.VertexCount(), 3);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.VertexAt(0), Point2d(0.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.VertexAt(1), Point2d(3.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.VertexAt(2), Point2d(3.0, 4.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.StartPoint(), Point2d(0.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.EndPoint(), Point2d(3.0, 4.0), 1e-12);
    ASSERT_LT(std::abs(openPath.Length() - 7.0), 1e-12);
    ASSERT_LT(std::abs(openPath.LengthAt(0.5) - 3.5), 1e-12);
    ASSERT_LT(std::abs(openPath.ParameterAtLength(3.5) - 0.5), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.PointAt(0.5), Point2d(3.0, 0.5), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.PointAtLength(3.5), Point2d(3.0, 0.5), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.PointAtLength(-2.0, true), Point2d(0.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.PointAtLength(9.0, true), Point2d(3.0, 4.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.PointAtLength(-2.0, false), Point2d(-2.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openPath.PointAtLength(9.0, false), Point2d(3.0, 6.0), 1e-12);

    const Box2d openBox = openPath.Bounds();
    ASSERT_TRUE(openBox.IsValid());
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openBox.MinPoint(), Point2d(0.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openBox.MaxPoint(), Point2d(3.0, 4.0), 1e-12);

    auto broken = std::make_shared<LineSegment2d>(Point2d(10.0, 10.0), Point2d(11.0, 10.0));
    Polyline2d invalidOpen({first, broken}, PolylineClosure::Open);
    ASSERT_FALSE(invalidOpen.IsValid());

    auto closeA = std::make_shared<LineSegment2d>(Point2d(0.0, 0.0), Point2d(1.0, 0.0));
    auto closeB = std::make_shared<LineSegment2d>(Point2d(1.0, 0.0), Point2d(0.0, 1.0));
    auto closeC = std::make_shared<LineSegment2d>(Point2d(0.0, 1.0), Point2d(0.0, 0.0));
    Polyline2d closedPath({closeA, closeB, closeC}, PolylineClosure::Closed);

    ASSERT_TRUE(closedPath.IsValid());
    ASSERT_TRUE(closedPath.IsClosed());
    ASSERT_EQ(closedPath.VertexCount(), 3);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(closedPath.StartPoint(), Point2d(0.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(closedPath.EndPoint(), Point2d(0.0, 0.0), 1e-12);

    auto line = std::make_shared<LineSegment2d>(Point2d(0.0, 0.0), Point2d(1.0, 0.0));
    auto arc = std::make_shared<ArcSegment2d>(
        Point2d(1.0, 1.0),
        1.0,
        -kPi / 2.0,
        0.0,
        ArcDirection::CounterClockwise);
    Polyline2d mixedPath({line, arc}, PolylineClosure::Open);

    ASSERT_TRUE(mixedPath.IsValid());
    ASSERT_LT(std::abs(mixedPath.Length() - (1.0 + kPi / 2.0)), 1e-12);
    const Box2d mixedBox = mixedPath.Bounds();
    ASSERT_TRUE(mixedBox.IsValid());
    GEOMETRY_TEST_ASSERT_POINT_NEAR(mixedBox.MinPoint(), Point2d(0.0, 0.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(mixedBox.MaxPoint(), Point2d(2.0, 1.0), 1e-12);
}



