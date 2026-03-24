#include <cassert>
#include <memory>

#include "sdk/GeometryBoxTree.h"
#include "sdk/GeometryKDTree.h"
#include "sdk/GeometrySegmentSearch.h"
#include "sdk/GeometryTopology.h"
#include "support/GeometryTestSupport.h"

using geometry::sdk::ArcSegment2d;
using geometry::sdk::Box2d;
using geometry::sdk::GeometryBoxTree2d;
using geometry::sdk::GeometryKDTree2d;
using geometry::sdk::GeometrySegmentSearch2d;
using geometry::sdk::LineSegment2d;
using geometry::sdk::MultiPolygon2d;
using geometry::sdk::Point2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::PolygonContainment2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;

int main()
{
    GeometryBoxTree2d boxTree;
    boxTree.Add(1, Box2d::FromMinMax(Point2d{0.0, 0.0}, Point2d{2.0, 2.0}));
    boxTree.Add(2, Box2d::FromMinMax(Point2d{3.0, 3.0}, Point2d{4.0, 4.0}));
    assert(boxTree.Query(Box2d::FromMinMax(Point2d{1.0, 1.0}, Point2d{3.1, 3.1})).size() == 2);

    GeometryKDTree2d kdTree;
    kdTree.Add(7, Point2d{1.0, 1.0});
    kdTree.Add(8, Point2d{3.0, 3.0});
    assert(kdTree.Query(Point2d{1.0, 1.0}).size() == 1);
    assert(kdTree.Nearest(Point2d{2.9, 3.1})->id == 8);

    GeometrySegmentSearch2d search;
    const std::size_t lineId = search.Add(LineSegment2d(Point2d{0.0, 0.0}, Point2d{4.0, 0.0}));
    const std::size_t arcId = search.Add(ArcSegment2d(Point2d{0.0, 0.0}, 2.0, 0.0, 1.0));
    (void)arcId;
    assert(search.QueryWithinDistance(Point2d{2.0, 0.5}, 1.0).size() >= 1);
    assert(search.Nearest(Point2d{2.0, -0.2})->id == lineId);

    const Polygon2d outer(
        Polyline2d(
            {Point2d{0.0, 0.0}, Point2d{10.0, 0.0}, Point2d{10.0, 10.0}, Point2d{0.0, 10.0}},
            PolylineClosure::Closed));
    const Polygon2d inner(
        Polyline2d(
            {Point2d{2.0, 2.0}, Point2d{4.0, 2.0}, Point2d{4.0, 4.0}, Point2d{2.0, 4.0}},
            PolylineClosure::Closed));
    assert(geometry::sdk::Relate(outer, inner) == PolygonContainment2d::FirstContainsSecond);
    const auto topology = geometry::sdk::BuildPolygonTopology(MultiPolygon2d{{outer, inner}});
    assert(topology.Roots().size() == 1);

    return 0;
}
