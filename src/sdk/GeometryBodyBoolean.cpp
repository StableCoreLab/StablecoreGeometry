#include "sdk/GeometryBodyBoolean.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] BodyBooleanResult3d MakeInvalidInputResult()
{
    BodyBooleanResult3d result;
    result.issue = BodyBooleanIssue3d::InvalidInput;
    result.message = "Body boolean input must contain at least one valid face.";
    return result;
}

[[nodiscard]] BodyBooleanResult3d MakeUnsupportedResult()
{
    BodyBooleanResult3d result;
    result.issue = BodyBooleanIssue3d::UnsupportedOperation;
    result.message =
        "3D body boolean interface is reserved for Delphi parity fast-track; implementation remains open.";
    return result;
}

[[nodiscard]] bool HasFaces(const BrepBody& body)
{
    return body.FaceCount() > 0;
}

[[nodiscard]] bool HasFaces(const PolyhedronBody& body)
{
    return body.FaceCount() > 0;
}

template <typename BodyT>
[[nodiscard]] BodyBooleanResult3d MakeResultForBodies(const BodyT& first, const BodyT& second)
{
    if (!HasFaces(first) || !HasFaces(second))
    {
        return MakeInvalidInputResult();
    }

    return MakeUnsupportedResult();
}
} // namespace

BodyBooleanResult3d IntersectBodies(const BrepBody& first, const BrepBody& second, BodyBooleanOptions3d)
{
    return MakeResultForBodies(first, second);
}

BodyBooleanResult3d UnionBodies(const BrepBody& first, const BrepBody& second, BodyBooleanOptions3d)
{
    return MakeResultForBodies(first, second);
}

BodyBooleanResult3d DifferenceBodies(const BrepBody& first, const BrepBody& second, BodyBooleanOptions3d)
{
    return MakeResultForBodies(first, second);
}

BodyBooleanResult3d IntersectBodies(const PolyhedronBody& first, const PolyhedronBody& second, BodyBooleanOptions3d)
{
    return MakeResultForBodies(first, second);
}

BodyBooleanResult3d UnionBodies(const PolyhedronBody& first, const PolyhedronBody& second, BodyBooleanOptions3d)
{
    return MakeResultForBodies(first, second);
}

BodyBooleanResult3d DifferenceBodies(
    const PolyhedronBody& first,
    const PolyhedronBody& second,
    BodyBooleanOptions3d)
{
    return MakeResultForBodies(first, second);
}
} // namespace geometry::sdk
