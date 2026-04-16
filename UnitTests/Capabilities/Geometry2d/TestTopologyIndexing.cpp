#include <gtest/gtest.h>
#include <memory>

#include "Geometry2d/BoxTree2d.h"
#include "Geometry2d/KDTree2d.h"
#include "Geometry2d/SegmentSearch2d.h"
#include "Brep/Topology.h"
#include "Support/GeometryTestSupport.h"



using Geometry::ArcSegment2d;
using Geometry::Box2d;
using Geometry::BoxTree2d;
using Geometry::KDTree2d;
using Geometry::SegmentSearch2d;
using Geometry::LineSegment2d;
using Geometry::MultiPolygon2d;
using Geometry::Point2d;
using Geometry::Polygon2d;
using Geometry::PolygonContainment2d;
using Geometry::Polyline2d;
using Geometry::PolylineClosure;

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
    ASSERT_EQ(Relate(outer, inner), PolygonContainment2d::FirstContainsSecond);
    ASSERT_EQ(Relate(inner, outer), PolygonContainment2d::SecondContainsFirst);
    const auto topology = BuildPolygonTopology(MultiPolygon2d{{outer, inner}});
    ASSERT_EQ(topology.Roots().size(), 1); const auto duplicateTopology = BuildPolygonTopology(MultiPolygon2d{outer, outer}); ASSERT_EQ(duplicateTopology.Roots().size(), 1); ASSERT_EQ(duplicateTopology.ParentOf(1), 0);

    const Polygon2d touching(
        Polyline2d(
            {Point2d{10.0, 2.0}, Point2d{12.0, 2.0}, Point2d{12.0, 4.0}, Point2d{10.0, 4.0}},
            PolylineClosure::Closed));
    const PolygonContainment2d touchingRelation = Relate(outer, touching);
    ASSERT_TRUE(touchingRelation == PolygonContainment2d::Touching ||
        touchingRelation == PolygonContainment2d::Intersecting);
    ASSERT_EQ(Relate(outer, outer), PolygonContainment2d::Equal);
    ASSERT_TRUE(Contains(outer, outer));

    const Polygon2d intersecting(
        Polyline2d(
            {Point2d{8.0, 8.0}, Point2d{12.0, 8.0}, Point2d{12.0, 12.0}, Point2d{8.0, 12.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Relate(outer, intersecting), PolygonContainment2d::Intersecting);

    const Polygon2d sharedEdgeTouch(
        Polyline2d(
            {Point2d{10.0, 0.0}, Point2d{12.0, 0.0}, Point2d{12.0, 10.0}, Point2d{10.0, 10.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Relate(outer, sharedEdgeTouch), PolygonContainment2d::Touching);

    const Polygon2d overlapWithInterior(
        Polyline2d(
            {Point2d{8.0, 0.0}, Point2d{12.0, 0.0}, Point2d{12.0, 6.0}, Point2d{8.0, 6.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Relate(outer, overlapWithInterior), PolygonContainment2d::Intersecting);

    const Polygon2d sameOuterWithHole(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 10.0}, Point2d{0.0, 10.0}},
            PolylineClosure::Closed),
        {Polyline2d(
            {Point2d{3.0, 3.0}, Point2d{3.0, 7.0}, Point2d{7.0, 7.0}, Point2d{7.0, 3.0}},
            PolylineClosure::Closed)});
    ASSERT_EQ(Relate(outer, sameOuterWithHole), PolygonContainment2d::FirstContainsSecond);

    const Polygon2d sameOuterWithDifferentHole(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 10.0}, Point2d{0.0, 10.0}},
            PolylineClosure::Closed),
        {Polyline2d(
            {Point2d{1.0, 1.0}, Point2d{1.0, 2.0}, Point2d{2.0, 2.0}, Point2d{2.0, 1.0}},
            PolylineClosure::Closed)});
    ASSERT_EQ(Relate(sameOuterWithHole, sameOuterWithDifferentHole), PolygonContainment2d::Intersecting);

    const Polygon2d boundaryOverlappingContained(
        Polyline2d(
            {Point2d{2.0, 0.0}, Point2d{6.0, 0.0}, Point2d{6.0, 10.0}, Point2d{2.0, 10.0}},
            PolylineClosure::Closed));
    ASSERT_EQ(Relate(outer, boundaryOverlappingContained), PolygonContainment2d::FirstContainsSecond);

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
    const auto normalizedTopology = BuildPolygonTopology(MultiPolygon2d{noisyOuter, smallInner});
    ASSERT_EQ(normalizedTopology.Roots().size(), 1);
    ASSERT_EQ(normalizedTopology.ParentOf(1), 0);
    ASSERT_EQ(Relate(noisyOuter, smallInner), PolygonContainment2d::FirstContainsSecond);
}







