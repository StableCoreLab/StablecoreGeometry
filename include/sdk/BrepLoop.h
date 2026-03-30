#pragma once

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "export/GeometryExport.h"
#include "sdk/BrepCoedge.h"

namespace geometry::sdk
{
class GEOMETRY_API BrepLoop
{
public:
    BrepLoop() = default;
    explicit BrepLoop(std::vector<BrepCoedge> coedges) : coedges_(std::move(coedges)) {}

    [[nodiscard]] bool IsValid() const
    {
        if (coedges_.empty())
        {
            return false;
        }

        for (const BrepCoedge& coedge : coedges_)
        {
            if (!coedge.IsValid())
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] std::size_t CoedgeCount() const
    {
        return coedges_.size();
    }

    [[nodiscard]] BrepCoedge CoedgeAt(std::size_t index) const
    {
        return coedges_.at(index);
    }

    [[nodiscard]] const std::vector<BrepCoedge>& Coedges() const
    {
        return coedges_;
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "BrepLoop{coedgeCount=" << CoedgeCount() << "}";
        return stream.str();
    }

private:
    std::vector<BrepCoedge> coedges_{};
};
} // namespace geometry::sdk
