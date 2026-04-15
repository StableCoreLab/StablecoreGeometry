#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"

namespace Geometry::Sdk
{
enum class ISCPolylineClosure
{
    Open,
    Closed
};

class GEOMETRY_API ISCPolyline2d
{
public:
    ISCPolyline2d();
    explicit ISCPolyline2d(ISCPolylineClosure closure);
    ISCPolyline2d(std::vector<SCPoint2d> points, ISCPolylineClosure closure = ISCPolylineClosure::Open);
    ISCPolyline2d(const ISCPolyline2d& other);
    ISCPolyline2d(ISCPolyline2d&& other) noexcept;
    ISCPolyline2d& operator=(const ISCPolyline2d& other);
    ISCPolyline2d& operator=(ISCPolyline2d&& other) noexcept;
    ~ISCPolyline2d();

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] bool IsClosed() const;
    [[nodiscard]] std::size_t PointCount() const;
    [[nodiscard]] std::size_t SegmentCount() const;
    [[nodiscard]] double Length() const;
    [[nodiscard]] SCPoint2d PointAt(std::size_t index) const;
    [[nodiscard]] SCBox2d Bounds() const;
    [[nodiscard]] std::string DebugString() const;

private:
    struct SCImpl;
    std::unique_ptr<SCImpl> impl_;

    explicit ISCPolyline2d(std::unique_ptr<SCImpl> impl);

};
} // namespace Geometry::Sdk
