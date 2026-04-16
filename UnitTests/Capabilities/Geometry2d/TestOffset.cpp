#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <vector>

#include "Core/Offset.h"
#include "Core/Relation.h"
#include "Core/ShapeOps.h"
#include "Support/GeometryTestSupport.h"



using Geometry::Sdk::ArcSegment2d;
using Geometry::Sdk::LineSegment2d;
using Geometry::Sdk::MultiPolygon2d;
using Geometry::Sdk::Point2d;
using Geometry::Sdk::PointContainment2d;
using Geometry::Sdk::Polygon2d;
using Geometry::Sdk::Polyline2d;
using Geometry::Sdk::PolylineClosure;

TEST(OffsetTest, CoversCurrentCapabilities)
{
    const LineSegment2d line(Point2d{0.0, 0.0}, Point2d{4.0, 0.0});
    const LineSegment2d shiftedLine = Offset(line, 2.0);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(shiftedLine.startPoint, (Point2d{0.0, 2.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(shiftedLine.endPoint, (Point2d{4.0, 2.0}), 1e-12);

    const ArcSegment2d ccwArc(
        Point2d{0.0, 0.0},
        5.0,
        0.0,
        std::acos(-1.0) * 0.5);
    const ArcSegment2d insetArc = Offset(ccwArc, 1.0);
    GEOMETRY_TEST_ASSERT_NEAR(insetArc.radius, 4.0, 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(insetArc.StartPoint(), (Point2d{4.0, 0.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(insetArc.EndPoint(), (Point2d{0.0, 4.0}), 1e-12);

    const Polyline2d openPath(
        {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 3.0}},
        PolylineClosure::Open);
    const Polyline2d openOffset = Offset(openPath, 1.0);
    ASSERT_TRUE(openOffset.IsValid());
    ASSERT_EQ(openOffset.PointCount(), 3);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openOffset.PointAt(0), (Point2d{0.0, 1.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openOffset.PointAt(1), (Point2d{3.0, 1.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(openOffset.PointAt(2), (Point2d{3.0, 3.0}), 1e-12);

    const Polyline2d ccwRing(
        {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
        PolylineClosure::Closed);
    const Polygon2d polygon(ccwRing);
    const Polygon2d grownPolygon = Offset(polygon, 1.0);
    ASSERT_TRUE(grownPolygon.IsValid());
    GEOMETRY_TEST_ASSERT_POINT_NEAR(grownPolygon.OuterRing().PointAt(0), (Point2d{-1.0, -1.0}), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(grownPolygon.OuterRing().PointAt(2), (Point2d{5.0, 5.0}), 1e-12);

    const Polyline2d holeRing(
        {Point2d{1.0, 1.0}, Point2d{1.0, 3.0}, Point2d{3.0, 3.0}, Point2d{3.0, 1.0}},
        PolylineClosure::Closed);
    const Polygon2d polygonWithHole(ccwRing, {holeRing});
    const Polygon2d offsetWithHole = Offset(polygonWithHole, 0.25);
    ASSERT_TRUE(offsetWithHole.IsValid());
    ASSERT_GT(offsetWithHole.Area(), polygonWithHole.Area());
    GEOMETRY_TEST_ASSERT_POINT_NEAR(offsetWithHole.OuterRing().PointAt(0), (Point2d{-0.25, -0.25}), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(offsetWithHole.OuterRing().PointAt(2), (Point2d{4.25, 4.25}), 1e-12);

    const Polygon2d concave(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{5.0, 0.0}, Point2d{5.0, 1.0}, Point2d{2.0, 1.0}, Point2d{2.0, 4.0}, Point2d{0.0, 4.0}},
            PolylineClosure::Closed));
    const Polygon2d concaveOffset = Offset(concave, 0.5);
    ASSERT_TRUE(concaveOffset.IsValid());
    ASSERT_GT(concaveOffset.Area(), concave.Area());

    const MultiPolygon2d disjoint{
        Polygon2d(Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{2.0, 0.0}, Point2d{2.0, 2.0}, Point2d{0.0, 2.0}},
            PolylineClosure::Closed)),
        Polygon2d(Polyline2d(
            {Point2d{4.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 2.0}, Point2d{4.0, 2.0}},
            PolylineClosure::Closed))};
    const MultiPolygon2d expandedDisjoint = Offset(disjoint, 0.5);
    ASSERT_EQ(expandedDisjoint.Count(), 2);

    const Polygon2d clockwiseOuter(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{0.0, 4.0}, Point2d{4.0, 4.0}, Point2d{4.0, 0.0}},
            PolylineClosure::Closed));
    const Polygon2d clockwiseExpanded = Offset(clockwiseOuter, 0.5);
    ASSERT_TRUE(clockwiseExpanded.IsValid());
    ASSERT_GT(clockwiseExpanded.Area(), clockwiseOuter.Area());

    const Polygon2d narrowDonut(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 6.0}, Point2d{0.0, 6.0}},
            PolylineClosure::Closed),
        {Polyline2d(
            {Point2d{3.5, 2.0}, Point2d{3.5, 4.0}, Point2d{6.5, 4.0}, Point2d{6.5, 2.0}},
            PolylineClosure::Closed)});
    const Polygon2d inwardRecovered = Offset(narrowDonut, -0.75);
    ASSERT_TRUE(inwardRecovered.IsValid());
    ASSERT_LT(inwardRecovered.Area(), narrowDonut.Area());
    for (std::size_t i = 0; i < inwardRecovered.OuterRing().PointCount(); ++i)
    {
        const PointContainment2d containment =
            LocatePoint(inwardRecovered.OuterRing().PointAt(i), narrowDonut);
        ASSERT_NE(containment, PointContainment2d::Outside);
    }

    const MultiPolygon2d multiWithHole{
        narrowDonut,
        Polygon2d(
            Polyline2d(
                {Point2d{14.0, 0.0}, Point2d{20.0, 0.0}, Point2d{20.0, 5.0}, Point2d{14.0, 5.0}},
                PolylineClosure::Closed))};
    const MultiPolygon2d inwardMultiRecovered = Offset(multiWithHole, -0.8);
    ASSERT_FALSE(inwardMultiRecovered.IsEmpty());
    for (std::size_t polygonIndex = 0; polygonIndex < inwardMultiRecovered.Count(); ++polygonIndex)
    {
        const Polygon2d& recovered = inwardMultiRecovered[polygonIndex];
        for (std::size_t pointIndex = 0; pointIndex < recovered.OuterRing().PointCount(); ++pointIndex)
        {
            const Point2d point = recovered.OuterRing().PointAt(pointIndex);
            const bool inFirst = LocatePoint(point, narrowDonut) != PointContainment2d::Outside;
            const bool inSecond =
                LocatePoint(point, multiWithHole[1]) != PointContainment2d::Outside;
            ASSERT_TRUE(inFirst || inSecond);
        }
    }
}

TEST(OffsetTest, PreservesSinglePolygonHoleSemanticsAfterRebuild)
{
    const Polygon2d source(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{4.0, 0.0}, Point2d{4.0, 4.0}, Point2d{0.0, 4.0}},
            PolylineClosure::Closed),
        {Polyline2d(
            {Point2d{1.0, 1.0}, Point2d{1.0, 3.0}, Point2d{3.0, 3.0}, Point2d{3.0, 1.0}},
            PolylineClosure::Closed)});

    const Polygon2d outward = Offset(source, 0.5);
    ASSERT_TRUE(outward.IsValid());
    ASSERT_EQ(outward.HoleCount(), 1);
    ASSERT_GT(outward.Area(), source.Area());
    GEOMETRY_TEST_ASSERT_POINT_NEAR(outward.OuterRing().PointAt(0), (Point2d{-0.5, -0.5}), 1e-9);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(outward.OuterRing().PointAt(2), (Point2d{4.5, 4.5}), 1e-9);

    const Polyline2d recoveredHole = outward.HoleAt(0);
    GEOMETRY_TEST_ASSERT_NEAR(recoveredHole.Bounds().MinPoint().x, 1.5, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(recoveredHole.Bounds().MinPoint().y, 1.5, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(recoveredHole.Bounds().MaxPoint().x, 2.5, 1e-9);
    GEOMETRY_TEST_ASSERT_NEAR(recoveredHole.Bounds().MaxPoint().y, 2.5, 1e-9);

    const Polygon2d inward = Offset(source, -0.4);
    ASSERT_TRUE(inward.IsValid());
    ASSERT_EQ(inward.HoleCount(), 1);
    ASSERT_LT(inward.Area(), source.Area());
    for (std::size_t i = 0; i < inward.OuterRing().PointCount(); ++i)
    {
        ASSERT_NE(LocatePoint(inward.OuterRing().PointAt(i), source), PointContainment2d::Outside);
    }
}

TEST(OffsetTest, RecoversRepresentativeReverseEdgeSelfIntersectionCase)
{
    const Polygon2d source(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 1.0}, Point2d{2.5, 1.0}, Point2d{2.5, 4.0}, Point2d{6.0, 4.0},
             Point2d{6.0, 5.0}, Point2d{0.0, 5.0}},
            PolylineClosure::Closed));

    const Polygon2d outward = Offset(source, 0.5);
    ASSERT_TRUE(outward.IsValid());
    ASSERT_GT(outward.Area(), source.Area());

    const Polygon2d inward = Offset(source, -0.4);
    ASSERT_TRUE(inward.IsValid());
    ASSERT_LT(inward.Area(), source.Area());
    for (std::size_t i = 0; i < inward.OuterRing().PointCount(); ++i)
    {
        ASSERT_NE(LocatePoint(inward.OuterRing().PointAt(i), source), PointContainment2d::Outside);
    }
}

TEST(OffsetTest, SupportsNarrowBridgeSplitViaMultiPolygonOffsetApi)
{
    const Polygon2d source(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{2.0, 0.0}, Point2d{2.0, 0.8}, Point2d{4.0, 0.8}, Point2d{4.0, 0.0}, Point2d{6.0, 0.0},
             Point2d{6.0, 2.0}, Point2d{4.0, 2.0}, Point2d{4.0, 1.2}, Point2d{2.0, 1.2}, Point2d{2.0, 2.0}, Point2d{0.0, 2.0}},
            PolylineClosure::Closed));

    const MultiPolygon2d split = OffsetToMultiPolygon(source, -0.35);
    ASSERT_FALSE(split.IsEmpty());
    ASSERT_GE(split.Count(), 2);
    for (std::size_t polygonIndex = 0; polygonIndex < split.Count(); ++polygonIndex)
    {
        const Polygon2d& piece = split[polygonIndex];
        ASSERT_TRUE(piece.IsValid());
        for (std::size_t pointIndex = 0; pointIndex < piece.OuterRing().PointCount(); ++pointIndex)
        {
            ASSERT_NE(LocatePoint(piece.OuterRing().PointAt(pointIndex), source), PointContainment2d::Outside);
        }
    }
}








