#include "sdk/GeometrySearchPoly.h"

#include <cmath>

#include "sdk/GeometryMetrics.h"
#include "sdk/GeometryPathOps.h"
#include "sdk/GeometryRelation.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] SearchPolyCandidate2d MakeCandidate(const Polygon2d& polygon)
{
    return SearchPolyCandidate2d{
        polygon,
        std::abs(Area(polygon)),
        polygon.HoleCount(),
    };
}
} // namespace

SearchPolyResult2d SearchPolygons(const MultiPolyline2d& lines, SearchPolyOptions2d options)
{
    SearchPolyResult2d result;
    result.usedAutoClose = options.autoClose;
    result.usedAutoExtend = options.autoExtend;
    result.usedSyntheticEdges = options.allowFakeEdges;

    if (options.epsilon <= 0.0 || lines.Count() == 0)
    {
        result.issue = SearchPolyIssue2d::InvalidInput;
        return result;
    }

    result.polygons = BuildMultiPolygonByLines(lines, options.epsilon);
    if (result.polygons.Count() == 0)
    {
        result.issue = SearchPolyIssue2d::NoClosedPolygonFound;
        return result;
    }

    result.candidates.reserve(result.polygons.Count());
    for (std::size_t i = 0; i < result.polygons.Count(); ++i)
    {
        if (result.polygons[i].IsValid())
        {
            result.candidates.push_back(MakeCandidate(result.polygons[i]));
        }
    }

    if (result.candidates.empty())
    {
        result.issue = SearchPolyIssue2d::NoClosedPolygonFound;
        result.polygons = {};
        return result;
    }

    return result;
}

std::optional<SearchPolyCandidate2d> SearchPolygonContainingPoint(
    const MultiPolyline2d& lines,
    const Point2d& point,
    SearchPolyOptions2d options)
{
    const SearchPolyResult2d result = SearchPolygons(lines, options);
    if (!result.IsSuccess() || !point.IsValid())
    {
        return std::nullopt;
    }

    for (const SearchPolyCandidate2d& candidate : result.candidates)
    {
        const PointContainment2d containment = LocatePoint(point, candidate.polygon, options.epsilon);
        if (containment != PointContainment2d::Outside)
        {
            return candidate;
        }
    }

    return std::nullopt;
}
} // namespace geometry::sdk
