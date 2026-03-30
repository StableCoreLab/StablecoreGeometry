#pragma once

#include <sstream>
#include <string>

#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"

namespace geometry::sdk
{
class GEOMETRY_API BrepVertex
{
public:
    BrepVertex() = default;
    explicit BrepVertex(Point3d point) : point_(point) {}

    [[nodiscard]] bool IsValid() const
    {
        return point_.IsValid();
    }

    [[nodiscard]] Point3d Point() const
    {
        return point_;
    }

    [[nodiscard]] std::string DebugString() const
    {
        std::ostringstream stream;
        stream << "BrepVertex{point=" << point_.DebugString() << "}";
        return stream.str();
    }

private:
    Point3d point_{};
};
} // namespace geometry::sdk
