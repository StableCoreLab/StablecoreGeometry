#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "sdk/Surface.h"

namespace geometry::sdk
{
class GEOMETRY_API NurbsSurface final : public Surface
{
public:
    NurbsSurface() = default;
    NurbsSurface(
        int degreeU,
        int degreeV,
        std::size_t controlPointCountU,
        std::size_t controlPointCountV,
        std::vector<Point3d> controlPoints,
        std::vector<double> knotsU,
        std::vector<double> knotsV)
        : degreeU_(degreeU),
          degreeV_(degreeV),
          controlPointCountU_(controlPointCountU),
          controlPointCountV_(controlPointCountV),
          controlPoints_(std::move(controlPoints)),
          knotsU_(std::move(knotsU)),
          knotsV_(std::move(knotsV))
    {
    }

    [[nodiscard]] bool IsValid(const GeometryTolerance3d& tolerance = {}) const override
    {
        if (degreeU_ < 1 || degreeV_ < 1 || controlPointCountU_ < 2 || controlPointCountV_ < 2 ||
            controlPoints_.size() != controlPointCountU_ * controlPointCountV_ || knotsU_.size() < 2 ||
            knotsV_.size() < 2)
        {
            return false;
        }

        for (const Point3d& point : controlPoints_)
        {
            if (!point.IsValid())
            {
                return false;
            }
        }

        for (std::size_t i = 1; i < knotsU_.size(); ++i)
        {
            if (knotsU_[i] + tolerance.parameterEpsilon < knotsU_[i - 1])
            {
                return false;
            }
        }
        for (std::size_t i = 1; i < knotsV_.size(); ++i)
        {
            if (knotsV_[i] + tolerance.parameterEpsilon < knotsV_[i - 1])
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] Intervald URange() const override
    {
        return knotsU_.size() >= 2 ? Intervald{knotsU_.front(), knotsU_.back()} : Intervald{};
    }

    [[nodiscard]] Intervald VRange() const override
    {
        return knotsV_.size() >= 2 ? Intervald{knotsV_.front(), knotsV_.back()} : Intervald{};
    }

    [[nodiscard]] Point3d PointAt(double u, double v) const override
    {
        if (controlPoints_.empty() || controlPointCountU_ < 2 || controlPointCountV_ < 2)
        {
            return {};
        }

        const auto sampleIndex = [](double normalized, std::size_t count) {
            const double scaled = std::clamp(normalized, 0.0, 1.0) * static_cast<double>(count - 1);
            const std::size_t index = std::min(static_cast<std::size_t>(std::floor(scaled)), count - 2);
            return std::pair<std::size_t, double>{index, scaled - static_cast<double>(index)};
        };

        const Intervald uRange = URange();
        const Intervald vRange = VRange();
        const double normalizedU = uRange.Length() <= geometry::kDefaultEpsilon ? 0.0 : (u - uRange.min) / uRange.Length();
        const double normalizedV = vRange.Length() <= geometry::kDefaultEpsilon ? 0.0 : (v - vRange.min) / vRange.Length();
        const auto [iu, tu] = sampleIndex(normalizedU, controlPointCountU_);
        const auto [iv, tv] = sampleIndex(normalizedV, controlPointCountV_);

        const Point3d& p00 = controlPoints_[iv * controlPointCountU_ + iu];
        const Point3d& p10 = controlPoints_[iv * controlPointCountU_ + iu + 1];
        const Point3d& p01 = controlPoints_[(iv + 1) * controlPointCountU_ + iu];
        const Point3d& p11 = controlPoints_[(iv + 1) * controlPointCountU_ + iu + 1];
        const Point3d lower = p00 + (p10 - p00) * tu;
        const Point3d upper = p01 + (p11 - p01) * tu;
        return lower + (upper - lower) * tv;
    }

    [[nodiscard]] SurfaceEval3d Evaluate(double u, double v, int derivativeOrder = 1) const override
    {
        derivativeOrder = std::clamp(derivativeOrder, 0, 1);
        SurfaceEval3d eval{};
        eval.point = PointAt(u, v);
        eval.derivativeOrder = derivativeOrder;
        if (derivativeOrder >= 1)
        {
            const double du = std::max(URange().Length(), 1.0) * 1e-6;
            const double dv = std::max(VRange().Length(), 1.0) * 1e-6;
            eval.derivativeU = (PointAt(u + du, v) - PointAt(u - du, v)) / (2.0 * du);
            eval.derivativeV = (PointAt(u, v + dv) - PointAt(u, v - dv)) / (2.0 * dv);
            eval.normal = Cross(eval.derivativeU, eval.derivativeV);
        }
        return eval;
    }

    [[nodiscard]] Box3d Bounds() const override
    {
        Box3d bounds{};
        for (const Point3d& point : controlPoints_)
        {
            bounds.ExpandToInclude(point);
        }
        return bounds;
    }

    [[nodiscard]] std::unique_ptr<Surface> Clone() const override
    {
        return std::make_unique<NurbsSurface>(*this);
    }

private:
    int degreeU_{1};
    int degreeV_{1};
    std::size_t controlPointCountU_{0};
    std::size_t controlPointCountV_{0};
    std::vector<Point3d> controlPoints_{};
    std::vector<double> knotsU_{0.0, 1.0};
    std::vector<double> knotsV_{0.0, 1.0};
};
} // namespace geometry::sdk
