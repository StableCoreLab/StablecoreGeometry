#include "sdk/GeometryMeshRepair.h"

#include <array>
#include <deque>
#include <utility>
#include <vector>

#include "sdk/GeometryMeshOps.h"

namespace geometry::sdk
{
namespace
{
constexpr std::size_t kNoTriangle = std::size_t(-1);

[[nodiscard]] std::array<std::array<std::size_t, 2>, 3> TriangleEdges(
    const TriangleMesh::TriangleIndices& tri)
{
    return {{
        {tri[0], tri[1]},
        {tri[1], tri[2]},
        {tri[2], tri[0]},
    }};
}

[[nodiscard]] bool HasSameSharedEdgeDirection(
    const TriangleMesh& mesh,
    std::size_t firstTriangle,
    std::size_t secondTriangle)
{
    const auto firstEdges = TriangleEdges(mesh.TriangleIndicesAt(firstTriangle));
    const auto secondEdges = TriangleEdges(mesh.TriangleIndicesAt(secondTriangle));
    for (const auto& first : firstEdges)
    {
        for (const auto& second : secondEdges)
        {
            const bool sameUndirectedEdge =
                (first[0] == second[0] && first[1] == second[1]) ||
                (first[0] == second[1] && first[1] == second[0]);
            if (!sameUndirectedEdge)
            {
                continue;
            }

            return first[0] == second[0] && first[1] == second[1];
        }
    }

    return false;
}
} // namespace

TriangleMeshRepair3d OrientTriangleMeshConsistently(const TriangleMesh& mesh, double eps)
{
    if (!mesh.IsValid(eps))
    {
        return {false, MeshRepairIssue3d::InvalidMesh, {}};
    }

    if (!IsManifoldTriangleMesh(mesh))
    {
        return {false, MeshRepairIssue3d::NonManifold, {}};
    }

    const auto adjacency = ComputeTriangleAdjacency(mesh);
    std::vector<int> flipParity(mesh.TriangleCount(), -1);
    for (std::size_t seed = 0; seed < mesh.TriangleCount(); ++seed)
    {
        if (flipParity[seed] != -1)
        {
            continue;
        }

        std::deque<std::size_t> queue{seed};
        flipParity[seed] = 0;
        while (!queue.empty())
        {
            const std::size_t triangleIndex = queue.front();
            queue.pop_front();
            for (std::size_t neighbor : adjacency[triangleIndex].adjacentTriangles)
            {
                if (neighbor == kNoTriangle)
                {
                    continue;
                }

                const bool sameDirection = HasSameSharedEdgeDirection(mesh, triangleIndex, neighbor);
                const int expectedParity = sameDirection ? 1 - flipParity[triangleIndex] : flipParity[triangleIndex];
                if (flipParity[neighbor] == -1)
                {
                    flipParity[neighbor] = expectedParity;
                    queue.push_back(neighbor);
                    continue;
                }

                if (flipParity[neighbor] != expectedParity)
                {
                    return {false, MeshRepairIssue3d::NonOrientable, {}};
                }
            }
        }
    }

    TriangleMesh repaired = mesh;
    auto& triangles = repaired.Triangles();
    for (std::size_t i = 0; i < triangles.size(); ++i)
    {
        if (flipParity[i] != 1)
        {
            continue;
        }

        std::swap(triangles[i][1], triangles[i][2]);
    }

    return {true, MeshRepairIssue3d::None, std::move(repaired)};
}
} // namespace geometry::sdk
