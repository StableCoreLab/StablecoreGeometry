#include <gtest/gtest.h>
#include <memory>

#include "sdk/BoxTree2d.h"
#include "sdk/KDTree2d.h"
#include "sdk/SegmentSearch2d.h"
#include "sdk/Topology.h"
#include "support/GeometryTestSupport.h"

using Geometry::Sdk::ArcSegment2d;
using Geometry::Sdk::Box2d;
using Geometry::Sdk::BoxTree2d;
using Geometry::Sdk::KDTree2d;
using Geometry::Sdk::SegmentSearch2d;
using Geometry::Sdk::LineSegment2d;
using Geometry::Sdk::MultiPolygon2d;
using Geometry::Sdk::Point2d;
using Geometry::Sdk::Polygon2d;
using Geometry::Sdk::PolygonContainment2d;
using Geometry::Sdk::Polyline2d;
using Geometry::Sdk::PolylineClosure;

TEST(TopologyIndexingTest, CoversCurrentCapabilities)
{
    BoxTree2d boxTree;
    boxTree.Add(1, Box2d::FromMinMax(Point2d{0.0, 0.0}, Point2d{2.0, 2.0}));
    boxTree.Add(2, Box2d::FromMinMax(Point2d{3.0, 3.0}, Point2d{4.0, 4.0}));
    ASSERT_EQ(boxTree.Query(Box2d::FromMinMax(Point2d{1.0, 1.0}, Point2d{3.1, 3.1})).size(), 2);

    KDTree2d kdTree;
    kdTree.Add(7, Point2d{1.0, 1.0});
    kdTree.Add(8, Point2d{3.0, 3.0});
    ASSERT_EQ(kdTree.Query(Point2d{1.0, 1.0}).size(), 1);
    ASSERT_EQ(kdTree.Nearest(Point2d{2.9, 3.1})->id, 8);

    SegmentSearch2d search;
    const std::size_t lineId = search.Add(LineSegment2d(Point2d{0.0, 0.0}, Point2d{4.0, 0.0}));
    const std::size_t arcId = search.Add(ArcSegment2d(Point2d{0.0, 0.0}, 2.0, 0.0, 1.0));
    (void)arcId;
    ASSERT_GE(search.QueryWithinDistance(Point2d{2.0, 0.5}, 1.0).size(), 1);
    ASSERT_EQ(search.Nearest(Point2d{2.0, -0.2})->id, lineId);

    const Polygon2d outer(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 10.0}, Point2d{0.0, 10.0}},
            PolylineClosure::Closed));
    const Polygon2d inner(
        Polyline2d(
            {Point2d{2.0, 2.0}, Point2d{4.0, 2.0}, Point2d{4.0, 4.0}, Point2d{2.0, 4.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Geometry::Sdk::Relate(outer, inner), PolygonContainment2d::FirstContainsSecond);
    ASSERT_EQ(Geometry::Sdk::Relate(inner, outer), PolygonContainment2d::SecondContainsFirst);
    const auto topology = Geometry::Sdk::BuildPolygonTopology(MultiPolygon2d{{outer, inner}});
    ASSERT_EQ(topology.Roots().size(), 1); const auto duplicateTopology = Geometry::Sdk::BuildPolygonTopology(MultiPolygon2d{outer, outer}); ASSERT_EQ(duplicateTopology.Roots().size(), 1); ASSERT_EQ(duplicateTopology.ParentOf(1), 0);

    const Polygon2d touching(
        Polyline2d(
            {Point2d{10.0, 2.0}, Point2d{12.0, 2.0}, Point2d{12.0, 4.0}, Point2d{10.0, 4.0}},
            PolylineClosure::Closed));
    const PolygonContainment2d touchingRelation = Geometry::Sdk::Relate(outer, touching);
    ASSERT_TRUE(touchingRelation == PolygonContainment2d::Touching ||
        touchingRelation == PolygonContainment2d::Intersecting);
    ASSERT_EQ(Geometry::Sdk::Relate(outer, outer), PolygonContainment2d::Equal);
    ASSERT_TRUE(Geometry::Sdk::Contains(outer, outer));

    const Polygon2d intersecting(
        Polyline2d(
            {Point2d{8.0, 8.0}, Point2d{12.0, 8.0}, Point2d{12.0, 12.0}, Point2d{8.0, 12.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Geometry::Sdk::Relate(outer, intersecting), PolygonContainment2d::Intersecting);

    const Polygon2d sharedEdgeTouch(
        Polyline2d(
            {Point2d{10.0, 0.0}, Point2d{12.0, 0.0}, Point2d{12.0, 10.0}, Point2d{10.0, 10.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Geometry::Sdk::Relate(outer, sharedEdgeTouch), PolygonContainment2d::Touching);

    const Polygon2d overlapWithInterior(
        Polyline2d(
            {Point2d{8.0, 0.0}, Point2d{12.0, 0.0}, Point2d{12.0, 6.0}, Point2d{8.0, 6.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Geometry::Sdk::Relate(outer, overlapWithInterior), PolygonContainment2d::Intersecting);

    const Polygon2d sameOuterWithHole(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 10.0}, Point2d{0.0, 10.0}},
            PolylineClosure::Closed),
        {Polyline2d(
            {Point2d{3.0, 3.0}, Point2d{3.0, 7.0}, Point2d{7.0, 7.0}, Point2d{7.0, 3.0}},
            PolylineClosure::Closed)});
    ASSERT_EQ(Geometry::Sdk::Relate(outer, sameOuterWithHole), PolygonContainment2d::FirstContainsSecond);

    const Polygon2d sameOuterWithDifferentHole(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 10.0}, Point2d{0.0, 10.0}},
            PolylineClosure::Closed),
        {Polyline2d(
            {Point2d{1.0, 1.0}, Point2d{1.0, 2.0}, Point2d{2.0, 2.0}, Point2d{2.0, 1.0}},
            PolylineClosure::Closed)});
    ASSERT_EQ(Geometry::Sdk::Relate(sameOuterWithHole, sameOuterWithDifferentHole), PolygonContainment2d::Intersecting);

    const Polygon2d boundaryOverlappingContained(
        Polyline2d(
            {Point2d{2.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 10.0}, Point2d{2.0, 10.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Geometry::Sdk::Relate(outer, boundaryOverlappingContained), PolygonContainment2d::FirstContainsSecond);

    const Polygon2d noisyOuter(
        Polyline2d(
            {Point2d{0.0, 0.0},
             Point2d{10.0, 0.0},
             Point2d{10.0, 5.0},
             Point2d{10.0, 5.000000001},
             Point2d{10.0, 10.0},
             Point2d{0.0, 10.0},
             Point2d{0.0, 5.000000001},
             Point2d{0.0, 5.0}},
            PolylineClosure::Closed));
    const Polygon2d smallInner(
        Polyline2d(
            {Point2d{2.0, 2.0}, Point2d{3.0, 2.0}, Point2d{3.0, 3.0}, Point2d{2.0, 3.0}},
            PolylineClosure::Closed));
    const auto normalizedTopology = Geometry::Sdk::BuildPolygonTopology(MultiPolygon2d{noisyOuter, smallInner});
    ASSERT_EQ(normalizedTopology.Roots().size(), 1);
    ASSERT_EQ(normalizedTopology.ParentOf(1), 0);
    ASSERT_EQ(Geometry::Sdk::Relate(noisyOuter, smallInner), PolygonContainment2d::FirstContainsSecond);
}
