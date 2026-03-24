#include "sdk/GeometryTopology.h"

#include <sstream>

#include "sdk/GeometryRelation.h"

namespace geometry::sdk
{
PolygonTopology2d::PolygonTopology2d(const MultiPolygon2d& polygons, double eps)
{
    Build(polygons, eps);
}

bool PolygonTopology2d::Build(const MultiPolygon2d& polygons, double eps)
{
    polygons_ = polygons;
    nodes_.clear();
    roots_.clear();
    nodes_.resize(polygons.Count());
    for (std::size_t i = 0; i < polygons.Count(); ++i)
    {
        nodes_[i].polygonIndex = i;
        std::size_t bestParent = static_cast<std::size_t>(-1);
        double bestArea = 0.0;
        for (std::size_t j = 0; j < polygons.Count(); ++j)
        {
            if (i == j)
            {
                continue;
            }
            if (Contains(polygons[j], polygons[i], eps))
            {
                const double area = Area(polygons[j]);
                if (bestParent == static_cast<std::size_t>(-1) || area < bestArea)
                {
                    bestParent = j;
                    bestArea = area;
                }
            }
        }
        nodes_[i].parentIndex = bestParent;
    }

    for (std::size_t i = 0; i < nodes_.size(); ++i)
    {
        if (nodes_[i].parentIndex == static_cast<std::size_t>(-1))
        {
            roots_.push_back(i);
        }
        else
        {
            nodes_[nodes_[i].parentIndex].children.push_back(i);
        }
    }
    return true;
}

std::size_t PolygonTopology2d::PolygonCount() const { return polygons_.Count(); }
bool PolygonTopology2d::IsEmpty() const { return polygons_.IsEmpty(); }
bool PolygonTopology2d::IsValid() const { return nodes_.size() == polygons_.Count(); }
const MultiPolygon2d& PolygonTopology2d::Polygons() const { return polygons_; }
const PolygonTopologyNode2d& PolygonTopology2d::Node(std::size_t index) const { return nodes_.at(index); }
const std::vector<std::size_t>& PolygonTopology2d::Roots() const { return roots_; }
const std::vector<std::size_t>& PolygonTopology2d::ChildrenOf(std::size_t index) const { return nodes_.at(index).children; }
std::size_t PolygonTopology2d::ParentOf(std::size_t index) const { return nodes_.at(index).parentIndex; }

std::string PolygonTopology2d::DebugString() const
{
    std::ostringstream stream;
    stream << "PolygonTopology2d{polygonCount=" << PolygonCount() << ", rootCount=" << roots_.size() << "}";
    return stream.str();
}

bool ContainsPoint(const Polyline2d& ring, const Point2d& point, double eps)
{
    const PointContainment2d containment = LocatePoint(point, ring, eps);
    return containment == PointContainment2d::Inside || containment == PointContainment2d::OnBoundary;
}

bool ContainsPoint(const Polygon2d& polygon, const Point2d& point, double eps)
{
    const PointContainment2d containment = LocatePoint(point, polygon, eps);
    return containment == PointContainment2d::Inside || containment == PointContainment2d::OnBoundary;
}

bool Contains(const Polygon2d& outer, const Polygon2d& inner, double eps)
{
    for (std::size_t i = 0; i < inner.OuterRing().PointCount(); ++i)
    {
        const PointContainment2d containment = LocatePoint(inner.OuterRing().PointAt(i), outer, eps);
        if (containment == PointContainment2d::Outside)
        {
            return false;
        }
    }
    return true;
}

PolygonContainment2d Relate(const Polygon2d& first, const Polygon2d& second, double eps)
{
    if (Contains(first, second, eps) && Contains(second, first, eps))
    {
        return PolygonContainment2d::Equal;
    }
    if (Contains(first, second, eps))
    {
        return PolygonContainment2d::FirstContainsSecond;
    }
    if (Contains(second, first, eps))
    {
        return PolygonContainment2d::SecondContainsFirst;
    }
    return PolygonContainment2d::Disjoint;
}

PolygonTopology2d BuildPolygonTopology(const MultiPolygon2d& polygons, double eps)
{
    return PolygonTopology2d(polygons, eps);
}
} // namespace geometry::sdk
