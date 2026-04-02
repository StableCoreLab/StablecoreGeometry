#include "sdk/GeometryBrepEditing.h"

#include <vector>

namespace geometry::sdk
{
BrepLoopEdit3d InsertCoedge(const BrepLoop& loop, std::size_t index, const BrepCoedge& coedge)
{
    if (!loop.IsValid() || !coedge.IsValid())
    {
        return {false, BrepLoopEditIssue3d::InvalidLoop, {}};
    }

    if (index > loop.CoedgeCount())
    {
        return {false, BrepLoopEditIssue3d::InvalidIndex, {}};
    }

    std::vector<BrepCoedge> coedges = loop.Coedges();
    coedges.insert(coedges.begin() + static_cast<std::ptrdiff_t>(index), coedge);

    BrepLoop edited(std::move(coedges));
    if (!edited.IsValid())
    {
        return {false, BrepLoopEditIssue3d::InvalidResult, {}};
    }

    return {true, BrepLoopEditIssue3d::None, std::move(edited)};
}

BrepLoopEdit3d RemoveCoedge(const BrepLoop& loop, std::size_t index)
{
    if (!loop.IsValid())
    {
        return {false, BrepLoopEditIssue3d::InvalidLoop, {}};
    }

    if (index >= loop.CoedgeCount())
    {
        return {false, BrepLoopEditIssue3d::InvalidIndex, {}};
    }

    std::vector<BrepCoedge> coedges = loop.Coedges();
    coedges.erase(coedges.begin() + static_cast<std::ptrdiff_t>(index));

    BrepLoop edited(std::move(coedges));
    if (!edited.IsValid())
    {
        return {false, BrepLoopEditIssue3d::InvalidResult, {}};
    }

    return {true, BrepLoopEditIssue3d::None, std::move(edited)};
}

BrepLoopEdit3d FlipCoedgeDirection(const BrepLoop& loop, std::size_t index)
{
    if (!loop.IsValid())
    {
        return {false, BrepLoopEditIssue3d::InvalidLoop, {}};
    }

    if (index >= loop.CoedgeCount())
    {
        return {false, BrepLoopEditIssue3d::InvalidIndex, {}};
    }

    std::vector<BrepCoedge> coedges = loop.Coedges();
    const BrepCoedge target = coedges[index];
    coedges[index] = BrepCoedge(target.EdgeIndex(), !target.Reversed());

    BrepLoop edited(std::move(coedges));
    if (!edited.IsValid())
    {
        return {false, BrepLoopEditIssue3d::InvalidResult, {}};
    }

    return {true, BrepLoopEditIssue3d::None, std::move(edited)};
}
} // namespace geometry::sdk
