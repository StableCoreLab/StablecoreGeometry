#include "sdk/GeometryRelation.h"

#include <cmath>

#include "sdk/GeometryProjection.h"

namespace geometry::sdk
{
PointPlaneSide3d LocatePoint(
    const Point3d& point,
    const Plane& plane,
    const GeometryTolerance3d& tolerance)
{
    if (!plane.IsValid(tolerance.distanceEpsilon))
    {
        return PointPlaneSide3d::OnPlane;
    }

    const double signedDistance = plane.SignedDistanceTo(point, tolerance.distanceEpsilon);
    if (signedDistance > tolerance.distanceEpsilon)
    {
        return PointPlaneSide3d::Above;
    }
    if (signedDistance < -tolerance.distanceEpsilon)
    {
        return PointPlaneSide3d::Below;
    }
    return PointPlaneSide3d::OnPlane;
}

PointContainment2d LocatePoint(
    const Point3d& point,
    const PolyhedronFace3d& face,
    const GeometryTolerance3d& tolerance)
{
    if (!face.IsValid(tolerance.distanceEpsilon))
    {
        return PointContainment2d::Outside;
    }

    if (LocatePoint(point, face.SupportPlane(), tolerance) != PointPlaneSide3d::OnPlane)
    {
        return PointContainment2d::Outside;
    }

    const FaceProjection3d projection = ProjectFaceToPolygon2d(face, tolerance);
    if (!projection.success)
    {
        return PointContainment2d::Outside;
    }

    const Vector3d delta = point - projection.origin;
    return LocatePoint(
        Point2d{Dot(delta, projection.uAxis), Dot(delta, projection.vAxis)},
        projection.polygon,
        tolerance.distanceEpsilon);
}

PointContainment2d LocatePoint(
    const Point3d& point,
    const BrepFace& face,
    const GeometryTolerance3d& tolerance)
{
    if (!face.IsValid(tolerance) || face.SupportSurface() == nullptr)
    {
        return PointContainment2d::Outside;
    }

    const BrepFaceProjection3d projection = ProjectPointToBrepFace(point, face, tolerance);
    if (!projection.success ||
        projection.distanceSquared > tolerance.distanceEpsilon * tolerance.distanceEpsilon)
    {
        return PointContainment2d::Outside;
    }

    if (projection.onBoundary)
    {
        return PointContainment2d::OnBoundary;
    }

    return projection.onTrimmedFace ? PointContainment2d::Inside : PointContainment2d::Outside;
}

bool IsParallel(
    const Vector3d& first,
    const Vector3d& second,
    const GeometryTolerance3d& tolerance)
{
    const double firstLength = first.Length();
    const double secondLength = second.Length();
    if (firstLength <= tolerance.distanceEpsilon || secondLength <= tolerance.distanceEpsilon)
    {
        return false;
    }

    const Vector3d cross = Cross(first, second);
    return cross.Length() <= tolerance.angleEpsilon * firstLength * secondLength + tolerance.distanceEpsilon;
}

bool IsPerpendicular(
    const Vector3d& first,
    const Vector3d& second,
    const GeometryTolerance3d& tolerance)
{
    const double firstLength = first.Length();
    const double secondLength = second.Length();
    if (firstLength <= tolerance.distanceEpsilon || secondLength <= tolerance.distanceEpsilon)
    {
        return false;
    }

    return std::abs(Dot(first, second)) <= tolerance.angleEpsilon * firstLength * secondLength +
           tolerance.distanceEpsilon;
}
} // namespace geometry::sdk
