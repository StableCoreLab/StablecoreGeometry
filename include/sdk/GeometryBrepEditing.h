#pragma once

#include <cstddef>

#include "export/GeometryExport.h"
#include "sdk/BrepLoop.h"

namespace geometry::sdk
{
enum class BrepLoopEditIssue3d
{
    None,
    InvalidLoop,
    InvalidIndex,
    InvalidResult
};

struct GEOMETRY_API BrepLoopEdit3d
{
    bool success{false};
    BrepLoopEditIssue3d issue{BrepLoopEditIssue3d::None};
    BrepLoop loop{};

    [[nodiscard]] bool IsValid() const
    {
        return !success || loop.IsValid();
    }
};

[[nodiscard]] GEOMETRY_API BrepLoopEdit3d InsertCoedge(
    const BrepLoop& loop,
    std::size_t index,
    const BrepCoedge& coedge);

[[nodiscard]] GEOMETRY_API BrepLoopEdit3d RemoveCoedge(
    const BrepLoop& loop,
    std::size_t index);

[[nodiscard]] GEOMETRY_API BrepLoopEdit3d FlipCoedgeDirection(
    const BrepLoop& loop,
    std::size_t index);
} // namespace geometry::sdk
