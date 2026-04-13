#include <gtest/gtest.h>

#include "sdk/Geometry.h"
#include "support/GeometryTestSupport.h"

using geometry::sdk::BuildMultiPolygonByLines;
using geometry::sdk::Circle2d;
using geometry::sdk::CutPolygon;
using geometry::sdk::Ellipse2d;
using geometry::sdk::LineSegment2d;
using geometry::sdk::MultiPolyline2d;
using geometry::sdk::NormalizePolygonByLines;
using geometry::sdk::Point2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;
using geometry::sdk::Rectangle2d;
using geometry::sdk::SubPolyline;

TEST(ShapesPathopsTest, CoversCurrentCapabilities)
{
    const Circle2d circle(Point2d{1.0, 2.0}, 3.0);
    ASSERT_TRUE(circle.IsValid());
    GEOMETRY_TEST_ASSERT_NEAR(circle.Area(), 28.274333882308138, 1e-9);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(circle.Bounds().MinPoint(), (Point2d{-2.0, -1.0}), 1e-12);
    ASSERT_TRUE(circle.ToPolygon(16).IsValid());

    const Rectangle2d rectangle(Point2d{0.0, 0.0}, 4.0, 2.0, 0.0);
    ASSERT_TRUE(rectangle.IsValid());
    GEOMETRY_TEST_ASSERT_NEAR(rectangle.Area(), 8.0, 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(rectangle.ToPolygon().OuterRing().PointAt(0), (Point2d{-2.0, -1.0}), 1e-12);

    const Ellipse2d ellipse(Point2d{0.0, 0.0}, 3.0, 2.0, 0.0);
    ASSERT_TRUE(ellipse.IsValid());
    GEOMETRY_TEST_ASSERT_NEAR(ellipse.Area(), 18.84955592153876, 1e-9);
    ASSERT_TRUE(ellipse.ToPolygon(32).IsValid());

    const Polyline2d path(
        {Point2d{0.0, 0.0}, Point2d{3.0, 0.0}, Point2d{3.0, 4.0}},
        PolylineClosure::Open);
    const Polyline2d sub = SubPolyline(path, 2.0, 5.0);
    ASSERT_TRUE(sub.IsValid());
    GEOMETRY_TEST_ASSERT_POINT_NEAR(sub.PointAt(0), (Point2d{2.0, 0.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(sub.PointAt(sub.PointCount() - 1), (Point2d{3.0, 2.0}), 1e-12);

    const Polygon2d square(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
            PolylineClosure::Closed));
    const auto cut = CutPolygon(square, LineSegment2d(Point2d{2.0, -1.0}, Point2d{2.0, 5.0}));
    ASSERT_TRUE(cut.success);
    ASSERT_EQ(cut.left.Count(), 1);
    ASSERT_EQ(cut.right.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(cut.left[0].Area(), 8.0, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(cut.right[0].Area(), 8.0, 1e-9);

    const Polygon2d donut(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{8.0, 0.0}, Point2d{8.0, 8.0}, Point2d{0.0, 8.0}},
            PolylineClosure::Closed),
        {Polyline2d(
            {Point2d{3.0, 3.0}, Point2d{3.0, 5.0}, Point2d{5.0, 5.0}, Point2d{5.0, 3.0}},
            PolylineClosure::Closed)});
    const auto donutCut = CutPolygon(donut, LineSegment2d(Point2d{4.0, -2.0}, Point2d{4.0, 10.0}));
    ASSERT_TRUE(donutCut.success);
    double leftArea = 0.0;
    double rightArea = 0.0;
    for (std::size_t i = 0; i < donutCut.left.Count(); ++i)
    {
        leftArea += donutCut.left[i].Area();
    }
    for (std::size_t i = 0; i < donutCut.right.Count(); ++i)
    {
        rightArea += donutCut.right[i].Area();
    }
    GEOMETRY_TEST_ASSERT_NEAR(leftArea, 30.0, 1e-6);
    GEOMETRY_TEST_ASSERT_NEAR(rightArea, 30.0, 1e-6);

    const MultiPolyline2d closedLines{
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{2.0, 0.0}, Point2d{2.0, 2.0}, Point2d{0.0, 2.0}},
            PolylineClosure::Closed),
        Polyline2d(
            {Point2d{5.0, 5.0}, Point2d{6.0, 5.0}, Point2d{6.0, 6.0}, Point2d{5.0, 6.0}},
            PolylineClosure::Closed)};
    const auto closedMulti = BuildMultiPolygonByLines(closedLines);
    ASSERT_EQ(closedMulti.Count(), 2);

    const MultiPolyline2d openSquareLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open)};
    const auto openSquare = BuildMultiPolygonByLines(openSquareLines);
    ASSERT_EQ(openSquare.Count(), 1);
    ASSERT_EQ(openSquare[0].HoleCount(), 0);
    GEOMETRY_TEST_ASSERT_NEAR(openSquare[0].Area(), 16.0, 1e-9);

    const MultiPolyline2d nestedOpenLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{6.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{6.0, 0.0}, Point2d{6.0, 6.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{6.0, 6.0}, Point2d{0.0, 6.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 6.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{2.0, 2.0}, Point2d{4.0, 2.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 2.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{2.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{2.0, 4.0}, Point2d{2.0, 2.0}}, PolylineClosure::Open)};
    const auto nested = BuildMultiPolygonByLines(nestedOpenLines);
    ASSERT_EQ(nested.Count(), 1);
    ASSERT_EQ(nested[0].HoleCount(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(nested[0].Area(), 32.0, 1e-9);

    const MultiPolyline2d branchedLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{2.0, -1.0}, Point2d{2.0, 2.0}}, PolylineClosure::Open)};
    const auto branched = BuildMultiPolygonByLines(branchedLines);
    ASSERT_EQ(branched.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(branched[0].Area(), 16.0, 1e-9);

    const MultiPolyline2d dirtyNearClosedLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.12}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{2.0, 4.0}, Point2d{2.0, 5.0}}, PolylineClosure::Open)};
    const auto dirtyNearClosed = BuildMultiPolygonByLines(dirtyNearClosedLines);
    ASSERT_EQ(dirtyNearClosed.Count(), 1);
    GEOMETRY_TEST_ASSERT_NEAR(dirtyNearClosed[0].Area(), 16.0, 1e-6);

    const MultiPolyline2d autoExtendLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 1.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.15, 1.0}, Point2d{-0.15, 0.25}}, PolylineClosure::Open)};
    const auto autoExtended = BuildMultiPolygonByLines(autoExtendLines);
    ASSERT_EQ(autoExtended.Count(), 1);
    ASSERT_GE(autoExtended[0].Area(), 15.5);
    ASSERT_LE(autoExtended[0].Area(), 16.5);

    const MultiPolyline2d ambiguousFakeLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.25}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.15, 0.25}, Point2d{-0.15, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.15, 0.0}, Point2d{0.15, 0.0}}, PolylineClosure::Open)};
    const auto ambiguousFake = BuildMultiPolygonByLines(ambiguousFakeLines);
    ASSERT_EQ(ambiguousFake.Count(), 1);
    ASSERT_EQ(ambiguousFake[0].HoleCount(), 0);
    ASSERT_GE(ambiguousFake[0].Area(), 15.5);
    ASSERT_LE(ambiguousFake[0].Area(), 16.5);

    const MultiPolyline2d branchHeavyAmbiguousLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{6.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{6.0, 0.0}, Point2d{6.0, 6.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{6.0, 6.0}, Point2d{0.0, 6.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 6.0}, Point2d{0.0, 0.4}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.2, 0.4}, Point2d{-0.2, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.2, 0.0}, Point2d{0.3, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{3.0, 6.0}, Point2d{3.0, 7.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 3.0}, Point2d{-1.0, 3.0}}, PolylineClosure::Open)};
    const auto branchHeavyAmbiguous = BuildMultiPolygonByLines(branchHeavyAmbiguousLines);
    ASSERT_EQ(branchHeavyAmbiguous.Count(), 1);
    ASSERT_EQ(branchHeavyAmbiguous[0].HoleCount(), 0);
    ASSERT_GE(branchHeavyAmbiguous[0].Area(), 35.0);
    ASSERT_LE(branchHeavyAmbiguous[0].Area(), 37.0);

    const MultiPolyline2d branchScoredAmbiguousLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.4}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.2, 0.4}, Point2d{-0.2, -0.2}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.2, -0.2}, Point2d{1.0, -0.2}}, PolylineClosure::Open),
        Polyline2d({Point2d{2.0, -1.0}, Point2d{2.0, 2.0}}, PolylineClosure::Open)};
    const auto branchScoredAmbiguous = BuildMultiPolygonByLines(branchScoredAmbiguousLines);
    ASSERT_EQ(branchScoredAmbiguous.Count(), 1);
    ASSERT_EQ(branchScoredAmbiguous[0].HoleCount(), 0);
    GEOMETRY_TEST_ASSERT_NEAR(branchScoredAmbiguous[0].Area(), 16.0, 1e-6);

    const Polygon2d noisyBoundary(
        Polyline2d(
            {Point2d{0.0, 0.0},
             Point2d{4.0, 0.0},
             Point2d{4.0, 2.0},
             Point2d{4.0, 2.000000001},
             Point2d{4.0, 4.0},
             Point2d{0.0, 4.0},
             Point2d{0.0, 2.000000001},
             Point2d{0.0, 2.0}},
            PolylineClosure::Closed));
    const Polygon2d normalizedByLines = NormalizePolygonByLines(noisyBoundary);
    ASSERT_TRUE(normalizedByLines.IsValid());
    GEOMETRY_TEST_ASSERT_NEAR(normalizedByLines.Area(), 16.0, 1e-6);
}



