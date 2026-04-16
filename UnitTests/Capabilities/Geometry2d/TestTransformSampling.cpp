#include <gtest/gtest.h>
#include <cmath>

#include "Core/Editing.h"
#include "Core/Sampling.h"
#include "Core/Transform.h"
#include "Support/GeometryTestSupport.h"



using Geometry::Sdk::ArcSegment2d;
using Geometry::Sdk::Box2d;
using Geometry::Sdk::LineSegment2d;
using Geometry::Sdk::InsertPoint;
using Geometry::Sdk::Mirror;
using Geometry::Sdk::Normalize;
using Geometry::Sdk::Point2d;
using Geometry::Sdk::Polygon2d;
using Geometry::Sdk::Polyline2d;
using Geometry::Sdk::PolylineClosure;
using Geometry::Sdk::Rotate;
using Geometry::Sdk::SampleByMaxAngle;
using Geometry::Sdk::Stretch;
using Geometry::Sdk::Vector2d;
using Geometry::Sdk::Translate;

TEST(TransformSamplingTest, CoversCurrentCapabilities)
{
    const LineSegment2d line(Point2d{0.0, 0.0}, Point2d{2.0, 0.0});
    const auto moved = Translate(line, Vector2d{1.0, 2.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(moved.startPoint, (Point2d{1.0, 2.0}), 1e-12);

    const auto rotated = Rotate(Point2d{1.0, 0.0}, Point2d{0.0, 0.0}, std::acos(-1.0) * 0.5);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(rotated, (Point2d{0.0, 1.0}), 1e-12);

    const auto mirrored = Mirror(Point2d{1.0, 2.0}, Point2d{0.0, 0.0}, Vector2d{1.0, 0.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(mirrored, (Point2d{1.0, -2.0}), 1e-12);

    const auto stretched = Stretch(Point2d{1.0, 1.0}, Box2d::FromMinMax(Point2d{0.0, 0.0}, Point2d{2.0, 2.0}), Vector2d{3.0, 0.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(stretched, (Point2d{4.0, 1.0}), 1e-12);

    const ArcSegment2d arc(Point2d{0.0, 0.0}, 1.0, 0.0, std::acos(-1.0) * 0.5);
    const auto arcSamples = SampleByMaxAngle(arc, std::acos(-1.0) * 0.25);
    ASSERT_EQ(arcSamples.size(), 3);

    const Polyline2d path(
        {Point2d{0.0, 0.0}, Point2d{2.0, 0.0}, Point2d{2.0, 0.0}, Point2d{2.0, 2.0}},
        PolylineClosure::Open);
    const auto normalized = Normalize(path);
    ASSERT_EQ(normalized.PointCount(), 3);

    const auto inserted = InsertPoint(
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Point2d{2.0, 0.0});
    ASSERT_EQ(inserted.PointCount(), 3);

    const Polygon2d polygon(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
            PolylineClosure::Closed));
    const auto movedPolygon = Translate(polygon, Vector2d{1.0, 0.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(movedPolygon.OuterRing().PointAt(0), (Point2d{1.0, 0.0}), 1e-12);
}








