#include <gtest/gtest.h>

#include "sdk/GeometryBodyBoolean.h"
#include "sdk/PolyhedronBody.h"

using geometry::sdk::BodyBooleanIssue3d;
using geometry::sdk::IntersectBodies;
using geometry::sdk::PolyhedronBody;

TEST(BodyBoolean3dGapTest, DelphiGradeBodyAndShellBooleanRemainOpen)
{
    const PolyhedronBody first;
    const PolyhedronBody second;
    const auto result = IntersectBodies(first, second);
    (void)result;

    GTEST_SKIP() << "Known 3D gap: Delphi-grade body/shell boolean workflow remains open. "
                    "Current fast-track now covers invalid-input contract, deterministic "
                    "identical/disjoint closed-body subsets including identical difference-empty, axis-aligned contained-body "
                    "intersection/union subsets, axis-aligned single-box overlap "
                    "subsets, face-touching axis-aligned box union subsets whose result remains "
                    "one closed box, face-touching external difference subsets that preserve "
                    "the original closed box, axis-aligned contained difference-empty subsets, "
                    "axis-aligned edge/vertex-touching ordered multi-body union subsets, axis-aligned edge/vertex-touching external difference subsets, "
                    "and deterministic disjoint ordered-multi-body union / disjoint / axis-aligned face/edge/vertex-touching empty "
                    "intersection subsets. Non-box overlap, non-axis-aligned or richer touching "
                    "intersection semantics, "
                    "shell-policy variants, and topology-preserving healing integration are still "
                    "pending.";
}

TEST(BodyBoolean3dGapTest, FaceTouchingLShapeUnionRemainsUnsupportedWithExplicitGap)
{
    GTEST_SKIP() << "Known 3D gap: axis-aligned inputs that would produce an L-shaped union still exceed the current single-box touching contract. "
                    "Expected future capability: return an explicit unsupported result for non-box unions instead of pretending they are a single closed body.";
}

TEST(BodyBoolean3dGapTest, RotatedBoxIntersectionRemainsUnsupported)
{
    GTEST_SKIP() << "Known 3D gap: intersection between an axis-aligned box and a rotated box is not yet supported by the deterministic box-only subset. "
                    "Expected future capability: either a supported non-axis-aligned intersection result or an explicit unsupported response with a clear boundary.";
}

TEST(BodyBoolean3dGapTest, ContainedShellPolicyOptionStillHasNoEffectAndStaysGap)
{
    GTEST_SKIP() << "Known 3D gap: shell-policy options are still not part of the public boolean contract. "
                    "Expected future capability: either prove the option has a deterministic effect or keep it explicitly unsupported.";
}
