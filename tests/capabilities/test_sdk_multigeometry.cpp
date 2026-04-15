#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <vector>

#include "sdk/Geometry.h"

using Geometry::Point2d;
using Geometry::Sdk::ArcSegment2d;
using Geometry::Sdk::Box2d;
using Geometry::Sdk::BoxTree2d;
using Geometry::Sdk::KDTree2d;
using Geometry::Sdk::SegmentSearch2d;
using Geometry::Sdk::LineSegment2d;
using Geometry::Sdk::MultiPolygon2d;
using Geometry::Sdk::MultiPolyline2d;
using Geometry::Sdk::Polygon2d;
using Geometry::Sdk::PolygonContainment2d;
using Geometry::Sdk::PolygonTopology2d;
using Geometry::Sdk::Polyline2d;
using Geometry::Sdk::PolylineClosure;

namespace
{
constexpr double kPi = 3.141592653589793238462643383279502884;
}

TEST(SdkMultigeometryTest, CoversCurrentCapabilities)
{
    const Polyline2d outerRing(
        {Point2d{0.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 6.0}, Point2d{0.0, 6.0}},
        PolylineClosure::Closed);
    const Polyline2d innerRing(
        {Point2d{1.0, 1.0}, Point2d{2.0, 1.0}, Point2d{2.0, 2.0}, Point2d{1.0, 2.0}},
        PolylineClosure::Closed);
    const Polyline2d siblingRing(
        {Point2d{8.0, 8.0}, Point2d{10.0, 8.0}, Point2d{10.0, 10.0}, Point2d{8.0, 10.0}},
        PolylineClosure::Closed);

    MultiPolyline2d multiPolyline({outerRing, innerRing});
    ASSERT_EQ(multiPolyline.Count(), 2);
    ASSERT_TRUE(multiPolyline.IsValid());
    ASSERT_EQ(multiPolyline.PointCount(), outerRing.PointCount() + innerRing.PointCount());
    ASSERT_TRUE(multiPolyline.Bounds().IsValid());

    const Polygon2d outerPolygon(outerRing);
    const Polygon2d innerPolygon(innerRing);
    const Polygon2d siblingPolygon(siblingRing);
    MultiPolygon2d multiPolygon({outerPolygon, innerPolygon, siblingPolygon});

    ASSERT_EQ(multiPolygon.Count(), 3);
    ASSERT_TRUE(multiPolygon.IsValid());
    ASSERT_EQ(multiPolygon.HoleCount(), 0);

    ASSERT_TRUE(Geometry::Sdk::ContainsPoint(outerPolygon, Point2d{0.5, 0.5}));
    ASSERT_FALSE(Geometry::Sdk::ContainsPoint(outerPolygon, Point2d{8.5, 8.5}));
    ASSERT_TRUE(Geometry::Sdk::Contains(outerPolygon, innerPolygon));
    ASSERT_EQ(Geometry::Sdk::Relate(outerPolygon, innerPolygon), PolygonContainment2d::FirstContainsSecond);

    PolygonTopology2d topology = Geometry::Sdk::BuildPolygonTopology(multiPolygon);
    ASSERT_EQ(topology.Count(), 3);
    ASSERT_EQ(topology.Roots().size(), 2);
    ASSERT_EQ(topology.ParentOf(1), 0);
    ASSERT_EQ(topology.ChildrenOf(0).size(), 1);
    ASSERT_TRUE(topology.IsValid());

    BoxTree2d boxTree;
    boxTree.Add(1, Box2d::FromMinMax(Point2d{0.0, 0.0}, Point2d{2.0, 2.0}));
    boxTree.Add(2, Box2d::FromMinMax(Point2d{1.0, 1.0}, Point2d{3.0, 3.0}));
    boxTree.Add(3, Box2d::FromMinMax(Point2d{5.0, 5.0}, Point2d{6.0, 6.0}));
    const auto overlaps = boxTree.Query(Box2d::FromMinMax(Point2d{1.5, 1.5}, Point2d{2.5, 2.5}));
    ASSERT_EQ(overlaps.size(), 2);
    ASSERT_EQ(boxTree.QueryContaining(Point2d{1.0, 1.0}).size(), 2);
    ASSERT_TRUE(boxTree.Contains(3));

    KDTree2d kdTree;
    kdTree.Add(1, Point2d{0.0, 0.0});
    kdTree.Add(2, Point2d{1.0, 1.0});
    kdTree.Add(3, Point2d{5.0, 5.0});
    const auto exact = kdTree.Query(Point2d{1.0, 1.0});
    ASSERT_EQ(exact.size(), 1);
    ASSERT_EQ(exact[0], 2);
    const auto nearest = kdTree.Nearest(Point2d{1.2, 1.2});
    ASSERT_TRUE(nearest.has_value());
    ASSERT_EQ(nearest->id, 2);

    SegmentSearch2d segmentSearch;
    const std::size_t lineId = segmentSearch.Add(LineSegment2d(Point2d{0.0, 0.0}, Point2d{4.0, 0.0}));
    const std::size_t arcId = segmentSearch.Add(ArcSegment2d(
        Point2d{4.0, 0.0},
        1.0,
        kPi / 2.0,
        kPi / 2.0));
    ASSERT_TRUE(segmentSearch.Contains(lineId));
    ASSERT_TRUE(segmentSearch.Contains(arcId));
    ASSERT_TRUE(segmentSearch.QueryIntersecting(Box2d::FromMinMax(Point2d{10.0, 10.0}, Point2d{11.0, 11.0})).empty());
    const auto nearestSegment = segmentSearch.Nearest(Point2d{2.0, 1.0});
    ASSERT_TRUE(nearestSegment.has_value());
    ASSERT_EQ(nearestSegment->id, lineId);
}


