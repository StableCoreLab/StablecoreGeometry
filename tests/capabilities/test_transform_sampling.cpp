#include <gtest/gtest.h>
#include <cmath>

#include "sdk/Editing.h"
#include "sdk/Sampling.h"
#include "sdk/Transform.h"
#include "support/GeometryTestSupport.h"

using geometry::sdk::ArcSegment2d;
using geometry::sdk::Box2d;
using geometry::sdk::LineSegment2d;
using geometry::sdk::Point2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;
using geometry::sdk::Vector2d;

TEST(TransformSamplingTest, CoversCurrentCapabilities)
{
    const LineSegment2d line(Point2d{0.0, 0.0}, Point2d{2.0, 0.0});
    const auto moved = geometry::sdk::Translate(line, Vector2d{1.0, 2.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(moved.startPoint, (Point2d{1.0, 2.0}), 1e-12);

    const auto rotated = geometry::sdk::Rotate(Point2d{1.0, 0.0}, Point2d{0.0, 0.0}, std::acos(-1.0) * 0.5);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(rotated, (Point2d{0.0, 1.0}), 1e-12);

    const auto mirrored = geometry::sdk::Mirror(Point2d{1.0, 2.0}, Point2d{0.0, 0.0}, Vector2d{1.0, 0.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(mirrored, (Point2d{1.0, -2.0}), 1e-12);

    const auto stretched = geometry::sdk::Stretch(Point2d{1.0, 1.0}, Box2d::FromMinMax(Point2d{0.0, 0.0}, Point2d{2.0, 2.0}), Vector2d{3.0, 0.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(stretched, (Point2d{4.0, 1.0}), 1e-12);

    const ArcSegment2d arc(Point2d{0.0, 0.0}, 1.0, 0.0, std::acos(-1.0) * 0.5);
    const auto arcSamples = geometry::sdk::SampleByMaxAngle(arc, std::acos(-1.0) * 0.25);
    ASSERT_EQ(arcSamples.size(), 3);

    const Polyline2d path(
        {Point2d{0.0, 0.0}, Point2d{2.0, 0.0}, Point2d{2.0, 0.0}, Point2d{2.0, 2.0}},
        PolylineClosure::Open);
    const auto normalized = geometry::sdk::Normalize(path);
    ASSERT_EQ(normalized.PointCount(), 3);

    const auto inserted = geometry::sdk::InsertPoint(
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Point2d{2.0, 0.0});
    ASSERT_EQ(inserted.PointCount(), 3);

    const Polygon2d polygon(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
            PolylineClosure::Closed));
    const auto movedPolygon = geometry::sdk::Translate(polygon, Vector2d{1.0, 0.0});
    GEOMETRY_TEST_ASSERT_POINT_NEAR(movedPolygon.OuterRing().PointAt(0), (Point2d{1.0, 0.0}), 1e-12);
}


