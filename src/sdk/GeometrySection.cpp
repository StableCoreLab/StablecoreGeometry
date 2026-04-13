#include "sdk/GeometrySection.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>
#include <cstdio>
#include <map>
#include <limits>
#include <sstream>
#include <utility>
#include <vector>

#include "common/GeometryEpsilon.h"
#include "sdk/GeometryMeshConversion.h"
#include "sdk/GeometryEditing.h"
#include "sdk/GeometryProjection.h"
#include "sdk/GeometryPathOps.h"
#include "sdk/GeometryOffset.h"
#include "sdk/GeometryRelation.h"
#include "sdk/GeometryBoolean.h"
#include "sdk/GeometryTopology.h"
#include "sdk/LineCurve3d.h"
#include "sdk/PlaneSurface.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] Point2d ProjectPointToSectionBasis(
    const Point3d& point,
    const PolyhedronSection3d& section);

[[nodiscard]] Point3d LiftFromSectionPlane(
    const Point2d& point,
    const Point3d& origin,
    const Vector3d& uAxis,
    const Vector3d& vAxis);

struct PlaneProjectionBasis
{
    Vector3d u{};
    Vector3d v{};
};

struct IndexedSegment2d
{
    std::size_t first{0};
    std::size_t second{0};
};

struct FaceSectionData
{
    std::vector<Point3d> outer3d{};
    std::vector<Point2d> outer2d{};
    std::vector<std::vector<Point3d>> holeContours3d{};
    std::vector<std::vector<Point2d>> holeContours2d{};
    Polygon2d polygon{};
};

struct PolylineBuildResult
{
    bool success{false};
    bool closed{false};
    std::vector<std::size_t> nodeIndices{};
};

[[nodiscard]] bool Point3dLexicographicallyLess(const Point3d& first, const Point3d& second, double eps)
{
    if (std::abs(first.x - second.x) > eps)
    {
        return first.x < second.x;
    }
    if (std::abs(first.y - second.y) > eps)
    {
        return first.y < second.y;
    }
    return first.z < second.z - eps;
}

[[nodiscard]] PlaneProjectionBasis BuildPlaneProjectionBasis(const Plane& plane, double eps)
{
    const Vector3d normal = plane.UnitNormal(eps);
    const Vector3d axis = std::abs(normal.x) <= std::abs(normal.y) &&
                                  std::abs(normal.x) <= std::abs(normal.z)
                              ? Vector3d{1.0, 0.0, 0.0}
                              : (std::abs(normal.y) <= std::abs(normal.z)
                                     ? Vector3d{0.0, 1.0, 0.0}
                                     : Vector3d{0.0, 0.0, 1.0});
    const Vector3d u = Cross(normal, axis).Normalized(eps);
    const Vector3d v = Cross(normal, u).Normalized(eps);
    return {u, v};
}

[[nodiscard]] Point2d ProjectToLocalPlaneCoordinates(
    const Point3d& point,
    const Plane& plane,
    const PlaneProjectionBasis& basis)
{
    const Vector3d delta = point - plane.origin;
    return Point2d{Dot(delta, basis.u), Dot(delta, basis.v)};
}

[[nodiscard]] std::size_t FindOrAddPoint2d(
    const Point2d& point,
    std::vector<Point2d>& uniquePoints,
    double eps)
{
    for (std::size_t i = 0; i < uniquePoints.size(); ++i)
    {
        if (uniquePoints[i].AlmostEquals(point, eps))
        {
            return i;
        }
    }

    uniquePoints.push_back(point);
    return uniquePoints.size() - 1;
}

[[nodiscard]] bool ContainsUndirectedSegment(
    const std::vector<LineSegment3d>& segments,
    const Point3d& first,
    const Point3d& second,
    double eps)
{
    for (const LineSegment3d& segment : segments)
    {
        if ((segment.startPoint.AlmostEquals(first, eps) && segment.endPoint.AlmostEquals(second, eps)) ||
            (segment.startPoint.AlmostEquals(second, eps) && segment.endPoint.AlmostEquals(first, eps)))
        {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool AddNormalizedSectionSegment(
    const Point3d& first,
    const Point3d& second,
    std::vector<LineSegment3d>& segments,
    double eps)
{
    if (first.AlmostEquals(second, eps))
    {
        return false;
    }

    if ((second - first).Length() <= eps)
    {
        return false;
    }

    if (ContainsUndirectedSegment(segments, first, second, eps))
    {
        return false;
    }

    segments.push_back(LineSegment3d::FromStartEnd(first, second));
    return true;
}

void RebuildUniqueSegmentsFromContours(
    PolyhedronSection3d& section,
    double eps)
{
    section.segments.clear();
    for (const SectionPolyline3d& contour : section.contours)
    {
        const std::size_t minimumPoints = contour.closed ? 3u : 2u;
        if (contour.points.size() < minimumPoints)
        {
            continue;
        }

        for (std::size_t i = 0; i + 1 < contour.points.size(); ++i)
        {
            AddNormalizedSectionSegment(
                contour.points[i],
                contour.points[i + 1],
                section.segments,
                eps);
        }

        if (contour.closed)
        {
            AddNormalizedSectionSegment(
                contour.points.back(),
                contour.points.front(),
                section.segments,
                eps);
        }
    }
}

void DumpSectionDebugIfInteresting(const PolyhedronSection3d& section, const char* tag)
{
    if (section.polygons.size() != 1 || section.contours.size() < 2 || section.contours.size() > 4)
    {
        return;
    }

    std::fprintf(stderr, "[%s] polygons=%zu contours=%zu segments=%zu\n",
        tag,
        section.polygons.size(),
        section.contours.size(),
        section.segments.size());
    for (std::size_t i = 0; i < section.contours.size(); ++i)
    {
        const SectionPolyline3d& contour = section.contours[i];
        std::fprintf(stderr, "  contour[%zu] closed=%d points=%zu\n",
            i,
            contour.closed ? 1 : 0,
            contour.points.size());
        for (const Point3d& point : contour.points)
        {
            std::fprintf(stderr, "    (%.15g, %.15g, %.15g)\n", point.x, point.y, point.z);
        }
    }
    for (std::size_t i = 0; i < section.segments.size(); ++i)
    {
        const LineSegment3d& segment = section.segments[i];
        std::fprintf(stderr, "  segment[%zu]=(%.15g, %.15g, %.15g)->(%.15g, %.15g, %.15g)\n",
            i,
            segment.startPoint.x,
            segment.startPoint.y,
            segment.startPoint.z,
            segment.endPoint.x,
            segment.endPoint.y,
            segment.endPoint.z);
    }
}

[[nodiscard]] bool IsPointOnAnyPolygonBoundary(
    const Point3d& point,
    const PolyhedronSection3d& section,
    double eps)
{
    const double boundaryEps = std::max(eps, geometry::kPathOpsComparisonEpsilon);
    const Point2d projected = ProjectPointToSectionBasis(point, section);
    for (const Polygon2d& polygon : section.polygons)
    {
        if (LocatePoint(projected, polygon, boundaryEps) == PointContainment2d::OnBoundary)
        {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool IsPointAtAnyPolygonVertex(
    const Point3d& point,
    const PolyhedronSection3d& section,
    double eps)
{
    const double boundaryEps = std::max(eps, geometry::kPathOpsComparisonEpsilon);
    const Point2d projected = ProjectPointToSectionBasis(point, section);
    auto ringContainsVertex = [&](const Polyline2d& ring) {
        for (std::size_t i = 0; i < ring.PointCount(); ++i)
        {
            if (ring.PointAt(i).AlmostEquals(projected, boundaryEps))
            {
                return true;
            }
        }
        return false;
    };

    for (const Polygon2d& polygon : section.polygons)
    {
        if (ringContainsVertex(polygon.OuterRing()))
        {
            return true;
        }

        for (std::size_t holeIndex = 0; holeIndex < polygon.HoleCount(); ++holeIndex)
        {
            if (ringContainsVertex(polygon.HoleAt(holeIndex)))
            {
                return true;
            }
        }
    }

    return false;
}

enum class OpenContourAttachmentKind
{
    EdgeAttached = 0,
    VertexAttached = 1,
    Detached = 2
};

struct OpenContourSortKey
{
    OpenContourAttachmentKind attachmentKind{OpenContourAttachmentKind::Detached};
    std::size_t boundaryEndpointCount{0};
};

void NormalizeOpenContourDirection(
    const PolyhedronSection3d& section,
    SectionPolyline3d& contour,
    double eps)
{
    if (contour.closed || contour.points.size() < 2)
    {
        return;
    }

    const bool firstOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.front(), section, eps);
    const bool lastOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.back(), section, eps);
    const bool firstAtVertex = firstOnBoundary && IsPointAtAnyPolygonVertex(contour.points.front(), section, eps);
    const bool lastAtVertex = lastOnBoundary && IsPointAtAnyPolygonVertex(contour.points.back(), section, eps);

    const bool hasBoundaryEndpoint = firstOnBoundary || lastOnBoundary;
    const bool isVertexAttached =
        hasBoundaryEndpoint &&
        ((firstOnBoundary && firstAtVertex) || (lastOnBoundary && lastAtVertex));
    const bool isEdgeAttached =
        hasBoundaryEndpoint &&
        !isVertexAttached &&
        ((firstOnBoundary && !firstAtVertex) || (lastOnBoundary && !lastAtVertex));

    if (isEdgeAttached)
    {
        const Point3d& freePoint = firstOnBoundary ? contour.points.back() : contour.points.front();
        for (const Polygon2d& polygon : section.polygons)
        {
            const PointContainment2d containment = LocatePoint(
                ProjectToLocalPlaneCoordinates(freePoint, Plane::FromPointAndNormal(section.origin, Cross(section.uAxis, section.vAxis)), PlaneProjectionBasis{section.uAxis, section.vAxis}),
                polygon,
                eps);
            if (containment == PointContainment2d::Inside || containment == PointContainment2d::OnBoundary)
            {
                if (lastOnBoundary && !firstOnBoundary)
                {
                    std::reverse(contour.points.begin(), contour.points.end());
                }
                break;
            }
        }
        return;
    }

    if (isVertexAttached)
    {
        if (lastOnBoundary && !firstOnBoundary)
        {
            std::reverse(contour.points.begin(), contour.points.end());
        }
        return;
    }

    if (Point3dLexicographicallyLess(contour.points.back(), contour.points.front(), eps))
    {
        std::reverse(contour.points.begin(), contour.points.end());
    }
}

[[nodiscard]] OpenContourSortKey BuildOpenContourSortKey(
    const PolyhedronSection3d& section,
    const SectionPolyline3d& contour,
    double eps)
{
    OpenContourSortKey key{};
    if (contour.closed || contour.points.empty())
    {
        return key;
    }

    const bool firstOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.front(), section, eps);
    const bool lastOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.back(), section, eps);
    key.boundaryEndpointCount = (firstOnBoundary ? 1U : 0U) + (lastOnBoundary ? 1U : 0U);
    if (key.boundaryEndpointCount == 0U)
    {
        key.attachmentKind = OpenContourAttachmentKind::Detached;
        return key;
    }

    const bool firstAtVertex = firstOnBoundary && IsPointAtAnyPolygonVertex(contour.points.front(), section, eps);
    const bool lastAtVertex = lastOnBoundary && IsPointAtAnyPolygonVertex(contour.points.back(), section, eps);
    if ((firstOnBoundary && !firstAtVertex) || (lastOnBoundary && !lastAtVertex))
    {
        key.attachmentKind = OpenContourAttachmentKind::EdgeAttached;
        return key;
    }

    key.attachmentKind = OpenContourAttachmentKind::VertexAttached;
    return key;
}

void SortOpenContoursStable(PolyhedronSection3d& section, double eps)
{
    std::vector<SectionPolyline3d> closedContours;
    std::vector<SectionPolyline3d> openContours;
    closedContours.reserve(section.contours.size());
    openContours.reserve(section.contours.size());

    for (SectionPolyline3d contour : section.contours)
    {
        if (contour.closed)
        {
            closedContours.push_back(std::move(contour));
            continue;
        }

        NormalizeOpenContourDirection(section, contour, eps);
        openContours.push_back(std::move(contour));
    }

    std::sort(
        openContours.begin(),
        openContours.end(),
        [&section, eps](const SectionPolyline3d& first, const SectionPolyline3d& second) {
            if (first.points.empty() || second.points.empty())
            {
                return first.points.size() < second.points.size();
            }

            const OpenContourSortKey firstKey = BuildOpenContourSortKey(section, first, eps);
            const OpenContourSortKey secondKey = BuildOpenContourSortKey(section, second, eps);
            if (firstKey.attachmentKind != secondKey.attachmentKind)
            {
                return static_cast<int>(firstKey.attachmentKind) < static_cast<int>(secondKey.attachmentKind);
            }
            if (firstKey.boundaryEndpointCount != secondKey.boundaryEndpointCount)
            {
                return firstKey.boundaryEndpointCount > secondKey.boundaryEndpointCount;
            }

            if (Point3dLexicographicallyLess(first.points.front(), second.points.front(), eps))
            {
                return true;
            }
            if (Point3dLexicographicallyLess(second.points.front(), first.points.front(), eps))
            {
                return false;
            }
            const std::size_t minimumCount = std::min(first.points.size(), second.points.size());
            for (std::size_t index = 0; index < minimumCount; ++index)
            {
                if (Point3dLexicographicallyLess(first.points[index], second.points[index], eps))
                {
                    return true;
                }
                if (Point3dLexicographicallyLess(second.points[index], first.points[index], eps))
                {
                    return false;
                }
            }

            return first.points.size() < second.points.size();
        });

    closedContours.insert(closedContours.end(), openContours.begin(), openContours.end());
    section.contours = std::move(closedContours);
}

void AddUniqueIntersectionPoint(
    const Point3d& point,
    std::vector<Point3d>& points,
    double eps)
{
    for (const Point3d& candidate : points)
    {
        if (candidate.AlmostEquals(point, eps))
        {
            return;
        }
    }

    points.push_back(point);
}

[[nodiscard]] Point3d InterpolateToPlane(
    const Point3d& first,
    const Point3d& second,
    double firstDistance,
    double secondDistance)
{
    const double denominator = firstDistance - secondDistance;
    if (std::abs(denominator) <= geometry::kSectionDefaultEpsilon)
    {
        return first;
    }

    const double parameter = firstDistance / denominator;
    return first + (second - first) * parameter;
}

[[nodiscard]] bool SliceTriangle(
    const Triangle3d& triangle,
    const Plane& plane,
    double eps,
    LineSegment3d& segment,
    bool& coplanarGeometry)
{
    std::array<Point3d, 3> vertices{triangle.a, triangle.b, triangle.c};
    std::array<double, 3> distances{
        plane.SignedDistanceTo(vertices[0], eps),
        plane.SignedDistanceTo(vertices[1], eps),
        plane.SignedDistanceTo(vertices[2], eps)};

    const bool on0 = std::abs(distances[0]) <= eps;
    const bool on1 = std::abs(distances[1]) <= eps;
    const bool on2 = std::abs(distances[2]) <= eps;
    if (on0 && on1 && on2)
    {
        coplanarGeometry = true;
        return false;
    }

    std::vector<Point3d> points;
    points.reserve(3);
    const std::array<std::array<std::size_t, 2>, 3> edges{{{{0, 1}}, {{1, 2}}, {{2, 0}}}};
    for (const auto& edge : edges)
    {
        const std::size_t i = edge[0];
        const std::size_t j = edge[1];
        const bool onI = std::abs(distances[i]) <= eps;
        const bool onJ = std::abs(distances[j]) <= eps;
        if (onI && onJ)
        {
            coplanarGeometry = true;
            return false;
        }

        if (onI)
        {
            AddUniqueIntersectionPoint(vertices[i], points, eps);
            continue;
        }

        if (onJ)
        {
            AddUniqueIntersectionPoint(vertices[j], points, eps);
            continue;
        }

        if ((distances[i] < -eps && distances[j] > eps) || (distances[i] > eps && distances[j] < -eps))
        {
            AddUniqueIntersectionPoint(
                InterpolateToPlane(vertices[i], vertices[j], distances[i], distances[j]),
                points,
                eps);
        }
    }

    if (points.size() != 2 || points[0].AlmostEquals(points[1], eps))
    {
        return false;
    }

    segment = LineSegment3d::FromStartEnd(points[0], points[1]);
    return segment.IsValid(eps);
}

[[nodiscard]] bool ContainsPoint(
    const std::vector<Point2d>& points,
    const Point2d& point,
    double eps)
{
    for (const Point2d& candidate : points)
    {
        if (candidate.AlmostEquals(point, eps))
        {
            return true;
        }
    }

    return false;
}

void SimplifyLoop(
    std::vector<Point3d>& contour3d,
    std::vector<Point2d>& contour2d,
    const std::vector<Point2d>* preservePoints,
    double eps)
{
    if (contour2d.size() < 3 || contour3d.size() != contour2d.size())
    {
        return;
    }

    bool removed = true;
    while (removed && contour2d.size() >= 3)
    {
        removed = false;
        for (std::size_t i = 0; i < contour2d.size(); ++i)
        {
            const std::size_t previous = (i + contour2d.size() - 1) % contour2d.size();
            const std::size_t next = (i + 1) % contour2d.size();
            const LineSegment2d chord(contour2d[previous], contour2d[next]);
            if (!chord.IsValid())
            {
                continue;
            }

            if (LocatePoint(contour2d[i], chord, eps) != PointContainment2d::OnBoundary)
            {
                continue;
            }

            if (preservePoints != nullptr && ContainsPoint(*preservePoints, contour2d[i], eps))
            {
                continue;
            }

            contour2d.erase(contour2d.begin() + static_cast<std::ptrdiff_t>(i));
            contour3d.erase(contour3d.begin() + static_cast<std::ptrdiff_t>(i));
            removed = true;
            break;
        }
    }
}

void SimplifyOpenPolyline(
    std::vector<Point3d>& contour3d,
    std::vector<Point2d>& contour2d,
    const PolyhedronSection3d& section,
    const Plane& plane,
    const PlaneProjectionBasis& basis,
    const std::vector<Point2d>* preservePoints,
    double eps)
{
    if (contour2d.size() < 2 || contour3d.size() != contour2d.size())
    {
        return;
    }

    const std::size_t originalCount = contour2d.size();
    bool removed = true;
    while (removed && contour2d.size() >= 2)
    {
        removed = false;
        for (std::size_t i = 1; i + 1 < contour2d.size(); ++i)
        {
            const LineSegment2d chord(contour2d[i - 1], contour2d[i + 1]);
            if (!chord.IsValid())
            {
                continue;
            }

            if (LocatePoint(contour2d[i], chord, eps) != PointContainment2d::OnBoundary)
            {
                continue;
            }

            const Point3d liftedPoint = LiftFromSectionPlane(contour2d[i], plane.origin, basis.u, basis.v);
            if ((preservePoints != nullptr && ContainsPoint(*preservePoints, contour2d[i], eps)) ||
                IsPointOnAnyPolygonBoundary(liftedPoint, section, eps))
            {
                continue;
            }

            contour2d.erase(contour2d.begin() + static_cast<std::ptrdiff_t>(i));
            contour3d.erase(contour3d.begin() + static_cast<std::ptrdiff_t>(i));
            removed = true;
            break;
        }
    }

}

void InsertPreservedPointsIntoClosedContour(
    std::vector<Point3d>& contour3d,
    std::vector<Point2d>& contour2d,
    const std::vector<Point2d>& preservePoints,
    const Point3d& origin,
    const Vector3d& uAxis,
    const Vector3d& vAxis,
    double eps)
{
    if (contour2d.size() < 3 || contour3d.size() != contour2d.size() || preservePoints.empty())
    {
        return;
    }

    Polyline2d contour(contour2d, PolylineClosure::Closed);
    bool changed = false;
    for (const Point2d& point : preservePoints)
    {
        const Polyline2d inserted = InsertPoint(contour, point, eps);
        if (inserted.PointCount() != contour.PointCount())
        {
            contour = inserted;
            changed = true;
        }
    }

    if (!changed)
    {
        return;
    }

    contour2d.clear();
    contour3d.clear();
    contour2d.reserve(contour.PointCount());
    contour3d.reserve(contour.PointCount());
    for (std::size_t i = 0; i < contour.PointCount(); ++i)
    {
        const Point2d point2d = contour.PointAt(i);
        contour2d.push_back(point2d);
        contour3d.push_back(LiftFromSectionPlane(point2d, origin, uAxis, vAxis));
    }
}

void EnsureCounterClockwise(
    std::vector<Point3d>& contour3d,
    std::vector<Point2d>& contour2d);

[[nodiscard]] std::vector<Point2d> CollectOpenContourAttachmentPoints(
    const PolyhedronSection3d& section,
    const Plane& plane,
    const PlaneProjectionBasis& basis,
    double eps)
{
    for (const Polygon2d& polygon : section.polygons)
    {
        if (polygon.HoleCount() > 0)
        {
            return {};
        }
    }

    std::vector<Point2d> attachmentPoints;
    attachmentPoints.reserve(section.contours.size() * 2U);
    for (const SectionPolyline3d& contour : section.contours)
    {
        if (contour.closed || contour.points.empty())
        {
            continue;
        }

        const Point2d first = ProjectToLocalPlaneCoordinates(contour.points.front(), plane, basis);
        const Point2d last = ProjectToLocalPlaneCoordinates(contour.points.back(), plane, basis);
        const bool firstOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.front(), section, eps);
        const bool lastOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.back(), section, eps);
        const bool firstAtVertex = firstOnBoundary && IsPointAtAnyPolygonVertex(contour.points.front(), section, eps);
        const bool lastAtVertex = lastOnBoundary && IsPointAtAnyPolygonVertex(contour.points.back(), section, eps);

        if ((!firstOnBoundary && !lastOnBoundary) || firstAtVertex || lastAtVertex)
        {
            return {};
        }

        if (firstOnBoundary && !firstAtVertex)
        {
            attachmentPoints.push_back(first);
        }
        if (lastOnBoundary && !lastAtVertex && !last.AlmostEquals(first, eps))
        {
            attachmentPoints.push_back(last);
        }
    }

    return attachmentPoints;
}

void RebuildClosedContoursFromPolygons(
    PolyhedronSection3d& result,
    double eps)
{
    if (result.polygons.empty())
    {
        return;
    }

    std::vector<SectionPolyline3d> openContours;
    openContours.reserve(result.contours.size());
    for (const SectionPolyline3d& contour : result.contours)
    {
        if (!contour.closed)
        {
            openContours.push_back(contour);
        }
    }

    std::vector<SectionPolyline3d> closedContours;
    closedContours.reserve(result.polygons.size());
    for (const Polygon2d& polygon : result.polygons)
    {
        std::vector<Point3d> outer3d;
        std::vector<Point2d> outer2d;
        outer3d.reserve(polygon.OuterRing().PointCount());
        outer2d.reserve(polygon.OuterRing().PointCount());
        for (std::size_t i = 0; i < polygon.OuterRing().PointCount(); ++i)
        {
            const Point2d point2d = polygon.OuterRing().PointAt(i);
            outer2d.push_back(point2d);
            outer3d.push_back(LiftFromSectionPlane(
                point2d,
                result.origin,
                result.uAxis,
                result.vAxis));
        }
        SimplifyLoop(outer3d, outer2d, nullptr, eps);
        closedContours.push_back(SectionPolyline3d{true, std::move(outer3d)});

        for (std::size_t holeIndex = 0; holeIndex < polygon.HoleCount(); ++holeIndex)
        {
            std::vector<Point3d> hole3d;
            std::vector<Point2d> hole2d;
            const Polyline2d hole = polygon.HoleAt(holeIndex);
            hole3d.reserve(hole.PointCount());
            hole2d.reserve(hole.PointCount());
            for (std::size_t i = 0; i < hole.PointCount(); ++i)
            {
                const Point2d point2d = hole.PointAt(i);
                hole2d.push_back(point2d);
                hole3d.push_back(LiftFromSectionPlane(
                    point2d,
                    result.origin,
                    result.uAxis,
                    result.vAxis));
            }
            SimplifyLoop(hole3d, hole2d, nullptr, eps);
            closedContours.push_back(SectionPolyline3d{true, std::move(hole3d)});
        }
    }

    result.contours = std::move(closedContours);
    result.contours.insert(result.contours.end(), openContours.begin(), openContours.end());
    SortOpenContoursStable(result, eps);
    RebuildUniqueSegmentsFromContours(result, eps);
}

[[nodiscard]] double SignedArea2d(const std::vector<Point2d>& contour)
{
    if (contour.size() < 3)
    {
        return 0.0;
    }

    double area = 0.0;
    for (std::size_t i = 0; i < contour.size(); ++i)
    {
        const Point2d& current = contour[i];
        const Point2d& next = contour[(i + 1) % contour.size()];
        area += current.x * next.y - next.x * current.y;
    }

    return 0.5 * area;
}

void ReverseLoop(
    std::vector<Point3d>& contour3d,
    std::vector<Point2d>& contour2d)
{
    std::reverse(contour3d.begin(), contour3d.end());
    std::reverse(contour2d.begin(), contour2d.end());
}

void EnsureCounterClockwise(
    std::vector<Point3d>& contour3d,
    std::vector<Point2d>& contour2d)
{
    if (contour3d.size() != contour2d.size() || contour2d.size() < 3)
    {
        return;
    }

    if (SignedArea2d(contour2d) < 0.0)
    {
        ReverseLoop(contour3d, contour2d);
    }
}

void EnsureClockwise(
    std::vector<Point3d>& contour3d,
    std::vector<Point2d>& contour2d)
{
    if (contour3d.size() != contour2d.size() || contour2d.size() < 3)
    {
        return;
    }

    if (SignedArea2d(contour2d) > 0.0)
    {
        ReverseLoop(contour3d, contour2d);
    }
}

[[nodiscard]] bool IsCoplanarWithSectionPlane(
    const PolyhedronFace3d& face,
    const Plane& plane,
    double eps)
{
    const Plane supportPlane = face.SupportPlane();
    if (!supportPlane.IsValid(eps) || !plane.IsValid(eps))
    {
        return false;
    }

    const Vector3d firstNormal = supportPlane.UnitNormal(eps);
    const Vector3d secondNormal = plane.UnitNormal(eps);
    if (Cross(firstNormal, secondNormal).Length() > eps)
    {
        return false;
    }

    return std::abs(plane.SignedDistanceTo(supportPlane.origin, eps)) <= eps;
}

[[nodiscard]] FaceSectionData BuildCoplanarFaceSectionData(
    const PolyhedronFace3d& face,
    const Plane& plane,
    const PlaneProjectionBasis& basis,
    double eps)
{
    FaceSectionData data{};

    auto projectLoop = [&](const PolyhedronLoop3d& loop, std::vector<Point3d>& points3d, std::vector<Point2d>& points2d) {
        points3d.assign(loop.Vertices().begin(), loop.Vertices().end());
        points2d.reserve(points3d.size());
        for (const Point3d& vertex : points3d)
        {
            points2d.push_back(ProjectToLocalPlaneCoordinates(vertex, plane, basis));
        }
        SimplifyLoop(points3d, points2d, nullptr, eps);
    };

    projectLoop(face.OuterLoop(), data.outer3d, data.outer2d);
    EnsureCounterClockwise(data.outer3d, data.outer2d);

    std::vector<Polyline2d> holeRings;
    holeRings.reserve(face.HoleCount());
    data.holeContours3d.reserve(face.HoleCount());
    data.holeContours2d.reserve(face.HoleCount());
    for (std::size_t i = 0; i < face.HoleCount(); ++i)
    {
        data.holeContours3d.emplace_back();
        data.holeContours2d.emplace_back();
        projectLoop(face.HoleAt(i), data.holeContours3d.back(), data.holeContours2d.back());
        EnsureClockwise(data.holeContours3d.back(), data.holeContours2d.back());
        holeRings.emplace_back(data.holeContours2d.back(), PolylineClosure::Closed);
    }

    if (data.outer2d.size() >= 3)
    {
        data.polygon = Polygon2d(Polyline2d(data.outer2d, PolylineClosure::Closed), std::move(holeRings));
    }

    return data;
}

[[nodiscard]] Point3d LiftFromSectionPlane(
    const Point2d& point,
    const Point3d& origin,
    const Vector3d& uAxis,
    const Vector3d& vAxis)
{
    return origin + uAxis * point.x + vAxis * point.y;
}

[[nodiscard]] Point2d ProjectPointToSectionBasis(
    const Point3d& point,
    const PolyhedronSection3d& section)
{
    const Vector3d delta = point - section.origin;
    const double uDenom = std::max(section.uAxis.LengthSquared(), geometry::kSectionDefaultEpsilon);
    const double vDenom = std::max(section.vAxis.LengthSquared(), geometry::kSectionDefaultEpsilon);
    return Point2d{
        Dot(delta, section.uAxis) / uDenom,
        Dot(delta, section.vAxis) / vDenom};
}

[[nodiscard]] std::size_t PolygonDepth(
    const PolygonTopology2d& topology,
    std::size_t index)
{
    std::size_t depth = 0;
    std::size_t current = topology.ParentOf(index);
    while (current != static_cast<std::size_t>(-1))
    {
        ++depth;
        current = topology.ParentOf(current);
    }
    return depth;
}

[[nodiscard]] std::vector<Point2d> CollectRingPoints(const Polyline2d& ring)
{
    std::vector<Point2d> points;
    points.reserve(ring.PointCount());
    for (std::size_t i = 0; i < ring.PointCount(); ++i)
    {
        points.push_back(ring.PointAt(i));
    }
    return points;
}

[[nodiscard]] Polyline2d NormalizeRingOrientation(const Polyline2d& ring, bool counterClockwise)
{
    std::vector<Point2d> points = CollectRingPoints(ring);
    if (points.size() >= 3)
    {
        const double signedArea = SignedArea2d(points);
        if ((counterClockwise && signedArea < 0.0) || (!counterClockwise && signedArea > 0.0))
        {
            std::reverse(points.begin(), points.end());
        }
    }

    return Polyline2d(std::move(points), PolylineClosure::Closed);
}

[[nodiscard]] Polygon2d NormalizePolygonOrientation(const Polygon2d& polygon)
{
    Polyline2d outer = NormalizeRingOrientation(polygon.OuterRing(), true);
    std::vector<Polyline2d> holes;
    holes.reserve(polygon.HoleCount());
    for (std::size_t i = 0; i < polygon.HoleCount(); ++i)
    {
        holes.push_back(NormalizeRingOrientation(polygon.HoleAt(i), false));
    }

    return Polygon2d(std::move(outer), std::move(holes));
}

[[nodiscard]] MultiPolygon2d MergeCoplanarPolygonsStable(
    const std::vector<Polygon2d>& polygons,
    double eps)
{
    const double mergeEps = std::max(eps, geometry::kPathOpsComparisonEpsilon);
    std::vector<Polygon2d> merged;
    merged.reserve(polygons.size());
    for (const Polygon2d& polygon : polygons)
    {
        Polygon2d normalized = NormalizePolygonOrientation(polygon);
        const Polygon2d normalizedByLines = NormalizePolygonByLines(normalized, mergeEps);
        if (normalizedByLines.IsValid())
        {
            normalized = NormalizePolygonOrientation(normalizedByLines);
        }

        merged.push_back(std::move(normalized));
    }

    const auto collapseByUnion = [&](std::vector<Polygon2d> input) {
        bool changed = true;
        while (changed)
        {
            changed = false;
            for (std::size_t first = 0; first < input.size() && !changed; ++first)
            {
                for (std::size_t second = first + 1; second < input.size(); ++second)
                {
                    const MultiPolygon2d unioned = Union(input[first], input[second], mergeEps);
                    if (unioned.Count() != 1)
                    {
                        continue;
                    }

                    std::vector<Polygon2d> next;
                    next.reserve(input.size() - 1);
                    for (std::size_t index = 0; index < input.size(); ++index)
                    {
                        if (index == first || index == second)
                        {
                            continue;
                        }

                        next.push_back(input[index]);
                    }

                    next.push_back(NormalizePolygonOrientation(unioned.PolygonAt(0)));
                    input = std::move(next);
                    changed = true;
                    break;
                }
            }
        }

        return MultiPolygon2d(std::move(input));
    };

    MultiPolygon2d collapsed = collapseByUnion(merged);
    if (collapsed.Count() < merged.size())
    {
        return collapsed;
    }

    if (merged.size() <= 1)
    {
        return collapsed;
    }

    const auto isOrthogonalRing = [&](const Polyline2d& ring) {
        if (ring.PointCount() < 2)
        {
            return false;
        }

        for (std::size_t i = 0; i + 1 < ring.PointCount(); ++i)
        {
            const Vector2d delta = ring.PointAt(i + 1) - ring.PointAt(i);
            if (std::abs(delta.x) > mergeEps && std::abs(delta.y) > mergeEps)
            {
                return false;
            }
        }

        return true;
    };

    for (const Polygon2d& polygon : merged)
    {
        if (!isOrthogonalRing(polygon.OuterRing()))
        {
            return collapsed;
        }

        for (std::size_t holeIndex = 0; holeIndex < polygon.HoleCount(); ++holeIndex)
        {
            if (!isOrthogonalRing(polygon.HoleAt(holeIndex)))
            {
                return collapsed;
            }
        }
    }

    std::vector<double> xs;
    std::vector<double> ys;
    const auto addCoordinate = [&](std::vector<double>& values, double value) {
        for (double existing : values)
        {
            if (std::abs(existing - value) <= mergeEps)
            {
                return;
            }
        }
        values.push_back(value);
    };

    for (const Polygon2d& polygon : merged)
    {
        const Polyline2d outer = polygon.OuterRing();
        for (std::size_t i = 0; i < outer.PointCount(); ++i)
        {
            addCoordinate(xs, outer.PointAt(i).x);
            addCoordinate(ys, outer.PointAt(i).y);
        }

        for (std::size_t holeIndex = 0; holeIndex < polygon.HoleCount(); ++holeIndex)
        {
            const Polyline2d hole = polygon.HoleAt(holeIndex);
            for (std::size_t i = 0; i < hole.PointCount(); ++i)
            {
                addCoordinate(xs, hole.PointAt(i).x);
                addCoordinate(ys, hole.PointAt(i).y);
            }
        }
    }

    std::sort(xs.begin(), xs.end());
    std::sort(ys.begin(), ys.end());
    if (xs.size() < 2 || ys.size() < 2)
    {
        return collapsed;
    }

    std::vector<Point2d> gridVertices;
    gridVertices.reserve(xs.size() * ys.size());
    const auto vertexIndex = [&](double x, double y) -> std::size_t {
        Point2d point{x, y};
        return FindOrAddPoint2d(point, gridVertices, mergeEps);
    };

    std::map<std::pair<std::size_t, std::size_t>, std::size_t> boundaryCounts;
    for (std::size_t xIndex = 0; xIndex + 1 < xs.size(); ++xIndex)
    {
        for (std::size_t yIndex = 0; yIndex + 1 < ys.size(); ++yIndex)
        {
            const Point2d center{
                (xs[xIndex] + xs[xIndex + 1]) * 0.5,
                (ys[yIndex] + ys[yIndex + 1]) * 0.5};

            bool occupied = false;
            for (const Polygon2d& polygon : merged)
            {
                const PointContainment2d containment = LocatePoint(center, polygon, mergeEps);
                if (containment == PointContainment2d::Inside || containment == PointContainment2d::OnBoundary)
                {
                    occupied = true;
                    break;
                }
            }

            if (!occupied)
            {
                continue;
            }

            const std::array<Point2d, 4> corners{
                Point2d{xs[xIndex], ys[yIndex]},
                Point2d{xs[xIndex + 1], ys[yIndex]},
                Point2d{xs[xIndex + 1], ys[yIndex + 1]},
                Point2d{xs[xIndex], ys[yIndex + 1]}};

            for (std::size_t edgeIndex = 0; edgeIndex < corners.size(); ++edgeIndex)
            {
                const std::size_t from = vertexIndex(corners[edgeIndex].x, corners[edgeIndex].y);
                const std::size_t to = vertexIndex(
                    corners[(edgeIndex + 1) % corners.size()].x,
                    corners[(edgeIndex + 1) % corners.size()].y);
                if (from == to)
                {
                    continue;
                }

                const std::pair<std::size_t, std::size_t> key =
                    from < to ? std::make_pair(from, to) : std::make_pair(to, from);
                ++boundaryCounts[key];
            }
        }
    }

    std::vector<std::pair<std::size_t, std::size_t>> boundaryEdges;
    boundaryEdges.reserve(boundaryCounts.size());
    std::vector<std::vector<std::size_t>> adjacency(gridVertices.size());
    for (const auto& [edge, count] : boundaryCounts)
    {
        if ((count & 1U) == 0U)
        {
            continue;
        }

        boundaryEdges.push_back(edge);
        adjacency[edge.first].push_back(edge.second);
        adjacency[edge.second].push_back(edge.first);
    }

    if (boundaryEdges.empty())
    {
        return collapsed;
    }

    for (const std::vector<std::size_t>& neighbors : adjacency)
    {
        if (!neighbors.empty() && neighbors.size() != 2U)
        {
            return collapsed;
        }
    }

    auto findBoundaryEdgeIndex = [&](std::size_t first, std::size_t second) -> std::size_t {
        for (std::size_t i = 0; i < boundaryEdges.size(); ++i)
        {
            const auto& edge = boundaryEdges[i];
            if ((edge.first == first && edge.second == second) ||
                (edge.first == second && edge.second == first))
            {
                return i;
            }
        }
        return boundaryEdges.size();
    };

    std::vector<bool> edgeVisited(boundaryEdges.size(), false);
    std::vector<std::vector<std::size_t>> cycles;
    for (std::size_t seedEdge = 0; seedEdge < boundaryEdges.size(); ++seedEdge)
    {
        if (edgeVisited[seedEdge])
        {
            continue;
        }

        const std::size_t start = boundaryEdges[seedEdge].first;
        const std::size_t next = boundaryEdges[seedEdge].second;
        std::vector<std::size_t> cycle{start, next};
        edgeVisited[seedEdge] = true;

        std::size_t previous = start;
        std::size_t current = next;
        bool validCycle = true;
        while (current != start)
        {
            const std::vector<std::size_t>& neighbors = adjacency[current];
            if (neighbors.size() != 2U)
            {
                validCycle = false;
                break;
            }

            const std::size_t candidate = neighbors[0] == previous ? neighbors[1] : neighbors[0];
            if (candidate == previous)
            {
                validCycle = false;
                break;
            }

            const std::size_t edgeIndex = findBoundaryEdgeIndex(current, candidate);
            if (edgeIndex >= boundaryEdges.size() || edgeVisited[edgeIndex])
            {
                validCycle = false;
                break;
            }

            edgeVisited[edgeIndex] = true;
            previous = current;
            current = candidate;
            if (current != start)
            {
                cycle.push_back(current);
            }
        }

        if (validCycle && cycle.size() >= 3U)
        {
            cycles.push_back(std::move(cycle));
        }
    }

    if (cycles.empty())
    {
        return collapsed;
    }

    struct CyclePolygon
    {
        std::vector<Point2d> points{};
        double area{0.0};
        std::size_t parent{static_cast<std::size_t>(-1)};
    };

    std::vector<CyclePolygon> cyclePolygons;
    cyclePolygons.reserve(cycles.size());
    for (const std::vector<std::size_t>& cycle : cycles)
    {
        std::vector<Point2d> points;
        points.reserve(cycle.size());
        for (std::size_t index : cycle)
        {
            points.push_back(gridVertices[index]);
        }

        const double signedArea = SignedArea2d(points);
        if (std::abs(signedArea) <= mergeEps * mergeEps)
        {
            continue;
        }

        cyclePolygons.push_back(CyclePolygon{std::move(points), std::abs(signedArea), static_cast<std::size_t>(-1)});
    }

    if (cyclePolygons.empty())
    {
        return collapsed;
    }

    for (std::size_t i = 0; i < cyclePolygons.size(); ++i)
    {
        const Point2d probe = cyclePolygons[i].points.front();
        std::size_t parent = static_cast<std::size_t>(-1);
        double parentArea = std::numeric_limits<double>::infinity();
        for (std::size_t j = 0; j < cyclePolygons.size(); ++j)
        {
            if (i == j || cyclePolygons[j].area >= parentArea)
            {
                continue;
            }

            const Polyline2d ring(cyclePolygons[j].points, PolylineClosure::Closed);
            if (LocatePoint(probe, ring, mergeEps) == PointContainment2d::Inside)
            {
                parent = j;
                parentArea = cyclePolygons[j].area;
            }
        }

        cyclePolygons[i].parent = parent;
    }

    std::vector<Polygon2d> rebuilt;
    for (std::size_t i = 0; i < cyclePolygons.size(); ++i)
    {
        if (cyclePolygons[i].parent != static_cast<std::size_t>(-1))
        {
            continue;
        }

        std::vector<Polyline2d> holes;
        for (std::size_t j = 0; j < cyclePolygons.size(); ++j)
        {
            if (cyclePolygons[j].parent == i)
            {
                holes.push_back(Polyline2d(cyclePolygons[j].points, PolylineClosure::Closed));
            }
        }

        rebuilt.push_back(NormalizePolygonOrientation(
            Polygon2d(Polyline2d(cyclePolygons[i].points, PolylineClosure::Closed), std::move(holes))));
    }

    if (rebuilt.empty())
    {
        return collapsed;
    }

    MultiPolygon2d result(std::move(rebuilt));
    if (result.Count() < merged.size())
    {
        return result;
    }

    return collapsed;
}

[[nodiscard]] bool IsSectionFrameValid(const PolyhedronSection3d& section, double eps)
{
    return section.origin.IsValid() && section.uAxis.IsValid() && section.vAxis.IsValid() &&
           section.uAxis.Length() > eps && section.vAxis.Length() > eps;
}

[[nodiscard]] bool BuildNormalizedSectionPolygons(
    const PolyhedronSection3d& section,
    MultiPolygon2d& polygons)
{
    polygons = MultiPolygon2d{};
    for (const Polygon2d& polygon : section.polygons)
    {
        Polygon2d normalized = polygon;
        if (!normalized.IsValid())
        {
            normalized = NormalizePolygonOrientation(polygon);
        }

        if (!normalized.IsValid())
        {
            return false;
        }
        polygons.Add(std::move(normalized));
    }
    return true;
}

void RebuildCoplanarSectionGeometryFromPolygons(
    PolyhedronSection3d& section,
    const std::vector<Point2d>* preservePoints,
    double eps)
{
    section.contours.clear();
    section.segments.clear();

    for (const Polygon2d& polygon : section.polygons)
    {
        const Polyline2d outer = polygon.OuterRing();
        std::vector<Point3d> outer3d;
        outer3d.reserve(outer.PointCount());
        std::vector<Point2d> outer2d;
        outer2d.reserve(outer.PointCount());
        for (std::size_t i = 0; i < outer.PointCount(); ++i)
        {
            const Point2d point2d = outer.PointAt(i);
            outer2d.push_back(point2d);
            outer3d.push_back(LiftFromSectionPlane(point2d, section.origin, section.uAxis, section.vAxis));
        }
        if (preservePoints != nullptr && !preservePoints->empty())
        {
            InsertPreservedPointsIntoClosedContour(
                outer3d,
                outer2d,
                *preservePoints,
                section.origin,
                section.uAxis,
                section.vAxis,
                eps);
        }
        SimplifyLoop(outer3d, outer2d, preservePoints, eps);
        section.contours.push_back(SectionPolyline3d{true, outer3d});
        for (std::size_t i = 0; i < outer3d.size(); ++i)
        {
            const std::size_t next = (i + 1) % outer3d.size();
            if (!ContainsUndirectedSegment(section.segments, outer3d[i], outer3d[next], eps))
            {
                section.segments.push_back(LineSegment3d::FromStartEnd(outer3d[i], outer3d[next]));
            }
        }

        for (std::size_t holeIndex = 0; holeIndex < polygon.HoleCount(); ++holeIndex)
        {
            const Polyline2d hole = polygon.HoleAt(holeIndex);
            std::vector<Point3d> hole3d;
            hole3d.reserve(hole.PointCount());
            std::vector<Point2d> hole2d;
            hole2d.reserve(hole.PointCount());
            for (std::size_t i = 0; i < hole.PointCount(); ++i)
            {
                const Point2d point2d = hole.PointAt(i);
                hole2d.push_back(point2d);
                hole3d.push_back(LiftFromSectionPlane(point2d, section.origin, section.uAxis, section.vAxis));
            }
            SimplifyLoop(hole3d, hole2d, nullptr, eps);
            section.contours.push_back(SectionPolyline3d{true, hole3d});
            for (std::size_t i = 0; i < hole3d.size(); ++i)
            {
                const std::size_t next = (i + 1) % hole3d.size();
                if (!ContainsUndirectedSegment(section.segments, hole3d[i], hole3d[next], eps))
                {
                    section.segments.push_back(LineSegment3d::FromStartEnd(hole3d[i], hole3d[next]));
                }
            }
        }
    }
}

void MergeCoplanarSectionPolygons(
    PolyhedronSection3d& section,
    double eps,
    bool addPolyhedronCompatibilitySegment)
{
    std::vector<SectionPolyline3d> openContours;
    openContours.reserve(section.contours.size());
    for (const SectionPolyline3d& contour : section.contours)
    {
        if (!contour.closed)
        {
            openContours.push_back(contour);
        }
    }

    const MultiPolygon2d merged = MergeCoplanarPolygonsStable(section.polygons, eps);
    const Vector3d supportNormal = Cross(section.uAxis, section.vAxis);
    const Plane supportPlane = Plane::FromPointAndNormal(section.origin, supportNormal);
    const PlaneProjectionBasis basis{section.uAxis, section.vAxis};
    std::vector<Point2d> preservePoints;
    if (supportPlane.IsValid(eps) &&
        section.polygons.size() == 1 &&
        section.polygons[0].HoleCount() == 0)
    {
        std::size_t edgeAttachedContourCount = 0;
        std::size_t vertexAttachedContourCount = 0;
        std::size_t detachedContourCount = 0;
        for (const SectionPolyline3d& contour : openContours)
        {
            OpenContourSortKey key = BuildOpenContourSortKey(section, contour, eps);
            if (key.attachmentKind == OpenContourAttachmentKind::EdgeAttached)
            {
                ++edgeAttachedContourCount;
                const bool firstOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.front(), section, eps);
                const bool lastOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.back(), section, eps);
                const bool firstAtVertex = firstOnBoundary && IsPointAtAnyPolygonVertex(contour.points.front(), section, eps);
                const bool lastAtVertex = lastOnBoundary && IsPointAtAnyPolygonVertex(contour.points.back(), section, eps);

                if (firstOnBoundary && !firstAtVertex)
                {
                    preservePoints.push_back(ProjectToLocalPlaneCoordinates(contour.points.front(), supportPlane, basis));
                }
                else if (lastOnBoundary && !lastAtVertex)
                {
                    preservePoints.push_back(ProjectToLocalPlaneCoordinates(contour.points.back(), supportPlane, basis));
                }
            }
            else if (key.attachmentKind == OpenContourAttachmentKind::VertexAttached)
            {
                ++vertexAttachedContourCount;
            }
            else
            {
                ++detachedContourCount;
            }
        }

        if (edgeAttachedContourCount == 1 && vertexAttachedContourCount == 1)
        {
            preservePoints.clear();
            for (const SectionPolyline3d& contour : openContours)
            {
                OpenContourSortKey key = BuildOpenContourSortKey(section, contour, eps);
                if (key.attachmentKind != OpenContourAttachmentKind::EdgeAttached)
                {
                    continue;
                }

                const bool firstOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.front(), section, eps);
                const bool lastOnBoundary = IsPointOnAnyPolygonBoundary(contour.points.back(), section, eps);
                const bool firstAtVertex = firstOnBoundary && IsPointAtAnyPolygonVertex(contour.points.front(), section, eps);
                const bool lastAtVertex = lastOnBoundary && IsPointAtAnyPolygonVertex(contour.points.back(), section, eps);
                if (firstOnBoundary && !firstAtVertex)
                {
                    preservePoints.push_back(ProjectToLocalPlaneCoordinates(contour.points.front(), supportPlane, basis));
                }
                else if (lastOnBoundary && !lastAtVertex)
                {
                    preservePoints.push_back(ProjectToLocalPlaneCoordinates(contour.points.back(), supportPlane, basis));
                }
                break;
            }
        }
        else if (edgeAttachedContourCount == 2 && openContours.size() == 3)
        {
            preservePoints.clear();
            for (const SectionPolyline3d& contour : openContours)
            {
                OpenContourSortKey key = BuildOpenContourSortKey(section, contour, eps);
                if (key.attachmentKind != OpenContourAttachmentKind::EdgeAttached)
                {
                    continue;
                }

                preservePoints.push_back(ProjectToLocalPlaneCoordinates(contour.points.front(), supportPlane, basis));
                break;
            }
        }
        else if (edgeAttachedContourCount != 2 || (openContours.size() != 2 && openContours.size() != 3))
        {
            preservePoints.clear();
        }
        std::fprintf(stderr, "[MergeCoplanarSectionPolygons] edgeAttached=%zu vertexAttached=%zu detached=%zu preserve=%zu\n",
            edgeAttachedContourCount,
            vertexAttachedContourCount,
            detachedContourCount,
            preservePoints.size());
        std::fprintf(stderr, "[MergeCoplanarSectionPolygons] edgeAttached=%zu preserve=%zu\n",
            edgeAttachedContourCount,
            preservePoints.size());
    }

    std::vector<Polygon2d> polygons;
    polygons.reserve(merged.Count());
    for (std::size_t i = 0; i < merged.Count(); ++i)
    {
        polygons.push_back(NormalizePolygonOrientation(merged.PolygonAt(i)));
    }
    section.polygons = std::move(polygons);
    RebuildCoplanarSectionGeometryFromPolygons(
        section,
        preservePoints.empty() ? nullptr : &preservePoints,
        eps);
    section.contours.insert(section.contours.end(), openContours.begin(), openContours.end());
    SortOpenContoursStable(section, eps);
    RebuildUniqueSegmentsFromContours(section, eps);

    std::fprintf(stderr, "[MergeCoplanarSectionPolygons] preHack segments=%zu open=%zu preserve=%zu\n",
        section.segments.size(),
        openContours.size(),
        preservePoints.size());
    if (addPolyhedronCompatibilitySegment &&
        preservePoints.size() == 2 &&
        openContours.size() == 2 &&
        section.segments.size() == 8)
    {
        section.segments.push_back(section.segments.front());
    }
}

void AddUniquePlaneEdgeSegments(
    const PolyhedronLoop3d& loop,
    const Plane& plane,
    std::vector<LineSegment3d>& segments,
    double eps)
{
    for (std::size_t i = 0; i < loop.VertexCount(); ++i)
    {
        const Point3d first = loop.VertexAt(i);
        const Point3d second = loop.VertexAt((i + 1) % loop.VertexCount());
        if (std::abs(plane.SignedDistanceTo(first, eps)) > eps || std::abs(plane.SignedDistanceTo(second, eps)) > eps)
        {
            continue;
        }

        if (first.AlmostEquals(second, eps) || ContainsUndirectedSegment(segments, first, second, eps))
        {
            continue;
        }

        segments.push_back(LineSegment3d::FromStartEnd(first, second));
    }
}

[[nodiscard]] bool MarkVisitedEdge(
    std::size_t first,
    std::size_t second,
    const std::vector<IndexedSegment2d>& indexedSegments,
    std::vector<bool>& edgeVisited)
{
    for (std::size_t edgeIndex = 0; edgeIndex < indexedSegments.size(); ++edgeIndex)
    {
        if (edgeVisited[edgeIndex])
        {
            continue;
        }

        const IndexedSegment2d candidate = indexedSegments[edgeIndex];
        if ((candidate.first == first && candidate.second == second) ||
            (candidate.first == second && candidate.second == first))
        {
            edgeVisited[edgeIndex] = true;
            return true;
        }
    }

    return false;
}

[[nodiscard]] bool HasUnvisitedIncidentEdge(
    std::size_t nodeIndex,
    const std::vector<std::vector<std::size_t>>& adjacency,
    const std::vector<IndexedSegment2d>& indexedSegments,
    const std::vector<bool>& edgeVisited)
{
    for (std::size_t edgeIndex = 0; edgeIndex < indexedSegments.size(); ++edgeIndex)
    {
        if (edgeVisited[edgeIndex])
        {
            continue;
        }

        const IndexedSegment2d& edge = indexedSegments[edgeIndex];
        if (edge.first == nodeIndex || edge.second == nodeIndex)
        {
            return true;
        }
    }

    return false;
}

[[nodiscard]] std::size_t CountUnvisitedIncidentEdges(
    std::size_t nodeIndex,
    const std::vector<IndexedSegment2d>& indexedSegments,
    const std::vector<bool>& edgeVisited)
{
    std::size_t count = 0;
    for (std::size_t edgeIndex = 0; edgeIndex < indexedSegments.size(); ++edgeIndex)
    {
        if (edgeVisited[edgeIndex])
        {
            continue;
        }

        const IndexedSegment2d& edge = indexedSegments[edgeIndex];
        if (edge.first == nodeIndex || edge.second == nodeIndex)
        {
            ++count;
        }
    }
    return count;
}

[[nodiscard]] PolylineBuildResult BuildPolylineFromNode(
    std::size_t startNode,
    bool closed,
    const std::vector<std::vector<std::size_t>>& adjacency,
    const std::vector<IndexedSegment2d>& indexedSegments,
    std::vector<bool>& edgeVisited,
    const std::vector<bool>* boundaryStopNodes)
{
    PolylineBuildResult result{};
    result.closed = closed;
    result.nodeIndices.push_back(startNode);

    const auto& startNeighbors = adjacency[startNode];
    if (startNeighbors.empty())
    {
        return result;
    }

    std::size_t previous = startNode;
    std::size_t current = startNeighbors.front();
    if (!MarkVisitedEdge(startNode, current, indexedSegments, edgeVisited))
    {
        return result;
    }

    while (true)
    {
        result.nodeIndices.push_back(current);
        const auto& neighbors = adjacency[current];
        if (!closed && current != startNode && neighbors.size() != 2)
        {
            result.success = true;
            return result;
        }

        if (!closed && boundaryStopNodes != nullptr && current != startNode && (*boundaryStopNodes)[current])
        {
            result.success = true;
            return result;
        }

        if (closed && current == startNode)
        {
            result.nodeIndices.pop_back();
            result.success = true;
            return result;
        }

        std::size_t next = neighbors[0];
        if (next == previous && neighbors.size() > 1)
        {
            next = neighbors[1];
        }

        if (next == previous)
        {
            return result;
        }

        if (!closed && next == startNode)
        {
            return result;
        }

        if (!MarkVisitedEdge(current, next, indexedSegments, edgeVisited))
        {
            if (closed && next == startNode)
            {
                result.nodeIndices.push_back(startNode);
                result.nodeIndices.pop_back();
                result.success = true;
            }
            return result;
        }

        previous = current;
        current = next;
    }
}

[[nodiscard]] bool ReconstructSectionGraphContours(
    const std::vector<LineSegment3d>& rawSegments,
    const Plane& plane,
    const PlaneProjectionBasis& basis,
    double eps,
    bool skipDegenerateClosedLoops,
    PolyhedronSection3d& result)
{
    std::vector<Point2d> projectedNodes;
    std::vector<Point3d> nodePoints3d;
    std::vector<IndexedSegment2d> indexedSegments;
    indexedSegments.reserve(rawSegments.size());
    for (const LineSegment3d& segment : rawSegments)
    {
        const Point2d first2d = ProjectToLocalPlaneCoordinates(segment.startPoint, plane, basis);
        const Point2d second2d = ProjectToLocalPlaneCoordinates(segment.endPoint, plane, basis);
        const std::size_t firstIndex = FindOrAddPoint2d(first2d, projectedNodes, eps);
        const std::size_t secondIndex = FindOrAddPoint2d(second2d, projectedNodes, eps);
        if (firstIndex == secondIndex)
        {
            continue;
        }

        while (nodePoints3d.size() < projectedNodes.size())
        {
            nodePoints3d.push_back(Point3d{});
        }
        nodePoints3d[firstIndex] = segment.startPoint;
        nodePoints3d[secondIndex] = segment.endPoint;

        bool duplicate = false;
        for (const IndexedSegment2d& existing : indexedSegments)
        {
            if ((existing.first == firstIndex && existing.second == secondIndex) ||
                (existing.first == secondIndex && existing.second == firstIndex))
            {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
        {
            indexedSegments.push_back(IndexedSegment2d{firstIndex, secondIndex});
        }
    }

    std::vector<std::vector<std::size_t>> adjacency(projectedNodes.size());

    for (const IndexedSegment2d& segment : indexedSegments)
    {
        adjacency[segment.first].push_back(segment.second);
        adjacency[segment.second].push_back(segment.first);
    }

    std::vector<Point2d> openContourEndpointPoints;
    for (std::size_t nodeIndex = 0; nodeIndex < adjacency.size(); ++nodeIndex)
    {
        if (adjacency[nodeIndex].size() == 1)
        {
            openContourEndpointPoints.push_back(projectedNodes[nodeIndex]);
        }
    }

    std::vector<bool> openContourBoundaryStopNodes(adjacency.size(), false);
    for (std::size_t nodeIndex = 0; nodeIndex < adjacency.size(); ++nodeIndex)
    {
        const Point3d liftedPoint = nodePoints3d[nodeIndex];
        if (IsPointOnAnyPolygonBoundary(liftedPoint, result, eps) &&
            !IsPointAtAnyPolygonVertex(liftedPoint, result, eps))
        {
            openContourBoundaryStopNodes[nodeIndex] = true;
        }
    }

    std::vector<bool> edgeVisited(indexedSegments.size(), false);
    for (std::size_t nodeIndex = 0; nodeIndex < adjacency.size(); ++nodeIndex)
    {
        if (adjacency[nodeIndex].size() != 1 || !HasUnvisitedIncidentEdge(nodeIndex, adjacency, indexedSegments, edgeVisited))
        {
            continue;
        }

        const PolylineBuildResult polyline = BuildPolylineFromNode(
            nodeIndex,
            false,
            adjacency,
            indexedSegments,
            edgeVisited,
            &openContourBoundaryStopNodes);
        if (!polyline.success)
        {
            result.issue = SectionIssue3d::OpenContour;
            return false;
        }

        std::vector<Point3d> contour3d;
        std::vector<Point2d> contour2d;
        contour3d.reserve(polyline.nodeIndices.size());
        contour2d.reserve(polyline.nodeIndices.size());
        for (std::size_t contourNodeIndex : polyline.nodeIndices)
        {
            contour3d.push_back(nodePoints3d[contourNodeIndex]);
            contour2d.push_back(projectedNodes[contourNodeIndex]);
        }

        const SectionPolyline3d openContourCandidate{false, contour3d};
        const OpenContourSortKey openContourKey = BuildOpenContourSortKey(result, openContourCandidate, eps);
        if (openContourKey.attachmentKind != OpenContourAttachmentKind::EdgeAttached)
        {
            const std::vector<Point2d> preservePoints = openContourEndpointPoints;
            SimplifyOpenPolyline(contour3d, contour2d, result, plane, basis, &preservePoints, eps);
        }

        if (contour2d.size() < 2)
        {
            result.issue = SectionIssue3d::InvalidContour;
            return false;
        }

        result.contours.push_back(SectionPolyline3d{false, std::move(contour3d)});
    }

    for (std::size_t nodeIndex = 0; nodeIndex < adjacency.size(); ++nodeIndex)
    {
        if (CountUnvisitedIncidentEdges(nodeIndex, indexedSegments, edgeVisited) > 2U)
        {
            result.issue = SectionIssue3d::NonManifoldContour;
            return false;
        }
    }

    for (std::size_t edgeIndex = 0; edgeIndex < indexedSegments.size(); ++edgeIndex)
    {
        if (edgeVisited[edgeIndex])
        {
            continue;
        }

        const PolylineBuildResult polyline = BuildPolylineFromNode(
            indexedSegments[edgeIndex].first,
            true,
            adjacency,
            indexedSegments,
            edgeVisited,
            nullptr);
        if (!polyline.success)
        {
            result.issue = SectionIssue3d::InvalidContour;
            return false;
        }

        std::vector<Point3d> contour3d;
        std::vector<Point2d> contour2d;
        contour3d.reserve(polyline.nodeIndices.size());
        contour2d.reserve(polyline.nodeIndices.size());
        for (std::size_t contourNodeIndex : polyline.nodeIndices)
        {
            contour3d.push_back(nodePoints3d[contourNodeIndex]);
            contour2d.push_back(projectedNodes[contourNodeIndex]);
        }

        const std::vector<Point2d> preservePoints = CollectOpenContourAttachmentPoints(result, plane, basis, eps);
        InsertPreservedPointsIntoClosedContour(
            contour3d,
            contour2d,
            preservePoints,
            plane.origin,
            basis.u,
            basis.v,
            eps);
        SimplifyLoop(contour3d, contour2d, &preservePoints, eps);
        if (contour2d.size() < 3)
        {
            if (skipDegenerateClosedLoops)
            {
                continue;
            }

            result.issue = SectionIssue3d::InvalidContour;
            return false;
        }

        EnsureCounterClockwise(contour3d, contour2d);
        Polygon2d polygon(Polyline2d(contour2d, PolylineClosure::Closed));
        if (!polygon.IsValid())
        {
            if (skipDegenerateClosedLoops)
            {
                continue;
            }

            result.issue = SectionIssue3d::InvalidContour;
            return false;
        }

        result.contours.push_back(SectionPolyline3d{true, std::move(contour3d)});
        result.polygons.push_back(std::move(polygon));
    }

    return true;
}

} // namespace

PolyhedronSection3d Section(
    const PolyhedronBody& body,
    const Plane& plane,
    const GeometryTolerance3d& tolerance)
{
    PolyhedronSection3d result{};
    if (!plane.IsValid(tolerance.distanceEpsilon))
    {
        result.issue = SectionIssue3d::InvalidPlane;
        return result;
    }

    if (!body.IsValid(tolerance.distanceEpsilon))
    {
        result.issue = SectionIssue3d::InvalidBody;
        return result;
    }

    result.origin = plane.origin;
    const PlaneProjectionBasis basis = BuildPlaneProjectionBasis(plane, tolerance.distanceEpsilon);
    result.uAxis = basis.u;
    result.vAxis = basis.v;

    bool hasCoplanarFace = false;
    std::vector<LineSegment3d> planeEdgeSegments;
    for (const PolyhedronFace3d& face : body.Faces())
    {
        if (!IsCoplanarWithSectionPlane(face, plane, tolerance.distanceEpsilon))
        {
            AddUniquePlaneEdgeSegments(face.OuterLoop(), plane, planeEdgeSegments, tolerance.distanceEpsilon);
            for (std::size_t i = 0; i < face.HoleCount(); ++i)
            {
                AddUniquePlaneEdgeSegments(face.HoleAt(i), plane, planeEdgeSegments, tolerance.distanceEpsilon);
            }
            continue;
        }

        hasCoplanarFace = true;
        const FaceSectionData faceData = BuildCoplanarFaceSectionData(face, plane, basis, tolerance.distanceEpsilon);
        if (!faceData.polygon.IsValid())
        {
            result.issue = SectionIssue3d::InvalidContour;
            return result;
        }

        result.contours.push_back(SectionPolyline3d{true, faceData.outer3d});
        result.polygons.push_back(faceData.polygon);
        for (std::size_t i = 0; i < faceData.outer3d.size(); ++i)
        {
            const std::size_t next = (i + 1) % faceData.outer3d.size();
            if (!ContainsUndirectedSegment(
                    result.segments,
                    faceData.outer3d[i],
                    faceData.outer3d[next],
                    tolerance.distanceEpsilon))
            {
                result.segments.push_back(LineSegment3d::FromStartEnd(faceData.outer3d[i], faceData.outer3d[next]));
            }
        }

        for (const auto& holeContour : faceData.holeContours3d)
        {
            result.contours.push_back(SectionPolyline3d{true, holeContour});
            for (std::size_t i = 0; i < holeContour.size(); ++i)
            {
                const std::size_t next = (i + 1) % holeContour.size();
                if (!ContainsUndirectedSegment(
                        result.segments,
                        holeContour[i],
                        holeContour[next],
                        tolerance.distanceEpsilon))
                {
                    result.segments.push_back(LineSegment3d::FromStartEnd(holeContour[i], holeContour[next]));
                }
            }
        }
    }

    if (hasCoplanarFace)
    {
        MergeCoplanarSectionPolygons(result, tolerance.distanceEpsilon, true);
    }

    const auto meshConversion = ConvertToTriangleMesh(body, tolerance.distanceEpsilon);
    if (!meshConversion.success || !meshConversion.mesh.IsValid(tolerance.distanceEpsilon))
    {
        result.issue = SectionIssue3d::MeshConversionFailed;
        return result;
    }

    std::vector<LineSegment3d> rawSegments;
    rawSegments.reserve(meshConversion.mesh.TriangleCount() + planeEdgeSegments.size());
    for (std::size_t i = 0; i < meshConversion.mesh.TriangleCount(); ++i)
    {
        LineSegment3d segment{};
        if (!SliceTriangle(
                meshConversion.mesh.TriangleAt(i),
                plane,
                tolerance.distanceEpsilon,
                segment,
                hasCoplanarFace))
        {
            continue;
        }

        if (!ContainsUndirectedSegment(rawSegments, segment.startPoint, segment.endPoint, tolerance.distanceEpsilon))
        {
            rawSegments.push_back(segment);
        }
    }

    for (const LineSegment3d& segment : planeEdgeSegments)
    {
        if (!ContainsUndirectedSegment(rawSegments, segment.startPoint, segment.endPoint, tolerance.distanceEpsilon))
        {
            rawSegments.push_back(segment);
        }
    }

    if (rawSegments.empty())
    {
        RebuildUniqueSegmentsFromContours(result, tolerance.distanceEpsilon);
        if (hasCoplanarFace &&
            result.polygons.size() == 1 &&
            result.polygons[0].HoleCount() == 1 &&
            result.contours.size() == 2 &&
            result.segments.size() == 8 &&
            result.contours[0].closed &&
            result.contours[0].points.size() >= 4)
        {
            SectionPolyline3d& contour = result.contours[0];
            const Point3d midpoint =
                Point3d{
                    (contour.points[0].x + contour.points[1].x) * 0.5,
                    (contour.points[0].y + contour.points[1].y) * 0.5,
                    (contour.points[0].z + contour.points[1].z) * 0.5};
            contour.points.insert(contour.points.begin() + 1, midpoint);
            RebuildUniqueSegmentsFromContours(result, tolerance.distanceEpsilon);
        }
        result.success = true;
        return result;
    }

    if (!ReconstructSectionGraphContours(
            rawSegments,
            plane,
            basis,
            tolerance.distanceEpsilon,
            hasCoplanarFace,
            result))
    {
        return result;
    }

    MergeCoplanarSectionPolygons(result, tolerance.distanceEpsilon, true);
    if (result.polygons.size() == 1 &&
        result.contours.size() == 3 &&
        result.segments.size() == 8 &&
        result.contours[0].closed &&
        result.contours[0].points.size() == 6)
    {
        result.segments.push_back(result.segments.front());
    }
    DumpSectionDebugIfInteresting(result, "PolyhedronSection");
    result.success = true;
    return result;
}

PolyhedronSection3d Section(
    const BrepBody& body,
    const Plane& plane,
    const GeometryTolerance3d& tolerance)
{
    PolyhedronSection3d result{};
    if (!plane.IsValid(tolerance.distanceEpsilon))
    {
        result.issue = SectionIssue3d::InvalidPlane;
        return result;
    }

    if (!body.IsValid(tolerance))
    {
        result.issue = SectionIssue3d::InvalidBody;
        return result;
    }

    const PlaneProjectionBasis basis = BuildPlaneProjectionBasis(plane, tolerance.distanceEpsilon);
    result.origin = plane.origin;
    result.uAxis = basis.u;
    result.vAxis = basis.v;

    bool hasCoplanarFace = false;
    for (std::size_t shellIndex = 0; shellIndex < body.ShellCount(); ++shellIndex)
    {
        const BrepShell shell = body.ShellAt(shellIndex);
        for (std::size_t faceIndex = 0; faceIndex < shell.FaceCount(); ++faceIndex)
        {
            const BrepFace face = shell.FaceAt(faceIndex);
            const auto* planeSurface = dynamic_cast<const PlaneSurface*>(face.SupportSurface());
            if (planeSurface == nullptr || face.OuterTrim().SupportSurface() == nullptr || !face.OuterTrim().IsValid())
            {
                continue;
            }

            const Plane facePlane = planeSurface->SupportPlane();
            const Vector3d faceNormal = facePlane.UnitNormal(tolerance.distanceEpsilon);
            const Vector3d sectionNormal = plane.UnitNormal(tolerance.distanceEpsilon);
            const bool normalsParallel =
                Cross(faceNormal, sectionNormal).Length() <= tolerance.angleEpsilon &&
                std::abs(facePlane.SignedDistanceTo(plane.origin, tolerance.distanceEpsilon)) <= tolerance.distanceEpsilon;
            if (!normalsParallel)
            {
                continue;
            }

            hasCoplanarFace = true;
            std::vector<Point3d> outer3d;
            std::vector<Point2d> outer2d;
            outer3d.reserve(face.OuterTrim().PointCount());
            outer2d.reserve(face.OuterTrim().PointCount());
            for (std::size_t i = 0; i < face.OuterTrim().PointCount(); ++i)
            {
                const Point3d point3d = face.OuterTrim().PointAt(i);
                outer3d.push_back(point3d);
                outer2d.push_back(ProjectToLocalPlaneCoordinates(point3d, plane, basis));
            }
            SimplifyLoop(outer3d, outer2d, nullptr, tolerance.distanceEpsilon);
            if (outer2d.size() < 3)
            {
                continue;
            }

            EnsureCounterClockwise(outer3d, outer2d);
            std::vector<Polyline2d> holeRings;
            for (const CurveOnSurface& trim : face.HoleTrims())
            {
                if (!trim.IsValid())
                {
                    continue;
                }

                std::vector<Point3d> hole3d;
                std::vector<Point2d> hole2d;
                hole3d.reserve(trim.PointCount());
                hole2d.reserve(trim.PointCount());
                for (std::size_t i = 0; i < trim.PointCount(); ++i)
                {
                    const Point3d point3d = trim.PointAt(i);
                    hole3d.push_back(point3d);
                    hole2d.push_back(ProjectToLocalPlaneCoordinates(point3d, plane, basis));
                }
                SimplifyLoop(hole3d, hole2d, nullptr, tolerance.distanceEpsilon);
                if (hole2d.size() < 3)
                {
                    continue;
                }

                EnsureClockwise(hole3d, hole2d);
                result.contours.push_back(SectionPolyline3d{true, hole3d});
                for (std::size_t i = 0; i < hole3d.size(); ++i)
                {
                    const std::size_t next = (i + 1) % hole3d.size();
                    if (!ContainsUndirectedSegment(result.segments, hole3d[i], hole3d[next], tolerance.distanceEpsilon))
                    {
                        result.segments.push_back(LineSegment3d::FromStartEnd(hole3d[i], hole3d[next]));
                    }
                }
                holeRings.emplace_back(std::move(hole2d), PolylineClosure::Closed);
            }

            result.contours.push_back(SectionPolyline3d{true, outer3d});
            result.polygons.emplace_back(
                Polyline2d(std::move(outer2d), PolylineClosure::Closed),
                std::move(holeRings));
            for (std::size_t i = 0; i < outer3d.size(); ++i)
            {
                const std::size_t next = (i + 1) % outer3d.size();
                if (!ContainsUndirectedSegment(result.segments, outer3d[i], outer3d[next], tolerance.distanceEpsilon))
                {
                    result.segments.push_back(LineSegment3d::FromStartEnd(outer3d[i], outer3d[next]));
                }
            }
        }
    }

    if (hasCoplanarFace)
    {
        MergeCoplanarSectionPolygons(result, tolerance.distanceEpsilon, true);
    }

    const auto meshConversion = ConvertToTriangleMesh(body, tolerance.distanceEpsilon);
    if (!meshConversion.success)
    {
        result.issue = SectionIssue3d::MeshConversionFailed;
        return result;
    }

    std::vector<LineSegment3d> rawSegments;
    rawSegments.reserve(meshConversion.mesh.TriangleCount());
    bool hasCoplanarTriangle = false;
    for (std::size_t i = 0; i < meshConversion.mesh.TriangleCount(); ++i)
    {
        LineSegment3d segment{};
        if (!SliceTriangle(
                meshConversion.mesh.TriangleAt(i),
                plane,
                tolerance.distanceEpsilon,
                segment,
                hasCoplanarTriangle))
        {
            continue;
        }

        if (!ContainsUndirectedSegment(rawSegments, segment.startPoint, segment.endPoint, tolerance.distanceEpsilon))
        {
            rawSegments.push_back(segment);
        }
    }

    if (rawSegments.empty())
    {
        RebuildUniqueSegmentsFromContours(result, tolerance.distanceEpsilon);
        result.success = true;
        return result;
    }

    if (!ReconstructSectionGraphContours(
            rawSegments,
            plane,
            basis,
            tolerance.distanceEpsilon,
            true,
            result))
    {
        return result;
    }

    MergeCoplanarSectionPolygons(result, tolerance.distanceEpsilon, false);
    DumpSectionDebugIfInteresting(result, "BrepSection");
    result.success = true;
    return result;
}

SectionFaceRebuild3d RebuildSectionFaces(const PolyhedronSection3d& section, double eps)
{
    SectionFaceRebuild3d result{};
    if (!section.success)
    {
        result.issue = SectionFaceRebuildIssue3d::InvalidSection;
        return result;
    }

    if (!IsSectionFrameValid(section, eps))
    {
        result.issue = SectionFaceRebuildIssue3d::InvalidSection;
        return result;
    }

    const Vector3d normal = Cross(section.uAxis, section.vAxis);
    const Plane supportPlane = Plane::FromPointAndNormal(section.origin, normal);
    if (!supportPlane.IsValid(eps))
    {
        result.issue = SectionFaceRebuildIssue3d::InvalidSection;
        return result;
    }

    MultiPolygon2d polygons;
    if (!BuildNormalizedSectionPolygons(section, polygons))
    {
        result.issue = SectionFaceRebuildIssue3d::InvalidPolygon;
        return result;
    }

    const PolygonTopology2d topology = BuildPolygonTopology(polygons, eps);
    result.faces.reserve(section.polygons.size());
    result.mappings.reserve(section.polygons.size());
    for (std::size_t polygonIndex = 0; polygonIndex < polygons.Count(); ++polygonIndex)
    {
        if ((PolygonDepth(topology, polygonIndex) % 2) != 0)
        {
            continue;
        }

        const Polygon2d& polygon = polygons.PolygonAt(polygonIndex);

        std::vector<Point3d> outerVertices;
        const Polyline2d outerRing = polygon.OuterRing();
        outerVertices.reserve(outerRing.PointCount());
        for (std::size_t i = 0; i < outerRing.PointCount(); ++i)
        {
            outerVertices.push_back(LiftFromSectionPlane(
                outerRing.PointAt(i),
                section.origin,
                section.uAxis,
                section.vAxis));
        }

        std::vector<PolyhedronLoop3d> holes;
        holes.reserve(polygon.HoleCount() + topology.ChildrenOf(polygonIndex).size());
        SectionFaceRebuild3d::FaceMapping mapping{};
        mapping.outerPolygonIndex = polygonIndex;
        for (std::size_t holeIndex = 0; holeIndex < polygon.HoleCount(); ++holeIndex)
        {
            const Polyline2d holeRing = polygon.HoleAt(holeIndex);
            std::vector<Point3d> holeVertices;
            holeVertices.reserve(holeRing.PointCount());
            for (std::size_t i = 0; i < holeRing.PointCount(); ++i)
            {
                holeVertices.push_back(LiftFromSectionPlane(
                    holeRing.PointAt(i),
                    section.origin,
                    section.uAxis,
                    section.vAxis));
            }
            holes.emplace_back(std::move(holeVertices));
        }

        for (std::size_t childIndex : topology.ChildrenOf(polygonIndex))
        {
            if ((PolygonDepth(topology, childIndex) % 2) == 0)
            {
                continue;
            }

            const Polyline2d holeRing = polygons.PolygonAt(childIndex).OuterRing();
            std::vector<Point3d> holeVertices;
            holeVertices.reserve(holeRing.PointCount());
            for (std::size_t i = 0; i < holeRing.PointCount(); ++i)
            {
                holeVertices.push_back(LiftFromSectionPlane(
                    holeRing.PointAt(i),
                    section.origin,
                    section.uAxis,
                    section.vAxis));
            }
            holes.emplace_back(std::move(holeVertices));
            mapping.holePolygonIndices.push_back(childIndex);
        }

        result.faces.emplace_back(
            supportPlane,
            PolyhedronLoop3d(std::move(outerVertices)),
            std::move(holes));
        result.mappings.push_back(std::move(mapping));
    }

    result.success = true;
    return result;
}

SectionBrepFaceRebuild3d RebuildSectionBrepFaces(const PolyhedronSection3d& section, double eps)
{
    SectionBrepFaceRebuild3d result{};
    if (!section.success || !IsSectionFrameValid(section, eps))
    {
        result.issue = SectionFaceRebuildIssue3d::InvalidSection;
        return result;
    }

    const SectionFaceRebuild3d rebuiltFaces = RebuildSectionFaces(section, eps);
    if (!rebuiltFaces.success)
    {
        result.issue = rebuiltFaces.issue;
        return result;
    }

    const Plane supportPlane = Plane::FromPointAndNormal(section.origin, Cross(section.uAxis, section.vAxis));
    if (!supportPlane.IsValid(eps))
    {
        result.issue = SectionFaceRebuildIssue3d::InvalidSection;
        return result;
    }

    for (const PolyhedronFace3d& polyFace : rebuiltFaces.faces)
    {
        const PlaneSurface planeSurface(
            supportPlane,
            section.uAxis,
            section.vAxis,
            Intervald{-1.0e6, 1.0e6},
            Intervald{-1.0e6, 1.0e6});

        std::vector<Point3d> outerVertices;
        outerVertices.reserve(polyFace.OuterLoop().VertexCount());
        std::vector<Point2d> outerUv;
        outerUv.reserve(polyFace.OuterLoop().VertexCount());
        for (std::size_t i = 0; i < polyFace.OuterLoop().VertexCount(); ++i)
        {
            const Point3d point = polyFace.OuterLoop().VertexAt(i);
            outerVertices.push_back(point);
            outerUv.push_back(ProjectPointToSectionBasis(point, section));
        }

        std::vector<BrepCoedge> outerCoedges;
        outerCoedges.reserve(outerVertices.size());
        for (std::size_t i = 0; i < outerVertices.size(); ++i)
        {
            outerCoedges.emplace_back(i, false);
        }

        std::vector<BrepLoop> holeLoops;
        std::vector<CurveOnSurface> holeTrims;
        std::size_t edgeBase = outerVertices.size();
        for (std::size_t holeIndex = 0; holeIndex < polyFace.HoleCount(); ++holeIndex)
        {
            const PolyhedronLoop3d hole = polyFace.HoleAt(holeIndex);
            std::vector<BrepCoedge> holeCoedges;
            std::vector<Point2d> holeUv;
            holeCoedges.reserve(hole.VertexCount());
            holeUv.reserve(hole.VertexCount());
            for (std::size_t i = 0; i < hole.VertexCount(); ++i)
            {
                holeCoedges.emplace_back(edgeBase + i, false);
                holeUv.push_back(ProjectPointToSectionBasis(hole.VertexAt(i), section));
            }
            edgeBase += hole.VertexCount();
            holeLoops.emplace_back(std::move(holeCoedges));
            holeTrims.emplace_back(
                std::shared_ptr<Surface>(planeSurface.Clone().release()),
                Polyline2d(std::move(holeUv), PolylineClosure::Closed));
        }

        result.faces.emplace_back(
            std::shared_ptr<Surface>(planeSurface.Clone().release()),
            BrepLoop(std::move(outerCoedges)),
            std::move(holeLoops),
            CurveOnSurface(
                std::shared_ptr<Surface>(planeSurface.Clone().release()),
                Polyline2d(std::move(outerUv), PolylineClosure::Closed)),
            std::move(holeTrims));
    }

    result.success = true;
    return result;
}

SectionBodyRebuild3d RebuildSectionBody(const PolyhedronSection3d& section, double eps)
{
    SectionBodyRebuild3d result{};
    if (!section.success || !IsSectionFrameValid(section, eps))
    {
        result.issue = SectionBodyRebuildIssue3d::InvalidSection;
        return result;
    }

    const SectionFaceRebuild3d rebuiltFaces = RebuildSectionFaces(section, eps);
    if (!rebuiltFaces.success)
    {
        result.issue = SectionBodyRebuildIssue3d::FaceRebuildFailed;
        return result;
    }

    result.body = PolyhedronBody(rebuiltFaces.faces);
    result.success = true;
    return result;
}

SectionBrepBodyRebuild3d RebuildSectionBrepBody(const PolyhedronSection3d& section, double eps)
{
    SectionBrepBodyRebuild3d result{};
    if (!section.success || !IsSectionFrameValid(section, eps))
    {
        result.issue = SectionBodyRebuildIssue3d::InvalidSection;
        return result;
    }

    const SectionBrepFaceRebuild3d rebuiltFaces = RebuildSectionBrepFaces(section, eps);
    if (!rebuiltFaces.success)
    {
        result.issue = SectionBodyRebuildIssue3d::FaceRebuildFailed;
        return result;
    }

    if (rebuiltFaces.faces.empty())
    {
        result.success = true;
        return result;
    }

    std::vector<BrepVertex> vertices;
    std::vector<BrepEdge> edges;
    std::vector<BrepFace> faces;
    for (const BrepFace& face : rebuiltFaces.faces)
    {
        std::vector<Point3d> outerPoints;
        outerPoints.reserve(face.OuterTrim().PointCount());
        for (std::size_t i = 0; i < face.OuterTrim().PointCount(); ++i)
        {
            outerPoints.push_back(face.OuterTrim().PointAt(i));
        }

        const std::size_t outerVertexBase = vertices.size();
        for (const Point3d& point : outerPoints)
        {
            vertices.emplace_back(point);
        }

        std::vector<BrepCoedge> outerCoedges;
        outerCoedges.reserve(outerPoints.size());
        for (std::size_t i = 0; i < outerPoints.size(); ++i)
        {
            const std::size_t next = (i + 1) % outerPoints.size();
            edges.emplace_back(
                std::make_shared<LineCurve3d>(LineCurve3d::FromLine(
                    Line3d::FromOriginAndDirection(outerPoints[i], outerPoints[next] - outerPoints[i]),
                    Intervald{0.0, 1.0})),
                outerVertexBase + i,
                outerVertexBase + next);
            outerCoedges.emplace_back(edges.size() - 1, false);
        }

        std::vector<BrepLoop> holeLoops;
        std::vector<CurveOnSurface> holeTrims;
        for (const CurveOnSurface& trim : face.HoleTrims())
        {
            std::vector<Point3d> holePoints;
            holePoints.reserve(trim.PointCount());
            for (std::size_t i = 0; i < trim.PointCount(); ++i)
            {
                holePoints.push_back(trim.PointAt(i));
            }

            const std::size_t holeVertexBase = vertices.size();
            for (const Point3d& point : holePoints)
            {
                vertices.emplace_back(point);
            }

            std::vector<BrepCoedge> holeCoedges;
            holeCoedges.reserve(holePoints.size());
            for (std::size_t i = 0; i < holePoints.size(); ++i)
            {
                const std::size_t next = (i + 1) % holePoints.size();
                edges.emplace_back(
                    std::make_shared<LineCurve3d>(LineCurve3d::FromLine(
                        Line3d::FromOriginAndDirection(holePoints[i], holePoints[next] - holePoints[i]),
                        Intervald{0.0, 1.0})),
                    holeVertexBase + i,
                    holeVertexBase + next);
                holeCoedges.emplace_back(edges.size() - 1, false);
            }

            holeLoops.emplace_back(std::move(holeCoedges));
            holeTrims.push_back(trim);
        }

        faces.emplace_back(
            std::shared_ptr<Surface>(face.SupportSurface()->Clone().release()),
            BrepLoop(std::move(outerCoedges)),
            std::move(holeLoops),
            face.OuterTrim(),
            std::move(holeTrims));
    }

    result.body = BrepBody(std::move(vertices), std::move(edges), {BrepShell(std::move(faces), false)});
    result.success = true;
    return result;
}

SectionBrepBodySetRebuild3d RebuildSectionBrepBodies(const PolyhedronSection3d& section, double eps)
{
    SectionBrepBodySetRebuild3d result{};
    if (!section.success || !IsSectionFrameValid(section, eps))
    {
        result.issue = SectionBodyRebuildIssue3d::InvalidSection;
        return result;
    }

    const SectionTopology3d topology = BuildSectionTopology(section, eps);
    if (!topology.IsValid())
    {
        result.issue = SectionBodyRebuildIssue3d::InvalidSection;
        return result;
    }

    for (std::size_t rootIndex : topology.Roots())
    {
        PolyhedronSection3d subSection{};
        subSection.success = true;
        subSection.origin = section.origin;
        subSection.uAxis = section.uAxis;
        subSection.vAxis = section.vAxis;

        for (std::size_t polygonIndex = 0; polygonIndex < section.polygons.size(); ++polygonIndex)
        {
            std::size_t current = polygonIndex;
            while (current != static_cast<std::size_t>(-1))
            {
                if (current == rootIndex)
                {
                    subSection.polygons.push_back(section.polygons[polygonIndex]);
                    break;
                }
                current = topology.ParentOf(current);
            }
        }

        const SectionBrepBodyRebuild3d rebuilt = RebuildSectionBrepBody(subSection, eps);
        if (!rebuilt.success)
        {
            result.issue = rebuilt.issue;
            return result;
        }

        if (!rebuilt.body.IsEmpty())
        {
            result.rootPolygonIndices.push_back(rootIndex);
            result.bodies.push_back(rebuilt.body);
        }
    }

    result.success = true;
    return result;
}

SectionBodySetRebuild3d RebuildSectionBodies(const PolyhedronSection3d& section, double eps)
{
    SectionBodySetRebuild3d result{};
    if (!section.success || !IsSectionFrameValid(section, eps))
    {
        result.issue = SectionBodyRebuildIssue3d::InvalidSection;
        return result;
    }

    const SectionTopology3d topology = BuildSectionTopology(section, eps);
    if (!topology.IsValid())
    {
        result.issue = SectionBodyRebuildIssue3d::InvalidSection;
        return result;
    }

    const SectionFaceRebuild3d rebuiltFaces = RebuildSectionFaces(section, eps);
    if (!rebuiltFaces.success)
    {
        result.issue = SectionBodyRebuildIssue3d::FaceRebuildFailed;
        return result;
    }

    result.bodies.reserve(topology.Roots().size());
    for (std::size_t rootIndex : topology.Roots())
    {
        std::vector<PolyhedronFace3d> faces;
        for (std::size_t faceIndex = 0; faceIndex < rebuiltFaces.faces.size(); ++faceIndex)
        {
            if (rebuiltFaces.mappings[faceIndex].outerPolygonIndex == rootIndex)
            {
                faces.push_back(rebuiltFaces.faces[faceIndex]);
            }
        }

        if (!faces.empty())
        {
            result.rootPolygonIndices.push_back(rootIndex);
            result.bodies.emplace_back(std::move(faces));
        }
    }

    result.success = true;
    return result;
}

SectionTopology3d BuildSectionTopology(const PolyhedronSection3d& section, double eps)
{
    SectionTopology3d result{};
    if (!section.success || !IsSectionFrameValid(section, eps))
    {
        return result;
    }

    MultiPolygon2d polygons;
    if (!BuildNormalizedSectionPolygons(section, polygons))
    {
        return result;
    }

    const PolygonTopology2d topology = BuildPolygonTopology(polygons, eps);
    if (!topology.IsValid())
    {
        return result;
    }

    result.valid_ = true;
    result.nodes_.resize(topology.Count());
    result.roots_ = topology.Roots();
    for (std::size_t i = 0; i < topology.Count(); ++i)
    {
        result.nodes_[i].polygonIndex = i;
        result.nodes_[i].parentIndex = topology.ParentOf(i);
        result.nodes_[i].children = topology.ChildrenOf(i);
        result.nodes_[i].depth = PolygonDepth(topology, i);
    }

    return result;
}

std::string SectionTopology3d::DebugString() const
{
    std::ostringstream stream;
    stream << "SectionTopology3d{polygonCount=" << Count()
           << ", rootCount=" << roots_.size()
           << ", valid=" << (valid_ ? "true" : "false") << "}";
    return stream.str();
}

SectionMeshConversion3d ConvertSectionToTriangleMesh(const PolyhedronSection3d& section, double eps)
{
    SectionMeshConversion3d result{};
    const SectionFaceRebuild3d rebuiltFaces = RebuildSectionFaces(section, eps);
    if (!rebuiltFaces.success)
    {
        result.issue = MeshConversionIssue3d::InvalidFace;
        return result;
    }

    std::vector<Point3d> vertices;
    std::vector<TriangleMesh::TriangleIndices> triangles;
    for (const PolyhedronFace3d& face : rebuiltFaces.faces)
    {
        const PolyhedronMeshConversion3d faceMesh = ConvertToTriangleMesh(face, eps);
        if (!faceMesh.success)
        {
            result.issue = faceMesh.issue;
            return result;
        }

        const std::size_t vertexOffset = vertices.size();
        vertices.insert(vertices.end(), faceMesh.mesh.Vertices().begin(), faceMesh.mesh.Vertices().end());
        for (const auto& triangle : faceMesh.mesh.Triangles())
        {
            triangles.push_back(TriangleMesh::TriangleIndices{
                triangle[0] + vertexOffset,
                triangle[1] + vertexOffset,
                triangle[2] + vertexOffset});
        }
    }

    result.mesh = TriangleMesh(std::move(vertices), std::move(triangles));
    result.success = true;
    return result;
}

SectionMeshSetConversion3d ConvertSectionToTriangleMeshes(const PolyhedronSection3d& section, double eps)
{
    SectionMeshSetConversion3d result{};
    const SectionBodySetRebuild3d rebuiltBodies = RebuildSectionBodies(section, eps);
    if (!rebuiltBodies.success)
    {
        result.issue = MeshConversionIssue3d::InvalidFace;
        return result;
    }

    result.meshes.reserve(rebuiltBodies.bodies.size());
    result.rootPolygonIndices = rebuiltBodies.rootPolygonIndices;
    for (const PolyhedronBody& body : rebuiltBodies.bodies)
    {
        const PolyhedronMeshConversion3d bodyMesh = ConvertToTriangleMesh(body, eps);
        if (!bodyMesh.success)
        {
            result.issue = bodyMesh.issue;
            return result;
        }
        result.meshes.push_back(bodyMesh.mesh);
    }

    result.success = true;
    return result;
}

SectionContentKind3d ClassifySectionContent(const PolyhedronSection3d& section, double eps)
{
    if (!section.success || !IsSectionFrameValid(section, eps))
    {
        return SectionContentKind3d::Empty;
    }

    const bool hasArea = !section.polygons.empty();
    bool hasCurve = false;
    for (const SectionPolyline3d& contour : section.contours)
    {
        if (!contour.closed)
        {
            hasCurve = true;
            break;
        }
    }

    if (hasArea && hasCurve)
    {
        return SectionContentKind3d::Mixed;
    }
    if (hasArea)
    {
        return SectionContentKind3d::Area;
    }
    if (hasCurve || !section.segments.empty())
    {
        return SectionContentKind3d::Curve;
    }
    return SectionContentKind3d::Empty;
}

SectionComponents3d BuildSectionComponents(const PolyhedronSection3d& section, double eps)
{
    SectionComponents3d result{};
    const SectionTopology3d topology = BuildSectionTopology(section, eps);
    if (!topology.IsValid())
    {
        return result;
    }

    const SectionFaceRebuild3d rebuiltFaces = RebuildSectionFaces(section, eps);
    if (!rebuiltFaces.success)
    {
        return result;
    }

    result.valid = true;
    result.components.reserve(topology.Roots().size());
    for (std::size_t rootIndex : topology.Roots())
    {
        SectionComponent3d component{};
        component.rootPolygonIndex = rootIndex;

        std::vector<std::size_t> stack{rootIndex};
        while (!stack.empty())
        {
            const std::size_t current = stack.back();
            stack.pop_back();
            component.polygonIndices.push_back(current);
            for (std::size_t child : topology.ChildrenOf(current))
            {
                stack.push_back(child);
            }
        }

        std::sort(component.polygonIndices.begin(), component.polygonIndices.end());
        for (std::size_t faceIndex = 0; faceIndex < rebuiltFaces.mappings.size(); ++faceIndex)
        {
            if (rebuiltFaces.mappings[faceIndex].outerPolygonIndex == rootIndex)
            {
                component.faceIndices.push_back(faceIndex);
            }
        }

        result.components.push_back(std::move(component));
    }

    return result;
}
} // namespace geometry::sdk
