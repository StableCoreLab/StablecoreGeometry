#include "sdk/PlaneSurface.h"

#include <algorithm>
#include <cmath>

namespace geometry::sdk
{
namespace
{
[[nodiscard]] Vector3d MakePerpendicularVector(const Vector3d& normal, double eps)
{
    const Vector3d normalized = normal.Normalized(eps);
    if (normalized.Length() <= eps)
    {
        return Vector3d{};
    }

    // Prefer world-aligned in-plane axes when possible so plane UV coordinates
    // remain stable and intuitive, especially for axis-aligned support planes.
    Vector3d referenceAxis{1.0, 0.0, 0.0};
    if (std::abs(Dot(normalized, referenceAxis)) >= 1.0 - eps)
    {
        referenceAxis = Vector3d{0.0, 1.0, 0.0};
    }

    const Vector3d inPlane = referenceAxis - normalized * Dot(referenceAxis, normalized);
    return inPlane.Normalized(eps);
}
} // namespace

PlaneSurface PlaneSurface::FromPlane(
    const Plane& plane,
    Intervald uRange,
    Intervald vRange,
    double eps)
{
    const Vector3d uAxis = MakePerpendicularVector(plane.normal, eps);
    const Vector3d vAxis = Cross(plane.UnitNormal(eps), uAxis).Normalized(eps);
    return PlaneSurface(plane, uAxis, vAxis, uRange, vRange);
}
} // namespace geometry::sdk
