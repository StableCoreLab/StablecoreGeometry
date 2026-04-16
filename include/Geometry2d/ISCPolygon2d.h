#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "Export/GeometryExport.h"
#include "Geometry2d/ISCPolyline2d.h"
#include "Core/GeometryTypes.h"

namespace Geometry::Sdk
{
class GEOMETRY_API ISCPolygon2d
{
public:
    ISCPolygon2d();
    explicit ISCPolygon2d(ISCPolyline2d outerRing);
    ISCPolygon2d(ISCPolyline2d outerRing, std::vector<ISCPolyline2d> holes);
    ISCPolygon2d(const ISCPolygon2d& other);
    ISCPolygon2d(ISCPolygon2d&& other) noexcept;
    ISCPolygon2d& operator=(const ISCPolygon2d& other);
    ISCPolygon2d& operator=(ISCPolygon2d&& other) noexcept;
    ~ISCPolygon2d();

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] std::size_t PointCount() const;
    [[nodiscard]] std::size_t SegmentCount() const;
    [[nodiscard]] std::size_t HoleCount() const;
    [[nodiscard]] ISCPolyline2d OuterRing() const;
    [[nodiscard]] ISCPolyline2d HoleAt(std::size_t index) const;
    [[nodiscard]] double Area() const;
    [[nodiscard]] SCBox2d Bounds() const;
    [[nodiscard]] std::string DebugString() const;

private:
    struct SCImpl;
    std::unique_ptr<SCImpl> impl_;
};
} // namespace Geometry::Sdk
