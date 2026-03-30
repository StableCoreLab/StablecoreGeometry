#include "sdk/GeometryHealing.h"

#include "sdk/GeometryMeshRepair.h"
#include "sdk/GeometryValidation.h"

namespace geometry::sdk
{
namespace
{
HealingIssue3d MapMeshRepairIssue(const MeshRepairIssue3d issue)
{
    switch (issue)
    {
    case MeshRepairIssue3d::None:
        return HealingIssue3d::None;
    case MeshRepairIssue3d::InvalidMesh:
        return HealingIssue3d::InvalidMesh;
    default:
        return HealingIssue3d::RepairFailed;
    }
}
} // namespace

MeshHealing3d Heal(const TriangleMesh& mesh, double eps)
{
    const MeshValidation3d validation = Validate(mesh, eps);
    if (!validation.valid)
    {
        return {false, HealingIssue3d::InvalidMesh, {}};
    }

    const TriangleMeshRepair3d closed = ClosePlanarBoundaryLoops(mesh, eps);
    if (closed.success)
    {
        return {true, HealingIssue3d::None, std::move(closed.mesh)};
    }

    const TriangleMeshRepair3d oriented = OrientTriangleMeshConsistently(mesh, eps);
    if (oriented.success)
    {
        return {true, HealingIssue3d::None, std::move(oriented.mesh)};
    }

    return {false, MapMeshRepairIssue(oriented.issue), {}};
}

PolyhedronHealing3d Heal(const PolyhedronBody& body, double eps)
{
    const PolyhedronValidation3d validation = Validate(body, eps);
    if (!validation.valid)
    {
        return {false, HealingIssue3d::InvalidPolyhedron, {}};
    }

    return {true, HealingIssue3d::None, body};
}

BrepHealing3d Heal(const BrepBody& body, const GeometryTolerance3d& tolerance)
{
    const BrepValidation3d validation = Validate(body, tolerance);
    if (!validation.valid)
    {
        return {false, HealingIssue3d::InvalidBrep, {}};
    }

    return {true, HealingIssue3d::None, body};
}
} // namespace geometry::sdk
