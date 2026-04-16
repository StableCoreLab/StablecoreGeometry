#pragma once

#include <string>
#include <vector>

#include "Export/GeometryExport.h"
#include "Brep/BrepBody.h"
#include "Core/GeometryTypes.h"
#include "Brep/PolyhedronBody.h"

namespace Geometry::Sdk
{
enum class BodyBooleanIssue3d
{
    None,
    InvalidInput,
    UnsupportedOperation
};

struct GEOMETRY_API BodyBooleanOptions3d
{
    GeometryTolerance3d tolerance{};
    bool operateOnShells{false};
    bool preserveInputOrientation{true};
};

struct GEOMETRY_API BodyBooleanResult3d
{
    BodyBooleanIssue3d issue{BodyBooleanIssue3d::None};
    BrepBody body{};
    std::vector<BrepBody> bodies{};
    bool producedEmptyResult{false};
    std::string message{};

    [[nodiscard]] bool IsSuccess() const
    {
        return issue == BodyBooleanIssue3d::None && (body.FaceCount() > 0 || !bodies.empty() || producedEmptyResult);
    }
};

[[nodiscard]] GEOMETRY_API BodyBooleanResult3d IntersectBodies(
    const BrepBody& first,
    const BrepBody& second,
    BodyBooleanOptions3d options = {});

[[nodiscard]] GEOMETRY_API BodyBooleanResult3d UnionBodies(
    const BrepBody& first,
    const BrepBody& second,
    BodyBooleanOptions3d options = {});

[[nodiscard]] GEOMETRY_API BodyBooleanResult3d DifferenceBodies(
    const BrepBody& first,
    const BrepBody& second,
    BodyBooleanOptions3d options = {});

[[nodiscard]] GEOMETRY_API BodyBooleanResult3d IntersectBodies(
    const PolyhedronBody& first,
    const PolyhedronBody& second,
    BodyBooleanOptions3d options = {});

[[nodiscard]] GEOMETRY_API BodyBooleanResult3d UnionBodies(
    const PolyhedronBody& first,
    const PolyhedronBody& second,
    BodyBooleanOptions3d options = {});

[[nodiscard]] GEOMETRY_API BodyBooleanResult3d DifferenceBodies(
    const PolyhedronBody& first,
    const PolyhedronBody& second,
    BodyBooleanOptions3d options = {});
} // namespace Geometry::Sdk
