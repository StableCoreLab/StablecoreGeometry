#pragma once

#include <memory>
#include <string>

#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"
#include "types/ISCSegment2.h"

namespace Geometry::Sdk
{
using Geometry::ISCSegmentKind2;

class GEOMETRY_API SCSegment2d
{
public:
    virtual ~SCSegment2d() = default;

    [[nodiscard]] virtual ISCSegmentKind2 Kind() const = 0;
    [[nodiscard]] virtual bool IsValid() const = 0;
    [[nodiscard]] virtual SCPoint2d StartPoint() const = 0;
    [[nodiscard]] virtual SCPoint2d EndPoint() const = 0;
    [[nodiscard]] virtual double Length() const = 0;
    [[nodiscard]] virtual SCBox2d Bounds() const = 0;
    [[nodiscard]] virtual SCPoint2d PointAt(double parameter) const = 0;
    [[nodiscard]] virtual SCPoint2d PointAtLength(double distanceFromStart, bool clampToSegment = false) const = 0;
    [[nodiscard]] virtual std::string DebugString() const = 0;
    [[nodiscard]] virtual std::unique_ptr<SCSegment2d> Clone() const = 0;
};
} // namespace Geometry::Sdk
