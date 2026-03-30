#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "sdk/Curve3d.h"

namespace geometry::sdk
{
class GEOMETRY_API NurbsCurve3d final : public Curve3d
{
public:
    NurbsCurve3d() = default;
    NurbsCurve3d(
        int degree,
        std::vector<Point3d> controlPoints,
        std::vector<double> knots,
        bool periodic = false)
        : degree_(degree), controlPoints_(std::move(controlPoints)), knots_(std::move(knots)), periodic_(periodic)
    {
    }

    [[nodiscard]] bool IsValid(const GeometryTolerance3d& tolerance = {}) const override
    {
        if (degree_ < 1 || controlPoints_.size() < 2 || knots_.size() < 2)
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

        for (std::size_t i = 1; i < knots_.size(); ++i)
        {
            if (knots_[i] + tolerance.parameterEpsilon < knots_[i - 1])
            {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] Intervald ParameterRange() const override
    {
        return knots_.size() >= 2 ? Intervald{knots_.front(), knots_.back()} : Intervald{};
    }

    [[nodiscard]] bool IsClosed(const GeometryTolerance3d& tolerance = {}) const override
    {
        return controlPoints_.size() >= 2 &&
               controlPoints_.front().AlmostEquals(controlPoints_.back(), tolerance.distanceEpsilon);
    }

    [[nodiscard]] bool IsPeriodic(const GeometryTolerance3d& tolerance = {}) const override
    {
        (void)tolerance;
        return periodic_;
    }

    [[nodiscard]] Point3d PointAt(double parameter) const override
    {
        if (controlPoints_.empty())
        {
            return {};
        }
        if (controlPoints_.size() == 1 || knots_.size() < 2)
        {
            return controlPoints_.front();
        }

        const Intervald range = ParameterRange();
        const double clamped = std::clamp(parameter, range.min, range.max);
        const double normalized =
            range.Length() <= geometry::kDefaultEpsilon ? 0.0 : (clamped - range.min) / range.Length();
        const double scaled = normalized * static_cast<double>(controlPoints_.size() - 1);
        const std::size_t index =
            std::min(static_cast<std::size_t>(std::floor(scaled)), controlPoints_.size() - 2);
        const double local = scaled - static_cast<double>(index);
        return controlPoints_[index] + (controlPoints_[index + 1] - controlPoints_[index]) * local;
    }

    [[nodiscard]] CurveEval3d Evaluate(double parameter, int derivativeOrder = 1) const override
    {
        derivativeOrder = std::clamp(derivativeOrder, 0, 2);
        CurveEval3d eval{};
        eval.point = PointAt(parameter);
        eval.derivativeOrder = derivativeOrder;
        if (derivativeOrder >= 1 && controlPoints_.size() >= 2)
        {
            const Intervald range = ParameterRange();
            const double step = range.Length() <= geometry::kDefaultEpsilon
                                    ? 1.0
                                    : range.Length() / static_cast<double>(controlPoints_.size() - 1);
            eval.firstDerivative = (PointAt(parameter + step) - PointAt(parameter - step)) / (2.0 * step);
        }
        if (derivativeOrder >= 2)
        {
            eval.secondDerivative = Vector3d{};
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

    [[nodiscard]] std::unique_ptr<Curve3d> Clone() const override
    {
        return std::make_unique<NurbsCurve3d>(*this);
    }

    [[nodiscard]] int Degree() const
    {
        return degree_;
    }

    [[nodiscard]] const std::vector<Point3d>& ControlPoints() const
    {
        return controlPoints_;
    }

    [[nodiscard]] const std::vector<double>& Knots() const
    {
        return knots_;
    }

private:
    int degree_{1};
    std::vector<Point3d> controlPoints_{};
    std::vector<double> knots_{0.0, 1.0};
    bool periodic_{false};
};
} // namespace geometry::sdk
