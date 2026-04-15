#include <array>
#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <numbers>

#include "sdk/Geometry.h"
#include "support/GeometryTestSupport.h"

using Geometry::ArcDirection;
using Geometry::SegmentKind2;
using Geometry::Sdk::ArcSegment2d;
using Geometry::Sdk::AxisProjection2d;
using Geometry::Sdk::AxisSample2d;
using Geometry::Sdk::Box2d;
using Geometry::Sdk::ClosestPoints2d;
using Geometry::Sdk::IntersectionKind2d;
using Geometry::Sdk::LineSegment2d;
using Geometry::Sdk::Point2d;
using Geometry::Sdk::Polygon2d;
using Geometry::Sdk::Polyline2d;
using Geometry::Sdk::PolylineClosure;
using Geometry::Sdk::RingOrientation2d;
using Geometry::Sdk::Segment2d;
using Geometry::Sdk::SegmentIntersection2d;
using Geometry::Sdk::SegmentProjection2d;
using Geometry::Sdk::SegmentSplit2d;
using Geometry::Sdk::SegmentTrim2d;
using Geometry::Sdk::SnapResult2d;
using Geometry::Sdk::Vector2d;

TEST(SdkAlgorithmsTest, CoversCurrentCapabilities)
{
    const LineSegment2d line(Point2d{0.0, 0.0}, Point2d{4.0, 0.0});
    const ArcSegment2d arc(Point2d{0.0, 0.0}, 2.0, 0.0, std::numbers::pi_v<double> * 0.5);

    GEOMETRY_TEST_ASSERT_NEAR(Geometry::Sdk::Distance(Point2d{1.0, 1.0}, line), 1.0, 1e-12);
    GEOMETRY_TEST_ASSERT_NEAR(
        Geometry::Sdk::Distance(Point2d{1.0, 1.0}, arc),
        std::abs(std::sqrt(2.0) - 2.0),
        1e-12);
    GEOMETRY_TEST_ASSERT_NEAR(line.Length(), 4.0, 1e-12);
    GEOMETRY_TEST_ASSERT_BOX_NEAR(arc.Bounds(), Box2d::FromMinMax(Point2d{0.0, 0.0}, Point2d{2.0, 2.0}), 1e-12);

    const SegmentProjection2d lineProjection =
        Geometry::Sdk::ProjectPointToLineSegment(Point2d{1.0, 2.0}, line, true);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(lineProjection.point, (Point2d{1.0, 0.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_NEAR(lineProjection.parameter, 0.25, 1e-12);

    const SegmentProjection2d arcProjection =
        Geometry::Sdk::ProjectPointToArcSegment(Point2d{1.0, 1.0}, arc, true);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(
        arcProjection.point,
        (Point2d{std::sqrt(2.0), std::sqrt(2.0)}),
        1e-12);

    GEOMETRY_TEST_ASSERT_POINT_NEAR(
        line.PointAtLength(2.0, true),
        (Point2d{2.0, 0.0}),
        1e-12);
    GEOMETRY_TEST_ASSERT_VECTOR_NEAR(
        Geometry::Sdk::TangentAt(line, 0.5),
        (Vector2d{1.0, 0.0}),
        1e-12);
    GEOMETRY_TEST_ASSERT_VECTOR_NEAR(
        Geometry::Sdk::NormalAt(line, 0.5),
        (Vector2d{0.0, 1.0}),
        1e-12);

    const LineSegment2d vertical(Point2d{2.0, -1.0}, Point2d{2.0, 1.0});
    const SegmentIntersection2d lineLineIntersection = Geometry::Sdk::Intersect(line, vertical);
    ASSERT_EQ(lineLineIntersection.kind, IntersectionKind2d::Point);
    ASSERT_EQ(lineLineIntersection.pointCount, 1);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(lineLineIntersection.points[0].point, (Point2d{2.0, 0.0}), 1e-12);

    const SegmentIntersection2d lineArcIntersection = Geometry::Sdk::Intersect(line, arc);
    ASSERT_TRUE(lineArcIntersection.HasIntersection());

    const ArcSegment2d secondArc(Point2d{2.0, 0.0}, 2.0, std::numbers::pi_v<double>, -std::numbers::pi_v<double> * 0.5);
    const SegmentIntersection2d arcArcIntersection = Geometry::Sdk::Intersect(arc, secondArc);
    ASSERT_TRUE(arcArcIntersection.HasIntersection());

    const ClosestPoints2d closest = Geometry::Sdk::ClosestPoints(line, secondArc);
    ASSERT_TRUE(closest.IsValid());

    const Polyline2d ccwRing(
        {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
        PolylineClosure::Closed);
    const Polyline2d holeRing = Geometry::Test::MakeSdkRectangleHoleRing(Point2d{1.0, 1.0}, Point2d{3.0, 3.0});
    const Polygon2d polygon(ccwRing, {holeRing});
    GEOMETRY_TEST_ASSERT_NEAR(polygon.Area(), 12.0, 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(Geometry::Sdk::Centroid(polygon), (Point2d{2.0, 2.0}), 1e-12);
    ASSERT_EQ(Geometry::Sdk::Orientation(ccwRing), RingOrientation2d::CounterClockwise);
    ASSERT_TRUE(Geometry::Sdk::IsCounterClockwise(ccwRing));
    ASSERT_TRUE(Geometry::Sdk::IsClockwise(holeRing));

    const LineSegment2d reversedLine = Geometry::Sdk::Reverse(line);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(reversedLine.startPoint, line.endPoint, 1e-12);
    const ArcSegment2d reversedArc = Geometry::Sdk::Reverse(arc);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(reversedArc.StartPoint(), arc.EndPoint(), 1e-12);
    const Polyline2d closedOpen = Geometry::Sdk::Close(Polyline2d({Point2d{0.0, 0.0}, Point2d{1.0, 0.0}, Point2d{1.0, 1.0}}, PolylineClosure::Open));
    ASSERT_TRUE(closedOpen.IsClosed());

    const AxisSample2d sample = Geometry::Sdk::SampleAxis(line, 0.5);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(sample.point, (Point2d{2.0, 0.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_VECTOR_NEAR(sample.tangent, (Vector2d{1.0, 0.0}), 1e-12);
    const AxisProjection2d axisProjection = Geometry::Sdk::ProjectPointToAxis(Point2d{1.0, 1.0}, line);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(axisProjection.projection.point, (Point2d{1.0, 0.0}), 1e-12);

    const SegmentSplit2d split = Geometry::Sdk::SplitSegment(line, 0.5);
    ASSERT_TRUE(split.success);
    ASSERT_EQ(split.first->Kind(), SegmentKind2::Line);
    ASSERT_EQ(split.second->Kind(), SegmentKind2::Line);

    const SegmentTrim2d trim = Geometry::Sdk::TrimSegment(arc, 0.25, 0.75);
    ASSERT_TRUE(trim.success);
    ASSERT_EQ(trim.segment->Kind(), SegmentKind2::Arc);

    const std::unique_ptr<Segment2d> lineClone = line.Clone();
    const std::unique_ptr<Segment2d> arcClone = arc.Clone();
    const std::array<const Segment2d*, 2> snapSegments{lineClone.get(), arcClone.get()};
    const SnapResult2d snap = Geometry::Sdk::SnapPointToSegments(Point2d{1.0, 1.0}, snapSegments, 2.0);
    ASSERT_TRUE(snap.snapped);
}


