#pragma once

#include <cstddef>
#include <initializer_list>
#include <string>
#include <vector>

#include "Export/GeometryExport.h"
#include "Geometry2d/ISCPolyline2d.h"

namespace Geometry::Sdk
{
class GEOMETRY_API SCMultiPolyline2d
{
public:
    SCMultiPolyline2d() = default;
    explicit SCMultiPolyline2d(std::vector<ISCPolyline2d> polylines);
    SCMultiPolyline2d(std::initializer_list<ISCPolyline2d> polylines);

    [[nodiscard]] std::size_t Count() const;
    [[nodiscard]] bool IsEmpty() const;
    [[nodiscard]] bool IsValid() const;

    void Clear();
    void Add(ISCPolyline2d polyline);

    [[nodiscard]] const ISCPolyline2d& PolylineAt(std::size_t index) const;
    [[nodiscard]] ISCPolyline2d& PolylineAt(std::size_t index);

    [[nodiscard]] const ISCPolyline2d& operator[](std::size_t index) const;
    [[nodiscard]] ISCPolyline2d& operator[](std::size_t index);

    [[nodiscard]] std::size_t PointCount() const;
    [[nodiscard]] std::size_t SegmentCount() const;
    [[nodiscard]] SCBox2d Bounds() const;
    [[nodiscard]] std::string DebugString() const;

    [[nodiscard]] const std::vector<ISCPolyline2d>& Data() const;
    [[nodiscard]] std::vector<ISCPolyline2d>& Data();

private:
    std::vector<ISCPolyline2d> polylines_{};
};
} // namespace Geometry::Sdk

