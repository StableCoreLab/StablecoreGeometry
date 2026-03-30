#include "sdk/GeometryProjection.h"

#include <algorithm>

namespace geometry::sdk
{
namespace
{
struct PlaneProjectionBasis
{
    Vector3d u{};
    Vector3d v{};
};

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
} // namespace

LineProjection3d ProjectPointToLine(
    const Point3d& point,
    const Line3d& line,
    const GeometryTolerance3d& tolerance)
{
    if (!line.IsValid(tolerance.distanceEpsilon))
    {
        return LineProjection3d{line.origin, 0.0, (point - line.origin).LengthSquared(), false};
    }

    const double directionLengthSquared = line.direction.LengthSquared();
    const double parameter = Dot(point - line.origin, line.direction) / directionLengthSquared;
    const Point3d projectedPoint = line.PointAt(parameter);
    return LineProjection3d{projectedPoint, parameter, (point - projectedPoint).LengthSquared(), true};
}

PlaneProjection3d ProjectPointToPlane(
    const Point3d& point,
    const Plane& plane,
    const GeometryTolerance3d& tolerance)
{
    if (!plane.IsValid(tolerance.distanceEpsilon))
    {
        return PlaneProjection3d{plane.origin, 0.0, (point - plane.origin).LengthSquared()};
    }

    const Vector3d unitNormal = plane.UnitNormal(tolerance.distanceEpsilon);
    const double signedDistance = Dot(point - plane.origin, unitNormal);
    const Point3d projectedPoint = point - unitNormal * signedDistance;
    return PlaneProjection3d{projectedPoint, signedDistance, signedDistance * signedDistance};
}

FaceProjection3d ProjectFaceToPolygon2d(const PolyhedronFace3d& face, const GeometryTolerance3d& tolerance)
{
    if (!face.IsValid(tolerance.distanceEpsilon))
    {
        return {};
    }

    const Plane plane = face.SupportPlane();
    const PlaneProjectionBasis basis = BuildPlaneProjectionBasis(plane, tolerance.distanceEpsilon);
    std::vector<Point2d> outerPoints;
    outerPoints.reserve(face.OuterLoop().VertexCount());
    for (const Point3d& vertex : face.OuterLoop().Vertices())
    {
        outerPoints.push_back(ProjectToLocalPlaneCoordinates(vertex, plane, basis));
    }

    std::vector<Polyline2d> holeRings;
    holeRings.reserve(face.HoleCount());
    for (std::size_t i = 0; i < face.HoleCount(); ++i)
    {
        std::vector<Point2d> holePoints;
        const PolyhedronLoop3d hole = face.HoleAt(i);
        holePoints.reserve(hole.VertexCount());
        for (const Point3d& vertex : hole.Vertices())
        {
            holePoints.push_back(ProjectToLocalPlaneCoordinates(vertex, plane, basis));
        }
        holeRings.emplace_back(std::move(holePoints), PolylineClosure::Closed);
    }

    Polygon2d polygon(Polyline2d(std::move(outerPoints), PolylineClosure::Closed), std::move(holeRings));
    if (!polygon.IsValid())
    {
        return {};
    }

    return FaceProjection3d{true, std::move(polygon), plane.origin, basis.u, basis.v};
}
} // namespace geometry::sdk
