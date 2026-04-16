#pragma once

#include <cstddef>
#include <initializer_list>
#include <string>
#include <vector>

#include "Export/GeometryExport.h"
#include "Geometry2d/ISCPolygon2d.h"

namespace Geometry::Sdk
{
class GEOMETRY_API SCMultiPolygon2d
{
public:
    SCMultiPolygon2d() = default;
    explicit SCMultiPolygon2d(std::vector<ISCPolygon2d> polygons);
    SCMultiPolygon2d(std::initializer_list<ISCPolygon2d> polygons);

    [[nodiscard]] std::size_t Count() const;
    [[nodiscard]] bool IsEmpty() const;
    [[nodiscard]] bool IsValid() const;

    void Clear();
    void Add(ISCPolygon2d polygon);

    [[nodiscard]] const ISCPolygon2d& PolygonAt(std::size_t index) const;
    [[nodiscard]] ISCPolygon2d& PolygonAt(std::size_t index);

    [[nodiscard]] const ISCPolygon2d& operator[](std::size_t index) const;
    [[nodiscard]] ISCPolygon2d& operator[](std::size_t index);

    [[nodiscard]] std::size_t PointCount() const;
    [[nodiscard]] std::size_t SegmentCount() const;
    [[nodiscard]] std::size_t HoleCount() const;
    [[nodiscard]] SCBox2d Bounds() const;
    [[nodiscard]] std::string DebugString() const;

    [[nodiscard]] const std::vector<ISCPolygon2d>& Data() const;
    [[nodiscard]] std::vector<ISCPolygon2d>& Data();

private:
    std::vector<ISCPolygon2d> polygons_{};
};
} // namespace Geometry::Sdk

