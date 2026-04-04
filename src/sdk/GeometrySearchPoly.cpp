#include "sdk/GeometrySearchPoly.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "sdk/GeometryMetrics.h"
#include "sdk/GeometryPathOps.h"
#include "sdk/GeometryRelation.h"

namespace geometry::sdk
{
namespace
{
struct GraphVertex2d
{
    Point2d point{};
    std::size_t degree{0};
};

[[nodiscard]] std::size_t FindOrAddVertex(
    std::vector<GraphVertex2d>& vertices,
    const Point2d& point,
    double epsilon)
{
    for (std::size_t index = 0; index < vertices.size(); ++index)
    {
        if (vertices[index].point.AlmostEquals(point, epsilon))
        {
            return index;
        }
    }

    vertices.push_back(GraphVertex2d{point, 0});
    return vertices.size() - 1;
}

[[nodiscard]] SearchPolyDiagnostics2d AnalyzeLineNetwork(
    const MultiPolyline2d& lines,
    double epsilon)
{
    SearchPolyDiagnostics2d diagnostics;
    diagnostics.inputPolylineCount = lines.Count();

    std::vector<GraphVertex2d> vertices;
    for (std::size_t lineIndex = 0; lineIndex < lines.Count(); ++lineIndex)
    {
        const Polyline2d& line = lines[lineIndex];
        if (!line.IsValid() || line.PointCount() < 2)
        {
            continue;
        }

        const std::size_t segmentCount = line.IsClosed() ? line.PointCount() : line.PointCount() - 1;
        for (std::size_t segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex)
        {
            const Point2d start = line.PointAt(segmentIndex);
            const Point2d end = line.PointAt((segmentIndex + 1) % line.PointCount());
            if (start.AlmostEquals(end, epsilon))
            {
                continue;
            }

            ++diagnostics.inputSegmentCount;
            const std::size_t from = FindOrAddVertex(vertices, start, epsilon);
            const std::size_t to = FindOrAddVertex(vertices, end, epsilon);
            if (from != to)
            {
                ++vertices[from].degree;
                ++vertices[to].degree;
            }
        }
    }

    diagnostics.uniqueVertexCount = vertices.size();
    for (const GraphVertex2d& vertex : vertices)
    {
        if (vertex.degree == 1)
        {
            ++diagnostics.danglingEndpointCount;
        }
        else if (vertex.degree > 2)
        {
            ++diagnostics.branchVertexCount;
        }
    }

    diagnostics.inferredSyntheticEdgeCount = diagnostics.danglingEndpointCount / 2U;
    return diagnostics;
}

[[nodiscard]] SearchPolyCandidate2d MakeCandidate(const Polygon2d& polygon)
{
    return SearchPolyCandidate2d{
        polygon,
        std::abs(Area(polygon)),
        polygon.HoleCount(),
        0};
}

void RankCandidates(std::vector<SearchPolyCandidate2d>& candidates)
{
    std::stable_sort(candidates.begin(), candidates.end(), [](const SearchPolyCandidate2d& left, const SearchPolyCandidate2d& right) {
        if (std::abs(left.absoluteArea - right.absoluteArea) > 1e-12)
        {
            return left.absoluteArea > right.absoluteArea;
        }
        if (left.holeCount != right.holeCount)
        {
            return left.holeCount < right.holeCount;
        }
        return left.polygon.OuterRing().PointCount() < right.polygon.OuterRing().PointCount();
    });

    for (std::size_t index = 0; index < candidates.size(); ++index)
    {
        candidates[index].rank = index;
    }
}
} // namespace

SearchPolyResult2d SearchPolygons(const MultiPolyline2d& lines, SearchPolyOptions2d options)
{
    SearchPolyResult2d result;
    if (options.epsilon <= 0.0 || lines.Count() == 0)
    {
        result.issue = SearchPolyIssue2d::InvalidInput;
        return result;
    }

    result.diagnostics = AnalyzeLineNetwork(lines, options.epsilon);
    result.polygons = BuildMultiPolygonByLines(lines, options.epsilon);
    if (result.polygons.Count() == 0)
    {
        result.issue = SearchPolyIssue2d::NoClosedPolygonFound;
        return result;
    }

    result.candidates.reserve(result.polygons.Count());
    for (std::size_t index = 0; index < result.polygons.Count(); ++index)
    {
        if (result.polygons[index].IsValid())
        {
            result.candidates.push_back(MakeCandidate(result.polygons[index]));
        }
    }

    if (result.candidates.empty())
    {
        result.issue = SearchPolyIssue2d::NoClosedPolygonFound;
        result.polygons = {};
        return result;
    }

    RankCandidates(result.candidates);

    if (result.diagnostics.danglingEndpointCount > 0)
    {
        result.usedAutoClose = options.autoClose;
        result.usedSyntheticEdges = options.allowFakeEdges;
    }
    if (result.diagnostics.branchVertexCount > 0)
    {
        result.usedAutoExtend = options.autoExtend;
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

    std::optional<SearchPolyCandidate2d> bestCandidate;
    double bestArea = std::numeric_limits<double>::infinity();
    for (const SearchPolyCandidate2d& candidate : result.candidates)
    {
        const PointContainment2d containment = LocatePoint(point, candidate.polygon, options.epsilon);
        if (containment == PointContainment2d::Outside)
        {
            continue;
        }

        if (!bestCandidate.has_value() || candidate.absoluteArea < bestArea)
        {
            bestArea = candidate.absoluteArea;
            bestCandidate = candidate;
        }
    }

    return bestCandidate;
}
} // namespace geometry::sdk
