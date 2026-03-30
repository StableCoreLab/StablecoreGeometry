#pragma once

#include <memory>

#include "sdk/Polyline2d.h"
#include "sdk/Surface.h"

namespace geometry::sdk
{
class GEOMETRY_API CurveOnSurface
{
public:
    CurveOnSurface() = default;
    CurveOnSurface(std::shared_ptr<Surface> supportSurface, Polyline2d uvCurve)
        : supportSurface_(std::move(supportSurface)), uvCurve_(std::move(uvCurve))
    {
    }

    [[nodiscard]] bool IsValid(const GeometryTolerance3d& tolerance = {}) const
    {
        return supportSurface_ != nullptr && supportSurface_->IsValid(tolerance) && uvCurve_.IsValid();
    }

    [[nodiscard]] const Surface* SupportSurface() const
    {
        return supportSurface_.get();
    }

    [[nodiscard]] const Polyline2d& UvCurve() const
    {
        return uvCurve_;
    }

private:
    std::shared_ptr<Surface> supportSurface_{};
    Polyline2d uvCurve_{};
};
} // namespace geometry::sdk
