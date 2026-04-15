#pragma once

#include <memory>

#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"

namespace Geometry::Sdk
{
class GEOMETRY_API SCCurve3d
{
public:
    virtual ~SCCurve3d() = default;

    [[nodiscard]] virtual bool IsValid(const SCGeometryTolerance3d& tolerance = {}) const = 0;
    [[nodiscard]] virtual SCIntervald ParameterRange() const = 0;
    [[nodiscard]] virtual bool IsClosed(const SCGeometryTolerance3d& tolerance = {}) const = 0;
    [[nodiscard]] virtual bool IsPeriodic(const SCGeometryTolerance3d& tolerance = {}) const = 0;
    [[nodiscard]] virtual Point3d PointAt(double parameter) const = 0;
    [[nodiscard]] virtual SCCurveEval3d Evaluate(double parameter, int derivativeOrder = 1) const = 0;
    [[nodiscard]] virtual SCBox3d Bounds() const = 0;
    [[nodiscard]] virtual std::unique_ptr<SCCurve3d> Clone() const = 0;

    [[nodiscard]] double StartT() const
    {
        return ParameterRange().min;
    }

    [[nodiscard]] double EndT() const
    {
        return ParameterRange().max;
    }
};
} // namespace Geometry::Sdk
