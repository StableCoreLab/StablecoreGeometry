#pragma once

#include <cstddef>
#include <vector>

#include "export/GeometryExport.h"
#include "sdk/BrepBody.h"
#include "sdk/PolyhedronBody.h"

namespace Geometry::Sdk
{
enum class BrepConversionIssue3d
{
    None,
    InvalidFace,
    InvalidBody,
    UnsupportedSurface,
    InvalidTrim
};

struct GEOMETRY_API BrepFaceConversion3d
{
    bool success{false};
    BrepConversionIssue3d issue{BrepConversionIssue3d::None};
    PolyhedronFace3d face{};

    [[nodiscard]] bool IsValid(double eps = Geometry::kDefaultEpsilon) const
    {
        return !success || face.IsValid(eps);
    }
};

struct GEOMETRY_API BrepBodyConversion3d
{
    bool success{false};
    BrepConversionIssue3d issue{BrepConversionIssue3d::None};
    std::size_t faceIndex{0};
    PolyhedronBody body{};

    [[nodiscard]] bool IsValid(double eps = Geometry::kDefaultEpsilon) const
    {
        return !success || body.IsValid(eps);
    }
};

struct GEOMETRY_API PolyhedronBrepBodyConversion3d
{
    bool success{false};
    BrepConversionIssue3d issue{BrepConversionIssue3d::None};
    std::size_t faceIndex{0};
    BrepBody body{};

    [[nodiscard]] bool IsValid(const GeometryTolerance3d& tolerance = {}) const
    {
        return !success || body.IsValid(tolerance);
    }
};

[[nodiscard]] GEOMETRY_API BrepFaceConversion3d ConvertToPolyhedronFace(
    const BrepFace& face,
    double eps = 1e-9);

[[nodiscard]] GEOMETRY_API BrepBodyConversion3d ConvertToPolyhedronBody(
    const BrepBody& body,
    double eps = 1e-9);

[[nodiscard]] GEOMETRY_API PolyhedronBrepBodyConversion3d ConvertToBrepBody(
    const PolyhedronBody& body,
    double eps = 1e-9);
} // namespace Geometry::Sdk
